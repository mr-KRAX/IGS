
#define _USE_MATH_DEFINES  
#include <cmath>
#include <map>
#include <tchar.h>
#include <windows.h>

#include "resource.h"

using namespace std;

TCHAR winClassName[] = _T("Class");
TCHAR winTitle[] = _T("Lab1");

// размеры поля вывода в мировой системе координат
double xLeft = -2;
double xRight = 2;
double yBottom = -2;
double yTop = 2;

// размеры окна
int winHeight = 500;
int winWidth = 700;

// размеры поля вывода в пикселях в клиентской области окна приложения
int nLeft = 35;
int nRight = winWidth - 35;
int mBottom = winHeight - 35;
int mTop = 35;

HBRUSH winBgBrush = CreateSolidBrush(RGB(230, 230, 230));
HPEN winBgPen = CreatePen(PS_SOLID, 3, RGB(230, 230, 230));
HFONT winFont;

//переход от x к пикселю n
inline int XToN(double x) {
  return (int)((x - xLeft) / (xRight - xLeft) * (nRight - nLeft)) + nLeft;
}

//переход от y к пикселю m
inline int YToM(double y) {
  return (int)((y - yBottom) / (yTop - yBottom) * (mTop - mBottom)) + mBottom;
}

void InitDefaultObjects() {
  winBgBrush = CreateSolidBrush(RGB(230, 230, 230));
  winBgPen = CreatePen(PS_SOLID, 3, RGB(230, 230, 230));

  LOGFONT lf;
  lf.lfHeight = 22;
  lf.lfWeight = FW_BOLD;
  lf.lfEscapement = 0; // угол наклона в десятых долях градуса
  lf.lfItalic = false;
  lf.lfUnderline = false;
  lf.lfStrikeOut = false;
  wcscpy_s(lf.lfFaceName, L"Courier New");
  winFont = CreateFontIndirect(&lf);
}

void ClearWin(HWND hwnd) {
  HDC hdc = GetDC(hwnd);
  HPEN prevPen = (HPEN)SelectObject(hdc, winBgPen);
  HBRUSH prevBrush = (HBRUSH)SelectObject(hdc, winBgBrush);
  Rectangle(hdc, 0, 0, winWidth + 100, winHeight + 100);

  SelectObject(hdc, prevPen);
  SelectObject(hdc, prevBrush);
  ReleaseDC(hwnd, hdc);
}


void DrawTask2Obj2(HWND hwnd) {
  SetWindowText(hwnd, L"Object 2 - Octogram");

  ClearWin(hwnd);
  HDC hdc = GetDC(hwnd);
  // Устанавливаем кисти
  HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));   // сплошной, белый
  HPEN bgPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));     // 3px, сплошной, ярко-синий
  HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255)); // 1px, сплошной, ярко-голубой
  HPEN objPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
  HBRUSH objBrush = CreateSolidBrush(RGB(0, 230, 255));

  // Отрисовываем фон
  SelectObject(hdc, bgBrush);
  SelectObject(hdc, bgPen);

  int offset = 2;
  Rectangle(hdc, nLeft - offset, mBottom + offset, nRight + offset, mTop - offset);
   // Отрисовываем оси
  POINT pt;
  SelectObject(hdc, axisPen);
  // OX
  MoveToEx(hdc, XToN(xLeft), YToM(0), &pt);
  LineTo(hdc, XToN(xRight), YToM(0));
  // OY
  MoveToEx(hdc, XToN(0), YToM(yBottom), &pt);
  LineTo(hdc, XToN(0), YToM(yTop));

  SelectObject(hdc, objBrush);
  SelectObject(hdc, objPen);
  
  POINT starPoints[16];
  double lMid = winHeight / 3 * sqrt(pow(cos(3*M_PI/8),2)+pow(sin(M_PI/8),2));
  int n = 0;
  for (int i = 0; i < 16; i += 2){
    double phi = M_PI/8+M_PI*(i/2)/4;
    starPoints[i].x = XToN(0) + winHeight / 3 * sin(phi);
    starPoints[i].y = YToM(0) + winHeight / 3 * cos(phi);
    starPoints[i+1].x = XToN(0) + lMid * sin(phi + M_PI / 8);
    starPoints[i+1].y = YToM(0) + lMid * cos(phi + M_PI / 8);
  }
  Polygon(hdc, starPoints, 16);
  TCHAR ss[10];
  SelectObject(hdc, winFont);
  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(0, 0, 0));
  for (int i = 0; i < 16; i+=2) {
    swprintf_s(ss, 10, L"%2d", i/2);

    int x = starPoints[i].x;
    int y = starPoints[i].y;
    x += x == winWidth/2 ? (-5) : (x > winWidth / 2 ? 0 : (-25));
    y += y == winHeight / 2  ? 0 :(y >= winHeight / 2 ? 5 : (-25));
    TextOut(hdc, x, y, ss, 2);

  }
  SetTextColor(hdc, RGB(255,255,255));
  TextOut(hdc, XToN(0) - 45, YToM(0)-10, L"Octagtam", 8);

  DeleteObject(bgBrush);
  DeleteObject(bgPen);
  DeleteObject(axisPen);
  DeleteObject(objPen);
  DeleteObject(objBrush);
  
  ReleaseDC(hwnd, hdc);
}

void DrawTask2Obj1(HWND hwnd) {
  SetWindowText(hwnd, L"Object 1 - Pie");
  ClearWin(hwnd);
  HDC hdc = GetDC(hwnd);
  // Устанавливаем кисти
  HBRUSH bgBrush = CreateSolidBrush(RGB(255, 255, 255));   // сплошной, белый
  HPEN bgPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));     // 3px, сплошной, ярко-синий
  HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255)); // 1px, сплошной, ярко-голубой
  HPEN objPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
  HBRUSH objBrush = CreateSolidBrush(RGB(0, 230, 255));

  // Отрисовываем фон
  SelectObject(hdc, bgBrush);
  SelectObject(hdc, bgPen);

  int offset = 2;
  Rectangle(hdc, nLeft - offset, mBottom + offset, nRight + offset, mTop - offset);

  // Отрисовываем оси
  POINT pt;
  SelectObject(hdc, axisPen);
  // OX
  MoveToEx(hdc, XToN(xLeft), YToM(0), &pt);
  LineTo(hdc, XToN(xRight), YToM(0));
  // OY
  MoveToEx(hdc, XToN(0), YToM(yBottom), &pt);
  LineTo(hdc, XToN(0), YToM(yTop));

  SelectObject(hdc, objBrush);
  SelectObject(hdc, objPen);

  Pie(hdc, XToN(-1.5), YToM(1), XToN(1.5), YToM(-1), XToN(1), YToM(-1), XToN(1), YToM(1));
  SelectObject(hdc, winFont);

  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(0, 0, 0));
  TextOut(hdc, XToN(1) - 15, YToM(0) - 10, L"Pie", 3);

  DeleteObject(bgBrush);
  DeleteObject(bgPen);
  DeleteObject(axisPen);
  DeleteObject(objPen);
  DeleteObject(objBrush);

  ReleaseDC(hwnd, hdc);
}

void DrawTask1Graphs(HWND hwnd) {
  SetWindowText(hwnd, L"f1 = |x|; f2 = 2 - x^2");
  ClearWin(hwnd);
  // PAINTSTRUCT ps;        //структура для работы контеста
  // hdc = BeginPaint(hwnd, &ps);
  HDC hdc = GetDC(hwnd); //дескриптор контекста устройства

  // Устанавливаем кисти
  HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 200));       // сплошной, синий
  HPEN bgPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));   // 3px, сплошной, ярко-желтый
  HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255)); // 1px, сплошной, ярко-голубой
  HPEN f1Pen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));     // 2px, сплошной, ярко-красный
  HPEN f2Pen = CreatePen(PS_SOLID, 2, RGB(0, 225, 0));     // 2px, тире, ярко-красный

  // Отрисовываем фон
  SelectObject(hdc, bgBrush);
  SelectObject(hdc, bgPen);

  int offset = 2;
  Rectangle(hdc, nLeft - offset, mBottom + offset, nRight + offset, mTop - offset);

  // Отрисовываем оси
  int n, m;
  POINT pt;
  SelectObject(hdc, axisPen);

  // OX
  n = XToN(xLeft);
  m = YToM(0);
  MoveToEx(hdc, n, m, &pt);
  n = XToN(xRight);
  m = YToM(0);
  LineTo(hdc, n, m);

  // OY
  n = XToN(0);
  m = YToM(yBottom);
  MoveToEx(hdc, n, m, &pt);
  n = XToN(0);
  m = YToM(yTop);
  LineTo(hdc, n, m);

  // Отрисовка функций
  map<double, double> f1, f2;
  const int N = 1000;
  double x;
  double dx = (xRight - xLeft) / (N - 1);
  for (int i = 0; i < N; i++) {
    x = xLeft + dx * i;
    f1[x] = abs(x);
    f2[x] = 2 - x * x;
  }

  // f1
  SelectObject(hdc, f1Pen);

  n = XToN(xLeft);
  m = YToM(f1[xLeft]);
  MoveToEx(hdc, n, m, &pt);
  for (int i = 1; i < N; i++) {
    x = xLeft + dx * i;
    n = XToN(x);
    m = YToM(f1[x]);
    LineTo(hdc, n, m);
  }

  // f2
  SelectObject(hdc, f2Pen);

  n = XToN(xLeft);
  m = YToM(f2[xLeft]);
  MoveToEx(hdc, n, m, &pt);
  for (int i = 1; i < N; i++) {
    x = xLeft + dx * i;
    n = XToN(x);
    m = YToM(f2[x]);
    LineTo(hdc, n, m);
  }

  // Отрисовываем доп инфу
  // Название графика и осей
  SetTextColor(hdc, RGB(255, 255, 255));
  //SetBkColor(hdc, RGB(127, 127, 127));
  SetBkMode(hdc, TRANSPARENT);

  TextOut(hdc, nRight - 15, (mBottom + mTop) / 2 - 20, _T("X"), 1);
  TextOut(hdc, (nLeft + nRight) / 2 + 5, mTop + 5, _T("Y"), 1);

  //выводим числа вдоль осей
  SetTextColor(hdc, RGB(0, 0, 0));
  TCHAR ss[10];
  swprintf_s(ss, 10, L"%6.1lf", xLeft);
  TextOut(hdc, nLeft - 15, mBottom + 7, ss, 6);
  swprintf_s(ss, 10, L"%6.1lf", xRight);
  TextOut(hdc, nRight - 25, mBottom + 7, ss, 6);

  swprintf_s(ss, 10, L"%6.1lf", yBottom);
  TextOut(hdc, nLeft - 40, mBottom - 7, ss, 6);
  swprintf_s(ss, 10, L"%6.1lf", yTop);
  TextOut(hdc, nLeft - 40, mTop - 7, ss, 6);

  SelectObject(hdc, winFont);

  SetBkMode(hdc, TRANSPARENT);
  SetTextColor(hdc, RGB(255, 0, 0));
  TextOut(hdc, XToN(1.4), YToM(1.4), L"f1 = |x|", 9);

  SetTextColor(hdc, RGB(0, 255, 0));
  TextOut(hdc, XToN(-1.9), YToM(-1.7), L"f2 = 2 - x^2", 13);

  DeleteObject(bgBrush);
  DeleteObject(bgPen);
  DeleteObject(axisPen);
  DeleteObject(f1Pen);
  DeleteObject(f2Pen);

  ReleaseDC(hwnd, hdc);
  //EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
                         WPARAM wParam, LPARAM lParam) {
  switch (message) {
  //case WM_PAINT:
  //  DrawTask1Graphs(hWnd);
  //  break;
  case WM_COMMAND:
    switch (LOWORD(wParam)) {
    case ID_FILE_EXIT:
      PostQuitMessage(0);
      break;
    case ID_TASK1_GRAPHS:
      DrawTask1Graphs(hWnd);
      break;

    case ID_TASK2_OBJ1:
      DrawTask2Obj1(hWnd);
      break;
    case ID_TASK2_OBJ2:
      DrawTask2Obj2(hWnd);
      break;
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

/* main */
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdParam,
                   int nCmdShow) {

  // Регистрация класса окна приложения
  WNDCLASS wc;

  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC)WndProc; //функция окна
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance; //дескриптор приложения
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  wc.lpszMenuName = (LPCTSTR)IDR_MENU1;
  wc.lpszClassName = winClassName; //имя класса окна

  if (!RegisterClass(&wc))
    return 0;

  // Cоздание окна приложения
  HWND hWnd;

  hWnd = CreateWindow(winClassName,               // имя класса окна
                      winTitle,                   // заголовок окна
                      WS_OVERLAPPED | WS_SYSMENU, // стиль окна
                      0,                          // x
                      0,                          // y
                      winWidth + 17,              // Width
                      winHeight + 39 + 20,        // Height
                      NULL,                       // дескриптор окна-родителя
                      NULL,                       // дескриптор меню
                      hInstance,                  // дескриптор приложения
                      NULL);

  if (!hWnd)
    return 0;

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  ClearWin(hWnd);
  InitDefaultObjects();

  // Цикл обработки сообщений
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg); // Посылает сообщение функции WndProc()
  }
  return 0;
}
