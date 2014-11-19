/*-------------------------------------------------------------------------------
Name               : parse.c
Author             : Marvin Raaijmakers
Description        : Provides functions for parsing an XML document
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
#include <ctype.h>

#include <mxml.h>


static char read_next_char (FILE *file);
static Boolean file_string_appear (FILE *file, char *string);
static char *read_string (FILE *file, Boolean (*stop)(char, FILE *));
static XmlContent *XmlParseString (FILE *file);
static XmlContent *XmlParseTag (FILE *file, XmlDocument *document);
static XmlContentList *XmlParseContents (FILE *file, XmlDocument *document, char *tag_name);
static char read_special_character (FILE *file);


static char attrval_surr; /* To store the character that surrounds the attribute
                           * value (needed by end_attribute_value()).
                           */
static struct {
	char	*string,  /* Every string should end with ';' (to speedup a little)*/
		character;
} spec_char[NUM_SPEC_CHARS] =	{{"lt;",   '<'},
				 {"gt;",   '>'},
				 {"amp;",  '&'},
				 {"apos;", '\''},
				 {"quot;", '"'}};

char
read_next_char (FILE *file)
/*
Input:
	file	- The file to read from.
Output:
	-
Returns:
	The first non-space character in file (this might be an EOF).
Description:
	This function returns the first non-space character in file (this might be
	an EOF).
*/
{
	char c;
	
	do {
		c = getc(file);
	} while (isspace(c));
	
	return (c);
}


Boolean
file_string_appear (	FILE	*file,
			char	*string )
/*
Input:
	file	- The file to read the string from.
	string	- The string to check.
Output:
	-
Returns:
	TRUE if string appears at the current position in file, otherwise FALSE.
Description:
	This function checks if string appears at the current position in file.
	If string appears in file, the file position indicator will be set after
	the occurrence of string, otherwise it will have the same value as before
	calling this function.
*/
{
	fpos_t	start_pos;
	
	fgetpos (file, &start_pos);
	while (*string != '\0')
	{
		if (*string != getc(file))
		{
			/* Restore file position indicator */
			fsetpos (file, &start_pos);
			return (FALSE);
		}
		string++;
	}
	return (TRUE);
}


Boolean
end_string (	char	*string,
		char	c,
		FILE	*file )
{
	return ((string[0] == c) && file_string_appear(file, &(string[1])));
}

Boolean
end_CDATA (	char	c,
		FILE	*file )
{
	return (end_string ("]]", c, file));
}

Boolean
end_comment (	char	c,
		FILE	*file )
{
	return (end_string ("-->", c, file));
}

Boolean
end_element_type (	char	c,
			FILE	*file )
{
	if (c == '>' || c == '/' || isspace(c))
	{
		fseek (file, -1, SEEK_CUR);
		return (TRUE);
	}
	return (FALSE);
}

Boolean
end_attribute_name (	char	c,
			FILE	*file )
{
	if (c == '>' || c == '/' || c == '=' || isspace(c))
	{
		fseek (file, -1, SEEK_CUR);
		return (TRUE);
	}
	return (FALSE);
}

Boolean
end_attribute_value (	char	c,
			FILE	*file )
{
	if (c == attrval_surr || c == '<' || c == '>')
	{
		fseek (file, -1, SEEK_CUR);
		return (TRUE);
	}
	return (FALSE);
}


char
*read_string (	FILE	*file,
		Boolean	(*stop)(char, FILE *) )
/*
Input:
	file	- The file to read the string from.
	stop	- function that indicates if it is time to stop reading
Output:
	-
Returns:
	A pointer to the string read from file.
Description:
	This function reads a string from file. It stops reading AFTER an EOF or the
	function stop returns TRUE. The EOF will not be included with the returned
	string. Note that when the function stop returns FALSE, it must restore the
	file position indicator. When stop returns TRUE and when it reads from file
	the file position indicator may not be restored.
*/
{
	fpos_t	string_pos,
		end_pos;
	int	string_size,
		count;
	char	*string,
		c;
	
	fgetpos (file, &string_pos);
	string_size = 0;
	/* Get the size of the string */
	do {
		c = getc (file);
		string_size++;
	} while (c != EOF && !(*stop)(c, file));
	/* Because, when counting string_size, we increased string_size one time
	 * too much, string_size is now equal to the size of the string plus the
	 * '\0' character.
	 */
	string = XmlMalloc(string_size);
	
	fgetpos (file, &end_pos);
	fsetpos (file, &string_pos);
	string_size--;
	/* Read the string */
	for (count = 0; count < string_size; count++)
	{
		string[count] = getc(file);
	}
	string[count] = '\0';
	fsetpos (file, &end_pos);
	return (string);
}


char
read_special_character (FILE *file)
/*
Input:
	file		- The file to read from
Output:
	-
Returns:
	If a special character was read it will be returned otherwise '\0' is
	returned.
Description:
	This function reads a special character, if there is one, from file and
	returns it. If no special character was read, the file position indicator
	of file will be restored. An special character begins with the '&' character
	which should be read before calling this function.
*/
{
	int count;
	
	for (count = 0; count < NUM_SPEC_CHARS; count++)
	{
		if (file_string_appear(file, spec_char[count].string))
		{
			return (spec_char[count].character);
		}
	}
	return ('\0');
}


XmlContent
*XmlParseString (FILE *file)
/*
Input:
	file		- The file to read from
Output:
	-
Returns:
	When no errors occured a pointer to a new XmlContent containing the readenc
	string. Otherwise NULL is returned.
Description:
	This function reads a string from file.
*/
{
	fpos_t		string_pos;
	int		string_size,
			count;
	XmlContent	*content;
	char		c, prev_c;
	
	fgetpos (file, &string_pos);
	string_size = 0;
	prev_c = ' ';
	/* Get the size of the string */
	do {
		c = getc (file);
		if (c == '>')
		{
			XmlSetErrorMsg ("'>' appearing without '<'.");
			return (NULL);
		}
		if (c == '&')
		{
			read_special_character(file);
		}
		/* Do not read double spaces */
		if (!(isspace(prev_c) && isspace(c)))
		{
			string_size++;
		}
		prev_c = c;
	} while (c != EOF && c != '<');
	
	content = XmlMalloc(sizeof(XmlContent));
	content->type = XmlCType_String;
	/* Because, when counting string_size, we increased string_size one time
	 * too much, string_size is now equal to the size of the string plus the
	 * '\0' character.
	 */
	content->data.string = XmlMalloc(string_size);
	
	fsetpos (file, &string_pos);
	string_size--;
	prev_c = ' ';
	count = 0;
	/* Read the string */
	while (count < string_size)
	{
		c = getc(file);
		if (c == '&')
		{
			c = read_special_character(file);
			if (c == '\0')
			{
				c = '&';
			}
		}
		if (isspace(c))
		{
			c = ' ';
		}
		/* Do not read double spaces */
		if (!(isspace(prev_c) && c == ' '))
		{
			content->data.string[count] = c;
			count++;
		}
		prev_c = c;
	}
	content->data.string[count] = '\0';
	return (content);
}


XmlAttributeList
*XmlParseAttributes (	FILE		*file,
			XmlDocument	*document  )
/*
Input:
	file		- The file to read from.
Output:
	document	- If the new content contains new attribute names they are
			  added to their list.
Returns:
	When no errors occured a pointer to a new XmlAttributeList. Otherwise NULL
	is returned.
Description:
	This function reads all attributes and their values of the current tag from
	file.
*/
{
	XmlAttributeList	*attributes;
	XmlAttribute		*attribute;
	char			*name, c;
	Boolean			finished;
	
	attributes = XmlMalloc(sizeof(XmlAttributeList));
	attributes->head = attributes->tail = NULL;
	finished = FALSE;
	while (!finished)
	{
		attribute = XmlMalloc(sizeof(XmlAttribute));
		c = read_next_char(file);
		if (c == EOF)
		{
			XmlSetErrorMsg ("Tag is not closed.");
			XmlClearAttributeList (attributes);
			XmlFree (attributes);
			XmlFree (attribute);
			return (NULL);
		}
		fseek (file, -1, SEEK_CUR);
		name = read_string(file, end_attribute_name);
		if (name[0] == '\0')
		{
			finished = TRUE;
			XmlFree (attribute);
		}
		else if (read_next_char(file) != '=' || feof(file))
		{
			XmlSetErrorMsg ("Tag contains attribute without a value.");
			XmlClearAttributeList (attributes);
			XmlFree (attributes);
			XmlFree (attribute);
			XmlFree (name);
			return (NULL);
		}
		else if ((attrval_surr = read_next_char(file)) != '\'' && attrval_surr != '"')
		{
			XmlSetErrorMsg ("Attribute value is not surrounded by \"'\" or '\"'.");
			XmlClearAttributeList (attributes);
			XmlFree (attributes);
			XmlFree (attribute);
			XmlFree (name);
			return (NULL);
		}
		else
		{
			attribute->value = read_string(file, end_attribute_value);
			if (read_next_char(file) != attrval_surr)
			{
				XmlSetErrorMsg ("Attribute value is not surrounded by \"'\" or '\"'.");
				XmlClearAttributeList (attributes);
				XmlFree (attributes);
				XmlFree (attribute->value);
				XmlFree (attribute);
				XmlFree (name);
				return (NULL);
			}
			attribute->name = XmlGetAttributeName(name, document, TRUE);
			XmlAttributeListAdd (attributes, attribute);
		}
		XmlFree (name);
	}
	return (attributes);
}


XmlContent
*XmlParseElement (	FILE		*file,
			XmlDocument	*document  )
/*
Input:
	file		- The file to read from.
Output:
	document	- If the new content contains new attribute names or element
			  types, they are added to their list.
Returns:
	When no errors occurred a pointer to a new XmlContent containing the readen
	element. Otherwise NULL is returned.
Description:
	This function reads an element from file. If the tag is the start-tag of a
	non-empty element, the contents and the end-tag of this element will be read.
	The '<' of the tag should already been read from file before calling this
	function.
*/
{
	XmlContent	*content;
	char		*tag_name,
			c;
	
	content = XmlMalloc(sizeof(XmlContent));
	content->type = XmlCType_Element;
	tag_name = read_string(file, end_element_type);
	content->data.element.type = XmlGetElementType(tag_name, document, TRUE);

	/* Read the attributes of the element */
	content->data.element.attributes = XmlParseAttributes(file, document);
	/* If an error occured */
	if (content->data.element.attributes == NULL)
	{
		XmlFree (tag_name);
		XmlFree (content);
		return (NULL);
	}
	c = read_next_char(file);
	if (c == '/')
	{
		if (getc(file) != '>')
		{
			XmlSetErrorMsg ("Expected '>' after '/'.");
			XmlFree (content);
			content = NULL;
		}
		else
		{
			content->data.element.is_empty = TRUE;
			content->data.element.contents = NULL;
		}
	}
	else if (c == '>')
	{
		content->data.element.is_empty = FALSE;
		content->data.element.contents = XmlParseContents(file, document, tag_name);
		if (content->data.element.contents == NULL)
		{
			XmlFree (content);
			content = NULL;
		}
	}
	XmlFree (tag_name);

	return (content);
}


XmlContent
*XmlParseTag (	FILE		*file,
		XmlDocument	*document  )
/*
Input:
	file		- The file to read from.
Output:
	document	- If the new content contains new attribute names or element
			  types, they are added to their list.
Returns:
	When no errors occured a pointer to a new XmlContent containing the readen
	tag. Otherwise NULL is returned.
Description:
	This function reads a tag from file. If the tag is the start-tag of a non-empty
	element, the contents and the end-tag of this element will be read. The '<' of
	the tag should already been read from file before calling this function.
*/
{
	char		c;
	XmlContent	*content;
		
	c = getc(file);
	if (c == EOF || c == '<')
	{
		XmlSetErrorMsg ("Document contains an unclosed-tag.");
		return (NULL);
	}
	else if (c == '!' || c == '?')
	{
		content = XmlMalloc(sizeof(XmlContent));
		/* If it is a CDATA section */
		if (c == '!' && file_string_appear(file, "[CDATA["))
		{
			content->data.string = read_string(file, end_CDATA);
			if (feof(file) || getc(file) != '>')
			{
				XmlSetErrorMsg ("Document contains an unclosed-tag.");
				XmlFree (content->data.string);
				XmlFree (content);
				return (NULL);
			}
			content->type = XmlCType_CDATA;
		}
		/* If it is a comment */
		else if (c == '!' && file_string_appear(file, "--"))
		{
			content->data.string = read_string(file, end_comment);
			/* Because we want to check what character was read the last time */
			fseek (file, -1, SEEK_CUR);
			/* If the last character read was a '>' */
			if (getc(file) != '>')
			{
				XmlSetErrorMsg ("Document contains an unclosed-tag.");
				XmlFree (content->data.string);
				XmlFree (content);
				return (NULL);
			}
			content->type = XmlCType_Comment;
		}
		else
		{
			/* The type of tag is not supported.
			 * Therefore we read until we reach a '>'.
			 */
			do {
				c = getc(file);
				if (c == EOF)
				{
					XmlSetErrorMsg ("Document contains an unclosed-tag.");
					XmlFree (content);
					return (NULL);
				}
			} while (c != '>');
			content->type = XmlCType_Unknown;
		}
	}
	else if (!isalpha(c))
	{
		XmlSetErrorMsg ("Document contains a tag with an invalid name.");
		return (NULL);
	}
	else
	{
		/* Because we already read one character of the tag */
		fseek (file, -1, SEEK_CUR);
		content = XmlParseElement(file, document);
	}
	return (content);
}


XmlContentList
*XmlParseContents (	FILE		*file,
			XmlDocument	*document,
			char		*tag_name )
/*
Input:
	file		- The file to read from
	tag_name	- The name of the current tag. It should be NULL if we are
			  going to read a root element.
Output:
	document	- If the new content contains new attribute names or element
			  types, they are added to their list.
Returns:
	A pointer to a the new XmlContentList when no errors occured. Otherwise NULL
	is returned.
Description:
	This function reads the content of the current tag or document from file.
*/
{
	XmlContentList	*contents;
	XmlContent	*new_content;
	Boolean		end_tag_found;
	char		c;
	int		count;
	
	contents = XmlCreateContentList();
	
	end_tag_found = FALSE;
	while (!feof(file) && !end_tag_found)
	{
		c = read_next_char(file);
		if (c != EOF)
		{
			if (c == '<')
			{
				c = getc(file);
				if (c == '/')
				{
					end_tag_found = TRUE;
				}
				else if (c == EOF)
				{
					XmlSetErrorMsg ("Document contains an end tag that is not closed.");
					XmlClearContentList (contents);
					XmlFree (contents);
					return (NULL);
				}
				else
				{
					/* Because we already read one character of the tag */
					fseek (file, -1, SEEK_CUR);
					new_content = XmlParseTag(file, document);
				}
			}
			else
			{
				/* Because we already read one character of the string */
				fseek (file, -1, SEEK_CUR);
				new_content = XmlParseString(file);
			}
			if (end_tag_found)
			{
				if (tag_name == NULL)
				{
					XmlSetErrorMsg ("Document contains an end tag without having a start tag.");
					XmlClearContentList (contents);
					XmlFree (contents);
					return (NULL);
				}
				/* Compare the type of the start tag with the type of the end tag */
				for (count = 0; tag_name[count] != '\0'; count++)
				{
					if (getc(file) != tag_name[count])
					{
						XmlSetErrorMsg ("Type of end tag does not match start tag.");
						XmlClearContentList (contents);
						XmlFree (contents);
						return (NULL);
					}
				}
				c = getc(file);
				if (!isspace(c) && c != '>')
				{
					XmlSetErrorMsg ("Type of end tag does not match start tag.");
					XmlClearContentList (contents);
					XmlFree (contents);
					return (NULL);
				}
				/* Read until '>' */
				while (c != '>')
				{
					if (!isspace(c))
					{
						XmlSetErrorMsg ("Document contains an end tag that is not closed.");
						XmlClearContentList (contents);
						XmlFree (contents);
						return (NULL);
					}
					c = getc(file);
				}
			}
			else
			{
				/* If an error occured */
				if (new_content == NULL)
				{
					XmlClearContentList (contents);
					XmlFree (contents);
					return (NULL);
				}
				XmlContentAppend (new_content, contents);
			}
		}
	}
	if (!end_tag_found && tag_name != NULL)
	{
		XmlSetErrorMsg ("End tag missing.");
		XmlClearContentList (contents);
		XmlFree (contents);
		return (NULL);
	}
	return (contents);
}


XmlDocument
*XmlParseDocument (char *filename)
/*
Input:
	filename	- The filename of the XML document to parse
Output:
	-
Returns:
	A XmlDocument pointer to the parsed XML document after succesfully parsing
	the document. Otherwise NULL is returned.
Description:
	This function parses the XML document whose name is the string pointed to by
	filename. If it fails to parse the file an error message describing why it
	failed can be retrieved by calling XmlGetErrorMsg().
*/
{
	FILE		*file;
	XmlDocument	*document;
	XmlContentList	*contents;
	
	file = fopen(filename, "r");
	if (file == NULL)
	{
		XmlSetErrorMsg ("Could not open the XML document.");
		return (NULL);
	}
	document = XmlCreateDocument();
	contents = XmlParseContents (file, document, NULL);
	if (contents == NULL)
	{
		XmlFree (document);
		return (NULL);
	}
	document->contents = *contents;
	XmlFree (contents);
	fclose (file);
	
	return (document);
}
