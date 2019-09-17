#include <Windows.h>
#include <cmath>
#include <cstdlib>
#define M_PI 3.14159265358979323846
#define BMP_WIDTH 220
#define BMP_HEIGHT 80
#define DEF_SPEED 6.00

LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
private:
	HWND _hwnd;
	MSG _msg;
	WNDCLASS _wc;
public:

	bool reg_window(HINSTANCE hInstance, LPCWSTR lpzClassName, WNDPROC lpfnWndProc) {
		_wc.hInstance = hInstance;
		_wc.lpszClassName = "MainFrame";
		_wc.lpfnWndProc = WndProc;
		_wc.style = CS_HREDRAW | CS_VREDRAW;
		_wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		_wc.lpszMenuName = NULL;
		_wc.cbClsExtra = 0;
		_wc.cbWndExtra = 0;
		_wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		if (!RegisterClass(&_wc))
		{
			return 0;
		}
	}

	void create_window(HINSTANCE hInstance) {
		_hwnd = CreateWindow("MainFrame", "LAB 1", WS_OVERLAPPEDWINDOW,
			200, 300, 400, 300, HWND_DESKTOP, NULL, hInstance, NULL);
	}

	void show_window() {
		ShowWindow(_hwnd, SW_SHOW);
		UpdateWindow(_hwnd);
	}

};

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

	void createRect(HWND hwnd, HBITMAP hBitmap) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HDC hLocalDC;
		hLocalDC = CreateCompatibleDC(hdc);
		BITMAP qBitmap;
		int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),
			reinterpret_cast<LPVOID>(&qBitmap));
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
		BOOL qRetBlit = BitBlt(hdc, _xCoord, _yCoord, qBitmap.bmWidth, qBitmap.bmHeight,
			hLocalDC, 0, 0, SRCCOPY);
		SelectObject(hLocalDC, hOldBmp);
		DeleteDC(hLocalDC);
		DeleteObject(hBitmap);
	}
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	Window myWin;

	myWin.reg_window(hInstance, L"MyWindowClass", WndProc);
	myWin.create_window(hInstance);
	myWin.show_window();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return 0;
}

LogoBmp logo;
RECT rectBmp;

void fillRectBmp(RECT& rect) {
	rect.top = logo.GetYCoord();
	rect.left = logo.GetXCoord();
	rect.bottom = logo.GetYCoord() + BMP_HEIGHT;
	rect.right = logo.GetXCoord() + BMP_WIDTH;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wheelDir = 0;
	POINT cursorPos;
	HPEN hpen = CreatePen(PS_DOT, 1, RGB(0, 0, 0));
	RECT rt;
	HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, "nike_logo.bmp", IMAGE_BITMAP, BMP_WIDTH, BMP_HEIGHT, LR_LOADFROMFILE);
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wParam) == 666) {
			if ((logo.GetXSpeed() != 0) || (logo.GetYSpeed() != 0)) {
				logo.SetXSpeed(0);
				logo.SetYSpeed(0);
			}
			else {
				logo.SetXSpeed(DEF_SPEED * cos(logo.GetLaw()));
				logo.SetYSpeed(DEF_SPEED * sin(logo.GetLaw()));
			}
		}
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
		RECT rectClient;
		GetClientRect(hwnd, &rectClient);
		GetCursorPos(&cursorPos);
		ClipCursor(&rectClient);
		fillRectBmp(rectBmp);
		if (PtInRect(&rectBmp, cursorPos)) {
			//SetCapture(hwnd);
			logo.SetDrag(TRUE);
		}
		break;
	case WM_MOUSEMOVE:
		GetCursorPos(&cursorPos);
		if ((logo.GetDrag()) && (GetKeyState(VK_LBUTTON) < 0)) {
			InvalidateRgn(hwnd, NULL, TRUE);
			logo.SetXCoord(cursorPos.x);
			logo.SetYCoord(cursorPos.y);
		}
		break;
	case WM_LBUTTONUP:
		//ReleaseCapture();
		if (logo.GetDrag()) {
			logo.SetDrag(FALSE);
		}
		break;
	case WM_TIMER:
	{
		GetClientRect(hwnd, &rt);
		InvalidateRect(hwnd, NULL, FALSE);
		logo.SetXCoord(logo.GetXCoord() + logo.GetXSpeed()); 
		logo.SetYCoord(logo.GetYCoord() + logo.GetYSpeed()); 	
		int width = rt.right;
		int height = rt.bottom;
		if (logo.GetXCoord() >= width - BMP_WIDTH) {
			logo.SetXSpeed(-abs(logo.GetXSpeed()));
		}
		if (logo.GetYCoord() > height - BMP_HEIGHT) {
			logo.SetYSpeed(-abs(logo.GetYSpeed()));
		}
		if (logo.GetXCoord() <= 0) {
			logo.SetXSpeed(abs(logo.GetXSpeed()));
		}
		if (logo.GetYCoord() <= 0) {
			logo.SetYSpeed(abs(logo.GetYSpeed()));
		}
	}
	break;
	case WM_PAINT:
	{
		logo.createRect(hwnd, hBitmap);
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