#include "framework.h"
#include "TextInsideTable.h"

#define MAX_LOADSTRING 100 // размер буфера на загрузку строки

#define WM_SETFONT 24

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

CONST INT numOfRows = 4;                    // количество строк в итоговой таблице
CONST INT numOfColumns = 4;                 // количество столбцов в итоговой таблице
char slist[numOfRows * numOfColumns][255];  // массив со строками, которые будут записываться в каждую из ячеек таблицы

// Предобъявление функций:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

VOID ArrayInitialization(HANDLE hFile);
VOID DrawTable(HWND hWnd, RECT wndRect);
VOID DrawLine(HDC hdc, COLORREF color, int x1, int y1, int x2, int y2);
VOID DrawVerticalTableLines(HDC hdc, COLORREF color, INT cellSizeX, INT tableSizeY);

// APIENTRY = WINAPI - Entry point
int APIENTRY wWinMain(HINSTANCE hInstance,     // указатель на начало исполняемого модуля
    HINSTANCE hPrevInstance, // same (not used)
    LPWSTR lpCmdLine,        // аргументы, передаваемые в программу
    int nCmdShow)            // число, показывающее, как будет развёрнуто окно
{
    UNREFERENCED_PARAMETER(hPrevInstance);  // убираем warnings о неиспользуемых переменных
    UNREFERENCED_PARAMETER(lpCmdLine);
    // Инициализация глобальных строк
    // Параметры: дискриптор экземпляра модуля, идентификатор загруженной строки,указатель на буфер, размер буфера
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TEXTINSIDETABLE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TEXTINSIDETABLE));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}
// ATOM = Access to Memory
// Регистрация класса окна
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex; // Same with WNDCLASS, but has sbSize variable and icon

    wcex.cbSize = sizeof(WNDCLASSEX); // specifies the size of the structure

    wcex.style = CS_HREDRAW | CS_VREDRAW;  // Внутренняя область окна должна быть перерисована при изменении ширины окна |
    // Внутренняя область окна должна быть перерисована при изменении высоты окна

    wcex.lpfnWndProc = WndProc;  // регистрация процедуры по обработке сообщений
    wcex.cbClsExtra = 0; // The number of extra bytes to allocate following the window-class structure. The system initializes the bytes to zero
    wcex.cbWndExtra = 0; // The number of extra bytes to allocate following the window instance. The system initializes the bytes to zero
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_TEXTINSIDETABLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

// Обрабатываем сообщения в главном окне
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: // создание окна, вызывается ещё до его отрисовки
    {
        const TCHAR szFileName[] = "Text/text.txt"; // File with the text

        // Создаём дискриптор файла (число, с помощью которого можно идентифицировать объект)
        // параметры: имя файла, режим доступа, совместный доступ, дескр. защиты, действие, атрибуты файла, дескр. шаблона файла
        HANDLE hFile = CreateFile(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (INVALID_HANDLE_VALUE == hFile) // если дескриптор не был создан
        {
            MessageBox(hWnd, "File not found!", "Error", MB_OK); // выводим сообщение об ошибке
        }
        else // если же всё хорошо
        {
            ArrayInitialization(hFile); // генерируем массивы текста для таблицы 
            CloseHandle(hFile); // закрываем дескриптор
        }
    } break;

    case WM_PAINT: // отрисовать главное окно
    {
        RECT wndRect; // размеры окна
        /*HFONT CreateFontA(
        [in] int    cHeight,
        [in] int    cWidth,
        [in] int    cEscapement,
        [in] int    cOrientation,
        [in] int    cWeight,
        [in] DWORD  bItalic,
        [in] DWORD  bUnderline,
        [in] DWORD  bStrikeOut,
        [in] DWORD  iCharSet,
        [in] DWORD  iOutPrecision,
        [in] DWORD  iClipPrecision,
        [in] DWORD  iQuality,
        [in] DWORD  iPitchAndFamily,
        [in] LPCSTR pszFaceName
        );*/
        HFONT hFont = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, _T("Consolas"));
        SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, NULL);
        /*hFont1 = CreateFont(48, 0, 0, 0, FW_DONTCARE, FALSE, TRUE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Impact"));
            hFontOriginal = (HFONT)SelectObject(hdc, hFont1);
            DrawText(hdc, TEXT("Drawing Text with Impact"), -1,&rect, DT_NOCLIP);

           hFont2 = CreateFont(36,20,-300,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Times New Roman"));
            SelectObject(hdc,hFont2);

           hFont3 = CreateFont(36,10,250,0,FW_DONTCARE,FALSE,TRUE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY, VARIABLE_PITCH,TEXT("Arial"));
            SelectObject(hdc,hFont3);
        */
        GetClientRect(hWnd, &wndRect); // получаем размер и записываем его в переменную
        DrawTable(hWnd, wndRect); // рисуем таблицу
    } break;

    case WM_DESTROY:
    {
        PostQuitMessage(0); // посылаем сообщение на выход
    } break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// генерируем массивы текста в ячейках таблицы
VOID ArrayInitialization(HANDLE hFile)
{
    // Суммарное количество ячеек с текстом
    const int num = numOfColumns * numOfRows;

    for (int i = 0; i < num; i++)
    {
        char cBufferText[255]; // Выделяем буфер
        DWORD dwByte;
        DWORD numOfBytesToRead = 200;
        // параметры: дескриптор файла на чтение, буфер для записи, число байтов для чтение, число прочитанных байтов, асинхронный буфер
        ReadFile(hFile, cBufferText, numOfBytesToRead, &dwByte, NULL); // читаем файл
        cBufferText[numOfBytesToRead] = 0; // признак конца строки

        // параметры: куда копируем, сколько байт копируем, откуда копируем
        strcpy_s(slist[i], sizeof(cBufferText), cBufferText); // копируем строку
    }
}

// метод по отрисовке таблицы в главном окне
VOID DrawTable(HWND hWnd, RECT wndRect)
{
    INT indent = 5, // отступ между текстом и границами ячейки (с 4-х сторон)
        maxRowHight = 0, // высота ячейки
        sizeOfColumn; // ширина ячейки

    PAINTSTRUCT ps; // структура для начала рисования
    // параметры: дескриптор окна, которое будет перекрашено; указатель на структуру об окрашивании
    HDC hdc = BeginPaint(hWnd, &ps); // создаём идентификатор контекста отображения
    RECT rect, cellForText;
    HBRUSH brush;
    COLORREF colorText = RGB(255, 255, 255), // цвет текста
        colorBack = RGB(66, 135, 245),   // цвет заднего фона
        colorLine = RGB(255, 255, 255);  // цвет линий таблицы

    brush = CreateSolidBrush(colorBack); // создаём кисть с цветом для заднего фона
    SelectObject(hdc, brush); // выбираем объект для рисования - главное окно
    Rectangle(hdc, wndRect.left, wndRect.top, wndRect.right, wndRect.bottom); // рисуем прямоугольник по размеру главного окна
    DeleteObject(brush); // удаляем кисть

    sizeOfColumn = wndRect.right / numOfColumns; // высчитываем ширину для каждой колонки

    for (int i = 0; i < numOfRows; i++)
    {

        rect.top = maxRowHight; // изначально выставляем высоту ячейки в 0

        for (int j = 0; j < numOfColumns; j++)
        {

            rect.left = sizeOfColumn * j; // левая координата ячейки
            rect.right = wndRect.right / numOfColumns * (j + 1); // правая координата ячейки

            SetBkMode(hdc, TRANSPARENT); // рисуем поверх фона. Фон остаётся нетронутым
            SetTextColor(hdc, colorText); // устанавливаем цвет текста

            cellForText.top = rect.top + indent; // верхняя граница для текста
            cellForText.right = rect.right - indent;  // правая граница для текста
            cellForText.left = rect.left + indent; // левая граница для текста

            // рисуеv отформатированный текст в заданном прямоугольнике
            // параметры: дескриптор окна, текст для вывода, длина выводимого текста, размер полня форматирования, параметры вывода текста
            // ps текст выводится слева направо
            DrawText(hdc, (LPCSTR)slist[numOfColumns * i + j], -1, &cellForText, DT_CALCRECT | DT_WORDBREAK | DT_LEFT | DT_EDITCONTROL);
            rect.right = wndRect.right / numOfColumns * (j + 1); // правая координата ячейки
            rect.bottom = cellForText.bottom + indent; // нижняя граница для текста

            DrawText(hdc, (LPCSTR)slist[numOfColumns * i + j], -1, &cellForText, DT_WORDBREAK); // отрисовка текста, сверху - настройка таблицы

            if (rect.bottom > maxRowHight) // если в ячейку занесли текст - увеличиваем высоту этой ячейки до нужного размера
                maxRowHight = rect.bottom;
        }
        //                       x1      y1            x2           y2
        DrawLine(hdc, colorLine, 0, maxRowHight, wndRect.right, maxRowHight); // рисуем горизонтальную линию
    }

    DrawVerticalTableLines(hdc, colorLine, sizeOfColumn, maxRowHight); // рисуем вертикальную линию

    SetBkMode(hdc, OPAQUE);  // Переустанавливаем режим фона в его состояние по умолчанию
    EndPaint(hWnd, &ps);  // освобождаем объект для рисования
}

// Рисуем горизонтальную линию
VOID DrawLine(HDC hdc, COLORREF color, int x1, int y1, int x2, int y2)
{
    // содзаём перо, параметры: стиль пера (сплошной), толщина пера, цвет пера
    HPEN pen = CreatePen(PS_INSIDEFRAME, 3, color);
    POINT pt;
    SelectObject(hdc, pen); // выбираем в качестве объекта контекст изображения 
    MoveToEx(hdc, x1, y1, &pt);  // обновляем позицию текущей точки
    LineTo(hdc, x2, y2); // соединяем текущую точку с новой
    DeleteObject(pen); // удаляем перо
}

// рисуем вертикальную линию
VOID DrawVerticalTableLines(HDC hdc, COLORREF color, INT cellSizeX, INT tableSizeY)
{
    for (int i = 1; i < numOfColumns; i++)
    {
        DrawLine(hdc, color, i * cellSizeX, 0, i * cellSizeX, tableSizeY);
    }
}

// функция, которая сохраняет обработку экземпляра и создает главное окно
// дескриптор экземпляра сохраняется в глобальной переменной и выводится на главную форму
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, // регистрируем оконный класс
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd) // если ошибка 
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow); // посылаем запрос на отображение окна
    UpdateWindow(hWnd); // обновляем окно
    return TRUE;
}