#include <Windows.h>
#include <tchar.h>
#include <cmath>
#include <cstdlib>
#include <type_traits>
#define M_PI           3.14159265358979323846
LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
TCHAR WinName[] = "MainFrame";

class Window {
public:
	HWND hwnd;
	HWND buttonStop;
	MSG msg;
	WNDCLASS wc;

	bool reg_window(HINSTANCE hInstance, LPCWSTR lpzClassName, WNDPROC lpfnWndProc) {
		wc.hInstance = hInstance;
		wc.lpszClassName = WinName;
		wc.lpfnWndProc = WndProc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszMenuName = NULL;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)(CTLCOLOR_MAX);

		if (!RegisterClass(&wc))
		{
			return 0;
		}
	}

	void create_window(HINSTANCE hInstance) {
		hwnd = CreateWindow(WinName, "LAB 1", WS_OVERLAPPEDWINDOW,
			200, 300, 400, 300, HWND_DESKTOP, NULL, hInstance, NULL);
	}

	void create_button(HINSTANCE hInstance) {
		buttonStop = CreateWindow("Button", "*", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			0, 0, 20, 20, hwnd, (HMENU)666, hInstance, NULL);
	}

	void show_window() {
		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);
	}

};

class Rect {
public:
	RECT rc;
	int x = 100, y = 100;
	int R;
	double vx, vy;
	double t;

	void createRect(HWND hwnd) {
		PAINTSTRUCT ps;
		HDC hdc;

		hdc = BeginPaint(hwnd, &ps);
		HBRUSH br, obr;
		br = CreateSolidBrush(RGB(255, 0, 0));
		obr = (HBRUSH)SelectObject(hdc, br);
		Ellipse(hdc, x - R, y - R, x + R, y + R);
		SelectObject(hdc, obr);
		DeleteObject(br);
		EndPaint(hwnd, &ps);
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

Rect r;
RECT& rc = r.rc;
int& x = r.x;
int& y = r.y; 
int& R = r.R; 
double& vx = r.vx;
double& vy = r.vy;
double& t = r.t;
double prevVx;
double prevVy;
int wheelDir;

void SaveState(double vx, double vy) {
	prevVx = vx;
	prevVy = vy;
}

double* GetPrevState() {
	double state[2] = { prevVx, prevVy };
	return state;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND:
		if (LOWORD(wParam) == 666) {
			if ((vx != 0) && (vy != 0)) {
				SaveState(vx, vy);
				vx = 0;
				vy = 0;
			}
			else {
				double* state = GetPrevState();
				vx = state[0];
				vy = state[1];
			}
		}
		break;
	case WM_MOUSEWHEEL:
		vx = 0;
		wheelDir = (SHORT)HIWORD(wParam);
		if (wheelDir > 0) {
			if (GetKeyState(VK_SHIFT) < 0) {
				vx = 6.0;
				vy = 0;
			}
			else {
				vy = 6.0;
			}
		}
		else {
			if (GetKeyState(VK_SHIFT) < 0) {
				vx = -6.0;
				vy = 0;
			}
			else {
				vy = -6.0;
			}
		}
		break;
	case WM_ACTIVATE:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		R = 15; 
		t = (rand() % 360 - 180) * M_PI / 180.0;
		vx = 10.0 * cos(t);
		vy = 10.0 * sin(t);
		SetTimer(hwnd, 1, 20, NULL);
		break;

	case WM_TIMER:
	{
		InvalidateRgn(hwnd, NULL, true);
		x += vx; y += vy; 
		RECT rt;
		GetClientRect(hwnd, &rt);
		int w = rt.right;
		int h = rt.bottom;
		if (x >= w - R)vx = -abs(vx); //
		if (y > h - R)vy = -abs(vy);
		if (x < R)vx = abs(vx); 
		if (y < R)vy = abs(vy); 

		rc.left = x - R - 1;
		rc.top = y - R - 1;
		rc.right = x + R + 1;
		rc.bottom = y + R + 1;
		InvalidateRgn(hwnd, NULL, false);
	}
	break;

	case WM_PAINT:
	{
		r.createRect(hwnd);
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