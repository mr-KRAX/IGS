

#define STRICT
#include <windows.h>
#include <windowsx.h>
#include <math.h>
#include <tchar.h>

#include "Matrix.h"


//название программы
WCHAR const szClassName[] = TEXT("Movement");
//заголовок окна   
WCHAR const szWindowTitle[] = TEXT("Clock");

//размеры окна вывода в мировой системе координат
double xLeft, xRight, yBottom, yTop;
//размеры окна вывода в пикселах в окне программы  
int nLeft, nRight, mBottom, mTop;

auto brown = RGB(194, 136, 12);
auto brown2 = RGB(222, 190, 120);
auto white = RGB(255, 255, 255);
auto grey = RGB(100,100,100);
auto red = RGB(222, 58, 58);
auto black = RGB(0,0,0);

unsigned int time = 0;
UINT uElapse = 25;
//создаем 6 матриц
Matrix2D R, T, D, A, T1, T2;


//флаг остановки и запуска таймера
int MoveStop;

//время задержки таймера
int clockSize, mCentre, nCentre;


//прототимы функций
BOOL RegisterApp(HINSTANCE hInst);
HWND CreateApp(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Move_OnCreate();
void Move_OnDestroy(HWND hwnd);
void Move_OnTimer(HWND hwnd);
void Move_OnKeydown(HWND hwnd);
void Move_OnPaint(HWND);

//дескриптор битовой карты
HBITMAP TheBitmap;

//дескриптор приложения
HINSTANCE hInstApp;

//главная функция
int PASCAL WinMain(HINSTANCE hInst,      //дескриптор(идентификатор) программы
	HINSTANCE hPrevInstance,
	LPSTR     lpszCmdParam,
	int       nCmdShow) {
	MSG msg;                         //структура для работы с сообщениями

	if (!RegisterApp(hInst))          //регистрация окна
		return FALSE;

	if (!CreateApp(hInst, nCmdShow))   //создание окна
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0))  //цикл обработка сообщений 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

//регистрация окна  
BOOL RegisterApp(HINSTANCE hInst) {
	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;     //стиль окна  
	wc.lpfnWndProc = WndProc;                     //имя оконной процедуры
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;                       //дескриптор программы
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //иконка 
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);     //форма курсора
	wc.hbrBackground = GetStockBrush(GRAY_BRUSH);   //цвет окна  
	wc.lpszMenuName = TEXT("APP_MENU");             //имя меню окна
	wc.lpszClassName = szClassName;                 //название программы 

	return RegisterClass(&wc);           //регистрация окна  
}

//создание окна  
HWND CreateApp(HINSTANCE hInstance, int nCmdShow) {
	hInstApp = hInstance;
	HWND hwnd;   //дескриптор окна 

	hwnd = CreateWindow(szClassName,      //название программы   
		szWindowTitle,        //заголовок окна  
		WS_OVERLAPPEDWINDOW,  //вид окна 
		CW_USEDEFAULT,        //x - координата окна  
		CW_USEDEFAULT,        //y - координата окна  
		CW_USEDEFAULT,        //ширина окна  
		CW_USEDEFAULT,        //высота окна  
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL)
		return hwnd;

	ShowWindow(hwnd, nCmdShow);   //показать окно 
	UpdateWindow(hwnd);          //обновить окно 

	return hwnd;
}

//оконная процедура обрабатывающая сообщения
LRESULT CALLBACK  WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_CREATE:
		Move_OnCreate();
		break;
	case WM_PAINT:
		Move_OnPaint(hwnd);
		break;
	case WM_TIMER:
		Move_OnTimer(hwnd);
		break;
	case WM_KEYDOWN:
		Move_OnKeydown(hwnd);
		break;
	case WM_DESTROY:
		Move_OnDestroy(hwnd);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0L;
}

//переход от x к пикселу n
inline int xn(double x) {
	return (int)((x - xLeft) / (xRight - xLeft) * (nRight - nLeft)) + nLeft;
}

//переход от y к пикселу m
inline int ym(double y) {
	return (int)((y - yBottom) / (yTop - yBottom) * (mTop - mBottom)) + mBottom;
}


//функция обрабатывает сообщение  WM_CREATE
void Move_OnCreate() {
	//размеры поля вывода в мировых и экранных координатах
	xLeft = -10; xRight = 10; yBottom = -6.0; yTop = 6.0;
	nLeft = 50; nRight = 650; mTop = 50; mBottom = 400;
	clockSize = 50;
	nCentre = (nRight - nLeft) / 2;
	mCentre = (mBottom - mTop) / 2;

	MoveStop = 1;
}

//функция обрабатывающая сообщение WM_KEYDOWN
void Move_OnKeydown(HWND hwnd) {
	if (MoveStop == 0) {
		KillTimer(hwnd, 1);
		MoveStop = 1;
	}
	else {
		SetTimer(hwnd, 1, uElapse, NULL);
		MoveStop = 0;
	}
}

//функция обрабатывает сообщение  WM_DESTROY
void Move_OnDestroy(HWND hwnd) {
	KillTimer(hwnd, 1);
	PostQuitMessage(0);   //закрывает окно 
}

void Background(HDC hdc) {
	HBRUSH hbrush, hbrushOld;
	HPEN hpen, hpenOld;

	hbrush = CreateSolidBrush(grey);
	SelectObject(hdc, hbrush);
	hpen = CreatePen(PS_SOLID, 1, grey);
	SelectObject(hdc, hpen);

	Rectangle(hdc, nLeft, mBottom, nRight, mTop);

	DeleteObject(hpen);
	DeleteObject(hbrush);
}

void Bmp(HDC hdc) {
HDC hdcMem = CreateCompatibleDC(hdc);
//загружаем в память bitmap-ресурс из модуля exe-файла
//hInstApp - идентификатор экземпляра приложения
//"BitmapShip" - идентификатор bitmap-ресурса (в файле ресурсов)
//hBitmap - идентификатор загруженного bitmap-ресурса
HBITMAP hBitmap = LoadBitmap(hInstApp, L"BitmapShip");

//выбираем bitmap-объект hBitmap в контекст памяти hdcMem
HBITMAP hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap); 

//копируем изображение из контекста памяти в контекст экрана 
BitBlt(hdc, nCentre - 640/2, mCentre - 480/2, 640, 480, hdcMem, 0, 0, SRCCOPY);

DeleteObject(hBitmap);     //убираем растровое изображение из памяти
SelectObject(hdcMem, hBitmapOld); //восстанавливаем контекст памяти
DeleteDC(hdcMem);                 //убираем контекст памяти

}

void Clock(HDC hdc) {
	HBRUSH hbrush;
	HPEN hpen;

	hbrush = CreateSolidBrush(brown);
	SelectObject(hdc, hbrush);
	hpen = CreatePen(PS_SOLID, 1, brown);
  SelectObject(hdc, hpen);

	POINT c[10];
	c[0] = { nCentre, mCentre - clockSize};
	c[1] = {c[0].x - clockSize, c[0].y + clockSize};
	c[2] = {c[0].x + clockSize, c[0].y + clockSize};
	c[3] = {c[0].x - clockSize, c[0].y - clockSize};
	c[4] = {c[0].x + clockSize, c[0].y - clockSize};
	c[5] = {c[0].x, c[0].y - 3 * clockSize};
	c[6] = {c[0].x + clockSize/3*2, c[0].y + clockSize/3*2};
	c[7] = {c[0].x - clockSize/3*2, c[0].y - clockSize/3*2};
	

	Rectangle(hdc, c[3].x, c[3].y, c[2].x, c[2].y);
	Polygon(hdc, &c[3], 3);

	DeleteObject(hbrush);
	hbrush = CreateSolidBrush(white);
	SelectObject(hdc, hbrush);
	Ellipse(hdc, c[7].x, c[7].y, c[6].x, c[6].y);

	DeleteObject(hpen);
	DeleteObject(hbrush);
}

void Ticker(HDC hdc, double fi) {
	HBRUSH hbrush;
	HPEN hpen;

	hbrush = CreateSolidBrush(brown2);
	hpen = CreatePen(PS_SOLID, 3, brown2);
	SelectObject(hdc, hbrush);
  SelectObject(hdc, hpen);

	POINT c[10];
	c[0] = { 0, 0};
	c[1] = {c[0].x, c[0].y + 3*clockSize};

	int arrowSize = 10;
	if (time % 2000 <= 1000) {
		c[4] = {c[1].x - clockSize, c[1].y};
		c[5] = {c[4].x-5, c[4].y};
		c[6] = {c[4].x+arrowSize, c[4].y-arrowSize/2};
		c[7] = {c[4].x+arrowSize, c[4].y+arrowSize/2};
	}
	else{
		c[4] = {c[1].x + clockSize, c[1].y};
		c[5] = {c[4].x + 5, c[4].y };
		c[6] = {c[4].x-arrowSize, c[4].y-arrowSize/2};
		c[7] = {c[4].x-arrowSize, c[4].y+arrowSize/2};
	}

	T1.transl(nCentre, mCentre - clockSize);
	R.rotate(fi);
	A = T1 * R;

	for (int n = 0; n < 10; n++)
		c[n] = affine(A, c[n]);	

	c[2] = {c[1].x - clockSize / 2, c[1].y - clockSize / 2};
	c[3] = {c[1].x + clockSize / 2, c[1].y + clockSize / 2};

	int width = 2;

	MoveToEx(hdc, c[0].x, c[0].y, 0);
	LineTo(hdc, c[1].x, c[1].y);
	Ellipse(hdc, c[2].x, c[2].y, c[3].x, c[3].y);

	DeleteObject(hpen);
	DeleteObject(hbrush);
	hbrush = CreateSolidBrush(red);
	hpen = CreatePen(PS_SOLID, 3, red);
	SelectObject(hdc, hbrush);
  SelectObject(hdc, hpen);

	MoveToEx(hdc, c[1].x, c[1].y, 0);
	LineTo(hdc, c[4].x, c[4].y);
	Polygon(hdc, &c[5], 3);


	DeleteObject(hpen);
	DeleteObject(hbrush);
}

void Arrows(HDC hdc, double hfi) {
	HBRUSH hbrush;
	HPEN hpen;

	hbrush = CreateSolidBrush(black);
	hpen = CreatePen(PS_SOLID, 1, black);
	SelectObject(hdc, hbrush);
  SelectObject(hdc, hpen);
	
	int nCentre = (nRight - nLeft) / 2;
	int mCentre = (mBottom - mTop) / 2;

	int w = 3;
	POINT h[4], m[4];
	h[0] = m[0] = { 0, 0};
	h[1] = m[1] = {h[0].x-w, h[0].y-w};
	h[3] = m[3] = {h[0].x+w, h[0].y-w};
	h[2] = {h[0].x, h[0].y-clockSize/3};
	m[2] = {m[0].x, m[0].y-clockSize/2};
	
	T1.transl(nCentre, mCentre - clockSize);
	R.rotate(hfi);
	A = T1 * R;
	for (int n = 0; n < 4; n++)
		h[n] = affine(A, h[n]);	

	R.rotate(hfi*12);
	A = T1 * R;

	for (int n = 0; n < 4; n++)
		m[n] = affine(A, m[n]);	

	Polygon(hdc, h, 4);
	Polygon(hdc, m, 4);


	DeleteObject(hpen);
	DeleteObject(hbrush);
}

//функция обрабатывает сообщение  WM_PAINT
void Move_OnPaint(HWND hwnd) {

	RECT rc;
	//дает размеры клиентской области окна <hwnd>
	GetClientRect(hwnd, &rc);

	//размеры окна в мировых координатах и в пикселях
	nLeft = rc.left; nRight = rc.right; mBottom = rc.bottom; mTop = rc.top;
	nCentre = (nRight - nLeft) / 2;
	mCentre = (mBottom - mTop) / 2;
	HDC hdc;
	PAINTSTRUCT ps;
	//получаем контекст экрана
	hdc = BeginPaint(hwnd, &ps);

	Background(hdc);
	Bmp(hdc);
	Ticker(hdc, 0);
	Clock(hdc);
	Arrows(hdc, 0);
	EndPaint(hwnd, &ps);
}

double ticketAngle(int currTime){
	int t = currTime % 2000;
	double lim = 0.6;
	double angle = -lim + (2*lim/1000 * (t % 1000));
	angle = t >= 1000 ? -angle : angle;
	return angle;
}

double arrowsAngle(int currTime){
	int t = currTime % 3600;
	double angle = 2*3.14 / 3600 * t;
 	return angle;
}

//функция обрабатывает сообщение  WM_TIMER
void Move_OnTimer(HWND hwnd) {
	time += uElapse;


	HDC hdcMem, hdcWin;
	HBITMAP hBitmap;
	//получаем контекст экрана
	hdcWin = GetDC(hwnd);
	//создаем контекст памяти совместимый с контекстом экрана
	hdcMem = CreateCompatibleDC(hdcWin);
	//создаем битовую карту совместмую с контекстом экрана               
	hBitmap = CreateCompatibleBitmap(hdcWin, nRight, mBottom);
	//помещаем битовую карту в контекст памяти          
	SelectObject(hdcMem, hBitmap);


	Background(hdcMem);
	Bmp(hdcMem);
	Ticker(hdcMem, ticketAngle(time));
	Clock(hdcMem);
	Arrows(hdcMem, arrowsAngle(time));

	//копируем контекст памяти в контекст экрана
	int xd = nLeft, yd = mTop;
	int Wd = nRight - nLeft, Hd = mBottom - mTop;
	int xs = nLeft, ys = mTop;
	BitBlt(hdcWin, xd, yd, Wd, Hd, hdcMem, xs, ys, SRCCOPY);

	DeleteDC(hdcMem);  //  уничтожаем контекст памяти
	ReleaseDC(hwnd, hdcWin);  //освобождаем контекст экрана
}