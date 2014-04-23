// bounceBall.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "bounceBall.h"
#include <objidl.h>
#include <gdiplus.h> 
#include <math.h>
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
Brush *g_brush; // 브러쉬 객체.
Bitmap *g_bmp;

enum {
	WIDTH = 50,
	MAX_BALL = 10,
};

// 2차원 벡터 구조체.
struct sVector
{
	float x, y;
	sVector() {}
	sVector(float _x, float _y) : x(_x), y(_y) {}
	sVector& operator=(const sVector &rhs) // 할당 연산자.
	{
		if (this != &rhs)
		{
			x = rhs.x;
			y = rhs.y;
		}
		return *this;
	}
	sVector operator-(const sVector &rhs) const // - 연산자.
	{
		sVector v;
		v.x = x - rhs.x;
		v.y = y - rhs.y;
		return v;
	}
	sVector operator+(const sVector &rhs) const // + 연산자
	{
		sVector v;
		v.x = x + rhs.x;
		v.y = y + rhs.y;
		return v;
	}
	sVector operator*(const sVector &rhs) const // * 연산자
	{
		sVector v;
		v.x = x * rhs.x;
		v.y = y * rhs.y;
		return v;
	}
	template<class T>
	sVector operator*(const T &f) const // 스칼라 곱 연산자.
	{
		sVector v;
		v.x = x * f;
		v.y = y * f;
		return v;
	}
	float length() { return (float)sqrt(x*x + y*y); } // 벡터 길이 리턴
	void normalize() // 벡터 정규화 
	{
		const float L = length();
		x /= L;
		y /= L;
	}
	void interpol( const sVector &from, const sVector &to, float f ) // 벡터 보간.
	{
		*this = from*(1.f-f) + to*f;
		normalize();
	}
};


//sVector g_ballPos(300,300); // 볼 위치
//sVector g_ballVel; // 볼 속력

struct sBall
{
	sVector pos;
	sVector vel;
};

sBall g_ball[ MAX_BALL]; // 볼 배열.


// Gdiplus 초기화.
void InitGdiPlus(HWND hWnd)
{
	//Start Gdiplus 
	Gdiplus::GdiplusStartup(&g_gdiplusToken, &g_gdiplusStartupInput, NULL); 
	g_graphics = new Graphics(hWnd);
	g_pen = new Pen(Color::Red);
	g_brush = new SolidBrush(Color::White);

	RECT cr;
	GetClientRect(hWnd, &cr);
	g_bmp = new Bitmap(cr.right-cr.left, cr.bottom-cr.top);
}

// Gdiplus 제거.
void ReleaseGdiPlus()
{
	delete g_bmp;
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
   //g_ballVel.x = 200;
   //g_ballVel.y = 300;

   // 타이머 실행.
   SetTimer(hWnd, 0, 1, NULL);

   // 볼 배열 초기화.
   for (int i=0; i < MAX_BALL; ++i)
   {
	   g_ball[ i].pos = sVector((float)i*WIDTH, (float)i*WIDTH);
	   g_ball[ i].vel = sVector(200,300);
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

			RECT rect;
			GetClientRect(hWnd, &rect);
			RectF destRect(REAL(rect.left), REAL(rect.top), REAL(rect.right-rect.left), REAL(rect.bottom-rect.top));

			Graphics *graph = Graphics::FromImage(g_bmp);
			graph->FillRectangle(g_brush, rect.left, rect.top, rect.right, rect.bottom);

			// 볼위치 g_ballPos 에 원을 출력한다.
			// 볼 출력.
			for (int i=0; i < MAX_BALL; ++i)
				graph->DrawEllipse(g_pen, (int)g_ball[ i].pos.x, (int)g_ball[ i].pos.y, WIDTH, WIDTH);				

			g_graphics->DrawImage(g_bmp, destRect);

			EndPaint(hWnd, &ps);
		}
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

			for (int i=0; i < MAX_BALL; ++i)
			{
				// left, right 충돌 검사, 충돌 했다면 x축 속력 반전.
				if (((g_ball[ i].vel.x < 0) && (cr.left > (int)g_ball[ i].pos.x)) || 
					((g_ball[ i].vel.x > 0) && (cr.right < (int)g_ball[ i].pos.x+WIDTH)))
					g_ball[ i].vel.x = -g_ball[ i].vel.x;

				// top, bottom 충돌 검사, 충돌 했다면 y축 속력 반전.
				if (((g_ball[ i].vel.y < 0) && (cr.top > (int)g_ball[ i].pos.y)) || 
					((g_ball[ i].vel.y  > 0) && (cr.bottom < (int)g_ball[ i].pos.y+WIDTH)))
					g_ball[ i].vel.y = -g_ball[ i].vel.y;

				// 속도 * 경과시간 = 이동 거리.
				// 볼위치 g_ballPos 에 이동 거리 만큼 값을 추가한다.
				g_ball[ i].pos.x += g_ball[ i].vel.x * eT;
				g_ball[ i].pos.y += g_ball[ i].vel.y * eT;

				for (int k=0; k < MAX_BALL; ++k)
				{
					if (i == k)
						continue;

					// 볼 사이 거리가 두 구의 반지름 길이 보다 작다면 충돌 한 것이다. 
					sVector dist = g_ball[ i].pos - g_ball[ k].pos;
					if (WIDTH >= dist.length())
					{// collision
						sVector dir = g_ball[ i].pos - g_ball[ k].pos;
						dir.normalize(); // 볼 간의 방향 벡터 계산,  단위벡터로 초기화.

						// 위치보정 (충돌 위치)
						g_ball[ i].pos = dir * (WIDTH) + g_ball[ k].pos; // 서로 겹치지 않게 위치 보정
						g_ball[ i].vel = dir * g_ball[ i].vel.length(); // 충돌 반사각 업데이트.
					}
				}
			}

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

