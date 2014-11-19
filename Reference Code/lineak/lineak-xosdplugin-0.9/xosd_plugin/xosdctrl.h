/***************************************************************************
                          xosdctrl.h  -  description
                             -------------------
    begin                : Sat Feb 22 2003
    copyright            : (C) 2003 by Sheldon Lee Wen
    email                : leewsb@hotmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef XOSDCTRL_H
#define XOSDCTRL_H

extern "C" {
#include <xosd.h>
#include <sys/time.h>
#include <semaphore.h>
}
#include <lineak/displayctrl.h>
#include <string>
#include <iostream>

class LConfig;
using namespace std;
/**Controls functioning of XOSD.
  *@author Sheldon Lee Wen
  */

class xosdCtrl : public displayCtrl {
public: 
  xosdCtrl();
  xosdCtrl(LConfig& config);
  xosdCtrl(string font, string color, int delay, xosd_pos pos, xosd_align align, int hoffset, int voffset, int soffset, int lines);
  virtual ~xosdCtrl();
  /** This creates the osd object */
  virtual void create(int lines);
  /** Reinitilize the object */
  virtual void init(LConfig& config, bool init = false);
  /** Reinitilize the object */
  virtual void init(bool init = false);
	/* Cleanup xosd */
	virtual void cleanup();
	/* Age the display and scroll */
	virtual void agenscroll();
	/* Age the display */
	virtual void age();
	/* Show the volume */
	virtual void volume(float volume);
	/* Show a command */
	virtual void show(string command);
 	/** Write property of string font. */
 	virtual void setFont( const string& _newVal);
 	/** Read property of string font. */
 	virtual const string& getFont();
 	/** Write property of string color. */
 	virtual void setColor( const string& _newVal);
 	/** Read property of string color. */
 	virtual const string& getColor();
 	/** Write property of int Timeout. */
 	virtual void setTimeout( const int& _newVal);
 	/** Read property of int Timeout. */
 	virtual const int& getTimeout();
 	/** Write property of int Forcewait. */
 	virtual void setForcewait( const bool& _newVal);
 	/** Read property of int Forcewait. */
 	virtual const bool& getForcewait();
 	/** Write property of xosd_pos Pos. */
 	virtual void setPos( const xosd_pos& _newVal);
 	/** Read property of xosd_pos Pos. */
 	virtual const xosd_pos& getPos();
 	/** Write property of xosd_align Align. */
 	virtual void setAlign( const xosd_align& _newVal);
 	/** Read property of xosd_align Align. */
 	virtual const xosd_align& getAlign();
 	/** Write property of int HorizontalOffset. */
 	virtual void setHorizontalOffset( const int& _newVal);
 	/** Read property of int HorizontalOffset. */
 	virtual const int& getHorizontalOffset();
 	/** Write property of int HorizontalOffset. */
 	virtual void setVerticalOffset( const int& _newVal);
 	/** Read property of int HorizontalOffset. */
 	virtual const int& getVerticalOffset();
 	/** Write property of int Shadow. */
 	virtual void setShadowOffset( const int& _newVal);
 	/** Read property of int Shadow. */
 	virtual const int& getShadowOffset();
 	/** Write property of int ScrollAge. */
 	virtual void setScrollAge( const bool& _newVal);
 	/** Read property of int ScrollAge. */
 	virtual const bool& getScrollAge();
 	/** Write property of int ScreenLine. */
 	virtual void setScreenLine( const int& _newVal);
 	/** Read property of int ScreenLine. */
 	virtual const int& getScreenLine();
 	/** Write property of int Lines. */
 	virtual void setLines( const int& _newVal);
 	/** Read property of int Lines. */
 	virtual const int& getLines();
  virtual void show();
  virtual void hide();
  /** Scroll the display lines number of lines */
  virtual void scroll(int lines = 0);
  virtual bool enabled() { return (osd != NULL); }
private:
	xosd *osd;
	struct timeval old_age,new_age;
 	/** Font typeface to use */
 	string Font;
 	/** Font color to use */
 	string Color;
 	/** Delay for display */
 	int Timeout;
 	/** Are we forced to wait for the display to clear? */
 	bool Forcewait;
 	/** Position of the screen in which to display our text. */
 	xosd_pos Pos;
 	/** Show a shadow or not? */
 	int ShadowOffset;
 	/** Number of lines on the screen to use. */
 	int ScreenLine;
 	/** Number of lines of display text. */
 	int Lines;
 	/** Screen offset */
 	int HorizontalOffset, VerticalOffset;
 	/** Age and scroll the display? */
 	bool ScrollAge;
 	/** Set the screen alignment of xosd. */
 	xosd_align Align;
	/** Semaphores for fixing threading issues */
	sem_t enter_sem, exit_sem;
	/** Flag for initialization */
	bool initialized;
};
#endif
