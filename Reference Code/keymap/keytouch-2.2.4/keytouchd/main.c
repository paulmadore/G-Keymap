/*----------------------------------------------------------------------------------
Name               : main.c
Author             : Marvin Raaijmakers
Description        : The main source file of keytouchd, that contains the
                     event-loop. This program grabs the key press events of the
                     extra function keys and performs the appropriate action when
                     such key is pressed.
Date of last change: 06-May-2006
History            : 
                     06-May-2006 XUngrabKeyboard() is now called before calling
                                 handle_key()
                     02-Jan-2006 - Added signalhandler clean_exit()
                                 - Runs clear_key_settings_list(), ungrabs all keys
                                   and grabs the keys of the new current keyboard
                                   when the current keyboard changes
                     01-Jan-2006 After calling get_current_keyboard(&tmp_keyboard_name)
                                 the unused keyboard name will be freed

    Copyright (C) 2005-2006 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <signal.h>
#include <sys/wait.h>

#include <mxml.h>
#include <keytouchd.h>


static Boolean catch_sigchld (void);
static void sigchld_handler (int signal);
static void clean_exit (int sig);


/* The variables below are global because
 * they are needed by clean_exit()
 */
static KTKeySettingsList	key_list;
static KTKeyboardName		keyboard_name;
static KTPreferences		user_preferences;
static Display			*display;
KTPluginList			plugin_list;


int
main (void)
{
	KTKeySettings		*key;
	KTKeyboardName		tmp_keyboard_name;
	XEvent			event;
	
	if (catch_sigchld() == FALSE)
	{
		exit (EXIT_FAILURE);
	}
	/* Connect to the server specified in the DISPLAY evironment variable */
	display = XOpenDisplay(NULL);
	if (display == NULL)
	{
		KTError ("Cannot connect to X server %s.", XDisplayName(NULL));
		exit (EXIT_FAILURE);
	}
	init_plugins (&plugin_list);
	read_plugins (&plugin_list);
	get_current_keyboard (&keyboard_name);
	read_configuration (&key_list, &keyboard_name);
	read_preferences (&user_preferences);
	grab_keys (&key_list, display);
	
	/* Trap key signals */
	signal (SIGINT, clean_exit);
	signal (SIGQUIT, clean_exit);
	signal (SIGTERM, clean_exit);
	
	while (1)
	{
		XNextEvent (display, &event);
		if (event.type == KeyPress)
		{
			get_current_keyboard (&tmp_keyboard_name);
			/* If the name of the current keyboard changed */
			if (strcmp(tmp_keyboard_name.model, keyboard_name.model) ||
			    strcmp(tmp_keyboard_name.manufacturer, keyboard_name.manufacturer))
			{
				/* We don't need the old keyboard name anymore */
				XmlFree (keyboard_name.model);
				XmlFree (keyboard_name.manufacturer);
				keyboard_name = tmp_keyboard_name;
				
				XUngrabKey (display, AnyKey, AnyModifier, DefaultRootWindow(display));
				/* Clear the old configuration */
				clear_key_settings_list (&key_list);
				
				/* Read the new configuration */
				read_configuration (&key_list, &keyboard_name);
				grab_keys (&key_list, display);
			}
			else
			{
				XmlFree (tmp_keyboard_name.model);
				XmlFree (tmp_keyboard_name.manufacturer);
			}
			
			key = key_settings_of_keycode(event.xkey.keycode, &key_list);
			if (key == NULL)
			{
				KTError ("Unknown key pressed, this might be a bug.", "");
			}
			else
			{
				/* Cancel the passive grab on the keyboard so that the
				 * user can press other keys and plugins can simulate key
				 * presses using XTestFakeKeyEvent() without grabbing
				 * these simulated events.
				 */
				XUngrabKeyboard (display, CurrentTime);
				XSync (display, False);
				handle_key (key, &user_preferences, display);
			}
		}
	}
}


void
clean_exit (int sig)
/*
Input:
	-
Output:
	-
Globals:
	key_list
	keyboard_name
	user_preferences
	display
Returns:
	-
Description:
	This function prepares the program to exit and then exits.
*/
{
	clear_key_settings_list (&key_list);
	XUngrabKey (display, AnyKey, AnyModifier, DefaultRootWindow(display));
	XmlFree (keyboard_name.model);
	XmlFree (keyboard_name.manufacturer);
	XmlFree (user_preferences.documents_dir);
	XmlFree (user_preferences.home_page);
	XmlFree (user_preferences.search_page);
	XmlFree (user_preferences.browser);
	XmlFree (user_preferences.email_program);
	XmlFree (user_preferences.chat_program);
	XCloseDisplay (display);
	
	exit (EXIT_SUCCESS);
}


void
sigchld_handler (int signal)
/*
Input:
	signal	- The signal number. This function will not check this number
		  because it should only be called on a SIGCHLD signal.
Output:
	-
Returns:
	-
Description:
	This function handles the terminates child process properly to prevent zomby
	processes.
*/
{
	int stat;
	
	while (waitpid(-1, &stat, WNOHANG) > 0)
		; /* NULL Statement */
}


Boolean
catch_sigchld (void)
/*
Input:
	-
Output:
	-
Returns:
	TRUE if the SIGCHLD signal was succesfully catched, otherwise FALSE.
Description:
	This function sets sigchld_handler() for handling the SIGCHLD signal which
	occurs when a child process was terminated.
*/
{
	struct sigaction signal_action;
	
	/* Run sigchld_handler() when a SIGCHLD signal is catched */
	signal_action.sa_handler = sigchld_handler;
	/* Specify that no other signal will be blocked
	 * when our signal handler is called. */
	sigemptyset (&signal_action.sa_mask);
	signal_action.sa_flags = SA_RESTART;
	/* If the SIGCHLD signal occurs, go to the function
	 * specified by the member sa_handler of the
	 * struct signal_action */
	if ((sigaction(SIGCHLD, &signal_action, NULL)) < 0)
	{
		KTError ("Could not catch the SIGCHLD signal.", "");
		return (FALSE);
	}
	return (TRUE);
}
