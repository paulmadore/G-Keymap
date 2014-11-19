/*-------------------------------------------------------------------------------
Name               : mxml.h
Author             : Marvin Raaijmakers
Description        : Main headerfile for the mxml library
Date of last change: 24-Jun-2006
History            : 24-Jun-2006 Added prototypes of XmlSetErrorMsg() and
                                 XmlGetErrorMsg()
                     01-Jan-2006 Added #ifndef INCLUDE_MXML_H stuff

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
#ifndef INCLUDE_MXML_H
#define INCLUDE_MXML_H

#define TRUE	1
#define FALSE	0

#define NUM_SPEC_CHARS	5
#define INDENT_SIZE	2

typedef char Boolean;


typedef struct _XmlElementType {
	char			*type;
	struct _XmlElementType	*next;
} XmlElementType;

typedef struct _XmlAttributeName {
	char				*name;
	struct _XmlAttributeName	*next;
} XmlAttributeName;

typedef char XmlAttributeValue;

typedef struct _XmlAttribute {
	XmlAttributeName	*name;
	XmlAttributeValue	*value;
	struct _XmlAttribute	*next;
} XmlAttribute;

typedef struct {
	XmlAttribute	*head,	/* The head of the list */
			*tail;	/* The tail of the list */
} XmlAttributeList;

typedef struct {
	Boolean			is_empty;
	XmlElementType		*type;
	XmlAttributeList	*attributes;
	struct _XmlContentList	*contents;
} XmlContentElement;

typedef char *XmlContentString;

typedef enum {
	XmlCType_CDATA,
	XmlCType_Comment,
	XmlCType_Element,
	XmlCType_String,
	XmlCType_Unknown,
	XmlNumCType
} XmlContentType;

typedef union {
	XmlContentString	string; /* Used for XmlCType_CDATA, XmlCType_Comment and XmlCType_String */
	XmlContentElement	element;
} XmlContentData;

/* A piece of content appearing of a XML document
 * or element.
 */
typedef struct _XmlContent {
	XmlContentType		type;
	XmlContentData		data;
	struct _XmlContent	*next;
} XmlContent;

/* A list of contents of a XML document or element.
 * The contents are in order as they appear in the
 * XML document */
typedef struct _XmlContentList {
	XmlContent	*head,	/* The head of the list */
			*tail;	/* The tail of the list */
	int		num_elements;
	struct {
		int        index;
		XmlContent *content;
	} prev_request;		/* Contains information about the previous XmlGetContent() call */
} XmlContentList;

/* A list of all element names appearing in the XML document */
typedef struct {
	XmlElementType	*head,	/* The head of the list */
			*tail;	/* The tail of the list */
} XmlElementTypeList;

/* A list of all attribute names appearing in the XML document */
typedef struct {
	XmlAttributeName	*head,	/* The head of the list */
				*tail;	/* The tail of the list */
} XmlAttributeNameList;

typedef struct {
	XmlElementTypeList	element_types;
	XmlAttributeNameList	attribute_names;
	XmlContentList		contents;
} XmlDocument;


#define XmlGetContentType(_content)		(((XmlContent *)_content)->type)
#define XmlAttributeGetValue(_attribute)	(((XmlAttribute *)_attribute)->value)
#define XmlGetElementTypeString(_element)	(((XmlContent *)_element)->data.element.type->type)
#define XmlElementContentList(_element)		(((XmlContent *)_element)->data.element.contents)
#define XmlElementIsEmpty(_element)		(((XmlContent *)_element)->data.element.is_empty)
#define XmlStringString(_string)		(((XmlContent *)_string)->data.string)
#define XmlCommentString(_comment)		(((XmlContent *)_comment)->data.string)
#define XmlCDATAString(_cdata)			(((XmlContent *)_cdata)->data.string)
#define XmlDocumentContentList(_document)	(&(((XmlDocument *)_document)->contents))


/**** Functions in parse.c ****/
extern XmlDocument *XmlParseDocument (char *filename);

/**** Functions in memory.c ****/
extern void *XmlMalloc (size_t size);
extern void *XmlRealloc (void *ptr, size_t size);
extern void XmlFree (void *pointer);

/**** Functions in error.c ****/
extern void XmlError (char *message);
extern void XmlSetErrorMsg (char *msg);
extern char *XmlGetErrorMsg (void);

/**** Functions in content.c ****/
extern void XmlContentAppend (XmlContent *content, XmlContentList *content_list);
extern void XmlContentRemove (XmlContent *content, XmlContentList *content_list);
extern XmlContent *XmlGetContent (XmlContentList *content_list, int index);
extern XmlContent *XmlGetContentOfType (XmlContentList *content_list, XmlContentType  type, int index);
extern XmlContent *XmlGetElementOfType (XmlContentList *content_list, XmlElementType *type, int index);
extern XmlContentList *XmlCreateContentList (void);
extern void XmlClearContentList (XmlContentList *content_list);
extern XmlContent *XmlCreateContent (XmlContentType type);
extern void XmlStringSetString (XmlContent *string, char *value);
extern void XmlCommentSetString (XmlContent *comment, char *value);
extern void XmlCDATASetString (XmlContent *cdata, char *value);

/**** Functions in document.c ****/
extern XmlDocument *XmlCreateDocument (void);
extern void XmlFreeDocument (XmlDocument *document);
extern XmlElementType *XmlGetElementType (char *type_name, XmlDocument *document, Boolean create_new);
extern XmlAttributeName *XmlGetAttributeName (char *attr_name, XmlDocument *document, Boolean create_new);

/**** Functions in element.c ****/
extern void XmlClearAttributeList (XmlAttributeList *attribute_list);
extern void XmlAttributeListAdd (XmlAttributeList *attribute_list, XmlAttribute *attribute);
extern char *XmlGetAttributeValue (XmlAttributeName *attribute_name, XmlContent *content);
extern char *XmlGetElementString (XmlContent *content, char *seperator);
extern XmlContent *XmlCreateElement (XmlElementType *type, Boolean isempty);
extern void XmlSetAttributeValue (XmlAttributeName *attribute_name, char *value, XmlContent *element);

/**** Functions in write.c ****/
extern Boolean XmlWriteDocument (XmlDocument *document, char *file_name);

#endif
