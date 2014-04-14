// WindowPenBrush.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WindowPenBrush.h"
#include <math.h>
#include <windowsx.h>
#include <vector>

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
HPEN g_hPen;
HBRUSH g_hBrush;
bool g_lbtnDown = false;
POINT g_downPos;
POINT g_movePos;

struct sLine {
	int state; // 0=down, 1=move, 2=up
	POINT pos;
};
std::vector<sLine> g_Lines; // 선그리기


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

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
	LoadString(hInstance, IDC_WINDOWPENBRUSH, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	//--------------------------------------------------------------------------------------------------
	// 펜 생성.
	g_hPen = CreatePen(
		PS_SOLID,	// 펜 타입 ex) PS_SOLID, PS_DASH, PS_DOT
		1,				// 펜 폭
		RGB(255,0,0) ); // 색깔.


	//--------------------------------------------------------------------------------------------------
	// 브러쉬 생성

	// 영역을 채우는 브러쉬
	//g_hBrush = CreateSolidBrush(RGB(0,0,255)); 

	// 줄선으로 채우는 브러쉬
	g_hBrush = CreateHatchBrush(
		HS_BDIAGONAL, // ex) HS_CROSS, HS_BDIAGONAL ...
		RGB(0,0,255)); 


	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWPENBRUSH));

	// 기본 메시지 루프입니다.
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}


	DeleteObject(g_hPen);

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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWPENBRUSH));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWPENBRUSH);
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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case ID_TEST1:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			SetTextColor(hdc, RGB(255,0,0));	// 텍스트 색깔 지정
			//SetBkColor(hdc, RGB(0,255,0)); // 텍스트 배경 색깔 지정

			TextOutA( hdc, 10, 10, "hello World", 11);	// x,y 위치에 텍스트 출력
			RECT r;
			SetRect(&r, 100, 100, 200, 200);	
			DrawTextA( hdc, "hello World", 11, &r, DT_CENTER); //사각형 위치에  텍스트 출력

			HPEN oldPen = (HPEN)SelectObject(hdc, g_hPen); // Pen 적용

			// 라인 긋기.
			LineTo(hdc, 100, 100);
			LineTo(hdc, 500, 100);
			MoveToEx(hdc, 500, 200, NULL);
			LineTo(hdc, 600, 300);

			HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, g_hBrush); // 브러쉬 적용.

			Rectangle(hdc, 300, 300, 400, 400);	// 박스 출력
			Ellipse(hdc, 400, 300, 500, 400);	// 타원형 출력

			// sin 곡선
			MoveToEx(hdc, 0, 300, NULL);
			for (float i=0; i < 10; i+=0.1f)
			{
				float y = sin(i) * 100;
				LineTo(hdc, (int)(i*100.f), (int)(y + 300.f));
			}

			if (g_lbtnDown)
			{
				//MoveToEx(hdc, g_downPos.x, g_downPos.y, NULL);
				//LineTo(hdc, g_movePos.x, g_movePos.y);
			}

			for (int i=0; i < (int)g_Lines.size(); ++i)
			{
				if (g_Lines[ i].state == 0)
					MoveToEx(hdc, g_Lines[ i].pos.x, g_Lines[ i].pos.y, NULL);
				else if (g_Lines[ i].state == 1)
					LineTo(hdc, g_Lines[ i].pos.x, g_Lines[ i].pos.y);
			}
			
			SelectObject(hdc, oldPen); // 원본 Pen으로 복귀.
			SelectObject(hdc, oldBrush); // 원본 Brush로 복귀
			EndPaint(hWnd, &ps);
		}
		break;

	// 마우스 왼쪽 버튼 다운 이벤트
	case WM_LBUTTONDOWN:
		{
			g_lbtnDown = true;
			g_downPos.x = GET_X_LPARAM(lParam); 
			g_downPos.y = GET_Y_LPARAM(lParam); 

			sLine line;
			line.state = 0;
			line.pos = g_downPos;
			g_Lines.push_back(line);
		}
		break;

	// 마우스 왼쪽 버튼 업 이벤트
	case WM_LBUTTONUP:
		{
			g_lbtnDown = false;

			sLine line;
			line.state = 2;
			g_Lines.push_back(line);
		}
		break;

	// 마우스 이동 이벤트 
	case WM_MOUSEMOVE:
		if (g_lbtnDown)
		{
			POINT pos;
			pos.x = GET_X_LPARAM(lParam); 
			pos.y = GET_Y_LPARAM(lParam); 
			g_movePos = pos;
			InvalidateRect(hWnd, NULL, TRUE);

			sLine line;
			line.state = 1;
			line.pos = pos;
			g_Lines.push_back(line);
		}
		break;

	// 키보드 다운
	case WM_KEYDOWN:
		break;
	
	case WM_ERASEBKGND:
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
