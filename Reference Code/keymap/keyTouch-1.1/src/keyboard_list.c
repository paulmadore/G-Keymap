/*-------------------------------------------------------------------------------
Name               : keyboard_list.c
Author             : Marvin Raaijmakers
Description        : Providing operations for KEYBOARD_CONFIG
Date of last change: 10-December-2004
History            :
                     20-November-2004 Created:
                                      - keyboard_model_new()
                     21-November-2004 Created:
                                      - kbl_model_add()
                                      - kbl_manufacture_new()
                                      - kbl_manufacture_add()
                     6-December-2004  Created:
                                      - kbl_manufacture_find()
                     10-December-2004 Modified:
                                      - kbl_manufacture_add()
                                      - kbl_model_add()

    Copyright (C) 2004 Marvin Raaijmakers

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    You can contact me at: marvin_raaijmakers(at)lycos(dot)nl
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <keytouch.h>


void
kbl_init (KEYBOARD_LIST *keyboard_list)
/*
input:
	-
output:
	keyboard_list	- List containing all available keyboards
returns:
	-
description:
	This function initializes keyboard_list
*/
{
	keyboard_list->manufacture_list_head = keyboard_list->manufacture_list_tail = NULL;
}



KEYBOARD_MODEL
*kbl_model_new (void)
/*
input:
	-
output:
	-
returns:
	The address of the new KEYBOARD_MODEL.
description:
	This function creates a new (initialized) KEYBOARD_MODEL and returns its address.
*/
{
	KEYBOARD_MODEL *new_keyboard_model;
	
	/* Allocate memory for the new keyboard model */
	new_keyboard_model = (KEYBOARD_MODEL *) malloc( sizeof(KEYBOARD_MODEL) );
	if (new_keyboard_model == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}
	
	new_keyboard_model->model_name[0] = '\0';
	new_keyboard_model->next = NULL;
	
	return (new_keyboard_model);
}


void
kbl_model_add (	KEYBOARD_MANUFACTURE	*manufacture,
		KEYBOARD_MODEL		*new_model    )
/*
input:
	new_model	- The new KEYBOARD_MODEL to add to model_list
output:
	manufacture	- The manufacture where new_model was added to
returns:
	-
description:
	This function adds new_model to manufacture so that all models of manufacture
	are in sequence.
*/
{
	KEYBOARD_MODEL_LIST	*model_list;
	KEYBOARD_MODEL		*model;
	
	model_list = &(manufacture->model_list);
	if (model_list->head == NULL) /* If model_list is empty */
	{
		model_list->head = model_list->tail = new_model;
	}
	  /* If the first model of model_list comes before new_model */
	else if (strcmp(model_list->head->model_name, new_model->model_name) < 0)
	{
		/* Find the model that will be the predecessor of new_model */
		for (	model = model_list->head;
			model->next && strcmp(model->next->model_name, new_model->model_name) < 0;
			model = model->next  )
			 ; /* NULL Statement */
		/* If new_model has no predecessor */
		if (model->next == NULL)
		{
			model_list->tail->next = new_model;
			model_list->tail = new_model;
		}
		else
		{
			new_model->next = model->next;
			model->next = new_model;
		}
	}
	else /* If new_model will be the first model of model_list */
	{
		new_model->next = model_list->head;
		model_list->head = new_model;
	}
}


KEYBOARD_MANUFACTURE
*kbl_manufacture_new (void)
/*
input:
	-
output:
	-
returns:
	The address of the new KEYBOARD_MANUFACTURE.
description:
	This function creates a new (initialized) KEYBOARD_MANUFACTURE and returns its address.
*/
{
	KEYBOARD_MANUFACTURE *new_keyboard_manufacture;
	
	/* Allocate memory for the new keyboard manufacture */
	new_keyboard_manufacture = (KEYBOARD_MANUFACTURE *) malloc( sizeof(KEYBOARD_MANUFACTURE) );
	if (new_keyboard_manufacture == NULL)
	{
		handle_fatal_error (FERR_MALLOC);
	}
	
	new_keyboard_manufacture->manufacture_name[0] = '\0';
	new_keyboard_manufacture->model_list.head = new_keyboard_manufacture->model_list.tail = NULL;
	new_keyboard_manufacture->next = NULL;
	
	return (new_keyboard_manufacture);
}


void
kbl_manufacture_add (	KEYBOARD_LIST		*keyboard_list,
			KEYBOARD_MANUFACTURE	*new_manufacture    )
/*
input:
	new_manufacture	- The new KEYBOARD_MANUFACTURE to add to keyboard_list
output:
	keyboard_list	- The list from which new_manufacture was added to
returns:
	-
description:
	This function adds new_manufacture to keyboard_list so that all manufactures in
	keyboad_list are in sequence.
*/
{
	KEYBOARD_MANUFACTURE *manufacture;
	
	if (keyboard_list->manufacture_list_head == NULL) /* If keyboard_list is empty */
	{
		keyboard_list->manufacture_list_head = keyboard_list->manufacture_list_tail = new_manufacture;
	}
	  /* If the first manufacture of keyboard_list comes before new_manufacture */
	else if (strcmp(keyboard_list->manufacture_list_head->manufacture_name, new_manufacture->manufacture_name) < 0)
	{
		/* Find the manufacture that will be the predecessor of new_manufacture */
		for (	manufacture = keyboard_list->manufacture_list_head;
			manufacture->next && strcmp(manufacture->next->manufacture_name, new_manufacture->manufacture_name) < 0;
			manufacture = manufacture->next  )
			 ; /* NULL Statement */
		/* If new_manufacture has no predecessor */
		if (manufacture->next == NULL)
		{
			keyboard_list->manufacture_list_tail->next = new_manufacture;
			keyboard_list->manufacture_list_tail = new_manufacture;
		}
		else
		{
			new_manufacture->next = manufacture->next;
			manufacture->next = new_manufacture;
		}
		/*keyboard_list->manufacture_list_tail->next = new_manufacture;
		keyboard_list->manufacture_list_tail = new_manufacture;*/
	}
	else /* If new_manufacture will be the first manufacture of keyboard_list */
	{
		new_manufacture->next = keyboard_list->manufacture_list_head;
		keyboard_list->manufacture_list_head = new_manufacture;
	}
}

KEYBOARD_MANUFACTURE
*kbl_manufacture_find (	KEYBOARD_LIST	*keyboard_list,
			char		manufacture_name[MAX_NAME_SIZE+1] )
/*
input:
	keyboard_list		- List containing all keyboards
	manufacture_name	- The name of the manufacture to find
output:
	-
returns:
	The address of the manufacture if it was found, otherwise NULL.
description:
	This function searches for a KEYBOARD_MANUFACTURE, named manufacture_name, in
	keyboard_list.
*/
{
	KEYBOARD_MANUFACTURE *manufacture;
	
	manufacture = keyboard_list->manufacture_list_head;
	while (manufacture && strcmp(manufacture->manufacture_name, manufacture_name) != EQUAL)
	{
		manufacture = manufacture->next;
	}
	
	return (manufacture);
}
