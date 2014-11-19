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

import java.util.HashMap;
import java.io.*;

/**
 * Generates a GUI that lists common commands and allows their selection
 * @author Alex Brick
 * @version 0.1
 */
public class MacroWindow implements SelectionListener
{
    String chosenCommand;
    
    private Shell s;
    private Table table;
    private Button select;
    private Button close;
    
    /**
     * Generates the GUI and displays it
     * @param d the <code>Display</code> to attach to
     * @param keyName the String that contains the key name that we are looking for
     */
    public MacroWindow(Display d, String keyName)
    {
        s = new Shell(d);
        s.setText("LinEAK Configuration Utility");
        s.setLayout(new RowLayout(SWT.VERTICAL));
        
        File commandFile = findCommandFile();
        
        HashMap commands = new HashMap();
        
        if(!s.isDisposed())
            commands = obtainMap(commandFile,keyName);
        
        if(!s.isDisposed())
        {
            Label instructions = new Label(s, SWT.WRAP);
            
            if(commands.size() == 0)
            {
                instructions.setText("There Were no Default Commands Detected for the Key " +
                        "\"" + keyName + "\".  Please press \"Close\" to continue with " +
                        "the configuration process.");
                
                close = new Button(s, SWT.PUSH);
                close.setText("Close Window");
                close.addSelectionListener(this);
            }
            else
            {
                instructions.setText("Using Command File: " + commandFile.getAbsolutePath() +
                        "\n\nThe below are autodetected commands for the " + keyName +
                        " key.  Please select one that you would like and press the \"Select\" " +
                        "button to have it automatically inserted for you.");
                
                table = new Table(s, SWT.MULTI|SWT.BORDER|SWT.FULL_SELECTION);
                table.setHeaderVisible(true);
                table.setLinesVisible(true);
                
                RowData tData = new RowData();
                tData.height = 500;
                table.setLayoutData(tData);
                
                TableColumn col1 = new TableColumn(table, SWT.NONE);
                col1.setText("Command");
                TableColumn col2 = new TableColumn(table, SWT.NONE);
                col2.setText("Description");
                
                Object[] keys = commands.keySet().toArray();
                
                for(int i = 0; i < keys.length; i++)
                {
                    String command = (String) keys[i];
                    String description = (String) commands.get(command);
                    
                    TableItem item = new TableItem(table, SWT.NONE);
                    item.setText(0, command);
                    item.setText(1, description);
                }
                
                table.getColumn(0).pack();
                table.getColumn(1).pack();
                
                select = new Button(s, SWT.PUSH);
                select.setText("Select");
                select.addSelectionListener(this);
            }
            
            s.pack();
            s.open();
            
            while(!s.isDisposed())
                if(!d.readAndDispatch())
                    d.sleep();
            s.dispose();
        }
    }
    
    private File findCommandFile()
    {
        String loc = new String(System.getProperty("user.home") + "/.lineak/lineakcommands");
        File theFile = new File(loc);
        
        if(theFile.exists())
            return theFile;
        
        loc = new String("/etc/lineakcommands");
        theFile = new File(loc);
        
        if(theFile.exists())
            return theFile;
        
        MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
        message.setText("Cannot Find File!");
        message.setMessage("Cannot Find /etc/lineakcommands or ~/.lineak/lineakcommands.  " +
        		"Because of this, you will be unable to use this feature.");
        
        message.open();
        s.dispose();
        
        return null;
    }
    
    /**
     * Looks through the file containing the commands and returns a map of each command
     * to its description
     * @param keyName the String of the key name that we are looking for
     * @return a HashMap of command to description
     */
    private HashMap obtainMap(File commandFile, String keyName)
    {
        try
        {
            BufferedReader bfrRead = new BufferedReader(new FileReader(commandFile));
            
            HashMap commands = new HashMap();
            boolean correct_name = false;
            
            while(bfrRead.ready())
            {
                String line = bfrRead.readLine();
                
                if(line.equals(""))
                    continue;
                else if(!correct_name && line.charAt(0) == ';')
                {
                	String[] currentNames = line.split(" ");
                    
                    for(int i = 0; i < currentNames.length; i++)
                    {
                        if(currentNames[i].equals(keyName))
                        {
                            correct_name = true;
                            break;
                        }
                    }
                }
                else if(correct_name)
                {
                    if(line.charAt(0) == ';')
                        correct_name = false;
                    else
                    {
                        String command = line;
                        String description = bfrRead.readLine();
                        
                        commands.put(command,description);
                    }
                }
                else if(correct_name && line.charAt(0) == ';')
                    break;
            }
            
            bfrRead.close();
            
            return commands;
        }
        catch(FileNotFoundException e)
        {
            MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
            message.setText("Cannot Find File!");
            message.setMessage("Cannot Find File /etc/lineakcommands.  Because of this, " +
                    "you will be unable to use this feature.");
            
            message.open();
            s.dispose();
        }
        catch(IOException e)
        {
            MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
            message.setText("IOException!!");
            message.setMessage("Unspecified Input/Output Exception!  The Autocommand " +
                    "Feature will be unavailable to you.");
            
            message.open();
            s.dispose();
        }
        
        return null;
    }
    
    /**
     * Handles all SelectionEvents thrown by this GUI
     */
    public void widgetSelected(SelectionEvent e)
    {
        if(e.getSource() == select)
        {
            TableItem[] items = table.getSelection();
            
            if(items.length == 0)
            {
                MessageBox message = new MessageBox(s, SWT.ICON_WARNING);
                message.setText("No Macro Chosen");
                message.setMessage("You Didn't Choose a Command.");
                
                message.open();
                s.dispose();
                return;
            }
            
            TableItem theItem = items[0];
            chosenCommand = theItem.getText(0);
            
            //System.out.println(chosenCommand);
            
            s.dispose();
        }
        else if(e.getSource() == close)
            s.dispose();
    }
    
    /**
     * Exists soley to satisfy the <code>SelectionListener</code> interface
     */
    public void widgetDefaultSelected(SelectionEvent e)
    {
        // empty
    }
}