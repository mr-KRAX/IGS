
//по заданным опорным точкам (векторам)
//стороим составную сплайновую поверхность

#define STRICT
#define _CRT_SECURE_NO_WARNINGS
#include <math.h>
#include <stdio.h>
#include <tchar.h>
#include <windows.h>

// Имя класса окна
WCHAR Cname[] = L"SurfClass";
// Заголовок окна
const char Title[] = "Lab7 Elementary B-spline Surface: rotation by mouse and keyboard (arrows)";

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

//координаты в пикселях на экране
struct POINT2 {
  int x, y;
};

//прототипы функций
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SurfCreate();
void SurfDestroy();
void SurfPaint(HWND);
void SurfDC(HWND);
void SurfLButtonDown(int, int);
void SurfMouseMove(int, int);
void SurfLButtonUp();
void Picture(HDC);
void Axes(HDC);
void LineCreate();
void LineDestroy();
void LinePaint(HWND);
void LineDC(HWND);
void LineLButtonDown(int, int);
void LineMouseMove(HWND, int, int);
void DrawBox(HWND, HDC, ANGLS);
void LineLButtonUp();
void PointCorns();
inline double Xe(double, double);
inline double Ye(double, double, double);
inline int xn(double);
inline int ym(double);
void SplineSurf(HDC);

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
  wc.lpszClassName = (LPCSTR)Cname;

  if (!RegisterClass(&wc)) //регистрация класса окна приложения
    return 0;

  //создание окна приложения
  //------------------------------------------------------------------------
  HWND hWnd; // идентификатор окна приложения

  hWnd = CreateWindow((LPCSTR)Cname,
                      (LPCSTR)Title,
                      WS_OVERLAPPEDWINDOW,
                      0,    //x - координата л.в. угла
                      0,    //y - координата л.в. угла
                      1024, //ширина окна
                      768,  //высота окна
                      //						 CW_USEDEFAULT,
                      //						 CW_USEDEFAULT,
                      //						 CW_USEDEFAULT,
                      //						 CW_USEDEFAULT,
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
    LineCreate();
    break;

  case WM_PAINT:
    LinePaint(hwnd);
    break;

  case WM_LBUTTONDOWN:
    x = LOWORD(lParam);
    y = HIWORD(lParam);
    LineLButtonDown(x, y);
    break;

  case WM_LBUTTONUP:
    LineLButtonUp();
    LineDC(hwnd);
    //				InvalidateRect(hwnd,NULL,TRUE);
    break;

  case WM_MOUSEMOVE:
    x = LOWORD(lParam);
    y = HIWORD(lParam);

    LineMouseMove(hwnd, x, y);
    LineDC(hwnd);
    break;

  case WM_KEYDOWN:
    switch (wParam) {
    case VK_LEFT:
      angl.fi += 10;
      LineDC(hwnd);
      //					InvalidateRect(hwnd,NULL,TRUE);
      break;

    case VK_RIGHT:
      angl.fi -= 10;
      LineDC(hwnd);
      //					InvalidateRect(hwnd,NULL,TRUE);
      break;

    case VK_UP:
      angl.teta += 10;
      LineDC(hwnd);
      //					InvalidateRect(hwnd,NULL,TRUE);
      break;

    case VK_DOWN:
      angl.teta -= 10;
      LineDC(hwnd);
      //					InvalidateRect(hwnd,NULL,TRUE);
      break;
    }
    break;

  case WM_DESTROY:
    LineDestroy();
    break;

  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
  }

  return 0L;
}

//глобальные переменые
//число элементарный B-сплайновых поверхностей Nelm x Melm
const int Nelm = 10, Melm = 10;
//число опорных векторов Np x Mp
const int Np = Nelm + 3, Mp = Melm + 3;

//координаты опорных точек
double Ppx[Np][Mp], Ppy[Np][Mp], Ppz[Np][Mp];

//размеры поля вывода в мировых координатах и в пикселях
double xe1, xe2, ye1, ye2;
int ne1, ne2, me1, me2;

//глобальные переменые
//максимальные длины координатных осей - мировые координтаы
double xmax, ymax, zmax;

//Текстура, число точек в рисунке текстуры
const int Ntext = 100;
//координты текстуры
double ut[Ntext], vt[Ntext];

//Выполняется первой после создания окна
//#pragma argsused
void LineCreate() {
  //размеры поля вывода в мировых координатах и в пикселях
  xe1 = -4.5;
  xe2 = 4.5;
  ye1 = -4.5;
  ye2 = 4.5;
  ne1 = 100;
  ne2 = 500;
  me1 = 440;
  me2 = 40;

  //максимальные длины координатных осей - мировые координтаы
  xmax = 3., ymax = 3., zmax = 3.;

  PointCorns();

  //углы поворота системы координат - начальные значения
  angl.fi = 30;
  angl.teta = 60;

  double R;

  //интервал изменеия координаты Z
  double zbeg = -3.5, zend = 0;

  //интервал изменеия угла phsi
  double phbeg = 0, phend = 90;

  //задаем опорные вектора на на выбраном элементе
  //поверхности кувшина
  //уравнение кувшина  ( 0.2 + 0.7*z + 0.7*sin(2.5*z))^2 = x^2 + y^2
  //уравнение поверхности кувшина в параметрическом виде
  //имеет два параметра : z, phsi

  //  x =R * cos(phsi)
  //  y =R * sin(phsi)
  // 	R = 0.2 + 0.7*z + 0.7*sin(2.5*z);

  double z, x, y, phsi, dz, dp;

  //задаем значения основных опорных точек в мировой системе координат
  dz = (zend - zbeg) / (Np - 3);
  dp = (phend - phbeg) / (Mp - 3);

  for (int n = 1; n < Np - 1; n++) {
    z = zbeg + dz * (n - 1);
    R = sqrt(1.0 - z * z / 16.0);
    for (int m = 1; m < Mp - 1; m++) {
      phsi = phbeg + dp * (m - 1);
      x = 2 * R * cos(3.14159 / 180 * phsi);
      y = 3 * R * sin(3.14159 / 180 * phsi);

      Ppx[n][m] = x;
      Ppy[n][m] = y;
      Ppz[n][m] = z;
    }
  }

  //задаем значения вспомогательных опорных точек в мировой
  //системе координат, так чтобы концы составной В-сплайновой
  //повенхности пемыкали к заданным опорным точкам.
  for (int n = 1; n < Np - 1; n++) {

    Ppx[n][0] = 2 * Ppx[n][1] - Ppx[n][2];
    Ppy[n][0] = 2 * Ppy[n][1] - Ppy[n][2];
    Ppz[n][0] = 2 * Ppz[n][1] - Ppz[n][2];

    Ppx[n][Mp - 1] = 2 * Ppx[n][Mp - 2] - Ppx[n][Mp - 3];
    Ppy[n][Mp - 1] = 2 * Ppy[n][Mp - 2] - Ppy[n][Mp - 3];
    Ppz[n][Mp - 1] = 2 * Ppz[n][Mp - 2] - Ppz[n][Mp - 3];
  }

  for (int m = 0; m < Mp; m++) {
    Ppx[0][m] = 2 * Ppx[1][m] - Ppx[2][m];
    Ppy[0][m] = 2 * Ppy[1][m] - Ppy[2][m];
    Ppz[0][m] = 2 * Ppz[1][m] - Ppz[2][m];

    Ppx[Np - 1][m] = 2 * Ppx[Np - 2][m] - Ppx[Np - 3][m];
    Ppy[Np - 1][m] = 2 * Ppy[Np - 2][m] - Ppy[Np - 3][m];
    Ppz[Np - 1][m] = 2 * Ppz[Np - 2][m] - Ppz[Np - 3][m];
  }

  //Задается массив для рисунка текстуры
  double ro, t, dt = 6.28 / (Ntext - 1);
  for (int i = 0; i < Ntext; i++) {
    t = dt * i;
    ro = 0.5 * cos(4. * t) * cos(4. * t);
    ut[i] = ro * cos(t) + 0.5;
    vt[i] = ro * sin(t) + 0.5;
  }
}

//координаты пустой коробки - в мировой системе координат
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

//#pragma argsused
void LineDestroy() {
  PostQuitMessage(0); //закрываем окно
}

//глобальные переменные - угловые коэффициенты
double sf, cf, st, ct;

//четыре В-сплайновые функции
double Bspl(int i, double t) {
  double b;
  double t2 = t * t, t3 = t2 * t;
  switch (i) {
  case 0:
    b = (1 - 3 * t + 3 * t2 - t3) / 6;
    break;
  case 1:
    b = (4 - 6 * t2 + 3 * t3) / 6;
    break;
  case 2:
    b = (1 + 3 * t + 3 * t2 - 3 * t3) / 6;
    break;
  case 3:
    b = t3 / 6;
    break;
  }
  return b;
}

//координаты 16-ти опорных векторов
//для элементарной В-сплайновой поверхности
double Pex[4][4], Pey[4][4], Pez[4][4];

//X-вая комонента точки сплайновой поверхности
double splinesX(double u, double v) {
  double S1 = 0, S2;
  int i, j;
  for (i = 0; i < 4; i++) {
    S2 = 0;
    for (j = 0; j < 4; j++)
      S2 += Pex[i][j] * Bspl(j, v);
    S1 += S2 * Bspl(i, u);
  }
  return S1;
}

//Y-вая комонента точки сплайновой поверхности
double splinesY(double u, double v) {
  double S1 = 0, S2;
  int i, j;
  for (i = 0; i < 4; i++) {
    S2 = 0;
    for (j = 0; j < 4; j++)
      S2 += Pey[i][j] * Bspl(j, v);
    S1 += S2 * Bspl(i, u);
  }
  return S1;
}

//Y-вая комонента точки сплайновой поверхности
double splinesZ(double u, double v) {
  double S1 = 0, S2;
  int i, j;
  for (i = 0; i < 4; i++) {
    S2 = 0;
    for (j = 0; j < 4; j++)
      S2 += Pez[i][j] * Bspl(j, v);
    S1 += S2 * Bspl(i, u);
  }
  return S1;
}

//переход в видовую систему координат и отрографическое проектирование
double Xe(double x, double y) {
  return -sf * x + cf * y;
}

//переход в видовую систему координат и отрографическое проектирование
double Ye(double x, double y, double z) {
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

//Рисует главную картину в окне
void LinePaint(HWND hwnd) {
  PAINTSTRUCT ps;

  //получаем контест устройства для экрана
  HDC hdcWin = BeginPaint(hwnd, &ps);

  //выводим число элементарных сплайновых поверхностей
  char strs[20];
  SetBkColor(hdcWin, RGB(128, 128, 128));
  SetTextColor(hdcWin, RGB(0xC0, 0xC0, 0xC0));

  TextOut(hdcWin, 518, 40, _T(" Surface  Number "), 17);
  sprintf(strs, "Nelm = %2d", Nelm);
  TextOut(hdcWin, 540, 60, (LPCSTR)strs, 9);
  sprintf(strs, "Melm = %2d", Melm);
  TextOut(hdcWin, 540, 80, (LPCSTR)strs, 9);

  TextOut(hdcWin, 500, 120, _T(" Control Poin Number "), 21);
  sprintf(strs, "  Np = %2d", Np);
  TextOut(hdcWin, 540, 140, (LPCSTR)strs, 9);
  sprintf(strs, "  Mp = %2d", Mp);
  TextOut(hdcWin, 540, 160, (LPCSTR)strs, 9);

  HDC hdc = CreateCompatibleDC(hdcWin); //создаем контекст
  //памяти связаный с контекстом экрана

  //памяти надо придать вид экрана - подходт битовая карта с форматом
  // как у экрана. В памяти будем рисовать на битовой карте
  HBITMAP hBitmap, hBitmapOld;
  hBitmap = CreateCompatibleBitmap(hdcWin, ne2, me1); //создаем
  //битовую карту совместмую с контекстом экрана
  hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap); //помещаем
  // битовую карту в контекст памяти

  //создание прямоугольной области для вывода углов поворота
  HRGN hrgn1 = CreateRectRgn(ne1, me2 - 30, ne2, me2 - 2);

  //заливаем выделенную область светлоголубым цветом
  HBRUSH hBrush0 = CreateSolidBrush(RGB(0xA6, 0xCA, 0xF0));
  FillRgn(hdc, hrgn1, hBrush0);

  //обведение границы области заданной кистью
  HBRUSH hBrush1 = CreateSolidBrush(RGB(0xFF, 0xFF, 0xC0));
  FrameRgn(hdc, hrgn1, hBrush1, 2, 2);

  //выводим значения углов поворота в область hrgn1
  char ss1[20];
  SetBkColor(hdc, RGB(0xA6, 0xCA, 0xF0));
  SetTextColor(hdc, RGB(0, 0, 0x80));
  sprintf(ss1, "fi = %4.0lf", angl.fi);
  TextOut(hdc, (ne1 + ne2) / 2 - 80, me2 - 25, (LPCSTR)ss1, 9);
  sprintf(ss1, "teta = %4.0lf", angl.teta);
  TextOut(hdc, (ne1 + ne2) / 2 + 20, me2 - 25, (LPCSTR)ss1, 11);

  //удаляем область hrgn1
  DeleteObject(hrgn1);

  //создание прямоугольной области
  HRGN hrgn2 = CreateRectRgn(ne1, me2, ne2, me1);

  //заливаем выделенную область светлосерым цветом
  HBRUSH hBrush2 = CreateSolidBrush(RGB(0xC0, 0xC0, 0xC0));
  FillRgn(hdc, hrgn2, hBrush2);

  //ограничиваем область вывода изображения
  SelectClipRgn(hdc, hrgn2);

  //вычисляем синусы и косисинусы новых значений углов поворота
  sf = sin(3.14159 * angl.fi / 180);
  cf = cos(3.14159 * angl.fi / 180);
  st = sin(3.14159 * angl.teta / 180);
  ct = cos(3.14159 * angl.teta / 180);

  //выбираем цвет для координатных осей
  HPEN hPen1, hPenOld;
  hPen1 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
  hPenOld = (HPEN)SelectObject(hdc, hPen1);

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
  TextOut(hdc, x2, y2, _T("X"), 1);

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

  TextOut(hdc, x2, y2, _T("Y"), 1);

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

  TextOut(hdc, x2, y2, _T("Z"), 1);

  //текущие координаты
  double xt, yt, zt;

  //скользим по сетке опорных векторов Pp[Np][Mp]
  //и выбираем 16 опорных векторов Pe[][] для элементарной
  //В-сплайновой поверхности
  for (int nc = 0; nc < Np - 3; nc++)
    for (int mc = 0; mc < Mp - 3; mc++) {

      for (int n = 0; n < 4; n++)
        for (int m = 0; m < 4; m++) {
          Pex[n][m] = Ppx[n + nc][m + mc];
          Pey[n][m] = Ppy[n + nc][m + mc];
          Pez[n][m] = Ppz[n + nc][m + mc];
        }
      //рисуем В-сплайновую поверхностьс помощью линий
      //разного цвета, лежащих на поверхности

      HPEN hPen4 = CreatePen(PS_SOLID, 1, RGB(255 - 100 * mc, 255 - 100 * nc, 100 * mc));
      SelectObject(hdc, hPen4);

      //"верикальные линии"
      int Ns = 10, Nsp = 2;
      double v, u, du = 1.0 / (Ns - 1), dv = 1.0 / (Nsp - 1);

      for (int i = 0; i < Nsp; i++) {
        v = dv * i;
        u = 0;

        xt = splinesX(u, v);
        yt = splinesY(u, v);
        zt = splinesZ(u, v);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        MoveToEx(hdc, x1, y1, 0);

        for (int j = 1; j < Ns; j++) {
          u = du * j;

          xt = splinesX(u, v);
          yt = splinesY(u, v);
          zt = splinesZ(u, v);

          xe = Xe(xt, yt);
          ye = Ye(xt, yt, zt);

          x1 = xn(xe);
          y1 = ym(ye);

          LineTo(hdc, x1, y1);
          //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
        }
      }

      //"горизонтальные линии"
      int Ms = 10, Msp = 2;
      dv = 1.0 / (Ms - 1);
      du = 1.0 / (Msp - 1);

      for (int i = 0; i < Msp; i++) {
        u = du * i;
        v = 0;

        xt = splinesX(u, v);
        yt = splinesY(u, v);
        zt = splinesZ(u, v);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        MoveToEx(hdc, x1, y1, 0);

        for (int j = 1; j < Ms; j++) {
          v = dv * j;

          xt = splinesX(u, v);
          yt = splinesY(u, v);
          zt = splinesZ(u, v);

          xe = Xe(xt, yt);
          ye = Ye(xt, yt, zt);

          x1 = xn(xe);
          y1 = ym(ye);

          LineTo(hdc, x1, y1);

          //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
        }
      }

      xt = splinesX(ut[0], vt[0]);
      yt = splinesY(ut[0], vt[0]);
      zt = splinesZ(ut[0], vt[0]);

      xe = Xe(xt, yt);
      ye = Ye(xt, yt, zt);

      x1 = xn(xe);
      y1 = ym(ye);

      MoveToEx(hdc, x1, y1, 0);

      for (int k = 1; k < Ntext; k++) {

        xt = splinesX(ut[k], vt[k]);
        yt = splinesY(ut[k], vt[k]);
        zt = splinesZ(ut[k], vt[k]);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        LineTo(hdc, x1, y1);

        //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
      }

      DeleteObject(hPen4);
    }

  //рисуем каркасную поверхность с помощью линий белого цветоа
  HPEN hPen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  SelectObject(hdc, hPen2);

  //вертикальные линии
  for (int m = 1; m < Mp - 1; m++) {
    xe = Xe(Ppx[1][m], Ppy[1][m]);
    ye = Ye(Ppx[1][m], Ppy[1][m], Ppz[1][m]);
    x1 = xn(xe);
    y1 = ym(ye);
    for (int k = 2; k < Np - 1; k++) {
      xe = Xe(Ppx[k][m], Ppy[k][m]);
      ye = Ye(Ppx[k][m], Ppy[k][m], Ppz[k][m]);
      x2 = xn(xe);
      y2 = ym(ye);
      MoveToEx(hdc, x1, y1, 0);
      LineTo(hdc, x2, y2);

      x1 = x2;
      y1 = y2;
    }
  }

  //горизонтальные  линии
  for (int k = 1; k < Np - 1; k++) {
    xe = Xe(Ppx[k][1], Ppy[k][1]);
    ye = Ye(Ppx[k][1], Ppy[k][1], Ppz[k][1]);
    x1 = xn(xe);
    y1 = ym(ye);
    for (int m = 2; m < Mp - 1; m++) {
      xe = Xe(Ppx[k][m], Ppy[k][m]);
      ye = Ye(Ppx[k][m], Ppy[k][m], Ppz[k][m]);
      x2 = xn(xe);
      y2 = ym(ye);
      MoveToEx(hdc, x1, y1, 0);
      LineTo(hdc, x2, y2);

      x1 = x2;
      y1 = y2;
    }
  }

  //рисуем коробку серым цветом
  HPEN hPen3;
  hPen3 = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
  SelectObject(hdc, hPen3);

  DrawBox(hwnd, hdc, angl);

  //востанавливаем в контексте старое перо
  SelectObject(hdc, hPenOld);
  //убираем созданные графические объекты
  DeleteObject(hPen1);
  DeleteObject(hPen2);
  DeleteObject(hPen3);
  DeleteObject(hBrush0);
  DeleteObject(hBrush1);
  DeleteObject(hBrush2);
  DeleteObject(hrgn2);

  BitBlt(hdcWin, ne1, me2 - 30, ne2, me1, hdc, ne1, me2 - 30, SRCCOPY); //копи-
  //руем контекст памяти в контекст экрана

  SelectObject(hdc, hBitmapOld); //востанавливаем контекст памяти
  DeleteObject(hBitmap);         //убираем битовую карту
  DeleteDC(hdc);                 //  освобождаем контекст памяти

  //освобождаем контекст устройства
  EndPaint(hwnd, &ps);
}

//Рисует главную картину в окне
void LineDC(HWND hwnd) {
  //	PAINTSTRUCT ps;

  //получаем контест устройства для экрана
  //получаем контест устройства
  HDC hdcWin = GetDC(hwnd);
  //	HDC hdcWin = BeginPaint(hwnd, &ps);

  HDC hdc = CreateCompatibleDC(hdcWin); //создаем контекст
  //памяти связаный с контекстом экрана

  //памяти надо придать вид экрана - подходт битовая карта с форматом
  // как у экрана. В памяти будем рисовать на битовой карте
  HBITMAP hBitmap, hBitmapOld;
  hBitmap = CreateCompatibleBitmap(hdcWin, ne2, me1); //создаем
  //битовую карту совместмую с контекстом экрана
  hBitmapOld = (HBITMAP)SelectObject(hdc, hBitmap); //помещаем
  // битовую карту в контекст памяти

  //создание прямоугольной области для вывода углов поворота
  HRGN hrgn1 = CreateRectRgn(ne1, me2 - 30, ne2, me2 - 1);

  //заливаем выделенную область светлоголубым цветом
  HBRUSH hBrush0 = CreateSolidBrush(RGB(0xA6, 0xCA, 0xF0));
  FillRgn(hdc, hrgn1, hBrush0);

  //обведение границы области заданной кистью
  HBRUSH hBrush1 = CreateSolidBrush(RGB(0xFF, 0xFF, 0xC0));
  FrameRgn(hdc, hrgn1, hBrush1, 2, 2);

  //выводим значения углов поворота в область hrgn1
  char ss2[22];
  SetBkColor(hdc, RGB(0xA6, 0xCA, 0xF0));
  SetTextColor(hdc, RGB(0, 0, 0x80));
  sprintf(ss2, "fi = %4.0lf", angl.fi);
  TextOut(hdc, (ne1 + ne2) / 2 - 80, me2 - 25, (LPCSTR)ss2, 9);
  sprintf(ss2, "teta = %4.0lf", angl.teta);
  TextOut(hdc, (ne1 + ne2) / 2 + 20, me2 - 25, (LPCSTR)ss2, 11);

  //удаляем область hrgn1
  DeleteObject(hrgn1);

  //создание прямоугольной области
  HRGN hrgn2 = CreateRectRgn(ne1, me2, ne2, me1);

  //заливаем выделенную область светлосерым цветом
  HBRUSH hBrush2 = CreateSolidBrush(RGB(0xC0, 0xC0, 0xC0));
  FillRgn(hdc, hrgn2, hBrush2);

  //ограничиваем область вывода изображения
  SelectClipRgn(hdc, hrgn2);

  //вычисляем синусы и косисинусы новых значений углов поворота
  sf = sin(3.14159 * angl.fi / 180);
  cf = cos(3.14159 * angl.fi / 180);
  st = sin(3.14159 * angl.teta / 180);
  ct = cos(3.14159 * angl.teta / 180);

  //выбираем цвет для координатных осей
  HPEN hPen1, hPenOld;
  hPen1 = CreatePen(PS_SOLID, 1, RGB(0, 255, 255));
  hPenOld = (HPEN)SelectObject(hdc, hPen1);

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
  TextOut(hdc, x2, y2, _T("X"), 1);

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

  TextOut(hdc, x2, y2, _T("Y"), 1);

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

  TextOut(hdc, x2, y2, _T("Z"), 1);

  HBRUSH hbrushX = CreateSolidBrush(RGB(255, 100, 100));
  HBRUSH hbrushOldX = (HBRUSH)SelectObject(hdc, hbrushX); //выбираем кисть hbrush
  HPEN hpenX = CreatePen(PS_SOLID, 1, RGB(255, 100, 100));
  HPEN hpenOldX = (HPEN)SelectObject(hdc, hpenX); // выбираем перо hpen

  xe = Xe(xmax, 0);
  ye = Ye(xmax, 0, 0);
  x1 = xn(xe);
  y1 = ym(ye);

  Ellipse(hdc, x1 - 4, y1 - 4, x1 + 4, y1 + 4);

  char sss3[10];
  sprintf(sss3, "%2.4f", xmax);
  TextOut(hdc, x1 - 5, y1 + 5, (LPCSTR)sss3, 3);

  xe = Xe(0, ymax);
  ye = Ye(0, ymax, 0);
  x1 = xn(xe);
  y1 = ym(ye);

  Ellipse(hdc, x1 - 4, y1 - 4, x1 + 4, y1 + 4);

  sprintf(sss3, "%2.4f", ymax);
  TextOut(hdc, x1 - 5, y1 + 5, (LPCSTR)sss3, 3);

  xe = Xe(0, 0);
  ye = Ye(0, 0, zmax);
  x1 = xn(xe);
  y1 = ym(ye);

  Ellipse(hdc, x1 - 4, y1 - 4, x1 + 4, y1 + 4);

  sprintf(sss3, "%2.4f", zmax);
  TextOut(hdc, x1 - 5, y1 + 5, (LPCSTR)sss3, 3);

  SelectObject(hdc, hbrushOldX);
  DeleteObject(hbrushX);
  SelectObject(hdc, hpenOldX);
  DeleteObject(hpenX);

  //текущие координаты
  double xt, yt, zt;

  //скользим по сетке опорных векторов Pp[Np][Mp]
  //и выбираем 16 опорных векторов Pe[][] для элементарной
  //В-сплайновой поверхности
  for (int nc = 0; nc < Np - 3; nc++)
    for (int mc = 0; mc < Mp - 3; mc++) {

      for (int n = 0; n < 4; n++)
        for (int m = 0; m < 4; m++) {
          Pex[n][m] = Ppx[n + nc][m + mc];
          Pey[n][m] = Ppy[n + nc][m + mc];
          Pez[n][m] = Ppz[n + nc][m + mc];
        }
      //рисуем В-сплайновую поверхностьс помощью линий
      //разного цвета, лежащих на поверхности

      HPEN hPen4 = CreatePen(PS_SOLID, 1, RGB(255 - 100 * mc, 255 - 100 * nc, 100 * mc));
      SelectObject(hdc, hPen4);

      //"верикальные линии"
      int Ns = 10, Nsp = 2;
      double v, u, du = 1.0 / (Ns - 1), dv = 1.0 / (Nsp - 1);

      for (int i = 0; i < Nsp; i++) {
        v = dv * i;
        u = 0;

        xt = splinesX(u, v);
        yt = splinesY(u, v);
        zt = splinesZ(u, v);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        MoveToEx(hdc, x1, y1, 0);

        for (int j = 1; j < Ns; j++) {
          u = du * j;

          xt = splinesX(u, v);
          yt = splinesY(u, v);
          zt = splinesZ(u, v);

          xe = Xe(xt, yt);
          ye = Ye(xt, yt, zt);

          x1 = xn(xe);
          y1 = ym(ye);

          LineTo(hdc, x1, y1);
          //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
        }
      }

      //"горизонтальные линии"
      int Ms = 10, Msp = 2;
      dv = 1.0 / (Ms - 1);
      du = 1.0 / (Msp - 1);

      for (int i = 0; i < Msp; i++) {
        u = du * i;
        v = 0;

        xt = splinesX(u, v);
        yt = splinesY(u, v);
        zt = splinesZ(u, v);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        MoveToEx(hdc, x1, y1, 0);

        for (int j = 1; j < Ms; j++) {
          v = dv * j;

          xt = splinesX(u, v);
          yt = splinesY(u, v);
          zt = splinesZ(u, v);

          xe = Xe(xt, yt);
          ye = Ye(xt, yt, zt);

          x1 = xn(xe);
          y1 = ym(ye);

          LineTo(hdc, x1, y1);

          //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
        }
      }

      xt = splinesX(ut[0], vt[0]);
      yt = splinesY(ut[0], vt[0]);
      zt = splinesZ(ut[0], vt[0]);

      xe = Xe(xt, yt);
      ye = Ye(xt, yt, zt);

      x1 = xn(xe);
      y1 = ym(ye);

      MoveToEx(hdc, x1, y1, 0);

      for (int k = 1; k < Ntext; k++) {

        xt = splinesX(ut[k], vt[k]);
        yt = splinesY(ut[k], vt[k]);
        zt = splinesZ(ut[k], vt[k]);

        xe = Xe(xt, yt);
        ye = Ye(xt, yt, zt);

        x1 = xn(xe);
        y1 = ym(ye);

        LineTo(hdc, x1, y1);

        //					SetPixel(hdc,x1,y1,RGB(255-100*mc,255-100*nc,100*mc));
      }

      DeleteObject(hPen4);
    }

  //рисуем каркасную поверхность с помощью линий белого цветоа
  HPEN hPen2 = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
  SelectObject(hdc, hPen2);

  //вертикальные линии
  for (int m = 1; m < Mp - 1; m++) {
    xe = Xe(Ppx[1][m], Ppy[1][m]);
    ye = Ye(Ppx[1][m], Ppy[1][m], Ppz[1][m]);
    x1 = xn(xe);
    y1 = ym(ye);
    for (int k = 2; k < Np - 1; k++) {
      xe = Xe(Ppx[k][m], Ppy[k][m]);
      ye = Ye(Ppx[k][m], Ppy[k][m], Ppz[k][m]);
      x2 = xn(xe);
      y2 = ym(ye);
      MoveToEx(hdc, x1, y1, 0);
      LineTo(hdc, x2, y2);

      x1 = x2;
      y1 = y2;
    }
  }

  //горизонтальные  линии
  for (int k = 1; k < Np - 1; k++) {
    xe = Xe(Ppx[k][1], Ppy[k][1]);
    ye = Ye(Ppx[k][1], Ppy[k][1], Ppz[k][1]);
    x1 = xn(xe);
    y1 = ym(ye);
    for (int m = 2; m < Mp - 1; m++) {
      xe = Xe(Ppx[k][m], Ppy[k][m]);
      ye = Ye(Ppx[k][m], Ppy[k][m], Ppz[k][m]);
      x2 = xn(xe);
      y2 = ym(ye);
      MoveToEx(hdc, x1, y1, 0);
      LineTo(hdc, x2, y2);

      x1 = x2;
      y1 = y2;
    }
  }

  //рисуем коробку серым цветом
  HPEN hPen3;
  hPen3 = CreatePen(PS_SOLID, 1, RGB(160, 160, 160));
  SelectObject(hdc, hPen3);

  DrawBox(hwnd, hdc, angl);

  //востанавливаем в контексте старое перо
  SelectObject(hdc, hPenOld);
  //убираем созданные графические объекты
  DeleteObject(hPen1);
  DeleteObject(hPen2);
  DeleteObject(hPen3);
  DeleteObject(hBrush0);
  DeleteObject(hBrush1);
  DeleteObject(hBrush2);
  DeleteObject(hrgn2);

  BitBlt(hdcWin, ne1, me2 - 30, ne2, me1, hdc, ne1, me2 - 30, SRCCOPY); //копи-
  //руем контекст памяти в контекст экрана

  SelectObject(hdc, hBitmapOld); //востанавливаем контекст памяти
  DeleteObject(hBitmap);         //убираем битовую карту
  DeleteDC(hdc);                 //  освобождаем контекст памяти

  //освобождаем контекст устройства
  ReleaseDC(hwnd, hdcWin);

  //освобождаем контекст устройства
  //	EndPaint(hwnd, &ps);
}

//при нажатии левой кнопки мыши рисует синию коробку
void LineLButtonDown(int x, int y) {
  Dat.ButtonDown = TRUE;
  Dat.Drawing = FALSE;

  anglOld.fi = angl.fi;
  anglOld.teta = angl.teta;
  corOld.x = x;
  corOld.y = y;
}

//при движении мыши и нажатой левой кнопки мыши
//стирает синюю коробку и рисует ее в другом положении
void LineMouseMove(HWND hwnd, int x, int y) {
  if (Dat.ButtonDown) {
    Dat.Drawing = TRUE;

    //получаем контест устройства
    HDC hdc = GetDC(hwnd);

    //создание прямоугольной области для вывода углов поворота
    HRGN hrgn1 = CreateRectRgn(ne1, me2 - 30, ne2, me2 - 1);

    //заливаем выделенную область светлоголубым цветом
    HBRUSH hBrush0 = CreateSolidBrush(RGB(0xA6, 0xCA, 0xF0));
    FillRgn(hdc, hrgn1, hBrush0);

    //обведение границы области заданной кистью
    HBRUSH hBrush1 = CreateSolidBrush(RGB(0xFF, 0xFF, 0xC0));
    FrameRgn(hdc, hrgn1, hBrush1, 2, 2);

    //выводим значения углов поворота в область hrgn1
    char ss4[20];
    SetBkColor(hdc, RGB(0xA6, 0xCA, 0xF0));
    SetTextColor(hdc, RGB(0, 0, 0x80));
    sprintf(ss4, "fi = %4.0lf", angl.fi);
    TextOut(hdc, (ne1 + ne2) / 2 - 80, me2 - 25, (LPCSTR)ss4, 9);
    sprintf(ss4, "teta = %4.0lf", angl.teta);
    TextOut(hdc, (ne1 + ne2) / 2 + 20, me2 - 25, (LPCSTR)ss4, 11);

    //удаляем область hrgn1
    DeleteObject(hrgn1);

    //убираем созданные графические объекты
    DeleteObject(hBrush0);
    DeleteObject(hBrush1);

    //освобождаем контекст устройства
    ReleaseDC(hwnd, hdc);

    angl.fi += corOld.x - x;
    angl.teta += corOld.y - y;
    corOld.x = x;
    corOld.y = y;

    anglOld.fi = angl.fi;
    anglOld.teta = angl.teta;
  }
}

//рисует коробку заданным цветом и под заданым углом
void DrawBox(HWND hwnd, HDC hdc, ANGLS an) {
  sf = sin(3.14159 * an.fi / 180);
  cf = cos(3.14159 * an.fi / 180);
  st = sin(3.14159 * an.teta / 180);
  ct = cos(3.14159 * an.teta / 180);

  double xe, ye;
  int x1, y1, x2, y2;
  double xt1, yt1, zt1, xt2, yt2, zt2;
  int j;

  for (int i = 0; i < 4; i++) {
    j = i + 1;
    if (j == 4)
      j = 0;
    xt1 = Point[i].x;
    yt1 = Point[i].y;
    zt1 = Point[i].z;
    xt2 = Point[j].x;
    yt2 = Point[j].y;
    zt2 = Point[j].z;

    xe = Xe(xt1, yt1);
    ye = Ye(xt1, yt1, zt1);
    x1 = xn(xe);
    y1 = ym(ye);

    xe = Xe(xt2, yt2);
    ye = Ye(xt2, yt2, zt2);
    x2 = xn(xe);
    y2 = ym(ye);

    MoveToEx(hdc, x1, y1, 0);
    LineTo(hdc, x2, y2);
  }

  for (int i = 4; i < 8; i++) {
    j = i + 1;
    if (j == 8)
      j = 4;
    xt1 = Point[i].x;
    yt1 = Point[i].y;
    zt1 = Point[i].z;
    xt2 = Point[j].x;
    yt2 = Point[j].y;
    zt2 = Point[j].z;

    xe = Xe(xt1, yt1);
    ye = Ye(xt1, yt1, zt1);
    x1 = xn(xe);
    y1 = ym(ye);

    xe = Xe(xt2, yt2);
    ye = Ye(xt2, yt2, zt2);
    x2 = xn(xe);
    y2 = ym(ye);

    MoveToEx(hdc, x1, y1, 0);
    LineTo(hdc, x2, y2);
  }

  for (int i = 0; i < 4; i++) {
    xt1 = Point[i].x;
    yt1 = Point[i].y;
    zt1 = Point[i].z;
    xt2 = Point[i + 4].x;
    yt2 = Point[i + 4].y;
    zt2 = Point[i + 4].z;

    xe = Xe(xt1, yt1);
    ye = Ye(xt1, yt1, zt1);
    x1 = xn(xe);
    y1 = ym(ye);

    xe = Xe(xt2, yt2);
    ye = Ye(xt2, yt2, zt2);
    x2 = xn(xe);
    y2 = ym(ye);

    MoveToEx(hdc, x1, y1, 0);
    LineTo(hdc, x2, y2);
  }

  for (int i = 0; i < 2; i++) {
    xt1 = Point[i].x;
    yt1 = Point[i].y;
    zt1 = Point[i].z;
    xt2 = Point[i + 2].x;
    yt2 = Point[i + 2].y;
    zt2 = Point[i + 2].z;

    xe = Xe(xt1, yt1);
    ye = Ye(xt1, yt1, zt1);
    x1 = xn(xe);
    y1 = ym(ye);

    xe = Xe(xt2, yt2);
    ye = Ye(xt2, yt2, zt2);
    x2 = xn(xe);
    y2 = ym(ye);

    MoveToEx(hdc, x1, y1, 0);
    LineTo(hdc, x2, y2);
  }
}

//устанавливает нужные флаги при отпускании правой кнопки мыши
void LineLButtonUp() {
  if (Dat.ButtonDown && Dat.Drawing) {
    Dat.Drawing = FALSE;
  }
  Dat.ButtonDown = FALSE;
}
