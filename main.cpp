/*
 * Projekt: win32_game_core
 * Autor: Kamil Sitarski (kamikaj)
 * Opis: Pierwszy krok w nauce renderowania 2D w czystym Win32 API (bez bibliotek zewnętrznych).
 *       Aplikacja tworzy okno i rysuje gradient w pamięci RAM, który następnie wyświetla na ekranie.
 *       Projekt oparty na serii "Handmade Hero" autorstwa Casey'ego Muratori'ego (Molly Rokcet).
 */
#include <windows.h>
#include <stdint.h>

/*
 * Alisay służące do nadawania czytelniejszych nazw dla static
* internal = lokalna funkcja/zmienna plikowa (widoczna tylko w tym pliku)
* local_persist = statyczna zmienna lokalna (przechowuje stan między wywołaniami)
* global_variable = zmienna globalna (ale tylko widoczna lokalnie dzięki static)*/
#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

//Globalny przełącznik stanu – jeśli false, pętla główna się zatrzyma.
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

/* RenderWeirdGradient
 * Renderuje prosty gradient w pamięci bitmapy (software rendering).
 * Kolory są zależne od pozycji X i Y oraz przesunięcia (offsetów).*/
internal void RenderWeirdGradient(int XOFFset, int YOFFset)
{
    int Width = BitmapWidth;
    int Heigth = BitmapHeight;
    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8 *) BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; Y++){
       uint32 *Pixel = (uint32 *) Row;
       for (int X = 0; X < BitmapWidth; X++) {
           uint8 Blue = (X + XOFFset);
           uint8 Green = (Y +YOFFset);
           /* Memory:  BB GG RR xx
                       * Register:xx RR GG BB
                       * Pixel 32-bits
                       **/
           *Pixel++ = ((Green << 8) | Blue);
           /*
            *Little endian architecture
            Pixel w pamięci: BB GG RR xx (00 00 00 00)
            0x xxRRGGBB

           *Pixel = (uint8) (X + XOFFset);//Blue
           ++Pixel;

           *Pixel = (uint8) (Y + YOFFset);//Green
           ++Pixel;

           *Pixel = 0;//Red
           ++Pixel;

           *Pixel = 0;
           ++Pixel;*/
       }
       Row += Pitch;
   }
}

/* Win32ResizeDIBSection
 * Tworzy lub odtwarza bitmapę w pamięci RAM (Device Independent Bitmap),
 * która będzie później renderowana w oknie.*/
internal void Win32ResizeDIBSection(int Width, int Height){
    if (BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapHeight = Height;
    BitmapWidth = Width;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BytesPerPixel = 4;
    int BitmapMemorySize = (BitmapWidth*BitmapHeight)*BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE );
}

/* Win32UpdateWindow
 * Wyświetla zawartość bitmapy RAM na ekranie, skalując ją do rozmiaru okna.*/
internal void Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height) {
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(   DeviceContext,/*
                     X, Y, Width, Height,
                     X, Y, Width, Height,*/
                     0, 0, BitmapWidth, BitmapHeight,
                     0, 0, WindowWidth, WindowHeight,
                     BitmapMemory,
                     &BitmapInfo,
                     DIB_RGB_COLORS, SRCCOPY);
}

/*To tzw. callback okna – reaguje na zdarzenia systemowe (np. resize, paint, zamykanie).*/
LRESULT Win32MainWindowCallback(
              HWND Window,
              UINT Message,
              WPARAM WParam,
              LPARAM LParam
) {
    LRESULT Result = 0;

    switch (Message) {
        case WM_SIZE: {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Height = ClientRect.bottom - ClientRect.top;
            int Width = ClientRect.right  - ClientRect.left;
            Win32ResizeDIBSection(Width, Height);
        } break;

        case WM_DESTROY: {
            Running = false;
        } break;

        case WM_CLOSE: {
            Running = false;
        } break;

        case WM_ACTIVATEAPP: {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right  - Paint.rcPaint.left;
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
            EndPaint(Window, &Paint);
        } break;

        default: {
          //  OutputDebugStringA("Unknown Message\n");
          Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }
    return Result;
}

/* WinMain
 * To główna Pętla aplikacji Windows (odpowiednik main() w typowej C/C++ aplikacji). Robi:
 * Tworzy i rejestruje klasę okna.
 * Tworzy okno (z CreateWindowEx).
 * Jeśli się udało, ustawia Running = true i wchodzi w pętlę komunikatów (GetMessage, TranslateMessage, DispatchMessage).
 * W pętli aplikacja reaguje na zdarzenia (klawiatura, mysz, zamykanie itp.)*/

int WINAPI WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
PSTR CommandLine, int ShowCode) {
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_OWNDC | CS_HREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
    //  WindowClass.hIcon =;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";
    if (RegisterClass(&WindowClass)) {
        HWND Window =
                CreateWindowEx(
                    0,                              // Optional window styles.
                    WindowClass.lpszClassName,                     // Window class
                    "Handmade Hero",    // Window text
                    WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

                    // Size and position
                    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                    NULL,       // Parent window
                    NULL,       // Menu
                    Instance,  // Instance handle
                    NULL        // Additional application data
                );


        /*Różnica między PeakMeessage(), a GetMessage:
        GetMessage oszczędza CPU blokując program dopóki nie dostanie komunikatu by dalej mógł pracować
        PeakMessage nie czek tylko sprawdza czy coś jest i nie przerywa działania programu
        (Używane w grach lub programach czasu rzeczywistego)*/
        if (Window){
            Running = true;
            int XOFFset = 0;
            int YOFFset = 0;
            while (Running) {
                MSG Message;
                while(PeekMessageA(&Message, 0,0,0,PM_REMOVE)) {
                    if (Message.message == WM_QUIT) {
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                RenderWeirdGradient(XOFFset, YOFFset);

                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);

                XOFFset++;
            }
        }
        else {

        }
    }
    return 0;

}
