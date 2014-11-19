/*---------------------------------------------------------------------------------
Name               : keyboard.c
Author             : Marvin Raaijmakers
Description        : Manages the keyboard list
Date of last change: 02-Feb-2006
History            : 
                     02-Feb-2006 find_keyboard_model() now checks if model is not
                                 0 before calling strcmp(model->name, name->model)
                     11-Jan-2006 Modified insert_keyboard() and
                                 insert_keyboard_model() so that they take
                                 kbfile_date as an argument
                     06-Jan-2006 Wrote find_keyboard_model()

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
#include <stdio.h>
#include <string.h>

#include <keytouch-keyboard.h>

static void insert_keyboard_model (char *name, struct tm kbfile_date, KTKeyboardManufacturer *manufacturer);
static KTKeyboardManufacturer *get_keyboard_manufacturer (char *name, KTKeyboardList *keyboard_list);


KTKeyboardManufacturer
*get_keyboard_manufacturer (	char		*name,
				KTKeyboardList	*keyboard_list  )
/*
Input:
	name		- The name of the manufacturer to get
	keyboard_list	- The list containing the keyboards
Output:
	-
Returns:
	A pointer to the KTKeyboardManufacturer that is named name.
Description:
	This function returns a pointer to the KTKeyboardManufacturer that is named
	name. If no such KTKeyboardManufacturer exists a new one is created.
*/
{
	KTKeyboardManufacturer	*manufacturer,
				*prev_manu,
				*next_manu;
	
	/* Find the manufacturer */
	for (prev_manu = NULL, manufacturer = keyboard_list->head;
	     manufacturer != NULL && strcmp(manufacturer->name, name) < 0;
	     prev_manu = manufacturer, manufacturer = manufacturer->next)
		; /* NULL Statement */
	
	/* If the manufacturer does not exist */
	if (manufacturer == NULL || strcmp(manufacturer->name, name) != EQUAL)
	{
		next_manu = manufacturer;
		manufacturer = (KTKeyboardManufacturer *) keytouch_malloc(sizeof(KTKeyboardManufacturer));
		manufacturer->name = (char *) keytouch_malloc(strlen(name)+1);
		manufacturer->model_list.head = manufacturer->model_list.tail = NULL;
		strcpy (manufacturer->name, name);
		if (prev_manu == NULL)
		{
			keyboard_list->head = manufacturer;
		}
		else
		{
			prev_manu->next = manufacturer;
		}
		manufacturer->next = next_manu;
		if (next_manu == NULL)
		{
			keyboard_list->tail = manufacturer;
		}
	}
	return (manufacturer);
}


void
insert_keyboard_model (	char			*name,
			struct tm		kbfile_date,
			KTKeyboardManufacturer	*manufacturer )
/*
Input:
	name		- The name of the model to insert
	kbfile_date	- The "last-changed" date of the keyboard file
Output:
	manufacturer	- Contains the list of models where the new model will be
			  added to
Returns:
	-
Description:
	This function inserts a new keyboard model to the model list of manufacturer
	if the model is not in the list already. The contents of kbfile_date will
	always be copied to the kbfile_last_change member of the keyboard model.
*/
{
	KTKeyboardModel	*model,
			*prev_model,
			*next_model;
	
	/* Find the model */
	for (prev_model = NULL, model = manufacturer->model_list.head;
	     model != NULL && strcmp(model->name, name) < 0;
	     prev_model = model, model = model->next)
		; /* NULL Statement */
	
	/* If the model does not exist */
	if (model == NULL || strcmp(model->name, name) != EQUAL)
	{
		next_model = model;
		model = (KTKeyboardModel *) keytouch_malloc(sizeof(KTKeyboardModel));
		model->name = (char *) keytouch_malloc(strlen(name)+1);
		strcpy (model->name, name);
		if (prev_model == NULL)
		{
			manufacturer->model_list.head = model;
		}
		else
		{
			prev_model->next = model;
		}
		model->next = next_model;
		if (next_model == NULL)
		{
			manufacturer->model_list.tail = model;
		}
	}
	model->kbfile_last_change = kbfile_date;
}


void
insert_keyboard (	KTKeyboardName	*keyboard_name,
			struct tm	kbfile_date,
			KTKeyboardList	*keyboard_list     )
/*
Input:
	keyboard_name	- The name of the keyboard to insert
	kbfile_date	- The "last-changed" date of the keyboard file
Output:
	keyboard_list	- The list where the keyboard together with kbfile_date will
			  be insterted in
Returns:
	-
Description:
	This function inserts the keyboard, named keyboard_name, in keyboard_list.
	If the manufacturer or model does not yet exist in keyboard_list, it will be
	created. A new model will only be created if it does not exist already. The
	contents of kbfile_date will always be copied to the kbfile_last_change
	member of the keyboard model.
*/
{
	KTKeyboardManufacturer	*manufacturer;
	
	manufacturer = get_keyboard_manufacturer(keyboard_name->manufacturer, keyboard_list);
	insert_keyboard_model (keyboard_name->model, kbfile_date, manufacturer);
}


void
copy_keyboard (	char		*file_name,
		KTKeyboardName	*keyboard_name )
/*
Input:
	file_name	- The file name of the keyboard file.
	keyboard_name	- The name of the keyboard of the keyboard file
Output:
	-
Returns:
	-
Description:
	This function copies the keyboard file, named file_name, of the keyboard,
	named keyboard_name, to the keyboard file directory.
*/
{
	char	*to_file_name;
	FILE	*from,
		*to;
	
	to_file_name = (char *) keytouch_malloc(strlen(KEYBOARD_FILES_DIR "/.") +
	                                        strlen(keyboard_name->manufacturer) +
	                                        strlen(keyboard_name->model) + 1);
	strcpy (to_file_name, KEYBOARD_FILES_DIR "/");
	strcat (to_file_name, keyboard_name->model);
	strcat (to_file_name, ".");
	strcat (to_file_name, keyboard_name->manufacturer);
	
	from = fopen(file_name, "r");
	if (from == NULL)
	{
		KTError (_("Failed to open the keyboard file \"%s\" for reading."), file_name);
		return;
	}
	to = fopen(to_file_name, "w");
	if (to == NULL)
	{
		KTError (_("Failed to copy the keyboard file to the "
		           "keyboard file directory (%s). Check your "
			   "permissions for this directory."), KEYBOARD_FILES_DIR);
		return;
	}
	
	/* Copy the file */
	while (!feof(from))
	{
		putc (getc(from), to);
	}
	
	fclose (from);
	fclose (to);
}


KTKeyboardModel
*find_keyboard_model (	KTKeyboardName	*name,
			KTKeyboardList	*keyboard_list )
/*
Input:
	name		- The name of the model to find
	keyboard_list	- The list to search for the model
Output:
	-
Returns:
	A pointer to the keyboard model if found, otherwise NULL.
Description:
	This function searches for a keyboard model, named 'name', in
	keyboard_list.
*/
{
	KTKeyboardModel		*model;
	KTKeyboardManufacturer	*manufacturer;
	
	/* Find the manufacturer */
	for (manufacturer = keyboard_list->head;
	     manufacturer != NULL && strcmp(manufacturer->name, name->manufacturer) < 0;
	     manufacturer = manufacturer->next)
		; /* NULL Statement */
	
	model = NULL;
	if (manufacturer && !strcmp(manufacturer->name, name->manufacturer))
	{
		/* Find the model */
		for (model = manufacturer->model_list.head;
		     model != NULL && strcmp(model->name, name->model) < 0;
		     model = model->next)
			; /* NULL Statement */
		/* If the model is not in the list */
		if (model && strcmp(model->name, name->model))
		{
			model = NULL;
		}
	}
	return (model);
}
