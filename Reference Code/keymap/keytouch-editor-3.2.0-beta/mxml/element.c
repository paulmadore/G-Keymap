/*-------------------------------------------------------------------------------
Name               : element.c
Author             : Marvin Raaijmakers
Description        : Provides functions for XML elements
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

#include <mxml.h>


static void append_element_string (	XmlContent	*element,
					char		*string,
					char		*seperator );
static int element_string_size (	XmlContent	*element,
					int		seperator_size  );



void
XmlAttributeListAdd (	XmlAttributeList	*attribute_list,
			XmlAttribute		*attribute       )
/*
Input:
	attribute	- The attribute to add to attribute_list
Output:
	attribute_list	- The attribute list where attribute was added to.
Returns:
	-
Description:
	This function adds attribute to attribute_list.
*/
{
	if (attribute_list != NULL && attribute != NULL)
	{
		attribute->next = NULL;
		if (attribute_list->head == NULL)
		{
			attribute_list->head = attribute_list->tail = attribute;
		}
		else
		{
			attribute_list->tail->next = attribute;
			attribute_list->tail = attribute;
		}
	}
}


void
XmlSetAttributeValue (	XmlAttributeName	*attribute_name,
			char			*value,
			XmlContent		*element         )
/*
Input:
	
	attribute_name	- The name of attribute to set the value to.
	value		- The value to set to the attribute
Output:
	element		- The element to where the attribute was set to.
Returns:
	-
Description:
	This function sets the value 'value' to the attribute, named attribute_name,
	in element.
*/
{
	XmlAttribute *attribute;
	
	/* Search for the attribute that may already exist in element */
	for (attribute = element->data.element.attributes->head;
	     attribute != NULL && attribute->name != attribute_name;
	     attribute = attribute->next)
		; /* NULL Statement */
	/* If the attribute already exists */
	if (attribute != NULL && attribute->value)
	{
		XmlFree (attribute->value);
	}
	else if (attribute == NULL)
	{
		attribute = XmlMalloc(sizeof(XmlAttribute));
		attribute->name = attribute_name;
		/* Add the attribute to the element */
		XmlAttributeListAdd (element->data.element.attributes, attribute);
	}
	attribute->value = XmlMalloc(strlen(value)+1);
	strcpy (attribute->value, value);
}


void
XmlClearAttributeList (XmlAttributeList *attribute_list)
/*
Input:
	-
Output:
	attribute_list	- The cleared XmlAttributeList.
Returns:
	-
Description:
	This function frees all attributes in attribute_list.
*/
{
	XmlAttribute	*attribute,
			*tmp;
	
	if (attribute_list != NULL)
	{
		attribute = attribute_list->head;
		while (attribute != NULL)
		{
			tmp = attribute;
			attribute = attribute->next;
			if (tmp->value != NULL)
			{
				XmlFree (tmp->value);
			}
			XmlFree (tmp);
		}
	}
	attribute_list->head = attribute_list->tail = NULL;
}


char
*XmlGetAttributeValue (	XmlAttributeName	*attribute_name,
			XmlContent		*content         )
/*
Input:
	content		- The element to get the attribute from.
	attribute_name	- The name of attribute to get the value from.
Output:
	-
Returns:
	The a pointer to a string which is the value of the attribute, named
	attribute_name, of the element content. If no such attribute exists
	NULL is returned.
Description:
	This function returns a pointer to a string which is the value of the
	attribute, named attribute_name, of the element content.
*/
{
	XmlAttribute *attribute;
	
	if (content->type != XmlCType_Element)
	{
		XmlError ("Tried to get an attribute value from content that is not an element.");
	}
	/* Find the matching attribute */
	for (attribute = content->data.element.attributes->head;
	     attribute != NULL && attribute->name != attribute_name;
	     attribute = attribute->next)
		; /* NULL Statement */
	if (attribute == NULL)
	{
		return (NULL);
	}
	return (attribute->value);
}


int
element_string_size (	XmlContent	*element,
			int		seperator_size  )
{
	XmlContent *content;
	int size;
	
	size = 0;	
	for (content = element->data.element.contents->head;
	     content != NULL;
	     content = content->next)
	{
		if (content->type == XmlCType_Element)
		{
			size += element_string_size(content, seperator_size);
		}
		else if (content->type == XmlCType_String)
		{
			size += strlen(content->data.string)+seperator_size;
		}
	}
	return (size);
}


void
append_element_string (	XmlContent	*element,
			char		*string,
			char		*seperator )
{
	XmlContent *content;
	
	for (content = element->data.element.contents->head;
	     content != NULL;
	     content = content->next)
	{
		if (content->type == XmlCType_Element)
		{
			append_element_string (content, string, seperator);
		}
		else if (content->type == XmlCType_String)
		{
			strcat (string, content->data.string);
			strcat (string, seperator);
		}
	}
}


char
*XmlGetElementString (	XmlContent	*content,
			char		*seperator )
/*
Input:
	content		- The element to get the string from.
	seperator	- Strings of different tags will be seperated by this
			  string.
Output:
	-
Returns:
	The a pointer to the string the element 'content' contains.
Description:
	This function returns a pointer to a string the element 'content' contains.
	Strings of different tags will be seperated by seperator. If the string is
	no longer needed it should be free with XmlFree().
*/
{
	int string_size, seperator_size;
	char *string;
	
	if (content == NULL || seperator == NULL)
	{
		return (NULL);
	}
	if (content->type != XmlCType_Element)
	{
		XmlError ("XmlGetElementString(): content is not XmlCType_Element.");
		return (NULL);
	}
	
	seperator_size = strlen(seperator);
	/* element_string_size() returns seperator_size too much */
	string_size = element_string_size(content, seperator_size);
	/* We use the last seperator_size bytes as an anti-overflow buffer */
	string = XmlMalloc(string_size+1);
	string[0] = '\0';
	append_element_string (content, string, seperator);
	/* The anti-overflow buffer is now filled with seperator */
	/* Remove the last seperator from the string */
	string[string_size-seperator_size] = '\0';
	/* Resize the allocated memory */
	string = XmlRealloc(string, string_size+1-seperator_size);
	
	return (string);
}


XmlContent
*XmlCreateElement (	XmlElementType	*type,
			Boolean		is_empty  )
/*
Input:
	type		- The type of the XmlContent to create
	is_empty	- Identifies if the element to create will be an empty element.
Output:
	-
Returns:
	A pointer to a new and initialized XmlContent which is an element.
Description:
	This function returns a pointer to a new and initialized element of type
	'type'.
*/
{
	XmlContent *new_element;
	
	new_element = XmlCreateContent(XmlCType_Element);
	new_element->data.element.type = type;
	new_element->data.element.is_empty = is_empty;
	return (new_element);
}
