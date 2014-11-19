/*---------------------------------------------------------------------------------
Name               : amixer.c
Author             : Marvin Raaijmakers
Description        : Plugin for keyTouch that can change the volume (using amixer).
Date of last change: 24-Sep-2006
History            : 24-Sep-2006 Added two new plugin functions:
                                 "Volume increase 10%" and "Volume decrease 10%"
                     05-Mar-2006 - clean_exit() will be used to exit the client
                                   process, that manages the volume bar, cleanly
                                 - update_window() now returns a boolean indicating
                                   if the function should be called again
                     29-Jan-2006 Added the GUI volume bar to the plugin

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
-----------------------------------------------------------------------------------*/
#define _GNU_SOURCE
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include <plugin.h>
#include <amixer-plugin.h>


void vol_increase (KTPreferences *preferences);
void vol_decrease (KTPreferences *preferences);
void vol_increase_10 (KTPreferences *preferences);
void vol_decrease_10 (KTPreferences *preferences);
void mute (KTPreferences *preferences);

static void create_window (VOLUMEBAR_INFO *volumebar_info);
static int get_current_volume (void);
static void update_volume_bar (GtkWidget *volume_bar);
static gboolean update_window (VOLUMEBAR_INFO *volumebar_info);
static void clean_exit (int sig);
static void start_window (void);
static char *get_keytouch_user_dir (void);
static void change_volume (char *command);


static Boolean is_muted = FALSE;


KeytouchPlugin plugin_struct = {
	{"Amixer", "Marvin Raaijmakers", "GPL 2", "2.3",
	 "This plugin allows you to change the volume. It also shows\n"
	 "the current volume when it changes. To use this plugin amixer\n"
	 "needs to be installed."},
	"amixer.so",
	5,
	{{"Volume increase",     KTPluginFunctionType_Function, {.function = vol_increase}},
	 {"Volume decrease",     KTPluginFunctionType_Function, {.function = vol_decrease}},
	 {"Volume increase 10%", KTPluginFunctionType_Function, {.function = vol_increase_10}},
	 {"Volume decrease 10%", KTPluginFunctionType_Function, {.function = vol_decrease_10}},
	 {"Mute",                KTPluginFunctionType_Function, {.function = mute}},
	}
};



void
create_window (VOLUMEBAR_INFO *volumebar_info)
/*
Input:
	-
Output:
	volumebar_info	- The window element points to the created window and the
			  volume_bar element points to the volume progressbar in the
			  window
Returns:
	-
Description:
	This function creates a window with a progressbar with the following
	properties:
	- It is positioned in the center ot the screen.
	- It has no window decorations and can not be resized by the user.
	- It will allways be above other windows.
	- It is visible on all desktops.
	- It will not be visible in the taskbar an pager.
	- It does not accept focus.
*/
{
	volumebar_info->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position (GTK_WINDOW (volumebar_info->window), GTK_WIN_POS_CENTER);
	gtk_window_set_resizable (GTK_WINDOW (volumebar_info->window), FALSE);
	gtk_window_set_decorated (GTK_WINDOW (volumebar_info->window), FALSE);
	/* The window will allways be above others */
	gtk_window_set_keep_above (GTK_WINDOW (volumebar_info->window), TRUE);
	/* Let the window be visible on all desktops: */
	gtk_window_stick (GTK_WINDOW (volumebar_info->window));
	/* This window will not be visible in the taskbar: */
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW (volumebar_info->window), TRUE);
	/* This window will not be visible in the pager: */
	gtk_window_set_skip_pager_hint (GTK_WINDOW (volumebar_info->window), TRUE);
	gtk_window_set_accept_focus (GTK_WINDOW (volumebar_info->window), FALSE);
	
	volumebar_info->volume_bar = gtk_progress_bar_new();
	gtk_widget_show (volumebar_info->volume_bar);
	gtk_container_add (GTK_CONTAINER (volumebar_info->window), volumebar_info->volume_bar);
	gtk_widget_set_size_request (volumebar_info->volume_bar, 231, 24);
	gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (volumebar_info->volume_bar), 0.52);
	gtk_progress_bar_set_pulse_step (GTK_PROGRESS_BAR (volumebar_info->volume_bar), 0.02);
	gtk_progress_bar_set_text (GTK_PROGRESS_BAR (volumebar_info->volume_bar), "Volume");
}


int
get_current_volume (void)
/*
Returns:
	The current volume retrieved from amixer. -1 will be returned when
	retrieving the volume failed.
*/
{
	FILE *amixer;
	char c;
	int volume = -1;
	
	amixer = popen ("amixer sget Master | grep \"Front Left:\"", "r");
	if (amixer)
	{
		do {
			c = getc(amixer);
			/* We have found the volume when the following appears:
			 * '[' followed by an integer followed by '%'
			 */
			if (c == '[' && fscanf(amixer, "%d", &volume) && (c = getc(amixer)) == '%')
			{
				break;
			}
			volume = -1;
		} while (c != '\n' && c != EOF);
		pclose (amixer);
	}
	return (volume);
}


void
update_volume_bar (GtkWidget *volume_bar)
/*
Output:
	volume_bar	- Will show the percentage of the current volume
*/
{
	int volume;
	gchar *text;
	
	volume = get_current_volume();
	if (volume && volume != -1)
	{
		text = g_strdup_printf("Volume %d%%", volume);
		if (text)
		{
			gtk_progress_bar_set_text (GTK_PROGRESS_BAR(volume_bar), text);
			g_free (text);
		}
	}
	else
	{
		volume = 0;
		gtk_progress_bar_set_text (GTK_PROGRESS_BAR(volume_bar), "Muted");
	}
	gtk_progress_set_percentage (GTK_PROGRESS(volume_bar), (gdouble)volume/100.0);
	/* Directly draw the progressbar: */
	while (g_main_context_iteration(NULL, FALSE))
		; /* NULL Statement */
}


gboolean
update_window (VOLUMEBAR_INFO *volumebar_info)
/*
Input:
	volumebar_info->close_time	- The time to close the window
Output:
	volumebar_info			- Will be updated
Returns:
	TRUE if this function should be called again after UPDATE_INTERVAL
	miliseconds, otherwise FALSE.
Description:
	This function destroys volumebar_info->window and escapes from the GTK main
	routine if the current time is later than volumebar_info->close_time. If not
	then the volume bar will be updated with the current volume.
*/
{
	MSGBUF msg;
	Boolean close_window;
	
	/* Check if there is a new message on the queue */
	if (msgrcv(volumebar_info->msgqid, &msg, sizeof(msg.time), 1, IPC_NOWAIT) != -1)
	{
		volumebar_info->close_time = msg.time + SHOW_WINDOW_TIME;
	}
	close_window = (time(NULL) > volumebar_info->close_time);
	if (!close_window)
	{
		update_volume_bar (volumebar_info->volume_bar);
	}
	else
	{
		gtk_widget_destroy (volumebar_info->window);
		gtk_main_quit();
	}
	return !close_window;
}


void
start_window (void)
/*
Description:
	This function creates a window with a volume bar and shows it
	SHOW_WINDOW_TIME seconds when it receives a message on the message queue.
	The key of the message queue is generated by running
	ftok(get_keytouch_user_dir(), MSGQ_AMIXER_PROJ_ID). The messages that are
	sent to this queue should contain the time they are sent. The volume window
	will be showed from the time this function receives the message, until the
	time the message was sent plus SHOW_WINDOW_TIME seconds.
*/
{
	MSGBUF		msg;
	VOLUMEBAR_INFO	volumebar_info;
	key_t		msgq_key;
	char		*keytouch_user_dir;
	
	gtk_init (0, NULL);
	keytouch_user_dir = get_keytouch_user_dir();
	/* Get the key for the message queue */
	msgq_key = ftok(keytouch_user_dir, MSGQ_AMIXER_PROJ_ID);
	free (keytouch_user_dir);
	if (msgq_key == -1)
	{
		perror ("keytouch amixer plugin");
		return;
	}
	/* Get the message queue identifier and create the queue if necessary */
	volumebar_info.msgqid = msgget(msgq_key, 0);
	if (volumebar_info.msgqid == -1)
	{
		perror ("keytouch amixer plugin");
		return;
	}
	while (1)
	{
		if (msgrcv(volumebar_info.msgqid, &msg, sizeof(msg.time), 1, 0) != -1)
		{
			volumebar_info.close_time = msg.time + SHOW_WINDOW_TIME;
			if (time(NULL) <= volumebar_info.close_time)
			{
				create_window (&volumebar_info);
				update_volume_bar (volumebar_info.volume_bar);
				gtk_widget_show (volumebar_info.window);
				g_timeout_add (UPDATE_INTERVAL, (GSourceFunc) update_window, &volumebar_info);
				gtk_main();
			}
		}
	}
}


char
*get_keytouch_user_dir (void)
/*
Returns:
	The address of some new allocated space which is a string containing the
	value of the environment variable HOME followed by "/.keytouch2".
*/
{
	char *keytouch_dir, *home;
	
	home = getenv("HOME");
	if (home == NULL)
	{
		fputs ("keytouch amixer plugin: could not get environment variable $HOME", stderr);
		exit (EXIT_FAILURE);
	}
	if (asprintf(&keytouch_dir, "%s/.keytouch2", home) == -1)
	{
		fputs ("keytouch amixer plugin: asprintf() failed. "
		       "This is probably caused because it failed to allocate memory.", stderr);
		exit (EXIT_FAILURE);
	}
	return (keytouch_dir);
}


void
clean_exit (int sig)
{
	exit (EXIT_SUCCESS);
}


void
send_volume_changed_signal (void)
/*
Description:
	This function sends a signal to the child program that manages the
	volumebar. The child will receive the signal and will show the volumebar.
	The child process will be created if it does not exist yet.
*/
{
	static int qid = -1;
	MSGBUF msg;
	
	/* If this is the first time this function was called */
	if (qid == -1)
	{
		key_t msgq_key;
		char *keytouch_user_dir;
		
		keytouch_user_dir = get_keytouch_user_dir();
		/* Get the key for the message queue */
		msgq_key = ftok(keytouch_user_dir, MSGQ_AMIXER_PROJ_ID);
		free (keytouch_user_dir);
		if (msgq_key == -1)
		{
			perror ("keytouch amixer plugin");
			return;
		}
		/* Get the message queue identifier and create the queue if necessary */
		qid = msgget(msgq_key, MSGQ_PERMISSIONS | IPC_CREAT);
		if (qid == -1)
		{
			perror ("keytouch amixer plugin");
			return;
		}
		if (fork() == 0)
		{
			/* Trap key signals */
			signal (SIGINT, clean_exit);
			signal (SIGQUIT, clean_exit);
			signal (SIGTERM, clean_exit);
			/* We will now start the run_window() function in our
			 * child process for showing a volume bar to the user
			 */
			start_window();
			exit (EXIT_SUCCESS); /* We will never get here because of
			                      * the infinite loop in run_window()
			                      */
		}
	}
	msg.mtype = 1;
	msg.time = time(NULL);
	if (msgsnd(qid, &msg, sizeof(msg.time), 0) == -1)
	{
		perror ("keytouch amixer plugin");
	}
}


void
change_volume (char *command)
/*
Input:
	command	- The command that changes the volume.
Description:
	This function executes 'command' in a child process and then calls
	send_volume_changed_signal().
*/
{
	if (fork() == 0)
	{
		execlp ("sh", "sh", "-c", command, NULL);
		exit (EXIT_SUCCESS);
	}
	else
	{
		send_volume_changed_signal();
	}
}


void
vol_increase (KTPreferences *preferences)
{
	is_muted = FALSE;
	change_volume ( CHANGE_VOL_CMD(VOL_DEFAULT_INCR) );
}


void
vol_decrease (KTPreferences *preferences)
{
	is_muted &= !get_current_volume();
	change_volume ( CHANGE_VOL_CMD(VOL_DEFAULT_DECR) );
}


void
vol_increase_10 (KTPreferences *preferences)
{
	is_muted = FALSE;
	change_volume ( CHANGE_VOL_CMD(VOL_10PERCENT_INCR) );
}


void
vol_decrease_10 (KTPreferences *preferences)
{
	is_muted &= !get_current_volume();
	change_volume ( CHANGE_VOL_CMD(VOL_10PERCENT_DECR) );
}


void
mute (KTPreferences *preferences)
{
	static int	prev_volume = -1;
	int		current_volume;
	char		*command = NULL;
	
	current_volume = get_current_volume();
	is_muted &= !current_volume;
	if (is_muted)
	{
		/* Tell amixer to set the volume to prev_volume */
		if (asprintf(&command, "amixer sset Master %d%% > /dev/null", prev_volume) == -1)
		{
			fputs ("keytouch amixer plugin: asprintf() failed. "
			       "This is probably caused because it failed to allocate memory.", stderr);
		}
	}
	else if (current_volume)
	{
		/* Tell amixer to set the volume to 0 */
		command = strdup("amixer sset Master 0% > /dev/null");
		if (command == NULL)
		{
			perror ("keytouch amixer plugin");
		}
		prev_volume = current_volume;
	}
	/* Do we have to mute/unmute? */
	if (command)
	{
		if (fork() == 0)
		{
			execlp ("sh", "sh", "-c", command, NULL);
			exit (EXIT_SUCCESS);
		}
		else
		{
			send_volume_changed_signal();
		}
		free (command);
		is_muted = !is_muted;
	}
}
