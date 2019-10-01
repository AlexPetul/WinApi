#define CLASS_NAME "MainClass" 
#define WINDOW_NAME "Multiplication Table"
#define MENU_CHANGESIZE_TITLE L"Change Size"
#define FONT_NAME L"Courier New"
#define FILE_PATH "table.txt"
#define FILE_MODE "r"
#define OPENFILE_ERROR_MESSAGE_CAPTION _T("Error")
#define OPENFILE_ERROR_MESSAGE_TEXT _T("File not found")
#define FILE_TABLE_ROWS 16
#define FILE_TABLE_COLUMNS 11
#define COLOR_TABLE RGB(191, 186, 190);


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL ReadFile();
VOID InitializeFont();
VOID PaintTable(HWND);
VOID FindTableParameters(HWND hwnd);
VOID FillTable();