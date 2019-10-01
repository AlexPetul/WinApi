#include <windows.h>
#include <tchar.h>
#include <fstream>
#include "table.h"

HINSTANCE hInst;
TCHAR WinClassName[] = _T(CLASS_NAME);
FILE* file;
HDC hdc;
PAINTSTRUCT ps;
RECT cellRt, clientRect;
LPMINMAXINFO lpMMI;
HBRUSH tableBrush;
COLORREF tableColor = COLOR_TABLE;
int windowWidth, windowHeight;
int cellWidth, cellHeight, borderWidth;
int rows = FILE_TABLE_ROWS, columns = FILE_TABLE_COLUMNS;
int table[FILE_TABLE_ROWS][FILE_TABLE_COLUMNS];
LOGFONT lf;
HFONT hFont;
CHAR fnt[32] = "Courier New";


int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	HWND hWnd;
	MSG msg;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_VREDRAW | CS_HREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = wcex.hIcon;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WinClassName;
	RegisterClassEx(&wcex);
	hWnd = CreateWindow(WinClassName, _T(WINDOW_NAME), WS_OVERLAPPEDWINDOW/* | WS_VSCROLL*/, CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, HWND_DESKTOP, NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}


BOOL ReadFile()
{
	std::ifstream in(FILE_PATH);
	if (!in.is_open())
	{
		return false;
	}
	else
	{
		for (int i = 0; i < FILE_TABLE_ROWS; i++)
			for (int j = 0; j < FILE_TABLE_COLUMNS; j++)
				in >> table[i][j];
		in.close();
		return true;
	}
}


VOID InitializeFont()
{
	lf.lfHeight = 20;
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = 0;
	lf.lfUnderline = 0;
	lf.lfStrikeOut = 0;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = VARIABLE_PITCH | FF_MODERN;
	strcpy_s(lf.lfFaceName, fnt);
	hFont = CreateFontIndirect(&lf);
}


VOID FindTableParameters(HWND hwnd)
{
	SelectObject(hdc, hFont);
	GetClientRect(hwnd, &clientRect);
	cellHeight = ceil(clientRect.bottom/16);
	cellWidth = (int)ceil(windowWidth / columns);
	borderWidth = (windowWidth - columns * cellWidth) / 2;
}


VOID PaintTable(HWND hWnd)
{
	FindTableParameters(hWnd);
	for (int rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		for (int colIndex = 0; colIndex < columns; colIndex++)
		{                
			cellRt.left = borderWidth + cellWidth * colIndex;
			cellRt.top = cellHeight * rowIndex;
			cellRt.right = borderWidth + cellWidth * (colIndex + 1);
			cellRt.bottom = cellHeight * (rowIndex + 1);
			FrameRect(hdc, &cellRt, tableBrush);
		}
	}
	FillTable();
}


VOID FillTable()
{
	wchar_t str[4];
	SelectObject(hdc, hFont);
	for (int rowIndex = 0; rowIndex < rows; rowIndex++)
	{
		for (int colIndex = 0; colIndex < columns; colIndex++)
		{
			_itow_s(table[rowIndex][colIndex], str, 10);
			TextOut(hdc, cellWidth * colIndex + (int)floor(cellWidth / 2), cellHeight * rowIndex + 1, (LPTSTR)str, strlen((LPTSTR)str));
		}
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	lpMMI = (LPMINMAXINFO)lParam;
	switch (message)
	{
	case WM_CREATE:
		if (!ReadFile()) 
		{
			MessageBox(hWnd, OPENFILE_ERROR_MESSAGE_TEXT, OPENFILE_ERROR_MESSAGE_CAPTION, MB_OK | MB_ICONHAND);
			ExitProcess(0);
		}
		else
		{
			InitializeFont();
			tableBrush = CreateSolidBrush(tableColor);
		}
		break;
	case WM_SIZE:
		windowWidth = LOWORD(lParam);
		windowHeight = HIWORD(lParam);
		break;
	case WM_GETMINMAXINFO:
		lpMMI->ptMinTrackSize.x = 300;
		lpMMI->ptMinTrackSize.y = 400;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		PaintTable(hWnd);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		DeleteObject(tableBrush);
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}