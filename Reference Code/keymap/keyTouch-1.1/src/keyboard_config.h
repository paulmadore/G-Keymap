/*-------------------------------------------------------------------------------
Name               : keyboard_config.h
Author             : Marvin Raaijmakers
Description        : Headerfile for providing operations for KEYBOARD_CONFIG
Date of last change: 19-November-2004
History            :
                     19-November-2004  Created:
                                       - keyboard_config_model()
                                       - keyboard_config_manufacture()

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

extern void		kbcfg_set_name	(KEYBOARD_CONFIG *keyboard_config, char model [MAX_NAME_SIZE+1],
					char manufacture [MAX_NAME_SIZE+1]);
extern void		kbcfg_init	(KEYBOARD_CONFIG *keyboard_config);
#define kbcfg_model(_kbcfg)			((_kbcfg)->keyboard_name.model)
#define kbcfg_manufacture(_kbcfg)		((_kbcfg)->keyboard_name.manufacture)
#define kbcfg_key_settings_list(_kbcfg)		(&((_kbcfg)->key_settings_list))

extern void 		ksl_add		(KEY_SETTINGS_LIST *key_settings_list, KEY_SETTINGS *new_key_settings);
extern KEY_SETTINGS	*ksl_find_key	(KEY_SETTINGS_LIST *key_settings_list, char *key_name);
extern void		ksl_clear	(KEY_SETTINGS_LIST *key_settings_list);
#define ksl_first_key(_ksl)			((_ksl)->head)

extern KEY_SETTINGS	*ks_new		(void);
#define ks_set_key_name(_ks, _key_name)			( strcpy((_ks)->key_name, (_key_name)) )
#define ks_set_key_sym(_ks, _key_sym)			( strcpy((_ks)->key_sym, (_key_sym)) )
#define ks_set_program_default(_ks, _prog_df)		( strcpy((_ks)->program_default, (_prog_df)) )
#define ks_set_program_user(_ks, _prog_usr)		( strcpy((_ks)->program_user, (_prog_usr)) )
#define ks_get_key_name(_ks)				((_ks)->key_name)
#define ks_get_key_sym(_ks)				((_ks)->key_sym)
#define ks_get_program_default(_ks)			((_ks)->program_default)
#define ks_get_program_user(_ks)			((_ks)->program_user)
