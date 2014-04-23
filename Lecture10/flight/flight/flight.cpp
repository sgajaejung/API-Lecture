// flight.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "flight.h"
#include <objidl.h>
#include <gdiplus.h> 
using namespace Gdiplus;
#pragma comment( lib, "gdiplus.lib" ) 

#include <vector>
#include "particle.h"
using std::vector;

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.

// Gdiplus 용 
GdiplusStartupInput g_gdiplusStartupInput; 
ULONG_PTR g_gdiplusToken;
Graphics *g_graphics;
Pen *g_pen; // 펜 객체.
Brush *g_brush; // 브러쉬 객체.
Brush *g_bulletBrush;
Bitmap *g_bmp;
Image *g_bg;
Rect g_hero(0, 0, 50, 50);
vector<Rect> g_bullets(2);
int g_front = 0;
int g_back = 0;
cParticleManager g_particleMng;

enum GAME_MODE
{
	SHOOTING,
	RACING,
};
GAME_MODE g_mode = RACING;


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


// Gdiplus 초기화.
void InitGdiPlus(HWND hWnd)
{
	//Start Gdiplus 
	Gdiplus::GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL); 
	g_graphics = new Graphics(hWnd);
	g_pen = new Pen(Color::Red);
	g_brush = new SolidBrush(Color::White);
	g_bulletBrush = new SolidBrush(Color::Red);

	RECT cr;
	GetClientRect(hWnd, &cr);
	g_bmp = new Bitmap(cr.right-cr.left, cr.bottom-cr.top);

	g_bg = Image::FromFile(L"bg.jpg");
}

// Gdiplus 제거.
void ReleaseGdiPlus()
{
	delete g_bmp;
	delete g_bg;
	delete g_bulletBrush;
	delete g_pen;
	delete g_brush;
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
	LoadString(hInstance, IDC_FLIGHT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FLIGHT));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	ReleaseGdiPlus();
	return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
//  설명:
//
//    Windows 95에서 추가된 'RegisterClassEx' 함수보다 먼저
//    해당 코드가 Win32 시스템과 호환되도록
//    하려는 경우에만 이 함수를 사용합니다. 이 함수를 호출해야
//    해당 응용 프로그램에 연결된
//    '올바른 형식의' 작은 아이콘을 가져올 수 있습니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FLIGHT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL; //MAKEINTRESOURCE(IDC_FLIGHT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
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

   InitGdiPlus(hWnd);

   SetTimer(hWnd, 0, 1, NULL);

   RECT cr;
   GetClientRect(hWnd, &cr);
   const int x = (cr.right-cr.left) / 2;
   const int y = cr.bottom - g_hero.Height - 1;
   g_hero.Offset(x, y);

   if (SHOOTING == g_mode)
   {
	   for (int i=0; i < (int)g_bullets.size(); ++i)
	   {
		   int xx = rand() % cr.right;
		   g_bullets[ i] = Rect(xx, 0, 10, 30);
	   }
   }
   else
   {
	   g_bullets[ 0] = Rect(cr.right/2 - 100, 0, 10, 30);
	   g_bullets[ 1] = Rect(cr.right/2 + 100, 0, 10, 30);
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND	- 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT	- 주 창을 그립니다.
//  WM_DESTROY	- 종료 메시지를 게시하고 반환합니다.
//
//
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

			RECT cr;
			GetClientRect(hWnd, &cr);
			Rect wndSize(cr.left, cr.top, cr.right, cr.bottom);

			Graphics *graph = Graphics::FromImage(g_bmp);
			//graph->FillRectangle(g_brush, wndSize);
			graph->DrawImage(g_bg, wndSize);

			for (int i=0; i < (int)g_bullets.size(); ++i)
				graph->FillRectangle(g_bulletBrush, g_bullets[ i]);

			graph->FillRectangle(g_brush, g_hero);
			g_graphics->DrawImage(g_bmp, wndSize);

			g_particleMng.Render(hdc);
			EndPaint(hWnd, &ps);
		}
		break;

	//case WM_KEYDOWN:
	//	switch (wParam)
	//	{
	//	//case VK_LEFT: g_Hero.Offset(-10, 0); break;
	//	//case VK_RIGHT: g_Hero.Offset(10, 0); break;
	//	case VK_UP:
	//	case VK_DOWN:
	//		break;
	//	}
	//	::InvalidateRect(hWnd, NULL, TRUE);
	//	break;

	case WM_TIMER:
		{
			RECT cr;
			GetClientRect(hWnd, &cr);

			if (SHOOTING == g_mode)
			{
				for (int i=0; i < (int)g_bullets.size(); ++i)
				{
					g_bullets[ i].Offset(0,10);
					if (g_bullets[ i].Y > cr.bottom)
					{
						g_bullets[ i].Offset(-g_bullets[ i].X,-g_bullets[ i].Y);
						g_bullets[ i].Offset(rand()%cr.right, 0);
					}
				}
			}
			else if (RACING == g_mode)
			{
				for (int i=0; i < (int)g_bullets.size(); ++i)
				{
					g_bullets[ i].Offset(0,10);
				}

				if (g_bullets[ g_back].Y > 20)
				{
					Rect r1 = g_bullets[ g_back];
					Rect r2 = g_bullets[ g_back+1];
					const int offset = (rand() % 40) - 20;
					r1.Offset(offset, -r1.Y);
					r2.Offset(offset, -r2.Y);

					if (cr.bottom < g_bullets[ g_front].Y)
					{
						g_bullets[ g_front] = r1;
						g_bullets[ g_front+1] = r2;

						g_back = g_front;

						g_front+=2;
						g_front %= g_bullets.size();
					}
					else
					{
						g_bullets.push_back(r1);
						g_bullets.push_back(r2);
						g_back += 2;
					}
				}
			}

			bool isRefresh = false;
			if ((GetKeyState(VK_LEFT) & 0x100) != 0)
			{
				isRefresh = true;
				g_hero.Offset(-20, 0);
			}
			if ((GetKeyState(VK_RIGHT) & 0x100) != 0)
			{
				isRefresh = true;
				g_hero.Offset(20, 0);
			}

			for (int i=0; i < (int)g_bullets.size(); ++i)
			{
				if (g_bullets[ i].IntersectsWith(g_hero))
				{// collision
					POINT pos = {g_bullets[ i].X, g_bullets[i].Y};
					g_particleMng.AddParticle( new cParticle(pos) );
				}
			}

			static int oldT = GetTickCount();
			const int curT = GetTickCount();
			const int elapseT = curT - oldT;
			oldT = curT;

			g_particleMng.Move(elapseT);
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
