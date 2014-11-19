/*-------------------------------------------------------------------------------
Name               : write.c
Author             : Marvin Raaijmakers
Description        : Provides functions for writing an XML document
Date of last change: 24-Jun-2006
History            : 24-Jun-2006 Replaced every call to XmlError() by a call to
                                 XmlSetErrorMsg()

    Copyright (C) 2005-2006 Marvin Raaijmakers

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

    You can contact me at: marvinr(at)users(dot)sf(dot)net
    (replace (at) by @ and (dot) by .)
---------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>

#include <mxml.h>

static char *get_special_char (char c);
static void indent (	FILE	*file,
			int	level    );
static Boolean XmlWriteContentList (	FILE		*file,
					XmlContentList	*content_list,
					XmlDocument	*document       );
static Boolean XmlWriteCDATA (		FILE		*file,
					XmlContent	*content,
					XmlDocument	*document       );
static Boolean XmlWriteComment (	FILE		*file,
					XmlContent	*content,
					XmlDocument	*document       );
static Boolean XmlWriteElement (	FILE		*file,
					XmlContent	*content,
					XmlDocument	*document       );
static Boolean XmlWriteUnknown (	FILE		*file,
					XmlContent	*content,
					XmlDocument	*document       );


static int level;	/* Indicates the depht of the recursion
			 * tree of XmlWriteElement() */


char
*get_special_char (char c)
/*
Input:
	c	- The character to check
Output:
	-
Returns:
	The string that replaces the special character. If c is not a special
	character NULL is returned.
Description:
	This function returns the string that replaces c.
*/
{
	int count;
	static struct {
		char	*string,  /* Every string should end with ';' (to speedup a little)*/
			character;
	} spec_char[NUM_SPEC_CHARS] =	{{"lt;",   '<'},
					 {"gt;",   '>'},
					 {"amp;",  '&'},
					 {"apos;", '\''},
					 {"quot;", '"'}};
	
	for (count = 0; count < NUM_SPEC_CHARS; count++)
	{
		if (c == spec_char[count].character)
		{
			return (spec_char[count].string);
		}
	}
	return (NULL);
}


void
indent (	FILE	*file,
		int	level    )
/*
Input:
	level	- The indent level
Output:
	file	- The spaces where written to this file
Returns:
	-
Description:
	This function writes INDENT_SIZE*level number of spaces to file.
*/
{
	level *= INDENT_SIZE;
	while (level)
	{
		putc (' ', file);
		level--;
	}
}

Boolean
XmlWriteCDATA (	FILE		*file,
		XmlContent	*content,
		XmlDocument	*document       )
/*
Input:
	content		- The CDATA to write to file
	document	- The document to where content appears in
Output:
	file		- The CDATA was written to this file
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes the contents of CDATA to file.
*/
{
	fprintf (file, "<![CDATA[%s]]>", content->data.string);
	return (TRUE);
}


Boolean
XmlWriteComment (	FILE		*file,
			XmlContent	*content,
			XmlDocument	*document )
/*
Input:
	content		- The comment to write to file
	document	- The document to where content appears in
Output:
	file		- The comment was written to this file
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes the contents of comment to file.
*/
{
	fprintf (file, "<!--%s-->", content->data.string);
	return (TRUE);
}


Boolean
XmlWriteString (	FILE		*file,
			XmlContent	*content,
			XmlDocument	*document )
/*
Input:
	content		- The string to write to file
	document	- The document to where content appears in
Output:
	file		- The string was written to this file
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes the contents of string to file.
*/
{
	int	count,
		string_len;
	char	*alias;
	
	string_len = strlen(content->data.string);
	for (count = 0; count < string_len; count++)
	{
		alias = get_special_char(content->data.string[count]);
		/* If we do not have to replace the character by an alias */
		if (alias == NULL)
		{
			putc (content->data.string[count], file);
		}
		else
		{
			/* Replace the character by an alias */
			fprintf (file, "&%s", alias);
		}
		if (content->data.string[count] == '\n' && count+1 < string_len)
		{
			indent (file, level);
		}
	}
	
	return (TRUE);
}


Boolean
XmlWriteUnknown (	FILE		*file,
			XmlContent	*content,
			XmlDocument	*document )
/*
Input:
	-
Output:
	-
Returns:
	TRUE
Description:
	-
*/
{
	return (TRUE);
}


Boolean
XmlWriteElement (	FILE		*file,
			XmlContent	*content,
			XmlDocument	*document )
/*
Input:
	content		- The element to write to file
	document	- The document to where content appears in
Output:
	file		- The element was written to this file
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes the contents of element to file.
*/
{
	XmlAttribute		*attribute;
	XmlContentElement	*element;
	char			attrval_surr;
	
	element = &(content->data.element);
	fprintf (file, "<%s", element->type->type);
	for (attribute = element->attributes->head;
	     attribute != NULL;
	     attribute = attribute->next)
	{
		/* Print the name of the attribute */
		fprintf (file, " %s=", attribute->name->name);
		/*** Decide what character will surround the value of the attribute : ***/
		if (strchr(attribute->value, '"'))
		{
			if (strchr(attribute->value, '\''))
			{
				XmlSetErrorMsg ("Attribute value contains both \" and ' "
				                "character, which is not allowed.");
				return (FALSE);
			}
			attrval_surr = '\'';
		}
		else
		{
			attrval_surr = '"';
		}
		/* Print the value of the attribute */
		fprintf (file, "%c%s%c", attrval_surr, attribute->value, attrval_surr);
	}
	if (element->is_empty)
	{
		fputs (" />", file);
	}
	else
	{
		putc ('>', file);
		level++;
		/* If the element contains only 1 XmlContent which is a string */
		if (element->contents->num_elements == 1 &&
			element->contents->head->type == XmlCType_String)
		{
			XmlWriteString (file, element->contents->head, document);
		}
		else
		{
			putc ('\n', file);
			if (!XmlWriteContentList(file, element->contents, document))
			{
				return (FALSE);
			}
			indent (file, level-1);
		}
		level--;
		fprintf (file, "</%s>", element->type->type);
	}
	return (TRUE);
}


Boolean
XmlWriteContentList (	FILE		*file,
			XmlContentList	*content_list,
			XmlDocument	*document       )
/*
Input:
	content_list	- The content list to write to file
	document	- The document to where content_list appears in
Output:
	file		- The contents where written to this file
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes the contents of content_list to file.
*/
{
	XmlContent	*content;
	Boolean (*write_content[XmlNumCType])(FILE *, XmlContent *, XmlDocument *) = {
		[XmlCType_CDATA] = XmlWriteCDATA,
		[XmlCType_Comment] = XmlWriteComment,
		[XmlCType_Element] = XmlWriteElement,
		[XmlCType_String] = XmlWriteString,
		[XmlCType_Unknown] = XmlWriteUnknown
	};
	
	for (content = content_list->head; content != NULL; content = content->next)
	{
		indent (file, level);
		/* If writing the content failed */
		if (!(*write_content[content->type])(file, content, document))
		{
			return (FALSE);
		}
		putc ('\n', file);
	}
	return (TRUE);
}


Boolean
XmlWriteDocument (	XmlDocument	*document,
			char		*file_name   )
/*
Input:
	document	- The document to write
	file_name	- The name of the file to write to
Output:
	-
Returns:
	TRUE on succes, otherwise FALSE.
Description:
	This function writes document to the file named file_name. If it fails to
	write the file an error message describing why it failed can be retrieved by
	calling XmlGetErrorMsg().
*/
{
	FILE	*file;
	Boolean	return_val;
	
	file = fopen(file_name, "w");
	if (file == NULL)
	{
		XmlSetErrorMsg ("Could not open the file for writing.");
		return (FALSE);
	}
	level = 0;
	return_val = XmlWriteContentList (file, &(document->contents), document);
	fclose (file);
	return (return_val);
}
