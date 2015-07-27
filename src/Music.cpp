#include ".\music.h"
#include <iostream>

Music::Music(void)
{
	lmciMidiHWnd = NULL;
}

Music::~Music(void)
{
}

HRESULT Music::lmciMidi( HWND hWnd )
{
	lmciMidiHWnd = hWnd;  
	return LDRAW_OK;   
}

HRESULT Music::lmciMidiPlay( LPSTR FileName )
{
	if(FileName == NULL)   return FALSE;  
	char buffer[256];  

	sprintf(buffer, "open \"%s\" type sequencer alias MUSIC",   FileName);  

	if(mciSendString("close all", NULL, 0, NULL) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  
	if(mciSendString(buffer, NULL, 0, NULL) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  
	if(mciSendString("play MUSIC from 0 notify", NULL, 0, lmciMidiHWnd) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  

	return LDRAW_OK;   
}

HRESULT Music::lmciMidiStatus()
{
	char buffer[256];  
	mciSendString("status MUSIC mode", buffer, 24, NULL);  
	if(strcmp(buffer, "not ready")==0)   return -1;  
	if(strcmp(buffer, "playing")==0)   return 1;  
	if(strcmp(buffer, "stopped")==0)   return 0;  
	return 0;   
}

HRESULT Music::lmciMidiStop()
{
	if(mciSendString("close all", NULL, 0, NULL) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  
	return LDRAW_OK;   
}

HRESULT Music::lmciMidiPause()
{
	//   Pause   if   we're   not   already   paused  
	if(mciSendString("stop MUSIC", NULL, 0, NULL) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  

	return LDRAW_OK;   
}

HRESULT Music::lmciMidiResume()
{
	//   Resume   midi  
	if(mciSendString("play MUSIC notify", NULL, 0, lmciMidiHWnd) != 0)  
	{  
		return LDRAW_COMMONERR;   
	}

	return LDRAW_OK;
}

HRESULT Music::lmciMidiRestart()
{
	//   Replay   midi  
	if(mciSendString("play MUSIC from 0 notify", NULL, 0, lmciMidiHWnd) != 0)  
	{  
		return LDRAW_COMMONERR;  
	}  

	return LDRAW_OK;   
}