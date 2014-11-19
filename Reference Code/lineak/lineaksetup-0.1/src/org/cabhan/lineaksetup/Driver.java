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

import org.eclipse.swt.widgets.*;

import java.io.*;
import java.util.HashMap;

/**
 * This is the Driver class for the lineaksetup program.  It coordinates all of the
 * different windows.
 * @author Alex Brick
 * @version 0.1
 */
public class Driver
{
    private static Display display;
    
    private static File defFile;
    private static String mixer;
    private static String cd;
    private static String code;
    private static String fullName;
    private static String[] keys;
    private static HashMap keyToCommand;
    private static String writeLocation;
    
    /**
     * Sets the Definition File
     * @param newFile the Definition File
     */
    public static void setDefFile(File newFile)
    {
        defFile = newFile;
    }
    
    /**
     * Returns the Definition File
     * @return the Definition File
     */
    public static File getDefFile()
    {
        return defFile;
    }
    
    /**
     * Sets the Mixer Device
     * @param newMixer the Mixer Device
     */
    public static void setMixer(String newMixer)
    {
        mixer = newMixer;
    }
    
    /**
     * Returns the Mixer Device
     * @return the Mixer Device
     */
    public static String getMixer()
    {
        return mixer;
    }
    
    /**
     * Sets the CD Device
     * @param newCD the CD Device
     */
    public static void setCD(String newCD)
    {
        cd = newCD;
    }
    
    /**
     * Returns the CD Device
     * @return the CD Device
     */
    public static String getCD()
    {
        return cd;
    }
    
    /**
     * Sets the Keyboard Code
     * @param newCode the Keyboard Code
     */
    public static void setCode(String newCode)
    {
        code = newCode;
    }
    
    /**
     * Returns the Keyboard Code
     * @return the Keyboard Code
     */
    public static String getCode()
    {
        return code;
    }
    
    /**
     * Sets the Keyboard's full name
     * @param newString the Keyboard's full name
     */
    public static void setFullName(String newString)
    {
        fullName = newString;
    }
    
    /**
     * Returns the Keyboard's full name
     * @return the Keyboard's full name
     */
    public static String getFullName()
    {
        return fullName;
    }
    
    /**
     * Sets the available Keys
     * @param newKeys the available Keys
     */
    public static void setKeys(String[] newKeys)
    {
        keys = newKeys;
    }
    
    /**
     * Returns the available Keys
     * @return the available Keys
     */
    public static String[] getKeys()
    {
        return keys;
    }
    
    /**
     * Sets the keyToCode Map
     * @param newMap the keyToCode Map
     */
    public static void setMap(HashMap newMap)
    {
        keyToCommand = newMap;
    }
    
    /**
     * Returns the keyToCode Map
     * @return the keyToCode Map
     */
    public static HashMap getMap()
    {
        return keyToCommand;
    }
    
    /**
     * Sets the write location
     * @param theLoc the write location
     */
    public static void setWriteLocation(String theLoc)
    {
        writeLocation = theLoc;
    }
    
    /**
     * Returns the write location
     * @return the write location
     */
    public static String getWriteLocation()
    {
        return writeLocation;
    }
    
    /**
     * Generates a WelcomeGUI
     */
    public static void welcome()
    {
        WelcomeGUI welcome = new WelcomeGUI(display);
    }
    
    /**
     * Generates a DefWindow
     */
    public static void defWindow()
    {
        DefWindow def = new DefWindow(display);
    }
    
    /**
     * Generates an InfoWindow
     */
    public static void moreInfo()
    {
        InfoWindow info = new InfoWindow(display);
    }
    
    /**
     * Generates a TypeWindow
     */
    public static void handleTypes()
    {
        TypeWindow type = new TypeWindow(display);
    }
    
    /**
     * Generates a CommandWindow
     */
    public static void setCommands()
    {
        CommandWindow command = new CommandWindow(display);
    }
    
    /**
     * Generates a ConfirmWindow
     */
    public static void confirm()
    {
        ConfirmWindow confirm = new ConfirmWindow(display);
    }
    
    /**
     * Generates a LocationWindow
     */
    public static void decideLocation()
    {
        LocationWindow location = new LocationWindow(display);
    }
    
    /**
     * Generates a WriteThread, executes it, and waits for it to finish
     */
    public static void write()
    {
        WriteThread write = new WriteThread(display);
        write.start();
        try { write.join(); } catch(InterruptedException e) { }
        
        done();
    }
    
    /**
     * Generates a FinishedWindow
     */
    public static void done()
    {
        FinishedWindow finished = new FinishedWindow(display);
    }
    
    /**
     * Generates a <code>Display</code>, prints out the GPL licensing information, and
     * calls the <code>welcome()</code> method. 
     * @param args commandline arguments
     */
    public static void main(String[] args)
    {
        display = new Display();
        
        System.out.println("\nlineaksetup version 0.1, Copyright (C) 2005 Alex Brick");
        System.out.println("lineaksetup comes with ABSOLUTELY NO WARRANTY; for details");
        System.out.println("please see Sections 11 and 12 of the GNU General Public License (GPL).");
        System.out.println("This is free software, and you are welcome to redistribute it");
        System.out.println("under certain conditions: see the GPL for details.");
              
        welcome();
    }
}