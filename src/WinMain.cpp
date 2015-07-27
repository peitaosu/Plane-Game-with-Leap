#include <Windows.h>
#include "Music.h"
#include "GameManager.h"
#include "HeroPlane.h"
#include "resource.h"
//#pragma comment(lib,"LIBCMT.LIB")
#include <process.h>
#include "LeapControl.h"

#define WindowWd 1280
#define WindowHt 720
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);
void Render();

//全局变量
int g_STATE;
UINT g_Time = timeGetTime();
GameManager g_Manager;
Music g_Music;
BOOL g_GamePause = FALSE;

//窗口控制
int ScreenWidth;
int ScreenHeight;

HWND hWnd;
HINSTANCE g_hInstance;
HDC g_hdc, g_hdcMem, g_hdcMask, g_hdcBuffer; //显示DC，内存DC，遮罩DC，缓冲DC
HBITMAP g_bmpMask, g_bmpBuffer; //遮罩位图，缓冲位图

//按键状态
BOOL LEFT_PRESSED, RIGHT_PRESSED, UP_PRESSED, DOWN_PRESSED;
BOOL FIRE_PRESSED;

int WINAPI WinMain( IN HINSTANCE hInstance, IN HINSTANCE hPrevInstance, IN LPSTR lpCmdLine, IN int nShowCmd )
{
	//LEAP MOTION
	SampleListener listener;
	Leap::Controller controller;
	controller.addListener(listener);
	controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

	g_hInstance = hInstance;

	MSG msg;

	WNDCLASS wnd;
	wnd.cbClsExtra = 0;
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_ARROW));
	wnd.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QUESTION));
	wnd.hInstance = hInstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = "WinClass";
	wnd.lpszMenuName = NULL;
	wnd.style = 0;

	if (!RegisterClass(&wnd))
		return 0;

	hWnd = CreateWindow(wnd.lpszClassName, "MyWin", WS_OVERLAPPEDWINDOW&~WS_MAXIMIZEBOX, 0, 0, WindowWd, WindowHt, HWND_DESKTOP, NULL, hInstance, NULL);

	//取得客户区域
	WINDOWINFO wi;
	GetWindowInfo(hWnd, &wi);
	ScreenWidth = wi.rcClient.right - wi.rcClient.left;
	ScreenHeight = wi.rcClient.bottom - wi.rcClient.top;

	SetWindowPos(hWnd, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN)>>1)-(ScreenWidth>>1), (GetSystemMetrics(SM_CYSCREEN)>>1)-(ScreenHeight>>1), 0, 0, SWP_NOSIZE);

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	g_Music.lmciMidi(hWnd);

	//设置随机数种子
	srand(g_Time);

	//获得DC
	g_hdc = GetDC(hWnd);
	g_hdcMem = CreateCompatibleDC(g_hdc);
	g_hdcMask = CreateCompatibleDC(g_hdc);
	g_hdcBuffer = CreateCompatibleDC(g_hdc);

	//创建位图
	g_bmpBuffer = CreateCompatibleBitmap(g_hdc, ScreenWidth, ScreenHeight);

	//把缓冲位图选入缓冲DC
	HPEN oldPen = (HPEN)SelectObject(g_hdcBuffer, g_bmpBuffer);

	//消息循环
	memset(&msg, 0, sizeof(MSG));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	//释放设备资源
	SelectObject(g_hdcBuffer, oldPen);
	DeleteObject(g_bmpBuffer);

	DeleteDC(g_hdcMem);
	DeleteDC(g_hdcMask);
	DeleteDC(g_hdcBuffer);
	ReleaseDC(hWnd, g_hdc);

	UnregisterClass(wnd.lpszClassName, hInstance);

	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (g_Manager.GetGameStatus())
		{
		case S_GAME:
			switch (wParam)
			{
			case VK_UP:
				UP_PRESSED = TRUE;
				break;

			case VK_DOWN:
				DOWN_PRESSED = TRUE;
				break;

			case VK_LEFT:
				LEFT_PRESSED = TRUE;
				break;

			case VK_RIGHT:
				RIGHT_PRESSED = TRUE;
				break;

			case 'z':
			case 'Z':
				FIRE_PRESSED = TRUE;
				break;

			case 'x':
			case 'X':
				if (g_Manager.GetHeroPlane()->GetState() != P_BOOM)
				{
					g_Manager.GetHeroPlane()->BoomStart(true);
				}			
				break;

			case VK_ESCAPE:
				g_GamePause = TRUE;
				//DialogBox(NULL, MAKEINTRESOURCE(IDD_NEWRECORD), NULL, (DLGPROC)DialogProc);
				if (MessageBox(hWnd, "要退回到菜单吗?", NULL, MB_OKCANCEL) == IDOK)
				{
					g_Manager.ClearAll();
					g_Manager.GotoMenu();
				}
				g_GamePause = FALSE;
				break;
			}
			break;

		default:
			g_Manager.KeyProcess(wParam);
			break;
		}
		break;

	case WM_KEYUP:
		switch (g_Manager.GetGameStatus())
		{
		case S_GAME:
			switch (wParam)
			{
			case VK_UP:
				UP_PRESSED = FALSE;
				break;

			case VK_DOWN:
				DOWN_PRESSED = FALSE;
				break;

			case VK_LEFT:
				LEFT_PRESSED = FALSE;
				break;

			case VK_RIGHT:
				RIGHT_PRESSED = FALSE;
				break;

			case 'z':
			case 'Z':
				FIRE_PRESSED = FALSE;
				break;
			}
			break;
		}
		break;

	case WM_TIMER:
		InvalidateRect(hWnd,NULL,TRUE);
		break;

	case WM_CREATE:
		SetTimer(hWnd, 1, 30, (TIMERPROC)Render);
		break;

	case WM_DESTROY:
		PostQuitMessage(WM_QUIT);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK DialogProc( HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		RECT rect;  
		POINT point;  

		GetClientRect(hwndDlg,&rect);
		point.x = (ScreenWidth-(rect.right-rect.left))>>1;
		point.y = (ScreenHeight - (rect.bottom-rect.top))>>1;

		ClientToScreen(hWnd,&point);  

		SetWindowPos(hwndDlg, HWND_TOP, point.x, point.y, 0, 0, SWP_NOSIZE);  
		SetFocus(hwndDlg);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			{
				char buffer[10] = {0};
				GetDlgItemText(hwndDlg, IDC_EDITNAME, buffer, 6);
				g_Manager.AddNewRecord(buffer);
				g_Manager.SetGameStatus(S_RANK);
				EndDialog(hwndDlg,0);
				g_GamePause = FALSE;
			}
			break;

		case IDCANCEL:
			g_Manager.GotoMenu();
			EndDialog(hwndDlg,0);
			g_GamePause = FALSE;
			break;
		}
		break;
	}
	return 0;
}

void Render()
{
	if (!g_GamePause)
	{
		g_Time = (UINT)timeGetTime();
		switch (g_Manager.GetGameStatus())
		{
		case S_MENU:
			g_Manager.ShowMenu();
			break;

		case S_MUSIC:
			g_Manager.ShowMusicRoom();
			break;

		case S_RANK:
			g_Manager.ShowRank();
			break;

		case S_GAME:
			g_Manager.GameScript();
			g_Manager.UpdateGame();
			g_Manager.ShowGame();
			break;

		case S_END:
			PostQuitMessage(WM_QUIT);
			break;
		}
		BitBlt(g_hdc, 0, 0, ScreenWidth, ScreenHeight, g_hdcBuffer, 0, 0, SRCCOPY);

		if (g_Music.lmciMidiStatus() == 0)
		{
			g_Music.lmciMidiRestart();
		}
	}
}