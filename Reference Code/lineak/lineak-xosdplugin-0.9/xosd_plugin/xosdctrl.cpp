/***************************************************************************
                          xosdctrl.cpp  -  description
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

#include <lineak/lconfig.h>
#include <lineak/lineak_core_functions.h>


using namespace lineak_core_functions;
using namespace std;


#include "xosdctrl.h"
//#include "lineak_core_functions.h"
//float xosdCtrl::MaxAudio = 25700.00;
//const float xosdCtrl::MUTE = -100.0000;

xosdCtrl::xosdCtrl(LConfig& config) : displayCtrl(config) {
  
  Lines = 2;
  initialized = false;  

  if (sem_init(&enter_sem, 0,1) != 0)
     error("enter_sem: Semaphore initialization failed");
  if (sem_init(&exit_sem,0,1) != 0)
     error("exit_sem: Semaphore initialization failed");

  init(config);

}

xosdCtrl::xosdCtrl(string font, string color, int delay, xosd_pos pos, xosd_align align, int hoffset, int voffset, int soffset, int lines): Lines(lines) {
  //ScrollAge = true;

  if (sem_init(&enter_sem, 0,1) != 0)
     error("enter_sem: Semaphore initialization failed");
  if (sem_init(&exit_sem,0,1) != 0)
     error("exit_sem: Semaphore initialization failed");

  initialized = false;

  create(lines);
  setFont(font);
  setColor(color);
  setTimeout(delay);
  setPos(pos);
  setAlign(align);
  setHorizontalOffset(hoffset);
  setVerticalOffset(voffset);
  setShadowOffset(soffset);
  /* Not really needed, but at least we aren't throwing around an unknown value */
  old_age.tv_sec=0;

  if (ScrollAge)
  	gettimeofday(&old_age,0);
}

xosdCtrl::xosdCtrl() : displayCtrl() {

  if (sem_init(&enter_sem, 0,1) != 0)
     error("enter_sem: Semaphore initialization failed");
  if (sem_init(&exit_sem,0,1) != 0)
     error("exit_sem: Semaphore initialization failed");
  /* Set my defaults */
  //ScrollAge = true;
  initialized = false;

  Lines = 2;
  Font = DISPLAY_FONT;
  Color = DISPLAY_COLOR;
  Timeout = atoi(DISPLAY_TIMEOUT);
  Pos = XOSD_bottom;
  Align = XOSD_center;
  HorizontalOffset = atoi(DISPLAY_HOFFSET);
  VerticalOffset = atoi(DISPLAY_VOFFSET);
  ShadowOffset = atoi(DISPLAY_SOFFSET);
  init();

}
xosdCtrl::~xosdCtrl(){
  /** Destroy the semaphores */
  sem_destroy(&enter_sem);
  sem_destroy(&exit_sem);
}
void xosdCtrl::create(int lines) {
    // Set some variables.
    ScrollAge=true;
    Forcewait=false;
    ScreenLine = 0;
    Lines=lines;

    sem_wait(&enter_sem);
    // Create the xosd object
    osd = xosd_create(Lines);
    sem_post(&enter_sem);

    /** This can happen in the situations where LANG is an asian character set */
    if (!osd) {
      cerr << "Error initializing osd: " << xosd_error << endl;
    }
}
void xosdCtrl::init(LConfig& config, bool init){
   if (initialized == false || init == true) { 
   vmsg("Initializing xosdCtrl");
   
   //ScrollAge=true;
   create(Lines);

   setFont(config["Display_font"]);
   setColor(config["Display_color"]);

   string position = config["Display_pos"];
   if (position == "top")
      setPos((xosd_pos)XOSD_top);
   if (position == "middle")
      setPos((xosd_pos)XOSD_middle);
   if (position == "bottom" || position == "")
      setPos((xosd_pos)XOSD_bottom);
   string align = config["Display_align"];
   if (align == "left")
      setAlign((xosd_align)XOSD_left);
   if (align == "center" || align == "")
      setAlign((xosd_align)XOSD_center);
   if (align == "right")
      setAlign((xosd_align)XOSD_right);

   setTimeout(atoi(config.getValue("Display_timeout").c_str()));
   setHorizontalOffset(atoi(config.getValue("Display_hoffset").c_str()));
   setVerticalOffset(atoi(config.getValue("Display_voffset").c_str()));
   setShadowOffset(atoi(config.getValue("Display_soffset").c_str()));

   //config.print(cout);
   vmsg("Our parameters!");
   vmsg(config["Display_font"]);
   vmsg(config["Display_color"]);
   vmsg(config["Display_pos"]);
   vmsg(config["Display_align"]);
   vmsg(config.getValue("Display_timeout"));
   vmsg(config.getValue("Display_hoffset"));
   vmsg(config.getValue("Display_voffset"));
   vmsg(config.getValue("Display_soffset"));
   vmsg("");

   //Forcewait=false;
   //ScreenLine = 0;
   /* Not really needed, but at least we aren't throwing around an unknown value */
    old_age.tv_sec=0;

    if (ScrollAge)
    	gettimeofday(&old_age,0);
    
    vmsg("Finished Initializing");
   }
   else
	   vmsg("XOSD Display controller already initialized.");
}
void xosdCtrl::init(bool init){

    if ( initialized == false || init == true) {
    vmsg("Initializing default xosdCtrl");

    /* Set my defaults */
    create(Lines);
    setFont(Font);
    setColor(Color);
    setTimeout(Timeout);
    setPos(Pos);
    setAlign(Align);
    setHorizontalOffset(HorizontalOffset);
    setVerticalOffset(VerticalOffset);
    setShadowOffset(ShadowOffset);

//     vmsg("Our parameters!");
//     vmsg(Font);
//     vmsg(Color);
//     vmsg(string(Pos));
//     vmsg(Align);
//     vmsg(Timeout);
//     vmsg(HorizontalOffset);
//     vmsg(VerticalOffset);
//     vmsg(ShadowOffset);
//     vmsg("");

    //Forcewait=false;
    //ScreenLine = 0;

   /* Not really needed, but at least we aren't throwing around an unknown value */
    old_age.tv_sec=0;

    if (ScrollAge)
    	gettimeofday(&old_age,0);

    vmsg("Finished Default Initialization");
    }
       else vmsg("XOSD Display controller already initialized.");
}

void xosdCtrl::cleanup() {
   scroll();
   vmsg("xosdCtrl::cleanup() -- Cleaning up xosd");
   if (osd != NULL) {
        vmsg("xosdCtrl::cleanup() -- xosd_destroy");
        /** For some reason, xosd_destroy hangs sometimes. */
        sem_wait(&enter_sem);
  	xosd_destroy (osd);
        sem_post(&enter_sem);
    	osd = NULL;
    }
   vmsg("xosdCtrl::cleanup() -- done cleanup");
}

void xosdCtrl::agenscroll(void) {
  /* Should we age the display? */
  if (ScrollAge)
  {
     gettimeofday(&new_age,0);
     if ((new_age.tv_sec - old_age.tv_sec) > ScrollAge)
     {
        scroll();
        ScreenLine=0;
     }
  }

  if (osd) {
  	if (ScreenLine >= xosd_get_number_lines(osd))
  	{
     		scroll(1);
     		ScreenLine = xosd_get_number_lines(osd)-1;
  	}
  		if (Forcewait && xosd_is_onscreen(osd)) {
     		xosd_wait_until_no_display(osd);
  	}
  }
}

void xosdCtrl::age(void) {
  old_age.tv_sec = new_age.tv_sec;
}

/* Modify the volume display output */
void xosdCtrl::volume(float ivolume) {
  float volcalc;
  scroll();
     if (ivolume == MUTE) {
        if (osd) {
	   sem_wait(&enter_sem);
           xosd_display (osd, ScreenLine++, XOSD_string, "Volume Muted");
           sem_post(&enter_sem);
				//agenscroll();
        }
     }
     else {
		volcalc = (float)ivolume;
		volcalc /= MaxAudio;
		volcalc *= 100.00;
		int volume = (int) volcalc;
		if (osd) {
                        sem_wait(&enter_sem);
			xosd_display (osd, 0, XOSD_string, "Volume");
			xosd_display (osd, 1, XOSD_percentage, volume);
                        sem_post(&enter_sem);
		} else
			error("Could not display volume level. osd is not defined.");
     }

}
/* Display an onscreen command */
void xosdCtrl::show(string command) {
	if (osd) {
		scroll();
                sem_wait(&enter_sem);
	  	xosd_display (osd, ScreenLine++, XOSD_string, command.c_str());
                sem_post(&enter_sem);
	}
}

/** Read property of string font. */
const string& xosdCtrl::getFont(){
	return Font;
}
/** Write property of string font. */
void xosdCtrl::setFont( const string& _newVal){
	Font = _newVal;
	if (osd) {
		//char *font =  (char *)malloc((strlen(Font.c_str())+1)*sizeof(char));
		//strcpy(font, Font.c_str());
		//if (xosd_set_font (osd, font) == -1)
		// If you cannot set the font, fall back to the default.
                sem_wait(&enter_sem);
		if (xosd_set_font (osd, Font.c_str()) == -1) {
		   error("xosd_set_font has failed: " + Font);
		   Font = DISPLAY_FONT;
		   error(string("Setting default font: ") + DISPLAY_FONT);
		   if (xosd_set_font (osd, DISPLAY_FONT) == -1)
			   error("Setting default font has failed");
		}
                sem_post(&enter_sem);
		if (!osd) {
		   error("osd has failed to initialize and is now invalid.");
		   osd = NULL;
		}
		//free(font);
	}
}
/** Read property of string color. */
const string& xosdCtrl::getColor(){
	return Color;
}
/** Write property of string color. */
void xosdCtrl::setColor( const string& _newVal){
	Color = _newVal;
	if (osd) {
		string tmp = '#' + Color;
		char *color =  (char *)malloc((strlen(tmp.c_str())+1)*sizeof(char));
		strcpy(color, tmp.c_str());
		sem_wait(&enter_sem);
		xosd_set_colour (osd, color);
		sem_post(&enter_sem);
		free(color);
	}
}
/** Read property of int delay. */
const int& xosdCtrl::getTimeout(){
	return Timeout;
}
/** Write property of int delay. */
void xosdCtrl::setTimeout( const int& _newVal){
	Timeout = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_timeout (osd, Timeout);
		sem_post(&enter_sem);
        }
}
/** Read property of int Forcewait. */
const bool& xosdCtrl::getForcewait(){
	return Forcewait;
}
/** Write property of int Forcewait. */
void xosdCtrl::setForcewait( const bool& _newVal){
	Forcewait = _newVal;
}
/** Read property of xosd_pos Pos. */
const xosd_pos& xosdCtrl::getPos(){
	return Pos;
}
/** Write property of xosd_pos Pos. */
void xosdCtrl::setPos( const xosd_pos& _newVal){
	Pos = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_pos (osd, Pos);
		sem_post(&enter_sem);
	}
}
/** Read property of int Offset. */
const int& xosdCtrl::getHorizontalOffset(){
	return HorizontalOffset;
}
/** Write property of int Offset. */
void xosdCtrl::setHorizontalOffset( const int& _newVal){
	HorizontalOffset = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_horizontal_offset (osd, HorizontalOffset);
		sem_post(&enter_sem);
	}
}
/** Read property of int Offset. */
const int& xosdCtrl::getVerticalOffset(){
	return VerticalOffset;
}
/** Write property of int Offset. */
void xosdCtrl::setVerticalOffset( const int& _newVal){
	VerticalOffset = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_vertical_offset (osd, VerticalOffset);
		sem_post(&enter_sem);
	}
}
/** Read property of int Shadow. */
const int& xosdCtrl::getShadowOffset(){
	return ShadowOffset;
}
/** Write property of int Shadow. */
void xosdCtrl::setShadowOffset( const int& _newVal){
	ShadowOffset = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_shadow_offset (osd, ShadowOffset);
		sem_post(&enter_sem);
        }
}
/** Read property of int ScrollAge. */
const bool& xosdCtrl::getScrollAge(){
	return ScrollAge;
}
/** Write property of int ScrollAge. */
void xosdCtrl::setScrollAge( const bool& _newVal){
	ScrollAge = _newVal;

    if (ScrollAge)
      gettimeofday(&old_age,0);  
}
/** Read property of int ScreenLine. */
const int& xosdCtrl::getScreenLine(){
	return ScreenLine;
}
/** Write property of int ScreenLine. */
void xosdCtrl::setScreenLine( const int& _newVal){
	ScreenLine = _newVal;
}
/** Read property of int Lines. */
const int& xosdCtrl::getLines(){
	return Lines;
}
/** Write property of int Lines. */
void xosdCtrl::setLines( const int& _newVal){
	Lines = _newVal;
}
const xosd_align& xosdCtrl::getAlign(){
	return Align;
}
/** Write property of xosd_align Align. */
void xosdCtrl::setAlign( const xosd_align& _newVal){
	Align = _newVal;
	if (osd) {
		sem_wait(&enter_sem);
		xosd_set_align (osd, Align);
		sem_post(&enter_sem);
	}
}
void xosdCtrl::show() {
	if (osd) {
		sem_wait(&enter_sem);
		xosd_show (osd);
		sem_post(&enter_sem);
	}
}
void xosdCtrl::hide() {
	if (osd) {
		sem_wait(&enter_sem);
		xosd_hide (osd);
		sem_post(&enter_sem);
	}
}
/** Read property of xosd_align Align. */
/** Scroll the display lines number of lines */
void xosdCtrl::scroll(int lines){
	if (osd) {
		if (lines != 0) {
			sem_wait(&enter_sem);
			xosd_scroll(osd,lines);
			sem_post(&enter_sem);
                }
		else {
			sem_wait(&enter_sem);
                        int nlines = 0;
                        nlines = xosd_get_number_lines(osd);
			xosd_scroll(osd,nlines);
			sem_post(&enter_sem);
                }
	}
}
