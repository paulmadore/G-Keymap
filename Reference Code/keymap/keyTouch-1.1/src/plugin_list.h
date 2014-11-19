/*-------------------------------------------------------------------------------
Name               : plugin_list.h
Author             : Marvin Raaijmakers
Description        : Headerfile for plugin_list.c
Date of last change: 11-December-2004
History            :
                     11-December-2004 Created this file

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

extern void		plugl_init		(PLUGIN_LIST *plugin_list);
extern PLUGIN		*plugl_plugin_new	(void);
extern void		plugl_plugin_add	(PLUGIN_LIST *plugin_list, PLUGIN *new_plugin);
#define plugl_first_plugin(_plugl)		((_plugl)->head)
#define plugl_is_empty(_plugl)			((_plugl)->head == NULL)
#define plugl_plugin_set_file(_plugin, _file)	( strncpy((_plugin)->file, (_file), MAX_FILE_SIZE) )
#define plugl_plugin_set_name(_plugin, _name)	( strncpy((_plugin)->name, (_name), MAX_PLUGIN_NAME_SIZE) )
#define plugl_plugin_get_file(_plugin)		((_plugin)->file)
#define plugl_plugin_get_name(_plugin)		((_plugin)->name)
