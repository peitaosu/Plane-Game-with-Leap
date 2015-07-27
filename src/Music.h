#pragma once
#include <Windows.h>

enum LDRAW
{
	LDRAW_OK,
	LDRAW_COMMONERR,
};

enum MUSIC_COMMAND
{
	M_PLAY,
	M_PAUSE,
	M_RESUME,
	M_STOP,
	M_RESTART,
};

class Music
{
public:
	Music(void);
	~Music(void);

	HRESULT lmciMidi(HWND hWnd);  
	HRESULT lmciMidiPlay(LPSTR FileName);  
	HRESULT lmciMidiStatus();  
	HRESULT lmciMidiStop();  
	HRESULT lmciMidiPause();  
	HRESULT lmciMidiResume();  
	HRESULT lmciMidiRestart();   

private:
	HWND lmciMidiHWnd;
};
