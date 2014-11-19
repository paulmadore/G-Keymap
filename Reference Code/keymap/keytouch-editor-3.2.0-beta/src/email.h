/*---------------------------------------------------------------------------------
Name               : email.h
Author             : Marvin Raaijmakers
Description        : Header file for email.c
Date of last change: 15-Mar-2006
History            :

    Copyright (C) 2006 Marvin Raaijmakers

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
#ifndef _EMAIL_H
#define _EMAIL_H

#ifndef TRUE
typedef int Boolean;
#	define TRUE	1
#	define FALSE	0
#endif

#define SMTP_TIMEOUT	4

#define SMTP_PORT	25

#define SMTP_REPLY_GREETING		220
#define SMTP_REPLY_GOODBYE		221
#define SMTP_REPLY_OK			250
#define SMTP_REPLY_START_MAIL_INPUT	354

extern Boolean email_file (	char		*to,
				char		*from,
				char		*subject,
				char		*filename,
				const char	*smtp_hostnames[] );

#endif
