/*---------------------------------------------------------------------------------
Name               : handle_key.c
Author             : Marvin Raaijmakers
Description        : Handles key press events
Date of last change: 03-Aug-2006

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
-----------------------------------------------------------------------------------*/
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>

#include <keytouchd.h>

static char *get_focused_window_name (Display *display);
static KTPluginKey *get_plugin_key (	KTPluginKeyFunction	*key_function,
					Display			*display        );
static void send_key_event (	Display		*display,
				Window		window,
				char		*keysym,
				unsigned int	state,
				int		type      );
static void emulate_press_modifiers (	unsigned int	state,
					Window		window,
					Display		*display  );
static void emulate_release_modifiers (	unsigned int	state,
					Window		window,
					Display		*display  );
static void emulate_key (	KTPluginKey	*key,
				Display		*display  );

char
*get_focused_window_name (Display *display)
/*
Input:
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	The name of the currently focussed window. If no window is focussed NULL is
	returned. If the focussed window has no name it returns the first ancestor
	of it that has a name. If there is no such ancestor NULL is returned.
Description:
	This function returns the name of the currently focussed window.
*/
{
	unsigned int	num_children;
	int		revert_to;
	Window		window,
			parent,
			root,
			*children;
	char		*window_name;
	
	XGetInputFocus (display, &window, &revert_to);
	/* If there is no focused window */
	if (window == 0)
	{
		return (NULL);
	}
	parent = window;
	/* Find the first ancestor with a window name */
	while (!XFetchName(display, window, &window_name) && parent != DefaultRootWindow(display))
	{
		window = parent;
		XQueryTree (display, window, &root, &parent, &children, &num_children);
		XFree (children);
	}
	
	return (window_name);
}


KTPluginKey
*get_plugin_key (	KTPluginKeyFunction	*key_function,
			Display			*display        )
/*
Input:
	key_function	- The key function to get the key information from
	display		- Specifies the connection to the X server
Output:
	-
Returns:
	A pointer to the KTPluginKey in key_function that belongs to the currently
	focussed window. If no KTPlugins key belongs to the currently focussed
	window NULL is returned.
Description:
	This function returns the KTPluginKey in key_function that belongs to the
	currently focussed window.
*/
{
	char	*window_name, /* The name of the window that has input focus */
		*substr;
	int	count;
	
	window_name = get_focused_window_name(display);
	if (window_name == NULL)
	{
		/* If there is a KTPluginKey for every application */
		if (key_function->num_programs &&
		    key_function->key[key_function->num_programs-1].window_name.name == NULL)
		{
			return (&key_function->key[key_function->num_programs-1]);
		}
	}
	else
	{
		for (count = 0; count < key_function->num_programs; count++)
		{
			/* This should only happen when count is equal
			 * to key_function->num_programs-1 */
			if (key_function->key[count].window_name.name == NULL)
			{
				XFree (window_name);
				return (&key_function->key[count]);
			}
			else
			{
				substr = strstr(window_name, key_function->key[count].window_name.name);
				if (substr != NULL)
				{
					/* If the substring must appear at the end of the window's name
					 * and it does.
					 * Or:
					 * If the substring must appear at the beginning of the window's
					 * name and it does. */
					if ((*(substr+strlen(key_function->key[count].window_name.name)) == '\0' &&
					     key_function->key[count].window_name.is_end) ||
					    (substr == window_name &&
					     !key_function->key[count].window_name.is_end))
					{
						XFree (window_name);
						return (&key_function->key[count]);
					}
				}
			}
		}
		XFree (window_name);
	}
	return (NULL);
}


void
send_key_event (	Display		*display,
			Window		window,
			char		*keysym,
			unsigned int	state,
			int		type      )
/*
Input:
	display	- Specifies the connection to the X server
	window	- Specifies the window the window the event is to be sent to
	keysym	- The string of the keysym of the key to send
	state	- The state of the key event
	type	- KeyPress or KeyRelease
Output:
	-
Returns:
	-
Description:
	This function sends a key press or release event to window.
*/
{
	long      event_mask;
	XKeyEvent key_event;
	
	if (type == KeyPress)
	{
		event_mask = KeyPressMask;
	}
	else
	{
		event_mask = KeyReleaseMask;
	}
	key_event.type = type;
	key_event.display = display;
	key_event.window = window;
	key_event.root = DefaultRootWindow(display);
	key_event.state = state;
	key_event.keycode = XKeysymToKeycode(display, XStringToKeysym(keysym));
	
	XSendEvent (display, window, False, event_mask, (XEvent *) &key_event);
	usleep (80000); /* This is a value where mozilla doesn't ignore the event */
}


void
emulate_release_modifiers (	unsigned int	state,
				Window		window,
				Display		*display  )
/*
Input:
	state	- The bitmask indication which modifiers are pressed
	window	- The window to send the key release event(s) to
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends for every modifier whos bit is set in state a
	KeyRelease event. It only does this for the following modifiers:
	- Shift
	- Control
	- Mod1
*/
{
	unsigned int tmp_state;
	
	tmp_state = state;
	if (state & ControlMask)
	{
		send_key_event (display, window, "Control_L", tmp_state, KeyRelease);
		tmp_state ^= ControlMask;
	}
	if (state & ShiftMask)
	{
		send_key_event (display, window, "Shift_L", tmp_state, KeyRelease);
		tmp_state ^= ShiftMask;
	}
	if (state & Mod1Mask)
	{
		send_key_event (display, window, "Alt_L", tmp_state, KeyRelease);
		tmp_state ^= Mod1Mask;
	}
}


void
emulate_press_modifiers (	unsigned int	state,
				Window		window,
				Display		*display  )
/*
Input:
	state	- The bitmask indication which modifiers are pressed
	window	- The window to send the key press event(s) to
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends for every modifier whos bit is set in state a
	KeyPress event. It only does this for the following modifiers:
	- Shift
	- Control
	- Mod1
*/
{
	unsigned int tmp_state;
	
	tmp_state = 0;
	if (state & ControlMask)
	{
		send_key_event (display, window, "Control_L", tmp_state, KeyPress);
		tmp_state |= ControlMask;
	}
	if (state & ShiftMask)
	{
		send_key_event (display, window, "Shift_L", tmp_state, KeyPress);
		tmp_state |= ShiftMask;
	}
	if (state & Mod1Mask)
	{
		send_key_event (display, window, "Alt_L", tmp_state, KeyPress);
		tmp_state |= Mod1Mask;
	}
}


void
emulate_key (	KTPluginKey	*key,
		Display		*display  )
/*
Input:
	key	- Information about the key events to emulate
	display	- Specifies the connection to the X server
Output:
	-
Returns:
	-
Description:
	This function sends key events to the window that has input focus on
	display. First this function sends for every modifier whos bit is set in
	key->state a KeyPress event. Then it sends a KeyPress followed by a
	KeyRelease event for the key that has the keysym key->keysym. Then it sends
	for every modifier whos bit is set in key->state a KeyRelease event.
*/
{
	Window		focus_window;
	int		revert_to;
	
	XGetInputFocus (display, &focus_window, &revert_to);
	
	emulate_press_modifiers (key->state, focus_window, display);
	send_key_event (display, focus_window, key->keysym, key->state, KeyPress);
	send_key_event (display, focus_window, key->keysym, key->state, KeyRelease);
	emulate_release_modifiers (key->state, focus_window, display);
	XSync (display, False);
}


void
handle_key (	KTKeySettings	*key,
		KTPreferences	*preferences,
		Display		*display       )
/*
Input:
	key		- Contians information about what to do
	preferences	- The users preferences
	display		- The display the key event occured on
Output:
	-
Returns:
	-
Description:
	This function handles a key event occured on display. The key variable
	points to a structure that tells this function what to do.
*/
{
	KTPluginFunction *plugin_function;
	KTPluginKey      *plugin_key;
	
	if (key->action.type == KTActionTypeProgram)
	{
		if (fork() == 0)
		{
    			execlp ("sh", "sh", "-c", key->action.program.command, NULL);
		}
	}
	else if (key->action.type == KTActionTypePlugin && key->action.plugin.function != NULL)
	{
		plugin_function = key->action.plugin.function;
		/* If it is a function we shoul call */
		if (plugin_function->type == KTPluginFunctionType_Function)
		{
			(*plugin_function->function.function) (preferences);
		}
		/* If it is a key press emulation */
		else if (plugin_function->type == KTPluginFunctionType_Key)
		{
			/* Get info about the key press we should emulate */
			plugin_key = get_plugin_key (&plugin_function->function.key_function, display);
			if (plugin_key != NULL)
			{
				emulate_key (plugin_key, display);
			}
		}
	}
}
