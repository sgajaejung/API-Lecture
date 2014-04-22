// mineFinder.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "mineFinder.h"
#include "mineTable.h"
#include <MMSystem.h>
#include <time.h>
#include "particle.h"


HWND g_hWnd;			// 전역 윈도우 핸들
cMineTable g_table;		// 지뢰 테이블 전역 변수
cParticleManager g_particleMng; // 파티클 관리자 (중요하지 않음)


#define MAX_LOADSTRING 100

HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: 여기에 코드를 입력합니다.
	MSG msg;
	HACCEL hAccelTable;

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MINEFINDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	srand((int)time(NULL)); // 랜덤 씨앗값 초기화
	g_table.Init();	// 지뢰 위치 설정

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	SetTimer(g_hWnd, 0, 1, NULL); // 파티클 애니메이션을 위해서 타이머를 생성함.

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINEFINDER));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINEFINDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MINEFINDER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   g_hWnd = hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			RECT rc;
			GetClientRect(hWnd, &rc);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rc.right-rc.left, rc.bottom-rc.top);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);
			HBRUSH hbrBkGnd = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
			FillRect(hdcMem, &rc, hbrBkGnd);
			DeleteObject(hbrBkGnd);

			g_table.Render(hdcMem);
			g_particleMng.Render(hdcMem);

			BitBlt(hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hdcMem, 0, 0, SRCCOPY);
			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);
		}
		break;

	case WM_LBUTTONUP:
		{
			POINT pos = {LOWORD(lParam),HIWORD(lParam)};
			if (g_table.Click(pos))
				InvalidateRect(hWnd, NULL, TRUE);

			if((GetKeyState(VK_RBUTTON) & 0x100) != 0)
				if (g_table.Erase(pos))
					InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_RBUTTONUP:
		{
			POINT pos = {LOWORD(lParam),HIWORD(lParam)};
			if (g_table.Check(pos))
				InvalidateRect(hWnd, NULL, TRUE);

			if((GetKeyState(VK_LBUTTON) & 0x100) != 0)
				if (g_table.Erase(pos))
					InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_KEYDOWN:
		if (wParam == VK_RETURN)
		{
			g_table.Init();
			InvalidateRect(hWnd, NULL, TRUE);
		}
		if (wParam == VK_SPACE)
		{
			g_table.Init(false);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_TIMER:
		{
			static int oldT = GetTickCount();
			const int curT = GetTickCount();
			const int elapseT = curT - oldT;
			if (g_particleMng.Move(elapseT))
				::InvalidateRect(hWnd, NULL, TRUE);

			oldT = curT;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

