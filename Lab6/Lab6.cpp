
//вращение одной системы координат относительно другой
//выполнятеся с помощь мышки и стрелок клавиатуры

#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <gl\gl.h>
#include <gl\glu.h>

// Имя класса окна
const char Cname[] = "SurfClass";
// Заголовок окна
const char Title[] = "OpenGL - Lab6";

//углы поворота видовой системы координат
struct ANGLS {
  double fi, teta;
};

static ANGLS angl, anglOld;

//координаты точек в мировой системе координат
struct POINT3 {
  double x, y, z;
};

static POINT3 Point[8];

//флаги для работы с мышкой
struct TDATA {

  BOOL ButtonDown;
  BOOL Drawing;
};

static TDATA Dat;

//координаты мышки
struct CORD {
  int x, y;
};

static CORD corOld;

//флаг для указания направления вращения
int FlagRotation;

//флаг для удаления шахматной доски
bool FlagDesk = true;

//флаг для вкл/выкл 2 света
bool FlagLight = true;

//прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SurfCreate();
void SurfDestroy();
void SurfPaint(HWND);
void PointCorns();
void SurfDC(HWND);
void SurfLButtonDown(int, int);
void SurfMouseMove(int, int);
void SurfLButtonUp();
void PointCorns();
void Picture(HWND, HDC);
void Axes(HDC);
inline double Xe(double, double);
inline double Ye(double, double, double);
inline int xn(double);
inline int ym(double);
void DrawOpenGL(HDC);

int WINAPI WinMain(HINSTANCE hInstance, //идентификатор приложения
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdParam,
                   int nCmdShow) {

  MSG msg; // структура для работы с сообщениями

  //регистрация класса окна приложения
  //------------------------------------------------------------------------
  WNDCLASS wc; // структура для регистрации класса окна приложения

  wc.style = 0;
  wc.lpfnWndProc = (WNDPROC)WndProc; //адрес функции окна
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance; //идентификатор приложения
  wc.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
  wc.lpszMenuName = 0;
  wc.lpszClassName = Cname;

  if (!RegisterClass(&wc)) //регистрация класса окна приложения
    return 0;

  //создание окна приложения
  //------------------------------------------------------------------------
  HWND hWnd; // идентификатор окна приложения

  hWnd = CreateWindow(Cname,
                      Title,
                      WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, //стиль окна
                      100, //x - координата л.в. угла
                      50,  //y - координата л.в. угла
                      650, //ширина окна
                      500, //высота окна
                      NULL,      //идентификатор окна-родителя
                      NULL,      //идентификатор меню
                      hInstance, //идентификатор приложения
                      NULL);

  if (!hWnd)
    return 0;

  // Рисуем окно. Для этого после функции ShowWindow,
  // рисующей  окно, вызываем функцию UpdateWindows,
  // посылающую сообщение WM_PAINT в функцию окна
  //--------------------------------------------------------------------------
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

  // Запускаем цикл обработки сообщений
  //-------------------------------------------------------------------------

  while (GetMessage(&msg, NULL, 0, 0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  return 0;
}

//оконная процедура обрабатывающая сообщения
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
                         WPARAM wParam, LPARAM lParam) {
  int x, y;
  switch (msg) {
  case WM_CREATE:
    SetTimer(hwnd, 1, 2, NULL);
    SurfCreate();
    break;

  case WM_PAINT:
    SurfPaint(hwnd);
    break;

  case WM_TIMER:
    switch (FlagRotation) {
    case 1:
      angl.fi += 0.5;
      break;

    case 2:
      angl.fi -= 0.5;
      break;

    case 3:
      angl.teta += 0.5;
      break;

    case 4:
      angl.teta -= 0.5;
      break;
    }
    SurfDC(hwnd);
    break;

  case WM_LBUTTONDOWN:
    x = LOWORD(lParam);
    y = HIWORD(lParam);
    SurfLButtonDown(x, y);
    break;

  case WM_LBUTTONUP:
    SurfLButtonUp();
    break;

  case WM_MOUSEMOVE:
    if (Dat.ButtonDown) {
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      SurfMouseMove(x, y);
      SurfDC(hwnd);
    }
    break;

  case WM_DESTROY:
    KillTimer(hwnd, 1);
    SurfDestroy();
    break;

  case WM_KEYDOWN:
    switch (wParam) {
    case VK_SPACE:
      FlagRotation = 0;
      //					angl.fi += 10;
      //					SurfDC(hwnd);
      break;
    case VK_LEFT:
      FlagRotation = 1;
      //					angl.fi += 10;
      //					SurfDC(hwnd);
      break;

    case VK_RIGHT:
      FlagRotation = 2;
      //					angl.fi -= 10;
      //					SurfDC(hwnd);
      break;

    case VK_UP:
      FlagRotation = 3;
      //					angl.teta += 10;
      //					SurfDC(hwnd);
      break;

    case VK_DOWN:
      FlagRotation = 4;
      //					angl.teta -= 10;
      //					SurfDC(hwnd);
      break;

    case VK_DELETE:
      FlagDesk = !FlagDesk;
      break;
    case 'L':
      FlagLight = !FlagLight;
    break;
    }

  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  return 0L;
}

//размеры поля вывода в мировых координатах и в пикселях
double xe1, xe2, ye1, ye2;
int ne1, ne2, me1, me2;

//максимальные длины координатных осей - мировые координтаы
double xmax, ymax, zmax;

//угловые коэффициенты
double sf, cf, st, ct;

//Выполняется при создании окна
void SurfCreate() {
  //размеры поля вывода в мировых координатах и в пикселях
  xe1 = -4.5;
  xe2 = 4.5;
  ye1 = -4.5;
  ye2 = 4.5;
  ne1 = 100;
  ne2 = 550;
  me1 = 440;
  me2 = 40;

  //максимальные длины координатных осей - мировые координтаы
  xmax = 3., ymax = 3., zmax = 3.;

  //углы поворота системы координат - начальные значения
  angl.fi = 30;
  angl.teta = 60;

  //координаты коробки в мировой системе координат
  PointCorns();

  FlagRotation = 0;
}

//Выполняется при закрытии окна
void SurfDestroy() {
  PostQuitMessage(0); //закрываем окно
}

//Рисует картину в ответ на сообщение WM_PAINT
void SurfPaint(HWND hwnd) {
  PAINTSTRUCT ps;

  //получаем контест окна
  HDC hdcWin = BeginPaint(hwnd, &ps);

  //рисуем графические объекты
  DrawOpenGL(hdcWin);

  //освобождаем контекст окна
  EndPaint(hwnd, &ps);
}

//Рисует картину в ответ на сообщение WM_MOUSEMOVE
void SurfDC(HWND hwnd) {

  //получаем контест окна
  HDC hdcWin = GetDC(hwnd);

  //рисуем графические объекты
  DrawOpenGL(hdcWin);

  //освобождаем контекст окна
  ReleaseDC(hwnd, hdcWin);
}

//рисуем графические объекты OpenGL
void DrawOpenGL(HDC hdcWin) {
  //структура описания формата пикселя
  PIXELFORMATDESCRIPTOR pfd;

  //зануляем все поля структуры pfd
  memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));

  //заполняем некоторые поля структуры
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cDepthBits = 32;

  //выбираем наиболее подходящий формат пикселей в контексте окна
  int iPixelFormat = ChoosePixelFormat(hdcWin, &pfd);
  //устанавливаем найденный формат пикселей в контектсте памяти
  SetPixelFormat(hdcWin, iPixelFormat, &pfd);

  //получаем контекст отображения OpenGL
  HGLRC hglrc = wglCreateContext(hdcWin);
  //устанавливаем текущий контекст отображения OpenGL
  wglMakeCurrent(hdcWin, hglrc);

  //включаем механизм z-буфура
  glClearColor(0, 0.5, 0, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearDepth(1.0);
  glEnable(GL_DEPTH_TEST);

  //	glViewport(100, 0, 450, 400);

  //установка матрицы перспективной проекции
  // gluPerspective(f,a,zN,zF) f-угол просмотра, a- коэффициент
  //пропорциональности, zN- расстояние до ближней плоскости отсечения
  //                    zF- расстояние до дальней плоскости отсечения
  gluPerspective(65, (double)1, 3, 40);
  //умножение текущей матрицы на матрицу переноса
  glTranslatef(0, 0, -10);
  //умножение текущей матрицы на матрицу пповорота
  glRotatef(90 - (GLfloat)angl.teta, 1, 0, 0);
  glRotatef(-(GLfloat)angl.fi, 0, 1, 0);

  glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  glEnable(GL_LIGHTING);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_NORMALIZE);

  //устанавливаем источники света
  GLfloat lightpos[4] = {3, 3, 6, 1};
  GLfloat lightdirection[3] = {-0.9f, -0.1f, -7.6f};

  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirection);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 4);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 50);
  glEnable(GL_LIGHT0);
  

  if (FlagLight) {
    GLfloat lightpos1[4] = {3, 3, -6, 1};
    GLfloat lightdirection1[3] = {-0.9f, -0.1f, 7.6f};

    glLightfv(GL_LIGHT0, GL_POSITION, lightpos1);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightdirection1);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 4);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 50);
    glEnable(GL_LIGHT0);

  }

  if (FlagDesk) {
    //Шахматное поле
    for (int j = -5; j < 5; j++)
      for (int i = -5; i < 5; i++) {
        if ((abs(i + j) % 2) == 0)
          glColor3f(1, 0, 0);
        else
          glColor3f(0.8f, 0.8f, 0.8f);

        glBegin(GL_QUADS);
        glVertex3f((GLfloat)i, 0, (GLfloat)j);
        glVertex3f((GLfloat)i + 1, 0, (GLfloat)j);
        glVertex3f((GLfloat)i + 1, 0, (GLfloat)j + 1);
        glVertex3f((GLfloat)i, 0, (GLfloat)j + 1);
        glEnd();
      }
  }

  //axes
  glColor3f(1.0f, 1.0f, 0.0f);
  //толщина линий - 3 пикселей
  glLineWidth(3.0);

  //OX
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.25f, 0.f);
  glVertex3f(5.5f, 0.25f, 0.f);
  glEnd();

  //OY
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.25f, 0.0f);
  glVertex3f(0.0f, 4.25f, 0.0f);
  glEnd();

  //OZ
  glBegin(GL_LINES);
  glVertex3f(0.0f, 0.25f, 0.0f);
  glVertex3f(0.0f, 0.25f, 5.5f);
  glEnd();


  //рисуем усеченную пирамиду

  GLfloat p1[3] = {1.2f, 1.8f, 0.6f};
  GLfloat p2[3] = {0.8f, 0.1f, 0.0f};
  GLfloat p3[3] = {2.4f, 0.1f, 0.0f};
  GLfloat p4[3] = {2.0f, 1.8f, 0.6f};
  GLfloat p5[3] = {2.0f, 1.8f, 1.2f};
  GLfloat p6[3] = {2.4f, 0.1f, 1.6f};
  GLfloat p7[3] = {0.8f, 0.1f, 1.6f};
  GLfloat p8[3] = {1.2f, 1.8f, 1.2f};

  glColor3f(0.0f, 0.0f, 1.0f);
  glTranslatef(0, 0, 2);
  glBegin(GL_POLYGON);
  glVertex3fv(p1);
  glVertex3fv(p2);
  glVertex3fv(p3);
  glVertex3fv(p4);
  glEnd();

  // glColor3f(0.0f, 0.5f, 1.0f);
  glBegin(GL_POLYGON);
  glVertex3fv(p3);
  glVertex3fv(p4);
  glVertex3fv(p5);
  glVertex3fv(p6);
  glEnd();

  // glColor3f(0.2f, 0.5f, 0.0f);
  glBegin(GL_POLYGON);
  glVertex3fv(p5);
  glVertex3fv(p6);
  glVertex3fv(p7);
  glVertex3fv(p8);
  glEnd();

  // glColor3f(1.0f, 0.3f, 0.1f);
  glBegin(GL_POLYGON);
  glVertex3fv(p1);
  glVertex3fv(p2);
  glVertex3fv(p7);
  glVertex3fv(p8);
  glEnd();

  // glColor3f(0.5f, 0.1f, 0.2f);
  glBegin(GL_POLYGON);
  glVertex3fv(p1);
  glVertex3fv(p4);
  glVertex3fv(p5);
  glVertex3fv(p8);
  glEnd();

  // glColor3f(0.5f, 0.1f, 0.2f);
  glBegin(GL_POLYGON);
  glVertex3fv(p2);
  glVertex3fv(p3);
  glVertex3fv(p6);
  glVertex3fv(p7);
  glEnd();

  //////////////////////////////////////////////////
  /////////////Отсекающая плоскость/////////////////
  //////////////////////////////////////////////////
  //GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};
  //glClipPlane(GL_CLIP_PLANE0, eqn); // идентифицируем плоскость отсечения
  //glEnable(GL_CLIP_PLANE0); // включаем первую плоскость отсечения
  //////////////////////////////////////////////////

  //рисуем сферу
  GLUquadricObj *quadricObj;
  quadricObj = gluNewQuadric();

  glColor3f(1.0f, 0.0f, 1.0f);
  glTranslatef(-1.4, 3.5, -3.0);
  gluSphere(quadricObj, 1.8, 100, 100);
  //ожидает завершения всех предыдущих команд OpenGL
  SwapBuffers(hdcWin);

  //завершаем работу с текущим контекстом отображения
  wglMakeCurrent(NULL, NULL);
  //освобождаем контекст отображения OpenGL
  wglDeleteContext(hglrc);
}

//рисуем координатные оси
void Axes(HDC hdc) {
  //выбираем цвет для координатных осей
  HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
  HPEN hPenOld = (HPEN)SelectObject(hdc, hPen);

  //координаты проектируемой точки в видовой системе координат
  double xe, ye;
  //координаты пикселов
  int x1, y1, x2, y2;

  //рисуем ось Ox
  xe = Xe(-xmax / 3, 0);
  ye = Ye(-xmax / 3, 0, 0);
  x1 = xn(xe);
  y1 = ym(ye);
  xe = Xe(1.5 * xmax, 0);
  ye = Ye(1.5 * xmax, 0, 0);
  x2 = xn(xe);
  y2 = ym(ye);
  MoveToEx(hdc, x1, y1, 0);
  LineTo(hdc, x2, y2);

  //делаем надпись на оси заданным цветом
  SetBkColor(hdc, RGB(0xC0, 0xC0, 0xC0));
  SetTextColor(hdc, RGB(120, 120, 120));
  TextOut(hdc, x2, y2, "X", 1);

  //рисуем ось Oy
  xe = Xe(0, -ymax / 3);
  ye = Ye(0, -ymax / 3, 0);
  x1 = xn(xe);
  y1 = ym(ye);
  xe = Xe(0, 1.5 * ymax);
  ye = Ye(0, 1.5 * ymax, 0);
  x2 = xn(xe);
  y2 = ym(ye);
  MoveToEx(hdc, x1, y1, 0);
  LineTo(hdc, x2, y2);

  TextOut(hdc, x2, y2, "Y", 1);

  //рисуем ось Oz
  xe = Xe(0, 0);
  ye = Ye(0, 0, -zmax / 3);
  x1 = xn(xe);
  y1 = ym(ye);
  xe = Xe(0, 0);
  ye = Ye(0, 0, 1.5 * zmax);
  x2 = xn(xe);
  y2 = ym(ye);
  MoveToEx(hdc, x1, y1, 0);
  LineTo(hdc, x2, y2);

  TextOut(hdc, x2, y2, "Z", 1);
  //востанавливаем в контексте старое перо
  SelectObject(hdc, hPenOld);
  //убираем созданные графические объекты
  DeleteObject(hPen);
}

//координаты коробки - в мировой системе координат
void PointCorns() {
  Point[0].x = xmax;
  Point[0].y = ymax;
  Point[0].z = -zmax;
  Point[1].x = -xmax;
  Point[1].y = ymax;
  Point[1].z = -zmax;
  Point[2].x = -xmax;
  Point[2].y = -ymax;
  Point[2].z = -zmax;
  Point[3].x = xmax;
  Point[3].y = -ymax;
  Point[3].z = -zmax;
  Point[4].x = xmax;
  Point[4].y = ymax;
  Point[4].z = zmax;
  Point[5].x = -xmax;
  Point[5].y = ymax;
  Point[5].z = zmax;
  Point[6].x = -xmax;
  Point[6].y = -ymax;
  Point[6].z = zmax;
  Point[7].x = xmax;
  Point[7].y = -ymax;
  Point[7].z = zmax;
}

//переход в видовую систему координат и отрографическое проектирование
inline double Xe(double x, double y) {
  return -sf * x + cf * y;
}

//переход в видовую систему координат и отрографическое проектирование
inline double Ye(double x, double y, double z) {
  return -ct * cf * x - ct * sf * y + st * z;
}

//перевод видовых координат в координты пикселей
//переход от координаты x к пикселю  n
inline int xn(double x) {
  return (int)((x - xe1) / (xe2 - xe1) * (ne2 - ne1)) + ne1;
}

//переход от координаты y к пикселю  m
inline int ym(double y) {
  return (int)((y - ye1) / (ye2 - ye1) * (me2 - me1)) + me1;
}

//при нажатии левой кнопки мыши
void SurfLButtonDown(int x, int y) {
  Dat.ButtonDown = TRUE;
  Dat.Drawing = FALSE;

  anglOld.fi = angl.fi;
  anglOld.teta = angl.teta;
  corOld.x = x;
  corOld.y = y;
}

//при движении мыши и нажатой левой кнопки мыши
void SurfMouseMove(int x, int y) {
  if (Dat.ButtonDown) {
    Dat.Drawing = TRUE;

    angl.fi += corOld.x - x;
    angl.teta += corOld.y - y;

    corOld.x = x;
    corOld.y = y;

    anglOld.fi = angl.fi;
    anglOld.teta = angl.teta;
  }
}

//устанавливает нужные флаги при отпускании правой кнопки мыши
void SurfLButtonUp() {
  if (Dat.ButtonDown && Dat.Drawing) {
    Dat.Drawing = FALSE;
  }
  Dat.ButtonDown = FALSE;
}
