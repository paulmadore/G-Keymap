/*-------------------------------------------------------------------------------
Name               : content.c
Author             : Marvin Raaijmakers
Description        : Provides functions for managing content lists and setting
                     data to XmlContent of type XmlCType_String,
                     XmlCType_Comment and XmlCType_CDATA
Date of last change: 20-Dec-2005
History            :
                     20-Dec-2005  Added code to free the attribute list of an
                                  element in XmlClearContent()

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
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include <mxml.h>


static void XmlClearContent (XmlContent *content);


XmlContentList
*XmlCreateContentList (void)
/*
Input:
	-
Output:
	-
Returns:
	A pointer to a new and initialized XmlContentList.
Description:
	This function creates a new and initialized XmlContentList.
*/
{
	XmlContentList	*content_list;
	
	/* Create the new XmlContentList */
	content_list = XmlMalloc(sizeof(XmlContentList));
	/* Initialize the new XmlContentListt: */
	content_list->head = content_list->tail = NULL;
	content_list->num_elements = 0;
	content_list->prev_request.index = INT_MAX;
	
	return (content_list);
}


void
XmlContentAppend (	XmlContent	*content,
			XmlContentList	*content_list  )
/*
Input:
	content		- The content to append to content_list
Output:
	content_list	- Where the content was added to
Returns:
	-
Description:
	This function appends content to content_list. If content->type is equal to
	XmlCType_Unknown the memory pointed to by content will be freed.
*/
{
	if (content == NULL)
	{
		XmlError ("XmlContentAppend(): The value of parameter 'content' is equal to NULL.");
		return;
	}
	if (content->type == XmlCType_Unknown)
	{
		XmlFree (content);
		return;
	}
	
	content->next = NULL;
	/* If content_list is empty */
	if (content_list->head == NULL)
	{
		content_list->head = content;
	}
	else
	{
		content_list->tail->next = content;
	}
	content_list->tail = content;
	content_list->num_elements++;
}


void
XmlClearContent (XmlContent *content)
/*
Input:
	content	- The content to clear
Output:
	content	- The cleared content
Returns:
	-
Description:
	This function frees the contents of 'content'.
*/
{
	switch (content->type)
	{
		case XmlCType_CDATA:
			XmlFree (content->data.string);
			break;
		case XmlCType_Comment:
			XmlFree (content->data.string);
			break;
		case XmlCType_Element:
			if (!content->data.element.is_empty)
			{
				XmlClearContentList (content->data.element.contents);
				XmlFree (content->data.element.contents);
			}
			XmlClearAttributeList (content->data.element.attributes);
			XmlFree (content->data.element.attributes);
			break;
		case XmlCType_String:
			XmlFree (content->data.string);
			break;
	}
}


void
XmlClearContentList (XmlContentList *content_list)
/*
Input:
	content_list	- A pointer to the XmlContentList to clear
Output:
	content_list	- A pointer to the cleared XmlContentList
Returns:
	-
Description:
	This function clears content_list.
*/
{
	XmlContent *content, *next;
	
	for (content = content_list->head; content != NULL; content = next)
	{
		next = content->next;
		XmlClearContent (content);
		XmlFree (content);
	}
}


void
XmlContentRemove (	XmlContent	*content,
			XmlContentList	*content_list  )
/*
Input:
	content		- The content to remove from content_list
Output:
	content_list	- Where the content was removed from
Returns:
	-
Description:
	This function removes the XmlContent content from content_list. content MUST
	appear in content_list. This function does not check if content appears in
	content_list, so the code that uses this function is reasponsible for that.
*/
{
	XmlContent	*prev_content,
			*next_content;
	
	if (content == NULL)
	{
		XmlError ("XmlContentRemove(): The value of parameter 'content' is equal to NULL.");
		return;
	}
	
	XmlClearContent (content);
	
	/* If content is the last element of content_list */
	if (content->next == NULL)
	{
		/* If content is the only element of content_list */
		if (content_list->head == content)
		{
			content_list->head = NULL;
		}
		else
		{
			/* Bad luck, we have to search for the XmlContent that
			 * appears before content.
			 */
			for (prev_content = content_list->head;
			     prev_content != NULL && prev_content->next != content;
			     prev_content = prev_content->next)
				; /* NULL Statement */
			if (prev_content == NULL)
			{
				XmlError ("XmlContentRemove(): content does not appear in content_list.");
				exit (EXIT_FAILURE);
			}
			content_list->tail = prev_content;
			prev_content->next = NULL;
		}
		XmlFree (content);
	}
	else
	{
		next_content = content->next;
		*content = *next_content;
		XmlFree (next_content);
		if (content->next == NULL)
		{
			content_list->tail = content;
		}
	}
	content_list->prev_request.index = INT_MAX;
	content_list->num_elements--;
}


XmlContent
*XmlGetContent (	XmlContentList	*content_list,
			int		index		)
/*
Input:
	content_list	- The list to get the XmlContent from
	index		- The index of the XmlContent in content_list
Output:
	-
Returns:
	A pointer to the requested XmlContent. If index is larger or equal than the
	number of elements in content_list, NULL will be returned.
Description:
	This function returns the index'th element of content_list.
*/
{
	XmlContent	*content;
	int		count;
	
	if (content_list == NULL || index >= content_list->num_elements)
	{
		return (NULL);
	}
	else if (index < 0)
	{
		XmlError ("Cannot get an element with index < 0 from a XmlContentList.");
		return NULL;
	}
	if (index >= content_list->prev_request.index)
	{
		/* Yeah, we are saving time ;) */
		content = content_list->prev_request.content;
		count = content_list->prev_request.index;
	}
	else
	{
		content = content_list->head;
		count = 0;
	}
	/* Get the requested XmlContent */
	for (/* count is already set */; count < index; count++)
	{
		content = content->next;
	}
	if (index < content_list->num_elements-1)
	{
		/* Save information about the current request */
		content_list->prev_request.index = index;
		content_list->prev_request.content = content;
	}
	return content;
}


XmlContent
*XmlGetContentOfType (	XmlContentList	*content_list,
			XmlContentType	type,
			int		index		)
/*
Input:
	content_list	- The list to get the XmlContent from
	type		- The type of the XmlContent to get
	index		- The index of the XmlContent of type 'type' in content_list
Output:
	-
Returns:
	A pointer to the requested XmlContent. If index is larger or equal than the
	number of elements, of type 'type', in content_list, NULL will be returned.
Description:
	This function returns the index'th element of content_list which type is
	'type'.
*/
{
	XmlContent	*content;
	int		count;
	
	if (content_list == NULL || index >= content_list->num_elements)
	{
		return (NULL);
	}
	else if (index < 0)
	{
		XmlError ("Cannot get an element with index < 0 from a XmlContentList.");
		return NULL;
	}
	content = content_list->head;
	count = -1;
	/* Get the requested XmlContent */
	while (count < index && content != NULL)
	{
		if (content->type == type)
		{
			count++;
		}
		if (count < index)
		{
			content = content->next;
		}
	}
	return content;
}


XmlContent
*XmlGetElementOfType (	XmlContentList	*content_list,
			XmlElementType	*type,
			int		index		)
/*
Input:
	content_list	- The list to get the XmlContent from
	type		- The type of the XmlElement to get
	index		- The index of the XmlContent, which contains a XmlElement
			  of type 'type', in content_list.
Output:
	-
Returns:
	A pointer to the requested XmlContent. If index is larger or equal than the
	number of elements, of type 'type', in content_list, NULL will be returned.
Description:
	This function returns the index'th XmlContent, which contains a XmlElement
	of type 'type', of content_list.
*/
{
	XmlContent	*content;
	int		count;
	
	if (content_list == NULL || index >= content_list->num_elements)
	{
		return (NULL);
	}
	else if (index < 0)
	{
		XmlError ("Cannot get an element with index < 0 from a XmlContentList.");
		return NULL;
	}
	content = content_list->head;
	count = -1;
	/* Get the requested XmlContent */
	while (count < index && content != NULL)
	{
		if (content->type == XmlCType_Element && content->data.element.type == type)
		{
			count++;
		}
		if (count < index)
		{
			content = content->next;
		}
	}
	return content;
}


XmlContent
*XmlCreateContent (XmlContentType type)
/*
Input:
	type	- The type of the XmlContent to create
Output:
	-
Returns:
	A pointer to a new and initialized XmlContent.
Description:
	This function returns a pointer to a new and initialized XmlContent of type
	'type'.
*/
{
	XmlContent *new_content;
	
	new_content = XmlMalloc(sizeof(XmlContent));
	new_content->type = type;
	if (type == XmlCType_Element)
	{
		new_content->data.element.type = NULL;
		new_content->data.element.type = NULL;
		new_content->data.element.attributes = XmlMalloc(sizeof(XmlAttributeList));
		new_content->data.element.attributes->head =
		  new_content->data.element.attributes->tail = NULL;
		new_content->data.element.contents = XmlCreateContentList();
	}
	else if (type == XmlCType_CDATA ||
	         type == XmlCType_Comment ||
	         type == XmlCType_String)
	{
		new_content->data.string = NULL;
	}
	return (new_content);
}


void
XmlStringSetString (	XmlContent	*string,
			char		*value   )
/*
Input:
	value	- The value to set to the string
Output:
	string	- The XmlContent that is the string where the value was set to
Returns:
	-
Description:
	This function sets value to string.
*/
{
	/* If the string contains data */
	if (string->data.string != NULL)
	{
		XmlFree (string->data.string);
	}
	string->data.string = XmlMalloc(strlen(value)+1);
	strcpy (string->data.string, value);
}

void
XmlCommentSetString (	XmlContent	*comment,
			char		*value   )
/*
Input:
	value	- The value to set to the comment
Output:
	comment	- The XmlContent that is the comment where the value was set to
Returns:
	-
Description:
	This function sets value to comment.
*/
{
	XmlStringSetString (comment, value);
}

void
XmlCDATASetString (	XmlContent	*cdata,
			char		*value   )
/*
Input:
	value	- The value to set to the cdata
Output:
	comment	- The XmlContent that is the cdata where the value was set to
Returns:
	-
Description:
	This function sets value to cdata.
*/
{
	XmlStringSetString (cdata, value);
}
