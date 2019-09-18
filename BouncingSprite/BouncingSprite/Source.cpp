#include <Windows.h>
#include <cmath>
#include <cstdlib>
#include "resource.h"
#define M_PI 3.14159265358979323846
#define BMP_WIDTH 220
#define BMP_HEIGHT 90
#define DEF_SPEED 5.00

LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class LogoBmp {
private:
	double _xSpeed, _ySpeed;
	double _law;
	int _xCoord, _yCoord;
	BOOL isDragging = FALSE;
public:

	BOOL GetDrag() { return isDragging; }

	void SetDrag(BOOL flag) { isDragging = flag; }

	void SetXCoord(int x) { _xCoord = x; }

	int GetXCoord() { return _xCoord; }

	void SetYCoord(int y) { _yCoord = y; }

	int GetYCoord() { return _yCoord; }

	double GetLaw() { return _law; }

	void SetLaw(double law) { _law = law; }

	double GetXSpeed() { return _xSpeed; }

	void SetXSpeed(double xSpeed) { this->_xSpeed = xSpeed; }

	double GetYSpeed() { return _ySpeed; }

	void SetYSpeed(double ySpeed) { this->_ySpeed = ySpeed; }
};

LogoBmp logo;
RECT rectBmp, rt, rectClient, criticalRgn;
HBITMAP hBitmap, hOldBmp;
POINT cursorPos;
BITMAP qBitmap;
HDC hdc, hLocalDC;
PAINTSTRUCT ps;
int wheelDir = 0;
int xCrd, yCrd;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	HWND _hwnd;
	MSG _msg;
	WNDCLASSEX _wc;

	_wc.cbSize = sizeof(WNDCLASSEX);
	_wc.style = CS_DBLCLKS;
	_wc.lpfnWndProc = WndProc;
	_wc.cbClsExtra = 0;
	_wc.cbWndExtra = 0;
	_wc.hInstance = hInstance;
	_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	_wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	_wc.lpszMenuName = NULL;
	_wc.lpszClassName = "MainFrame";
	_wc.hIconSm = _wc.hIcon;

	RegisterClassEx(&_wc);
	_hwnd = CreateWindow("MainFrame", "LAB 1", WS_OVERLAPPEDWINDOW^WS_THICKFRAME,
		0, 0, 800, 700, NULL, NULL, hInstance, NULL);
	ShowWindow(_hwnd, nCmdShow);
	UpdateWindow(_hwnd);

	while (GetMessage(&_msg, NULL, 0, 0))
	{
		TranslateMessage(&_msg); 
		DispatchMessage(&_msg); 
	}
	return (int)_msg.wParam;
}

void RecountCriticalRgn(RECT& rect) {
	criticalRgn.left = BMP_WIDTH / 2;
	criticalRgn.top = BMP_HEIGHT / 2;
	criticalRgn.right = rt.right - BMP_WIDTH / 2;
	criticalRgn.bottom = rt.bottom - BMP_HEIGHT / 2;
}

void fillRectBmp(RECT &rect) {
	rect.top = logo.GetYCoord();
	rect.left = logo.GetXCoord();
	rect.bottom = logo.GetYCoord() + BMP_HEIGHT;
	rect.right = logo.GetXCoord() + BMP_WIDTH;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CREATE:
		GetClientRect(hwnd, &rt);
		hBitmap = (HBITMAP)LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));
		break;
	case WM_MOUSEWHEEL:
		logo.SetXSpeed(0);
		wheelDir = (SHORT)HIWORD(wParam);
		if (wheelDir > 0) {
			if (GetKeyState(VK_SHIFT) < 0) {
				logo.SetXSpeed(DEF_SPEED);
				logo.SetYSpeed(0);
			}
			else {
				logo.SetYSpeed(DEF_SPEED);
			}
		}
		else {
			if (GetKeyState(VK_SHIFT) < 0) {
				logo.SetXSpeed(-DEF_SPEED);
				logo.SetYSpeed(0);
			}
			else {
				logo.SetYSpeed(-DEF_SPEED);
			}
		}
		break;
	case WM_KEYDOWN:
		if (wParam == VK_UP) {
			if (logo.GetYCoord() <= 0) {
				logo.SetYSpeed(0);
			}
			else {
				logo.SetXSpeed(0);
				logo.SetYSpeed(-DEF_SPEED);
			}
		}
		else if (wParam == VK_DOWN) {
			GetClientRect(hwnd, &rt);
			if (logo.GetYCoord() >= rt.bottom - BMP_HEIGHT) {
				logo.SetYSpeed(0);
			}
			else {
				logo.SetXSpeed(0);
				logo.SetYSpeed(DEF_SPEED);
			}
		}
		else if (wParam == VK_LEFT) {
			if (logo.GetXCoord() <= 0) {
				logo.SetXSpeed(0);
			}
			else {
				logo.SetYSpeed(0);
				logo.SetXSpeed(-DEF_SPEED);
			}
		}
		else if (wParam == VK_RIGHT) {
			GetClientRect(hwnd, &rt);
			if (logo.GetXCoord() >= rt.right - BMP_WIDTH) {
				logo.SetXSpeed(0);
			}
			else {
				logo.SetYSpeed(0);
				logo.SetXSpeed(DEF_SPEED);
			}
		}
		break;
	case WM_CHAR:
		if (wParam == 's') {
			if ((logo.GetYSpeed() == 0) && (logo.GetXSpeed() == 0)) {
				logo.SetXSpeed(DEF_SPEED * cos(logo.GetLaw()));
				logo.SetYSpeed(DEF_SPEED * sin(logo.GetLaw()));
			}
			else {
				logo.SetXSpeed(0);
				logo.SetYSpeed(0);
			}
		}
		break;
	case WM_KEYUP:
		if (wParam == VK_UP || wParam == VK_DOWN) {
			logo.SetYSpeed(0);
		}
		else if (wParam == VK_LEFT || wParam == VK_RIGHT) {
			logo.SetXSpeed(0);
		}
		break;
	case WM_ACTIVATE:
		logo.SetXCoord(LOWORD(lParam));
		logo.SetYCoord(HIWORD(lParam));
		logo.SetLaw((rand() % 360 - 180) * M_PI / 180.0);
		logo.SetXSpeed(DEF_SPEED * cos(logo.GetLaw()));
		logo.SetYSpeed(DEF_SPEED * sin(logo.GetLaw()));
		SetTimer(hwnd, 1, 15, NULL);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&cursorPos);
		fillRectBmp(rectBmp);
		if (PtInRect(&rectBmp, cursorPos)) {
			SetCapture(hwnd);
			logo.SetDrag(TRUE);
		}
		break;
	case WM_MOUSEMOVE:
		if ((logo.GetDrag()) && (GetKeyState(VK_LBUTTON) < 0)) {
			GetCursorPos(&cursorPos);
			InvalidateRgn(hwnd, NULL, TRUE);
			logo.SetXCoord(cursorPos.x - BMP_WIDTH / 2);
			logo.SetYCoord(cursorPos.y - BMP_HEIGHT / 2);
			GetClientRect(hwnd, &rt);

			xCrd = logo.GetXCoord();
			yCrd = logo.GetYCoord();
			
			if ((xCrd >= rt.right - BMP_WIDTH) || (yCrd > rt.bottom - BMP_HEIGHT) ||
				(xCrd <= 0) || (yCrd <= 0)) {
				ClipCursor(&criticalRgn);
			}
			break;
	case WM_LBUTTONUP:
		ReleaseCapture();
		ClipCursor(&rt);
		if (logo.GetDrag()) {
			logo.SetDrag(FALSE);
		}
		break;
	case WM_SIZE:
		GetClientRect(hwnd, &rt);
		RecountCriticalRgn(criticalRgn);
		break;
	case WM_TIMER:
	{
		InvalidateRgn(hwnd, NULL, FALSE);
		logo.SetXCoord(logo.GetXCoord() + logo.GetXSpeed());
		logo.SetYCoord(logo.GetYCoord() + logo.GetYSpeed());
		xCrd = logo.GetXCoord();
		yCrd = logo.GetYCoord();
		if (xCrd >= rt.right - BMP_WIDTH) {
			logo.SetXSpeed(-abs(logo.GetXSpeed()));
		}
		else if (yCrd > rt.bottom - BMP_HEIGHT) {
			logo.SetYSpeed(-abs(logo.GetYSpeed()));
		}
		else if (xCrd <= 0) {
			logo.SetXSpeed(abs(logo.GetXSpeed()));
		}
		else if (yCrd <= 0) {
			logo.SetYSpeed(abs(logo.GetYSpeed()));
		}
	}
	break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		hLocalDC = CreateCompatibleDC(hdc);
		GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
		hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
		BitBlt(hdc, logo.GetXCoord(), logo.GetYCoord(), qBitmap.bmWidth, qBitmap.bmHeight,
			hLocalDC, 0, 0, SRCCOPY);
		SelectObject(hLocalDC, hOldBmp);
		DeleteDC(hLocalDC);
		DeleteObject(hOldBmp);
		EndPaint(hwnd, &ps);
	}
	break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		}
	}
}