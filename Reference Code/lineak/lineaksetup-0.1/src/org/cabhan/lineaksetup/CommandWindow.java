/*  lineaksetup - Configuration Utility for the LinEAK daemon
 *  Copyright (C) 2005  Alex Brick
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

package org.cabhan.lineaksetup;

import org.eclipse.swt.*;
import org.eclipse.swt.widgets.*;
import org.eclipse.swt.layout.*;
import org.eclipse.swt.events.*;
import org.eclipse.swt.custom.ScrolledComposite;

import java.io.*;
import java.util.HashMap;
import java.util.Stack;
import java.util.regex.*;

/**
 * Generates a GUI that allows for the input of all commands
 * @author Alex Brick
 * @version 0.1
 */
public class CommandWindow implements SelectionListener
{
    private Shell s;
    private Button next;
    private String[] keys;
    private Text[] commands;
    private HashMap theMap;
    
    /**
     * Generates the GUI and displays it
     * @param d the <code>Display</code> to attach to
     */
    public CommandWindow(Display d)
    {
        keys = Driver.getKeys();
        
        if(keys == null)
            keys = getKeys();
        
        commands = new Text[keys.length];
        theMap = Driver.getMap();
        
        s = new Shell(d);
        s.setText("LinEAK Configuration Utility");
        s.setLayout(new RowLayout(SWT.VERTICAL));
        
        
        Label instructions = new Label(s, SWT.WRAP);
        instructions.setText("Please Enter the Command You Would Like to Assign to Each " +
                "Key.  To see a list of common commands for each key, please press the " +
                "\"A\" button.");
        
        RowData iData = new RowData();
        iData.width = 400;
        instructions.setLayoutData(iData);
        
        if(theMap != null)
            fillFromExistingMap();
        else
            makeBlankComposite();
        
        next = new Button(s, SWT.PUSH);
        next.setText("Next >>");
        next.addSelectionListener(this);
        
        s.pack();
        s.open();
        
        while(!s.isDisposed())
            if(!d.readAndDispatch())
                d.sleep();
            d.dispose();
    }
    
    /**
     * If there already exists a Map of Keys to Commands (for example, after pressing
     * "Previous" in a ConfirmWindow), an entry form is generated that automatically fills in
     * the commands
     */
    private void fillFromExistingMap()
    {
        ScrolledComposite sc = new ScrolledComposite(s, SWT.BORDER|SWT.V_SCROLL|SWT.FULL_SELECTION);
        sc.setLayout(new FillLayout());
        RowData scData = new RowData();
        scData.height = 600;
        sc.setLayoutData(scData);
        
        Composite c = new Composite(sc, SWT.NONE);
        sc.setContent(c);
        c.setLayout(new GridLayout(3,false));
        
        for(int i = 0; i < commands.length; i++)
        {
            Label theKey = new Label(c, SWT.SINGLE);
            theKey.setText(keys[i]);
            
            Text theCommand = new Text(c, SWT.SINGLE|SWT.BORDER);
            theCommand.setText((String) theMap.get(keys[i]));
            
            GridData gData = new GridData();
            gData.widthHint = 400;
            theCommand.setLayoutData(gData);
            
            commands[i] = theCommand;
            
            Button macro = new Button(c, SWT.PUSH);
            macro.setText("A");
            macro.setData("Key",keys[i]);
            macro.setData("Location", new Integer(i));
            macro.addSelectionListener(this);
            
            Label separator = new Label(c, SWT.SEPARATOR|SWT.HORIZONTAL);
            GridData sData = new GridData(GridData.FILL_HORIZONTAL);
            sData.horizontalSpan = 3;
            separator.setLayoutData(sData);
        }
        
        c.setSize(c.computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }
    
    /**
     * If there is no pre-existing Map of Keys to Commands, an empty entry form is created
     */
    private void makeBlankComposite()
    {
        ScrolledComposite sc = new ScrolledComposite(s, SWT.BORDER|SWT.V_SCROLL|SWT.FULL_SELECTION);
        sc.setLayout(new FillLayout());
        RowData scData = new RowData();
        scData.height = 600;
        sc.setLayoutData(scData);
        
        Composite c = new Composite(sc, SWT.NONE);
        sc.setContent(c);
        c.setLayout(new GridLayout(3,false));
        
        for(int i = 0; i < commands.length; i++)
        {
            Label theKey = new Label(c, SWT.SINGLE);
            theKey.setText(keys[i]);
            
            Text theCommand = new Text(c, SWT.SINGLE|SWT.BORDER);
            
            GridData gData = new GridData();
            gData.widthHint = 300;
            theCommand.setLayoutData(gData);
            
            commands[i] = theCommand;
            
            Button macro = new Button(c, SWT.PUSH);
            //Button macro = new Button(c, SWT.ARROW|SWT.DOWN);
            macro.setText("A");
            macro.setData("Key",keys[i]);
            macro.setData("Location", new Integer(i));
            macro.addSelectionListener(this);
            
            Label separator = new Label(c, SWT.SEPARATOR|SWT.HORIZONTAL);
            GridData sData = new GridData(GridData.FILL_HORIZONTAL);
            sData.horizontalSpan = 3;
            separator.setLayoutData(sData);
        }
        
        c.setSize(c.computeSize(SWT.DEFAULT, SWT.DEFAULT));
    }
    
    /**
     * Determines all supported keys from the Definition File
     * @return all supported keys
     */
    private String[] getKeys()
    {
        File defFile = Driver.getDefFile();
        //File defFile = new File("/etc/lineakkb.def"); // My test data (default definition location)
        String theCode = Driver.getCode();
        //String theCode = new String("LTCDLX500"); // My test data (my own keyboard)
        
        boolean in_code = false;
        
        Stack keys = new Stack();
        String[] theKeys = new String[0];
        int numKeys = 0;
        
        try
        {
            BufferedReader bfrRead = new BufferedReader(new FileReader(defFile));
            
            Pattern p2 = Pattern.compile("\\s+([\\w|]+)\\s+=\\s+\\d+"); // We are starting with p2 because this application is based on a Perl script I wrote
            Pattern p3 = Pattern.compile("^\\[END " + theCode + "\\]"); // to do the same via CLI.  I removed p1, but for my own memory, I am keeping these as
                                                                        // p2 and p3.
            
            while(bfrRead.ready())
            {
                String theLine = bfrRead.readLine();
                
                Matcher m2 = p2.matcher(theLine);
                Matcher m3 = p3.matcher(theLine);
                
                if(in_code || theLine.startsWith("[" + theCode + "]"))
                    in_code = true;
                
                if(!in_code)
                    continue;
                
                if(m2.matches())
                {
                    String key = m2.group(1);
                    String[] lineKeys = key.split("\\|");
                    
                    for(int i = 0; i < lineKeys.length; i++)
                    {
                        keys.push(lineKeys[i]);
                        numKeys++;
                    }
                }
                
                if(m3.matches())
                {
                    in_code = false;
                    break;
                }
            }
            bfrRead.close();
            
            theKeys = new String[numKeys];
            
            Stack qKeys = new Stack();
            
            while(!keys.empty())
                qKeys.push(keys.pop());
            
            for(int i = 0; i < numKeys; i++)
                theKeys[i] = (String) qKeys.pop();
        }
        
        catch(FileNotFoundException e)
        {
            MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
            message.setText("File Not Found!!");
            message.setMessage("Cannot Find " + defFile.getAbsolutePath() + "!!");
            
            message.open();
            System.exit(0);
        }
        catch(IOException e)
        {
            MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
            message.setText("IOException!!");
            message.setMessage("Unspecified Input/Output Exception!!");
            
            message.open();
            System.exit(0);
        }
        
        Driver.setKeys(theKeys);
        return theKeys;
    }
    
    /**
     * A basic test program
     * @param args commandline arguments
     */
    public static void main(String[] args)
    {
        Display d = new Display();
        CommandWindow command = new CommandWindow(d);
    }
    
    /**
     * Handles all Events generated by this GUI
     */
    public void widgetSelected(SelectionEvent e)
    {
        if(e.getSource() == next)
        {
            boolean not_all_fields = false;
            
            for(int i = 0; i < commands.length; i++)
            {
                if(commands[i].getText().length() == 0)
                    not_all_fields = true;
            }
            
            if(not_all_fields)
            {
                MessageBox message = new MessageBox(s, SWT.ICON_QUESTION|SWT.YES|SWT.NO);
                message.setText("Not All Fields Filled Out!");
                message.setMessage("Not all of the commands are filled out.  Would you " +
                        "like to continue anyway?");
                
                int response = message.open();
                
                if(response == SWT.NO)
                    return;
            }
            
            HashMap theMap = new HashMap();
            
            for(int i = 0; i < commands.length; i++)
                theMap.put(keys[i],commands[i].getText());
            
            s.dispose();
            Driver.setMap(theMap);
            Driver.confirm();
            
            //System.out.println("Ooga");
        }
        else if(((Button) e.getSource()).getText().equals("A"))
        {
            Button theButton = (Button) e.getSource();
            //System.out.println(theButton.isDisposed());
            //System.out.println(theButton.getText());
            String keyName = (String) theButton.getData("Key");
            
            MacroWindow macro = new MacroWindow(s.getDisplay(), keyName);
            
            //System.out.println(macro.chosenCommand);
            
            if(macro.chosenCommand != null)
            {
                int location = ((Integer) theButton.getData("Location")).intValue();
                commands[location].setText(macro.chosenCommand);
            }
        }
    }
    
    /**
     * Exists soley to satisfy the <code>SelectionListener</code> interface
     */
    public void widgetDefaultSelected(SelectionEvent e)
    {
        // empty
    }
}