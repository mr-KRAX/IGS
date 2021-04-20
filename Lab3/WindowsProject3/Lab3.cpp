#include <windows.h>
#include <math.h>
#include <tchar.h>
#include <commctrl.h>
#include "resource.h"


//прототипы функции
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void Line_DC(HWND hwnd);
HINSTANCE hInst;
HWND hDlgWin1;
HWND hDlgWin2;
HWND hPic;
HDC hdc;
void Line_Paint(HWND);
inline int ym(double);
inline int xn(double);
int nWidth = 350, mHeight = 300;
int OBJ_col_Red = 50, OBJ_col_Green = 50, OBJ_col_Blue = 200;
int BG_col_Red = 0, BG_col_Green = 100, BG_col_Blue = 200;

TCHAR cname[] = _T("Class");     //имя класса окна
TCHAR title[] = _T("Lab3");  //заголовок окна


//главная функция
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdParam,
    int       nCmdShow)
{
    hInst = hInstance;
    //регистрация класса окна приложения
    //------------------------------------------------------------------------
    WNDCLASS wc;  // структура для регистрации класса окна приложения

    wc.style = 0;
    wc.lpfnWndProc = (WNDPROC)WndProc;         //функция окна
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;       //дескриптор приложения
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.lpszMenuName = (LPCTSTR)IDR_MENU1;
    wc.lpszClassName = cname;   //имя класса окна

    if (!RegisterClass(&wc))  //регистрация класса окна приложения
        return 0;


    //создание окна приложения
    //------------------------------------------------------------------------
    HWND hWnd;       // дескриптор окна приложения

    hWnd = CreateWindow(cname,              //имя класса окна
        title,               //заголовок окна
        WS_OVERLAPPEDWINDOW, // стиль окна
        CW_USEDEFAULT,       // x
        CW_USEDEFAULT,       // y
        CW_USEDEFAULT,       // Width
        CW_USEDEFAULT,       // Height
        NULL,          //дескриптор окна-родителя
        NULL,          //дескриптор меню
        hInstance,     //дескриптор приложения
        NULL);

    if (!hWnd)
        return 0;


    // Рисуем окно.
    //--------------------------------------------------------------------------
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    // Запускаем цикл обработки сообщений
    MSG msg;    // структура для работы с сообщениями

    //-------------------------------------------------------------------------
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg); // Посылает сообщение функции WndProc()
    }

    return 0;

}

LRESULT CALLBACK DialProc(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        //сообщение при открытии диалогового окна
    case WM_INITDIALOG:
        TCHAR S[20];
        swprintf_s(S, 20, L"%d", nWidth);
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT1), S);
        swprintf_s(S, 20, L"%d", mHeight);
        SetWindowText(GetDlgItem(hDlg, IDC_EDIT2), S);

        break;

        //сообщение, приходящее от элементов диалогового окна
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)  //нажата кнопка
        {
            DestroyWindow(hDlg);
        }

        if (HIWORD(wParam) == EN_CHANGE)
        {
            GetWindowText(GetDlgItem(hDlg, IDC_EDIT1), S, 20);
            nWidth = _wtof(S);
            GetWindowText(GetDlgItem(hDlg, IDC_EDIT2), S, 20);
            mHeight = _wtof(S);
            HWND hWnd = GetParent(hDlg);
            SendMessage(hWnd, WM_USER, 0, 0);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;
    }
    return 0;
}

LRESULT CALLBACK CHANGE_F_COLOR(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    //дескрипторы окна родителя и линеек
    static HWND hWnd, hRED, hGREEN, hBLUE;
    static HWND hLine;
    //положение ползунков линеек
    static int colorRED = 0, colorGREEN = 0, colorBLUE = 200;
    static int colorL;
    static int IDC_Line, IDC_L;

    switch (message)
    {
        //сообщение при открытии диалогового окна
    case WM_INITDIALOG:
        hRED = GetDlgItem(hDlg, IDC_RED);
        //устанавливаем диапазон движка линейки RED
        SetScrollRange(hRED, SB_CTL, 0, 255, FALSE);
        //устанавливаем позицию движка линейки RED
        SetScrollPos(hRED, SB_CTL, colorRED, TRUE);
        //выводим в окне IDC_R позицию движка линейки RED
        SetDlgItemInt(hDlg, IDC_R, colorRED, 0);

        // получаем дескриптор линейки GREEN
        hGREEN = GetDlgItem(hDlg, IDC_GREEN);
        //устанавливаем диапазон движка линейки GREEN
        SetScrollRange(hGREEN, SB_CTL, 0, 255, FALSE);
        //устанавливаем позицию движка линейки GREEN
        SetScrollPos(hGREEN, SB_CTL, colorGREEN, TRUE);
        //выводим в окне IDC_G позицию движка линейки GREEN
        SetDlgItemInt(hDlg, IDC_G, colorGREEN, 0);

        // получаем дескриптор линейки BLUE
        hBLUE = GetDlgItem(hDlg, IDC_BLUE);
        //устанавливаем диапазон движка линейки BLUE
        SetScrollRange(hBLUE, SB_CTL, 0, 255, FALSE);
        //устанавливаем позицию движка линейки BLUE
        SetScrollPos(hBLUE, SB_CTL, colorBLUE, TRUE);
        //выводим в окне IDC_B позицию движка линейки BLUE
        SetDlgItemInt(hDlg, IDC_B, colorBLUE, 0);

        break;


        //сообщение, приходящее от элементов диалогового окна
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)  //нажата кнопка
        {
            DestroyWindow(hDlg);
        }
        break;

    case WM_VSCROLL:

        if ((HWND)lParam == hRED)
        {
            hLine = hRED;
            IDC_L = IDC_R;
            colorL = colorRED;
        }
        if ((HWND)lParam == hGREEN)
        {
            hLine = hGREEN;
            IDC_L = IDC_G;
            colorL = colorGREEN;
        }
        if ((HWND)lParam == hBLUE)
        {
            hLine = hBLUE;
            IDC_L = IDC_B;
            colorL = colorBLUE;
        }
        //устанавливаем фокус на линейку
        SetFocus(hLine);
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            if (colorL > 0)
                colorL--;
            break;

        case SB_LINEDOWN:
            if (colorL < 255)
                colorL++;
            break;

        case SB_PAGEUP:
            colorL -= 16;
            break;

        case SB_PAGEDOWN:
            colorL += 16;
            break;

        case SB_THUMBTRACK:

        case SB_THUMBPOSITION:
            colorL = HIWORD(wParam);
            break;
        }
        //выводим в окне IDC_L позицию движка линейки
        SetDlgItemInt(hDlg, IDC_L, colorL, 0);
        //устанавливаем позицию движка линейки
        SetScrollPos(hLine, SB_CTL, colorL, TRUE);

        if ((HWND)lParam == hRED)
            colorRED = colorL;
        if ((HWND)lParam == hGREEN)
            colorGREEN = colorL;
        if ((HWND)lParam == hBLUE)
            colorBLUE = colorL;

        //цвет окна вывода графика синусоиды
        OBJ_col_Red = colorRED; OBJ_col_Green = colorGREEN; OBJ_col_Blue = colorBLUE;

        //получаем дескриптор окна родителя
        hWnd = GetParent(hDlg);
        //перерисовываем окно родителя
        InvalidateRect(hWnd, NULL, TRUE);

        break;


    case WM_CTLCOLORSTATIC:
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_C_R))
        {
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(255, 0, 0));
        }
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_C_G))
        {
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(0, 255, 0));
        }
        if ((HWND)lParam == GetDlgItem(hDlg, IDC_C_B))
        {
            SetBkColor((HDC)wParam, RGB(255, 255, 255));
            return (LRESULT)CreateSolidBrush(RGB(0, 0, 255));
        }
        break;
    }
    return 0;
}

LRESULT CALLBACK CHANGE_B_COLOR(HWND hDlg, UINT message,
    WPARAM wParam, LPARAM lParam)
{
    //дескрипторы окна родителя и линеек
    static HWND hWnd, hRED, hGREEN, hBLUE;
    static HWND hLine;
    static HBRUSH hBrush;
    //положение ползунков линеек
    static int colorRED = 0, colorGREEN = 0, colorBLUE = 200;
    static int colorL;
    static int IDC_Line, IDC_L;

    switch (message)
    {
        //сообщение при открытии диалогового окна
    case WM_INITDIALOG:
        // получаем дескриптор слайдера
        hRED = GetDlgItem(hDlg, IDC_SL_Red);
        //устанавливаем min движка слайдера
        SendMessage(hRED, TBM_SETRANGEMIN, 0, 0);
        //устанавливаем max движка слайдера
        SendMessage(hRED, TBM_SETRANGEMAX, 0, 255);
        //устанавливаем движок в положение col
        SendMessage(hRED, TBM_SETPOS, TRUE, BG_col_Red);
        //выводим в окне IDC_TR1 позицию движка слайдера
        SetDlgItemInt(hDlg, IDC_BACK_R, BG_col_Red, 0);


        // получаем дескриптор слайдера
        hGREEN = GetDlgItem(hDlg, IDC_SL_GREEN);
        //устанавливаем min движка слайдера
        SendMessage(hGREEN, TBM_SETRANGEMIN, 0, 0);
        //устанавливаем max движка слайдера
        SendMessage(hGREEN, TBM_SETRANGEMAX, 0, 255);
        //устанавливаем движок в положение col
        SendMessage(hGREEN, TBM_SETPOS, TRUE, BG_col_Green);
        //выводим в окне IDC_TR1 позицию движка слайдера
        SetDlgItemInt(hDlg, IDC_BACK_G, BG_col_Green, 0);

        // получаем дескриптор слайдера
        hBLUE = GetDlgItem(hDlg, IDC_SL_BLUE);
        //устанавливаем min движка слайдера
        SendMessage(hBLUE, TBM_SETRANGEMIN, 0, 0);
        //устанавливаем max движка слайдера
        SendMessage(hBLUE, TBM_SETRANGEMAX, 0, 255);
        //устанавливаем движок в положение col
        SendMessage(hBLUE, TBM_SETPOS, TRUE, BG_col_Blue);
        //выводим в окне IDC_TR1 позицию движка слайдера
        SetDlgItemInt(hDlg, IDC_BACK_B, BG_col_Blue, 0);
        break;


        //сообщение, приходящее от элементов диалогового окна
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL)  //нажата кнопка
        {
            EndDialog(hDlg, 0);
        }
        if (LOWORD(wParam) == IDOK)  //нажата кнопка
        {
            //цвет окна вывода графика синусоиды
            BG_col_Red = colorRED; BG_col_Green = colorGREEN; BG_col_Blue = colorBLUE;

            //получаем дескриптор окна родителя
            hWnd = GetParent(hDlg);
            //перерисовываем окно родителя
            InvalidateRect(hWnd, NULL, TRUE);

            EndDialog(hDlg, 1);
        }
        break;

    case WM_HSCROLL:

        if ((HWND)lParam == hRED)
        {
            hLine = hRED;
            IDC_L = IDC_BACK_R;
            colorL = colorRED;
        }
        if ((HWND)lParam == hGREEN)
        {
            hLine = hGREEN;
            IDC_L = IDC_BACK_G;
            colorL = colorGREEN;
        }
        if ((HWND)lParam == hBLUE)
        {
            hLine = hBLUE;
            IDC_L = IDC_BACK_B;
            colorL = colorBLUE;
        }
        //устанавливаем фокус на линейку
        SetFocus(hLine);
        switch (LOWORD(wParam))
        {
        case SB_LINEUP:
            if (colorL > 0)
                colorL--;
            break;

        case SB_LINEDOWN:
            if (colorL < 255)
                colorL++;
            break;

        case SB_PAGEUP:
            colorL -= 51;
            break;

        case SB_PAGEDOWN:
            colorL += 51;
            break;

        case SB_THUMBTRACK:

        case SB_THUMBPOSITION:
            colorL = HIWORD(wParam);
            break;
        }
        //выводим в окне IDC_L позицию движка линейки
        SetDlgItemInt(hDlg, IDC_L, colorL, 0);
        //устанавливаем позицию движка линейки
        SetScrollPos(hLine, SB_CTL, colorL, TRUE);

        if ((HWND)lParam == hRED)
            colorRED = colorL;
        if ((HWND)lParam == hGREEN)
            colorGREEN = colorL;
        if ((HWND)lParam == hBLUE)
            colorBLUE = colorL;

        break;
    }
    return 0;
}

//функция окна, принимающая сообщения
LRESULT CALLBACK  WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
        //сообщение при обновлении окна
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;

        case ID_BOXES_NOMODEL1:
            if (IsWindow(hDlgWin1))
                break;
            hDlgWin1 = CreateDialog(hInst, (LPCTSTR)IDD_DIALOG_OBJ_COLOR, hWnd, CHANGE_F_COLOR);
            break;

        case ID_BOXES_NOMODEL2:
            if (IsWindow(hDlgWin2))
                break;
            hDlgWin2 = CreateDialog(hInst, (LPCTSTR)IDD_DIALOG_WIN_SIZE, hWnd, DialProc);
            break;
        case ID_BOXES_MODEL1:
            DialogBox(hInst, (LPCTSTR)IDD_DIALOG_BACK_COLOR, hWnd, CHANGE_B_COLOR);
            break;
        }

    case WM_PAINT:
        Line_Paint(hWnd);    //функция рисования
        break;

    case WM_USER:
        Line_DC(hWnd);   //функция рисования
        break;


        //сообщение при закрытии окна
    case WM_DESTROY:
        PostQuitMessage(0);  //выход из цикла сообщений
        break;

    case WM_CREATE:
        InvalidateRect(hWnd, NULL, TRUE);
        InitCommonControls();
        break;

        //обработка сообщений по умолчанию
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;

}


//размеры поля вывода в мировой системе координат
double xLeft, xRight, yBottom, yTop;
//размеры поля вывода в пикселях в клиентской области
//окна приложения
int    nLeft, nRight, mBottom, mTop;


void drow_graph(HWND hwnd, HDC hdc)
{
    HBRUSH hbrush;
    HPEN hpen;
    hbrush = CreateSolidBrush(RGB(OBJ_col_Red, OBJ_col_Green, OBJ_col_Blue));
    hpen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    //выбираем перо <hpen2> в контекст устройства <hdc>
    SelectObject(hdc, hpen);
    //выбираем перо <hpen3> в контекст устройства <hdc>
    SelectObject(hdc, hbrush);
    Pie(hdc, xn(xLeft + 1), ym(yBottom), xn(xRight - 1), ym(yTop),
        xn(xRight - 4 + (xRight - xLeft) / 4), ym(yBottom+1), xn(xRight - 4 + (xRight - xLeft) / 4), ym(yTop-1));
    float t = 5;
    //Pie(hdc, xn(-1.5*t), ym(1 * t), xn(1.5 * t), ym(-1 * t), xn(1 * t), ym(-1 * t), xn(1 * t), ym(1 * t));
    DeleteObject(hbrush);
}

void Line_DC(HWND hwnd)
{
    //размеры окна в мировых координатах и в пикселях
    xLeft = -5; xRight = 5; yBottom = -5; yTop = 5;
    //	nLeft = 100; nRight = 450; mBottom = 350; mTop = 50;
    nLeft = 100; nRight = nLeft + nWidth;
    mTop = 50;   mBottom = mTop + mHeight;

    HDC hdc;          //дескриптор контекста устройства
    //получаем контекст устройства <hdc> для окна <hwnd>
    hdc = GetDC(hwnd);

    HDC hdcMem;
    //создаем контекст памяти связаный с контекстом экрана
    hdcMem = CreateCompatibleDC(hdc);

    HBITMAP hBitmap, hBitmapOld;
    //создаем битовую карту совместмую с контекстом экрана
    hBitmap = CreateCompatibleBitmap(hdc, nRight, mBottom);
    //помещаем битовую карту в контекст памяти
    hBitmapOld = (HBITMAP)SelectObject(hdcMem, hBitmap);

    //HDC hdc;                  //дескриптор контекста устройства
    //PAINTSTRUCT ps;           //структура для работы контеста

    //получаем контекст устройства <hdc> для окна <hwnd>
    //hdc = BeginPaint(hwnd, &ps);

    HBRUSH hbrush, hbrushOld;            //дескрипторы кистей
    HPEN hpen1, hpen2, hpen3, hpenOld;    //дескрипторы перьев

    //создаем кисть <hbrush1>, стиль - сплошной, цвет - синий
    hbrush = CreateSolidBrush(RGB(255, 255, 255));
    //выбираем кисть <hbrush> в контекст устройства <hdc>,
    //запоминаем дескриптор старой кисти <hbrushOld>
    hbrushOld = (HBRUSH)SelectObject(hdcMem, hbrush);

    //создаем перо <hpen1>, стиль - сплошной, толщина 3 пиксела, цвет - ярко-желтый
    hpen1 = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
    //выбираем перо <hpen1> в контекст устройства <hdc>,
    //запоминаем дескриптор старого пера <hpenOld>
    hpenOld = (HPEN)SelectObject(hdcMem, hpen1);

    //рисуем прямоугольник с границей
    Rectangle(hdcMem, nLeft, mBottom, nRight, mTop);


    //создаем перо <hpen2>, стиль - сплошной, толщина 1 пиксел, цвет - ярко-голубой
    hpen2 = CreatePen(PS_SOLID, 1, RGB(133, 133, 133));
    //выбираем перо <hpen2> в контекст устройства <hdc>
    SelectObject(hdcMem, hpen2);

    int nb, ne, mb, me;
    POINT  pt;

    //рисуем ось OX
    nb = xn(xLeft); mb = ym(0);
    MoveToEx(hdcMem, nb, mb, &pt);
    ne = xn(xRight); me = ym(0);
    LineTo(hdcMem, ne, me);

    //рисуем ось OY
    nb = xn(0); mb = ym(yBottom);
    MoveToEx(hdcMem, nb, mb, &pt);
    ne = xn(0); me = ym(yTop);
    LineTo(hdcMem, ne, me);


    //рисуем график по двум массивам
    SelectObject(hdcMem, hpenOld);   //выбираем старое перо <hpenOld> в контекст устройства <hdc>
    DeleteObject(hpen1);         //уничтожаем перо <hpen1>
    DeleteObject(hpen2);
    drow_graph(hwnd, hdcMem);
    //создаем перо <hpen3>, стиль - сплошной, толщина 2 пиксел, цвет - ярко-красный

    SelectObject(hdc, hbrushOld); //выбираем старую кисть <hbrushOld> в контекст устройства <hdc>
    DeleteObject(hbrush);        //уничтожаем кисть <hbrush>

    BitBlt(hdc, nLeft, mTop, nRight, mBottom, hdcMem, nLeft, mTop, SRCCOPY);

    //выбираем старую битовую карту в контекст памяти
    SelectObject(hdcMem, hBitmapOld);
    DeleteObject(hBitmap); //убираем битовую карту
    DeleteDC(hdcMem);  //  освобождаем контекст памяти

     //освобождаем контекст экрана <hdc> в окне <hwnd>
    ReleaseDC(hwnd, hdc);

}

//функция вызывается на сообщение WM_PAINT
void Line_Paint(HWND hwnd)
{
    //размеры окна в мировых координатах и в пикселях
    xLeft = -5; xRight = 5; yBottom = -5; yTop = 5;
    //	nLeft = 100; nRight = 450; mBottom = 350; mTop = 50;
    nLeft = 100; nRight = nLeft + nWidth;
    mTop = 50;   mBottom = mTop + mHeight;


    HDC hdc;                  //дескриптор контекста устройства
    PAINTSTRUCT ps;           //структура для работы контеста

    //получаем контекст устройства <hdc> для окна <hwnd>
    hdc = BeginPaint(hwnd, &ps);

    HBRUSH hbrush, hbrushOld;            //дескрипторы кистей
    HPEN hpen1, hpen2, hpen3, hpenOld;    //дескрипторы перьев

    //создаем кисть <hbrush1>, стиль - сплошной, цвет - синий
    hbrush = CreateSolidBrush(RGB(BG_col_Red, BG_col_Green, BG_col_Blue));//123
    //выбираем кисть <hbrush> в контекст устройства <hdc>,
    //запоминаем дескриптор старой кисти <hbrushOld>
    hbrushOld = (HBRUSH)SelectObject(hdc, hbrush);

    //создаем перо <hpen1>, стиль - сплошной, толщина 3 пиксела, цвет - ярко-желтый
    hpen1 = CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
    //выбираем перо <hpen1> в контекст устройства <hdc>,
    //запоминаем дескриптор старого пера <hpenOld>
    hpenOld = (HPEN)SelectObject(hdc, hpen1);

    //рисуем прямоугольник с границей
    Rectangle(hdc, nLeft, mBottom, nRight, mTop);


    //создаем перо <hpen2>, стиль - сплошной, толщина 1 пиксел, цвет - ярко-голубой
    hpen2 = CreatePen(PS_SOLID, 1, RGB(133, 133, 133));
    //выбираем перо <hpen2> в контекст устройства <hdc>
    SelectObject(hdc, hpen2);

    int nb, ne, mb, me;
    POINT  pt;

    //рисуем ось OX
    nb = xn(xLeft); mb = ym(0);
    MoveToEx(hdc, nb, mb, &pt);
    ne = xn(xRight); me = ym(0);
    LineTo(hdc, ne, me);

    //рисуем ось OY
    nb = xn(0); mb = ym(yBottom);
    MoveToEx(hdc, nb, mb, &pt);
    ne = xn(0); me = ym(yTop);
    LineTo(hdc, ne, me);


    //рисуем график по двум массивам
    SelectObject(hdc, hpenOld);   //выбираем старое перо <hpenOld> в контекст устройства <hdc>
    DeleteObject(hpen1);         //уничтожаем перо <hpen1>
    DeleteObject(hpen2);
    drow_graph(hwnd, hdc);
    //создаем перо <hpen3>, стиль - сплошной, толщина 2 пиксел, цвет - ярко-красный

    SelectObject(hdc, hbrushOld); //выбираем старую кисть <hbrushOld> в контекст устройства <hdc>
    DeleteObject(hbrush);        //уничтожаем кисть <hbrush>

            //уничтожаем перо <hpen2>
    //DeleteObject(hpen3);         //уничтожаем перо <hpen3>

    EndPaint(hwnd, &ps);        //освобождаем контекст устройства <hdc> в окне <hwnd>
}

//переход от x к пикселю n
inline int xn(double x)
{
    return (int)((x - xLeft) / (xRight - xLeft) * (nRight - nLeft)) + nLeft;
}

//переход от y к пикселю m
inline int ym(double y)
{
    return (int)((y - yBottom) / (yTop - yBottom) * (mTop - mBottom)) + mBottom;
}

