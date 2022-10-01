#include <windows.h>
#include <tchar.h>

int sx = 100;
int sy = 10;
int checkUP = 0;
int checkClickOfMouse = 1;
int x_end = 500;
int y_end = 500;
HWND HMainWindow;
HINSTANCE hInstance;
HANDLE image;

void DrawRectangle(HWND hWnd, int sx, int sy, int checkClickOfMouse) {
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hWnd, &ps);
	HBRUSH hbrush = CreateSolidBrush(RGB(7, 135, 41));
	HPEN hPen = CreatePen(PS_SOLID, 15, RGB(207, 14, 14));
	SelectObject(hdc, hbrush);
	SelectObject(hdc, hPen);
	if (checkClickOfMouse == 1)
		Ellipse(hdc, sx, sy, sx + 50, sy + 50);
	else
		Rectangle(hdc, sx, sy, sx + 50, sy + 50);	
	EndPaint(hWnd, &ps);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");
int APIENTRY _tWinMain(HINSTANCE This,
	HINSTANCE Prev,
	LPTSTR cmd,
	int mode)
{
	HWND hWnd;
	MSG msg;
	WNDCLASS wc;
	hInstance = This;
	wc.hInstance = This;
	wc.lpszClassName = WinName;
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hIcon = LoadIcon(NULL, IDI_QUESTION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) return 0;

	hWnd = CreateWindow(WinName,
		_T("Lab_1"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		500,
		500,
		HWND_DESKTOP,
		NULL,
		This,
		NULL);
	ShowWindow(hWnd, mode);
	HMainWindow = hWnd;
	image = LoadImage(
		hInstance,
		L"photo.bmp",
		IMAGE_BITMAP,
		50,
		50,
		LR_DEFAULTCOLOR
	);
	auto a = GetLastError();
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		x_end = LOWORD(lParam);
		y_end = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		checkClickOfMouse = -checkClickOfMouse;
		InvalidateRect(HMainWindow, 0, TRUE);
		UpdateWindow(HMainWindow);
		DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
		break;
	case WM_KEYUP:
		checkUP = 0;
		break;
	case WM_MOUSEWHEEL:
		if ((short)HIWORD(wParam) > 0 && !(wParam & MK_SHIFT)) {
			if (sy > 0)
				sy -= 20;
			else
				sy += 20;
			DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
			InvalidateRect(HMainWindow, 0, TRUE);
			UpdateWindow(HMainWindow);
		}
		else if ((short)HIWORD(wParam) < 0 && !(wParam & MK_SHIFT)) {
			if (sy + 50 < y_end)
				sy += 20;
			else
				sy -= 20;
			DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
			InvalidateRect(HMainWindow, 0, TRUE);
			UpdateWindow(HMainWindow);
		}
		if ((short)HIWORD(wParam) > 0 && wParam & MK_SHIFT) {
			if (sx > 0)
				sx -= 20;
			else
				sx += 20;
			DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
			InvalidateRect(HMainWindow, 0, TRUE);
			UpdateWindow(HMainWindow);
		}
		else if ((short)HIWORD(wParam) < 0 && wParam & MK_SHIFT) {
			if (sx + 70 < x_end)
				sx += 20;
			else
				sx -= 20;
			DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
			InvalidateRect(HMainWindow, 0, TRUE);
			UpdateWindow(HMainWindow);
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_UP) {

			if (checkUP) {
				sy += 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sy > y_end)
					checkUP = 0;
			}
			else {

				sy -= 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sy <= 0) {
					checkUP = 1;
				}
			}
		}
		if (wParam == VK_DOWN) {
			if (checkUP) {
				sy -= 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sy < 0)
					checkUP = 0;
			}
			else {
				sy += 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sy + 50 > y_end) {
					checkUP = 1;
				}
			}
		}
		if (wParam == VK_LEFT) {
			if (checkUP) {
				sx += 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sx >= x_end) {
					checkUP = 0;
				}
			}
			else {

				sx -= 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sx <= 0) {
					checkUP = 1;
				}
			}
		}
		if (wParam == VK_RIGHT) {
			if (checkUP) {
				sx -= 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sx <= 0) {
					checkUP = 0;
				}
			}
			else {
				sx += 20;
				DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
				InvalidateRect(HMainWindow, 0, TRUE);
				UpdateWindow(HMainWindow);
				if (sx + 70 > x_end) {
					checkUP = 1;
				}
			}
		}
		break;
	case WM_PAINT:
		DrawRectangle(HMainWindow, sx, sy, checkClickOfMouse);
		break;
	case WM_DESTROY: PostQuitMessage(0);
		break;
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}