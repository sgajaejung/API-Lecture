// bounceBall.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "bounceBall.h"
#include <objidl.h>
#include <gdiplus.h> 
using namespace Gdiplus;
#pragma comment( lib, "gdiplus.lib" ) 

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

// Gdiplus 용 
GdiplusStartupInput g_gdiplusStartupInput; 
ULONG_PTR g_gdiplusToken;
Graphics *g_graphics;
Pen *g_pen; // 펜 객체.


// 2차원 벡터 구조체.
struct sVector
{
	float x,y;
};
sVector g_ballPos = {300,300}; // 볼 위치
sVector g_ballVel; // 볼 속력

// Gdiplus 초기화.
void InitGdiPlus(HWND hWnd)
{
	//Start Gdiplus 
	Gdiplus::GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL); 
	g_graphics = new Graphics(hWnd);
	g_pen = new Pen(Color::Red);
}

// Gdiplus 제거.
void ReleaseGdiPlus()
{
	delete g_graphics;
	// Shutdown Gdiplus 
	Gdiplus::GdiplusShutdown(g_gdiplusToken); 
}


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
	LoadString(hInstance, IDC_BOUNCEBALL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BOUNCEBALL));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ReleaseGdiPlus(); // Gdiplus 제거.
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BOUNCEBALL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_BOUNCEBALL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   InitGdiPlus(hWnd); // Gdiplus 생성.

   // 초기 Ball 속도 초기화.
   g_ballVel.x = 200;
   g_ballVel.y = 300;

   // 타이머 실행.
   SetTimer(hWnd, 0, 1, NULL);

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

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// 볼위치 g_ballPos 에 원을 출력한다.
		g_graphics->DrawEllipse(g_pen, (int)g_ballPos.x, (int)g_ballPos.y, 100, 100);
		EndPaint(hWnd, &ps);
		break;

	case WM_TIMER:
		{
			// 타이머 이벤트.
			// 1초에 1000번 호출하게 했지만, 실제로는 100번정도 호출 된다.

			// WM_TIMER 이벤트 호출 간격을 계산한다.  (초단위)
			static int oldT = GetTickCount();
			const int curT = GetTickCount();
			const int elapseT = curT - oldT;
			const float eT = (float)elapseT / 1000.f;
			oldT = curT;

			// 뷰 영역 외곽에 부딪치면 튕겨나가게 한다.
			RECT cr;
			GetClientRect(hWnd, &cr); // 뷰 영역 크기를 얻는다.

			// left, right 충돌 검사, 충돌 했다면 x축 속도 반전.
			if (((g_ballVel.x < 0) && (cr.left > (int)g_ballPos.x)) || 
				((g_ballVel.x > 0) && (cr.right < (int)g_ballPos.x)))
				g_ballVel.x = -g_ballVel.x;

			// top, bottom 충돌 검사, 충돌 했다면 y축 속도 반전.
			if (((g_ballVel.y < 0) && (cr.top > (int)g_ballPos.y)) || 
				((g_ballVel.y  > 0) && (cr.bottom < (int)g_ballPos.y)))
				g_ballVel.y = -g_ballVel.y;

			// 속도 * 경과시간 = 이동 거리.
			// 볼위치 g_ballPos 에 이동 거리 만큼 값을 추가한다.
			g_ballPos.x += g_ballVel.x * eT;
			g_ballPos.y += g_ballVel.y * eT;

			// 화면 리프레쉬
			::InvalidateRect(hWnd, NULL, TRUE);
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

