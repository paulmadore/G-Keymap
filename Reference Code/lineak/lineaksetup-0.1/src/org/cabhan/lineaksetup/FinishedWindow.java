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

/**
 * Generates a GUI that informs the user that the write has finished.
 * @author Alex Brick
 * @version 0.1
 */
public class FinishedWindow implements SelectionListener
{
    private Shell s;
    private Button close;
    
    /**
     * Generates the GUI and displays it
     * @param d the <code>Display</code> to attach to
     */
    public FinishedWindow(Display d)
    {
        String wroteTo = Driver.getWriteLocation();
        //String wroteTo = "/home/alex/omg";
        
        s = new Shell(d);
        s.setText("LinEAK Configuration Utility");
        s.setLayout(new RowLayout(SWT.VERTICAL));
        
        Label label = new Label(s, SWT.WRAP);
        label.setText("Congratulations!  Your LinEAK Configuration File Has Been Written " +
                "To " + wroteTo + " .\n\n" +
                "Again, Thank You For Using LinEAK!\n");
        
        close = new Button(s, SWT.PUSH);
        close.setText("Close");
        close.addSelectionListener(this);
        
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
        FinishedWindow finished = new FinishedWindow(d);
    }
    
    /**
     * Handles all <code>SelectionEvent</code>'s generated by this GUI
     */
    public void widgetSelected(SelectionEvent e)
    {
        if(e.getSource() == close)
        {
            s.getDisplay().dispose();
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