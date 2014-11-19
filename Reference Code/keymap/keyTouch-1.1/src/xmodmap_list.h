/*-------------------------------------------------------------------------------
Name               : xmodmap_list.h
Author             : Marvin Raaijmakers
Description        : Headerfile for providing operations for XMODMAP_LIST
Date of last change: 5-December-2004
History            :
                     5-December-2004   Created:
                                       - xmm_set_hardware_code()
                                       - xmm_set_key_sym()
                     6-December-2004   Created:
                                       - xmm_get_hardware_code()
                                       - xmm_get_key_sym()
                                       - xmml_is_empty()
                                       - xmml_first_xmodmap()

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

extern void	xmml_clear	(XMODMAP_LIST *xmodmap_list);
extern void	xmml_add	(XMODMAP_LIST *xmodmap_list, XMODMAP *new_xmodmap);
#define xmml_is_empty(_xmml)		((_xmml)->head == NULL)
#define xmml_first_xmodmap(_xmml)	((_xmml)->head)

extern XMODMAP	*xmm_new	(void);
#define xmm_set_hardware_code(_xmm, _hardware_code)	((_xmm)->hardware_code = (_hardware_code))
#define xmm_set_key_sym(_xmm, _key_sym)			( strcpy((_xmm)->key_sym, (_key_sym)) )
#define xmm_get_hardware_code(_xmm)			((_xmm)->hardware_code)
#define xmm_get_key_sym(_xmm)				((_xmm)->key_sym)
