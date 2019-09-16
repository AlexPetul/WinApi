#include <Windows.h>
#include <cmath>
#include <cstdlib>
#define M_PI           3.14159265358979323846

LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

class Window {
private:
	HWND _hwnd;
	HWND _buttonStop;
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

	void create_button(HINSTANCE hInstance) {
		_buttonStop = CreateWindow("Button", "*", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			0, 0, 20, 20, _hwnd, (HMENU)666, hInstance, NULL);
	}

	void show_window() {
		ShowWindow(_hwnd, SW_SHOW);
		UpdateWindow(_hwnd);
	}

};

class LogoBmp {
private:
	double _xSpeed, _ySpeed;
	double _xPrevSpeed, _yPrevSpeed;
	double _law;
	int _xCoord, _yCoord;
public:

	void SetXCoord(int x) { _xCoord = x; }

	int GetXCoord() { return _xCoord; }

	void SetYCoord(int y) { _yCoord = y; }

	int GetYCoord() { return _yCoord; }

	double GetLaw() { return _law; }

	void SetLaw(double law) {
		_law = law;
	}

	void SaveCurrSpeed() {
		_xPrevSpeed = _xSpeed;
		_yPrevSpeed = _ySpeed;
	}

	double* GetPrevSpeed() {
		double state[2] = { _xPrevSpeed, _yPrevSpeed };
		return state;
	}

	double GetXSpeed() {
		return _xSpeed;
	}

	void SetXSpeed(double xSpeed) {
		this->_xSpeed = xSpeed;
	}

	double GetYSpeed() {
		return _ySpeed;
	}

	void SetYSpeed(double ySpeed) {
		this->_ySpeed = ySpeed;
	}

	void createRect(HWND hwnd) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		HBITMAP hBitmap;
		hBitmap = (HBITMAP)LoadImage(NULL, "nike_logo.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
	myWin.create_button(hInstance);
	myWin.show_window();

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg); 
		DispatchMessage(&msg); 
	}
	return 0;
}

LogoBmp logo;

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int wheelDir = 0;
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wParam) == 666) {
			if ((logo.GetXSpeed() != 0) && (logo.GetYSpeed() != 0)) {
				logo.SaveCurrSpeed();
				logo.SetXSpeed(0);
				logo.SetYSpeed(0);
			}
			else {
				double* state = logo.GetPrevSpeed();
				logo.SetXSpeed(state[0]);
				logo.SetYSpeed(state[1]);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		logo.SetXSpeed(0);
		wheelDir = (SHORT)HIWORD(wParam);
		if (wheelDir > 0) {
			if (GetKeyState(VK_SHIFT) < 0) {
				logo.SetXSpeed(6.0);
				logo.SetYSpeed(0);
			}
			else {
				logo.SetYSpeed(6.0);
			}
		}
		else {
			if (GetKeyState(VK_SHIFT) < 0) {
				logo.SetXSpeed(-6.0);
				logo.SetYSpeed(0);
			}
			else {
				logo.SetYSpeed(-6.0);
			}
		}
		break;
	case WM_ACTIVATE:
		logo.SetXCoord(LOWORD(lParam));
		logo.SetYCoord(HIWORD(lParam));
		logo.SetLaw((rand() % 360 - 180) * M_PI / 180.0);
		logo.SetXSpeed(10.0 * cos(logo.GetLaw()));
		logo.SetYSpeed(10.0 * sin(logo.GetLaw()));
		SetTimer(hwnd, 1, 20, NULL);
		break;
	case WM_TIMER:
	{
		InvalidateRgn(hwnd, NULL, true);
		logo.SetXCoord(logo.GetXCoord() + logo.GetXSpeed()); 
		logo.SetYCoord(logo.GetYCoord() + logo.GetYSpeed()); 
		RECT rt;
		GetClientRect(hwnd, &rt);
		int w = rt.right;
		int h = rt.bottom;
		if (logo.GetXCoord() >= w - 20) {
			logo.SetXSpeed(-abs(logo.GetXSpeed()));
		}
		if (logo.GetYCoord() > h - 20) {
			logo.SetYSpeed(-abs(logo.GetYSpeed()));
		}
		if (logo.GetXCoord() < 20) {
			logo.SetXSpeed(abs(logo.GetXSpeed()));
		}
		if (logo.GetYCoord() < 20) {
			logo.SetYSpeed(abs(logo.GetYSpeed()));
		}
		InvalidateRgn(hwnd, NULL, false);
	}
	break;
	case WM_PAINT:
	{
		logo.createRect(hwnd);
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