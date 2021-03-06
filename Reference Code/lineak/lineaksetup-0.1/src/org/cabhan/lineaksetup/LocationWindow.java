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

/**
 * Generates a GUI that prompts the user for the write location
 * @author Alex Brick
 * @version 0.1
 */
public class LocationWindow implements SelectionListener
{
    private Shell s;
    private Text writeToMe;
    private Button next;
    
    private String home;
    
    /**
     * Generates the GUI and displays it
     * @param d the <code>Display</code> to attach to
     */
    public LocationWindow(Display d)
    {
        s = new Shell(d);
        s.setText("LinEAK Configuration Utility");
        s.setLayout(new RowLayout(SWT.VERTICAL));
        
        home = System.getProperty("user.home");
        
        Label instructions = new Label(s, SWT.SINGLE);
        instructions.setText("Please Enter The Location Where I Should Write Your Config " +
                "File To.\n\n" +
                "The Default Location is:\n" +
                home + "/.lineak/lineakd.conf\n");
        
        Composite c1 = new Composite(s, SWT.NONE);
        c1.setLayout(new RowLayout(SWT.HORIZONTAL));
        
        writeToMe = new Text(c1, SWT.SINGLE|SWT.BORDER);
        RowData wData = new RowData();
        wData.width = 330;
        writeToMe.setLayoutData(wData);
        writeToMe.setText(home + "/.lineak/lineakd.conf");
        
        next = new Button(c1, SWT.PUSH);
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
     * A basic test program
     * @param args commandline arguments
     */
    public static void main(String[] args)
    {
        Display d = new Display();
        LocationWindow location = new LocationWindow(d);
    }
    
    /**
     * Handles all <code>SelectionEvent</code>'s generated by this GUI
     */
    public void widgetSelected(SelectionEvent e)
    {
        if(e.getSource() == next)
        {
            String writeText = writeToMe.getText();
            writeText = writeText.replaceAll("~",home);
            
            if(writeText.length() == 0)
            {
                MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
                message.setText("No Location!!");
                message.setMessage("You Didn't Enter A Write Location!!");
                
                message.open();
            }
            else
            {
                File theFile = new File(writeText);
                
                if(theFile.canWrite() || !theFile.exists())
                {
                    if(theFile.isFile() || !theFile.exists())
                    {
                        Driver.setWriteLocation(writeText);
                        s.dispose();
                        Driver.write();
                        
                        //System.out.println(writeText);
                    }
                    else
                    {
                        MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
                        message.setText("Directory!");
                        message.setMessage("This Write Location is a Directory!!");
                        
                        message.open();
                    }
                }
                else
                {
                    MessageBox message = new MessageBox(s, SWT.ICON_ERROR);
                    message.setText("Not Writable!");
                    message.setMessage("The Location You Entered is Not Writable!!");
                    
                    message.open();
                }
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