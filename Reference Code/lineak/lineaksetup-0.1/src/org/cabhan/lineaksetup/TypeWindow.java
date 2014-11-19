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

import java.io.*;
import java.util.regex.*;
import java.util.HashMap;

public class TypeWindow implements SelectionListener
{
    private Shell s;
    private Table table;
    private Button next;
    private HashMap boardToCode;
    
    /**
     * Generates the GUI and displays it
     * @param d the <code>Display</code> to attach to
     */
    public TypeWindow(Display d)
    {
        boardToCode = new HashMap();
        
        s = new Shell(d);
        s.setText("LinEAK Configuration Utility");
        s.setLayout(new RowLayout(SWT.VERTICAL));
        
        Label instructions = new Label(s, SWT.WRAP);
        instructions.setText("Please Select Your Keyboard From the List Below and press " +
                "\"Next\"");
        
        makeTable();
        
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
    
    private void makeTable()
    {
        File defFile = Driver.getDefFile();
        //File defFile = new File("/etc/lineakkb.def"); // My test data (default definition file location)
        table = new Table(s, SWT.MULTI|SWT.BORDER|SWT.FULL_SELECTION);
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        
        RowData tData = new RowData();
        tData.height = 500;
        table.setLayoutData(tData);
        
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText("Keyboards");
        
        try
        {
            BufferedReader bfrRead = new BufferedReader(new FileReader(defFile));
            
            String currentCode = new String();
            String currentBrand = new String();
            
            Pattern p1 = Pattern.compile("^\\s*\\[([\\w\\d\\-]+)\\].*");
            Pattern p2 = Pattern.compile("^\\s+brandname\\s+=\\s+\"(\\w+)\"$");
            Pattern p3 = Pattern.compile("^\\s+modelname\\s+=\\s+\"([\\w\\d\\s()\\-]+)\"$");
            
            while(bfrRead.ready())
            {
                String theLine = bfrRead.readLine();
                
                Matcher m1 = p1.matcher(theLine);
                Matcher m2 = p2.matcher(theLine);
                Matcher m3 = p3.matcher(theLine);
                
                if(m1.matches())
                {
                    String theGroup = m1.group(1);
                    if(!theGroup.equals("KEYS") && !theGroup.startsWith("END"))
                        currentCode = theGroup;
                }
                else if(m2.matches())
                    currentBrand = m2.group(1);
                else if(m3.matches())
                {
                    String fullName = currentBrand + " " + m3.group(1);
                    
                    TableItem item = new TableItem(table, SWT.NONE);
                    item.setText(0, fullName);
                    boardToCode.put(fullName,currentCode);
                }
            }
            
            bfrRead.close();
            
            table.getColumn(0).pack();
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
    }
    
    /**
     * A basic test program
     * @param args commandline arguments
     */
    public static void main(String[] args)
    {
        Display d = new Display();
        TypeWindow type = new TypeWindow(d);
    }
    
    /**
     * Handles all <code>SelectionEvent</code>'s generated by this GUI
     */
    public void widgetSelected(SelectionEvent e)
    {
        if(e.getSource() == next)
        {
            TableItem[] items = table.getSelection();
            
            if(items.length == 0)
            {
                MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
                message.setText("No Keyboard!!");
                message.setMessage("You Didn't Select A Keyboard!!");
                
                message.open();
                return;
            }
            
            TableItem theItem = items[0];
            String chosenBoard = theItem.getText(0);
            String chosenCode = (String) boardToCode.get(chosenBoard);
            
            
            
            MessageBox message = new MessageBox(s, SWT.ICON_QUESTION|SWT.YES|SWT.NO);
            message.setText("Keyboard Okay?");
            message.setMessage("You have chosen the following keyboard:\n" + chosenBoard +
                    "\n\nIs this the correct choice?");
            
            int response = message.open();
            
            if(response == SWT.YES)
            {
                Driver.setCode(chosenCode);
                Driver.setFullName(chosenBoard);
                s.dispose();
                Driver.setCommands();
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