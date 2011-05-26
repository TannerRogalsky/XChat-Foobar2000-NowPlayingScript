/*
  XChat/Foorbar2000 Now Playing script
  Version: 1.0
  Web: http://sites.google.com/site/thetdawg/nowplaying
  Last build: 2010-11-11

  Copyright (c)2010  Tanner Rogalsky  All rights reserved.

  This file is part of XChat/LastFM Plug-in.

  XChat/Foobar2000 Now Playing script is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  XChat/Foobar2000 Now Playing script is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You can receive your copy of the GNU General Public License at <http://www.gnu.org/licenses/>.
*/

#include "xchat-plugin.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#define XCFOOBAR_VERSION_STR "1.0"
#define XCFOOBAR_NAME "Now Playing: Foobar2000"
#define XCFOOBAR_DESC "XChat/Foorbar2000 Now Playing script for X-Chat 2"

static xchat_plugin* ph = NULL;

static int foobar(char *word[], char *word_eol[], void *userdata) 
{
	LPCTSTR ptxt = NULL;

	// Simulate ctrl + shift + c press
	keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
	keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
	keybd_event(0x43, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

	// Simulate ctrl + shift + c release
	keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	keybd_event(0x43, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

	// ctrl + v pressed
	keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );
	keybd_event(0x56, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0 );

	// ctrl + v released
	keybd_event(VK_CONTROL, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	keybd_event(0x56, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	
	/*GetClipboardText(ptxt,256);
	if (ptxt != NULL){
		xchat_commandf(ph, "me %s", ptxt);
	}else{
		xchat_printf(ph, "Script did not pull text.");
	}*/

	return XCHAT_EAT_NONE;
}

void xchat_plugin_get_info(char **name, char **desc, char **version, void **reserved) {
  *name = XCFOOBAR_NAME;
  *desc = XCFOOBAR_DESC;
  *version = XCFOOBAR_VERSION_STR;
}


int xchat_plugin_init(xchat_plugin* plugin_handle, char **plugin_name, char **plugin_desc,
                      char **plugin_version, char *arg)
{
  ph = plugin_handle;
  *plugin_name = XCFOOBAR_NAME;
  *plugin_desc = XCFOOBAR_DESC;
  *plugin_version = XCFOOBAR_VERSION_STR;

  xchat_hook_command(ph, "np", XCHAT_PRI_NORM, foobar,
                     "Usage: /np - Copies the currently playing track from Foobar2000", 0);

  xchat_printf(ph, "XChat/Foorbar2000 Now Playing script v%s loaded.\n", XCFOOBAR_VERSION_STR);
  return 1;
}

int GetClipboardText(LPTSTR pszBuf, int nLength)
{
   int i = 0;
   *pszBuf = 0;
   if(OpenClipboard(NULL)) {
      HGLOBAL hMem = GetClipboardData(CF_TEXT);
      if(hMem) { // format is available, extract text
         LPCTSTR ptxt = (LPCTSTR)GlobalLock(hMem);
         for( ; ++i < nLength && *ptxt; ) // copy as much as will fit
            *pszBuf++ = *ptxt++;
         *pszBuf = 0; // ensure terminator

         GlobalUnlock(hMem); // we don't free it; owned by clipboard
      }

      CloseClipboard();
   }
   return i; // 0 indicates failure
}


// Return pointer to clipboard data and set bytes returned value
// If error occurs, set up error message, point to it, set bytes negative
// Whether successful or not, the caller SHOULD free the data
char *clipget()
{
	int k;
	char *buffer=NULL;
	char *data=NULL;
	char empty[80]="<Clipboard is empty>";
	int bytes = 0;

	// open the clipboard
	if (OpenClipboard(NULL))
	{
		HANDLE hData = GetClipboardData(CF_TEXT);
		char * buffer = (char*)GlobalLock(hData);
		GlobalUnlock(hData);
		CloseClipboard(); 

		// Return an error message
		if (buffer == NULL)
		{
			bytes = strlen(empty);
			data = (char *) malloc(bytes+1);
			strcpy(data,empty);
			bytes = bytes * -1;
		}
		// Return pointer to retrieved data
		else
		{
			bytes = strlen(buffer);
			data = (char *) malloc(bytes+1);
			strcpy(data,buffer);
		}
	}
	// Return an open clipboard failed message
	else
	{
		k = GetLastError();
		if (k < 0)
		bytes = k;
		else
		bytes = k * -1;
		sprintf(empty,"Error occurred opening clipboard - RC: %d",k);
		k = strlen(empty);
		data = (char *) malloc(k+1);
		strcpy(data,empty);
	}
	// Return pointer to data field allocated 
	// It's up to the caller to free the storage
	return data;
}