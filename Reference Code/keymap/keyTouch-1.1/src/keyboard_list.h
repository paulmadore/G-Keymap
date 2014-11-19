/*-------------------------------------------------------------------------------
Name               : keyboard_list.h
Author             : Marvin Raaijmakers
Description        : Headerfile for keyboard_list.c
Date of last change: 21-October-2004
History            :
                     21-October-2004 Created:
                                     - add_last_keyboard()

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

extern void			kbl_init		(KEYBOARD_LIST *keyboard_list);
extern KEYBOARD_MODEL		*kbl_model_new		(void);
extern void			kbl_model_add		(KEYBOARD_MANUFACTURE *manufacture, KEYBOARD_MODEL *new_model);
extern KEYBOARD_MANUFACTURE	*kbl_manufacture_new	(void);
extern void			kbl_manufacture_add	(KEYBOARD_LIST *keyboard_list, KEYBOARD_MANUFACTURE *new_manufacture);
extern KEYBOARD_MANUFACTURE	*kbl_manufacture_find	(KEYBOARD_LIST *keyboard_list, char manufacture_name[MAX_NAME_SIZE+1]);
#define kbl_is_empty(_kbl)				((_kbl)->manufacture_list_head == NULL)
#define kbl_manufacture_set_name(_manufacture, _name)	( strcpy((_manufacture)->manufacture_name, (_name)) )
#define kbl_manufacture_get_name(_manufacture)		((_manufacture)->manufacture_name)
#define kbl_model_set_name(_model, _name)		( strcpy((_model)->model_name, (_name)) )
#define kbl_model_get_name(_model)			((_model)->model_name)
#define kbl_last_manufacture(_kbl)			((_kbl)->manufacture_list_tail)
#define kbl_first_manufacture(_kbl)			((_kbl)->manufacture_list_head)
#define kbl_first_model(_manufacture)			((_manufacture)->model_list.head)
