/*-------------------------------------------------------------------------------
Name               : document.c
Author             : Marvin Raaijmakers
Description        : Provides functions for XmlDocument
Date of last change: ?
History            : ?

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
#include <limits.h>

#include <mxml.h>


XmlAttributeName
*XmlGetAttributeName (	char		*attr_name,
			XmlDocument	*document,
			Boolean		create_new  )
/*
Input:
	attr_name	- The name of the XmlAttributeName to get
	document	- The document that contains the type
	create_new	- Indicates whether the XmlAttributeName should be created
			  if it doesn't exist.
Output:
	-
Returns:
	A pointer that points to the XmlAttributeName which name is attr_name. If
	the XmlAttributeName doesn't exist and create_new is FALSE, NULL will be
	returned.
Description:
	This function returns a pointer to the XmlAttributeName, in document, whos
	name is attr_name. If no such XmlAttributeName exists and create_new is TRUE,
	it will be created.
*/
{
	XmlAttributeName	*attr,
				*prev_attr,
				*next_attr;
	
	for (prev_attr = NULL, attr = document->attribute_names.head;
	     attr != NULL && strcmp(attr->name, attr_name) < 0;
	     prev_attr = attr, attr = attr->next)
		; /* NULL Statement */

	if (attr == NULL || strcmp(attr->name, attr_name) > 0)
	{
		if (create_new)
		{
			next_attr = attr;
			attr = XmlMalloc(sizeof(XmlAttributeName));
			attr->name = XmlMalloc(strlen(attr_name)+1);
			strcpy (attr->name, attr_name);
			if (prev_attr == NULL)
			{
				document->attribute_names.head = attr;
			}
			else
			{
				prev_attr->next = attr;
			}
			attr->next = next_attr;
			if (next_attr == NULL)
			{
				document->attribute_names.tail = attr;
			}
		}
		else
		{
			attr = NULL;
		}
	}
	return (attr);
}


XmlElementType
*XmlGetElementType (	char		*type_name,
			XmlDocument	*document,
			Boolean		create_new  )
/*
Input:
	type_name	- The name of the XmlElementType to get
	document	- The document that contains the type
	create_new	- Indicates whether the XmlElementType should be created if
			  it doesn't exist.
Output:
	-
Returns:
	A pointer that points to the XmlElementType which name is type_name. If the
	XmlElementType doesn't exist and create_new is FALSE, NULL will be returned.
Description:
	This function returns a pointer to the XmlElementType, in document, whos
	name is type_name. If no such XmlElementType exists and create_new is TRUE,
	it will be created.
*/
{
	XmlElementType	*type,
			*prev_type,
			*next_type;
	
	for (prev_type = NULL, type = document->element_types.head;
	     type != NULL && strcmp(type->type, type_name) < 0;
	     prev_type = type, type = type->next)
		; /* NULL Statement */

	if (type == NULL || strcmp(type->type, type_name) > 0)
	{
		if (create_new)
		{
			next_type = type;
			type = XmlMalloc(sizeof(XmlElementType));
			type->type = XmlMalloc(strlen(type_name)+1);
			strcpy (type->type, type_name);
			if (prev_type == NULL)
			{
				document->element_types.head = type;
			}
			else
			{
				prev_type->next = type;
			}
			type->next = next_type;
			if (next_type == NULL)
			{
				document->element_types.tail = type;
			}
		}
		else
		{
			type = NULL;
		}
	}
	return (type);
}


void
XmlFreeDocument (XmlDocument *document)
/*
Input:
	document	- A pointer to the XmlDocument to free
Output:
	-
Returns:
	-
Description:
	This function frees the contents of document and document itself.
*/
{
	XmlAttributeName	*attribute_name;
	XmlElementType		*element_type;
	void			*next;
	
	XmlClearContentList (&document->contents);
	/* Free the element types */
	for (element_type = document->element_types.head;
	     element_type != NULL;
	     element_type = next)
	{
		next = element_type->next;
		XmlFree (element_type->type);
		XmlFree (element_type);
	}
	/* Free the attribute names */
	for (attribute_name = document->attribute_names.head;
	     attribute_name != NULL;
	     attribute_name = next)
	{
		next = attribute_name->next;
		XmlFree (attribute_name->name);
		XmlFree (attribute_name);
	}
	XmlFree (document);
}


XmlDocument
*XmlCreateDocument (void)
/*
Input:
	-
Output:
	-
Returns:
	A pointer to a new and initialized XmlDocument.
Description:
	This function creates a new and initialized XmlDocument.
*/
{
	XmlDocument	*document;
	
	/* Create the new document */
	document = XmlMalloc(sizeof(XmlDocument));
	/* Initialize the new document: */
	document->contents.head = document->contents.tail = NULL;
	document->contents.num_elements = 0;
	document->contents.prev_request.index = INT_MAX;
	document->element_types.head = document->element_types.tail = NULL;
	document->attribute_names.head = document->attribute_names.tail = NULL;
	
	return (document);
}
