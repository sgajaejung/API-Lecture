// windowBase.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "windowBase.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;								// 현재 인스턴스입니다.
TCHAR szTitle[MAX_LOADSTRING];					// 제목 표시줄 텍스트입니다.
TCHAR szWindowClass[MAX_LOADSTRING];			// 기본 창 클래스 이름입니다.
// TCHAR
// 유니코드 일 때는 wchar_t 타입을 가르킨다.
// 멀티바이트 코드 일 때는 char 타입을 가르킨다.


// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,	// 인스턴스 핸들, 특정 함수를 호출할 때 필요하다.
                     HINSTANCE hPrevInstance, // 쓸일 없음
                     LPTSTR    lpCmdLine, // 커맨드 라인 스트링
                     int       nCmdShow // 윈도우 창이 화면에 나타나는 형태. ex) SW_SHOWNORMAL, SW_MAXIMIZE
					 )
{
	UNREFERENCED_PARAMETER(hPrevInstance); // 참조되지 않는 파라메터가 warning  되는 것을 방지하기 위해.
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 전역 문자열을 초기화합니다.
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WINDOWBASE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 응용 프로그램 초기화를 수행합니다.
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	MSG msg; // 윈도우 메세지 구조체
	HACCEL hAccelTable;
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWBASE));

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
	// 윈도우 클래스 등록
	// 윈도우 메세지 프로시져, 아이콘, 메뉴, 클래스 이름을 등록한다.

	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;		// 윈도우 메세지 프로시져 설정
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWBASE));	// 아이콘 설정
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);	// 커서 설정
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WINDOWBASE); // 메뉴 설정
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)); // 아이콘 설정

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

   // 창을 생성합니다.
   hWnd = CreateWindow(
	   szWindowClass, 
	   szTitle,		// 창 이름 설정
	   WS_OVERLAPPEDWINDOW, // 창 타입 설정
      CW_USEDEFAULT,	// 창 위치 X
	  0, // 창 위치 Y
	  CW_USEDEFAULT,  // 창 폭
	  0, // 창 높이
	  NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow); // 창을 화면에 보이게 한다.
   UpdateWindow(hWnd); // 창 화면 갱신
   
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
	PAINTSTRUCT ps; // 페인트 구조체
	HDC hdc; // Device Context 핸들

	// WM_~  WindowMessage 의 약자.
	switch (message)
	{
	case WM_COMMAND:		// 메뉴를 선택하면 발생되는 이벤트
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴 선택을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	// 화면 갱신 메세지
	case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);	// Device Context 핸들 얻음.
			
			TextOutA( hdc, 10, 10, "hello World", 11);
			RECT r;
			SetRect(&r, 100, 100, 200, 200);
			DrawTextA( hdc, "hello World", 11, &r, DT_CENTER);

			EndPaint(hWnd, &ps);		// 화면 갱신 종료
		}
		break;

	// 윈도우 종료 메세지
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// 그 밖에 나머지 메세지는 DefWindowProc() 함수에서 처리한다.
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
