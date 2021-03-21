
#define _USE_MATH_DEFINES  
#include <cmath>
#include <list>
#include <tchar.h>
#include <windows.h>

using namespace std;

TCHAR winClassName[] = _T("Class");
TCHAR winTitle[] = _T("Lab2 - Astroid");

// ������� ���� ������ � ������� ������� ���������
double xLeft = -5;
double xRight = 5;
double yBottom = -5;
double yTop = 5;

// ������� ����
int winHeight = 500;
int winWidth = 700;

// ������� ���� ������ � �������� � ���������� ������� ���� ����������
int nLeft = 35;
int nRight = winWidth - 35;
int mBottom = winHeight - 35;
int mTop = 35;

//������� �� x � ������� n
inline int XToN(double x) {
  return (int)((x - xLeft) / (xRight - xLeft) * (nRight - nLeft)) + nLeft;
}

//������� �� y � ������� m
inline int YToM(double y) {
  return (int)((y - yBottom) / (yTop - yBottom) * (mTop - mBottom)) + mBottom;
}

struct Vector2 {
  double x;
  double y;
};

void DrawFigure(HWND hwnd) {
  //SetWindowText(hwnd, L"");
  //ClearWin(hwnd);
  PAINTSTRUCT ps;        //��������� ��� ������ ��������
  HDC hdc = BeginPaint(hwnd, &ps);
  POINT pt;
  //HDC hdc = GetDC(hwnd); //���������� ��������� ����������

  // ������������� �����
  HBRUSH bgBrush = CreateSolidBrush(RGB(0, 0, 200));       // ��������, �����
  HPEN bgPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));   // 3px, ��������, ����-������
  HPEN axisPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));     // 2px, ��������, ������
  HPEN f1Pen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));     // 2px, ��������, ����-�������
  HBRUSH frBrush = CreateSolidBrush(RGB(0, 255, 0));       // ��������, �������
  HBRUSH frHathcBrush = CreateHatchBrush(HS_CROSS,RGB(100, 100, 200)); // ������, ����-�������

  // ��������� ������� ������
  POINT trg[3];
  trg[0].x = nLeft;
  trg[0].y = mBottom;
  trg[1].x = nRight;
  trg[1].y = mBottom;
  trg[2].x = XToN(0);
  trg[2].y = mTop;

  HRGN hrgn = CreatePolygonRgn(trg, 3, ALTERNATE);
  SelectClipRgn(hdc, hrgn);
  FillRgn(hdc, hrgn, frHathcBrush);

  // ��������� ����
  SelectObject(hdc, axisPen);
  // OX
  MoveToEx(hdc, XToN(xLeft), YToM(0), &pt);
  LineTo(hdc, XToN(xRight), YToM(0));
  // OY
  MoveToEx(hdc, XToN(0), YToM(yBottom), &pt);
  LineTo(hdc, XToN(0), YToM(yTop));

  // ������ ������� 
  list<Vector2> f;
  double x, y;
  float maxPhi = 2 * M_PI;
  float dPhi = maxPhi / 1000;
  for (float phi = 0; phi <= maxPhi; phi += dPhi) {
    x = 3 * pow(cos(phi), 3);
    y = 3 * pow(sin(phi), 3);
    f.push_back({x, y});
  }

  // ��������� �������
  SelectObject(hdc, f1Pen);
  int n, m;
  n = XToN(f.begin()->x);
  m = YToM(f.begin()->y);
  MoveToEx(hdc, n, m, &pt);
  for (auto it = f.begin(); it != f.end(); it++) {
    x = it->x;
    y = it->y;
    n = XToN(x);
    m = YToM(y);
    LineTo(hdc, n, m);
  }

  // ��������� �����
  SelectObject(hdc, frBrush);
  FrameRgn(hdc, hrgn, frBrush, 3, 3);


  DeleteObject(bgBrush);
  DeleteObject(bgPen);
  DeleteObject(axisPen);
  DeleteObject(f1Pen);
  DeleteObject(frBrush);

  EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
  WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_PAINT:
    DrawFigure(hWnd);
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

  // ����������� ������ ���� ����������
  WNDCLASS wc;

  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC)WndProc; //������� ����
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance; //���������� ����������
  wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = winClassName; //��� ������ ����

  if (!RegisterClass(&wc))
    return 0;

  // C������� ���� ����������
  HWND hWnd;

  hWnd = CreateWindow(winClassName,               // ��� ������ ����
    winTitle,                   // ��������� ����
    WS_OVERLAPPED | WS_SYSMENU, // ����� ����
    0,                          // x
    0,                          // y
    winWidth + 17,              // Width
    winHeight + 39 + 20,        // Height
    NULL,                       // ���������� ����-��������
    NULL,                       // ���������� ����
    hInstance,                  // ���������� ����������
    NULL);

  if (!hWnd)
    return 0;

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  // ���� ��������� ���������
  MSG msg;
  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg); // �������� ��������� ������� WndProc()
  }
  return 0;
}
