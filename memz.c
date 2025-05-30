#include <windows.h>
#include <stdlib.h>
#include <time.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);

            static int offset = 0;
            offset = (offset + 3) % 20;
            SetTextColor(hdc, RGB(rand() % 256, rand() % 256, rand() % 256));
            SetBkMode(hdc, TRANSPARENT);
            TextOut(hdc, 50 + offset, 50 + offset, "MEMZ-style GDI Prank!", 22);

            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InvertScreenColors(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    RECT rc;
    GetClientRect(hwnd, &rc);
    BitBlt(hdc, 0, 0, rc.right, rc.bottom, hdc, 0, 0, NOTSRCCOPY);
    ReleaseDC(hwnd, hdc);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    srand((unsigned int)time(NULL));

    const char CLASS_NAME[] = "MemzPrankClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,
        CLASS_NAME,
        "MEMZ-style GDI Prank",
        WS_POPUP,
        0, 0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        NULL, NULL, hInstance, NULL);

    if (!hwnd) return 0;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    HDC hdc = GetDC(hwnd);

    MSG msg;
    int frame = 0;

    while (1) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                goto cleanup;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        int sw = GetSystemMetrics(SM_CXSCREEN);
        int sh = GetSystemMetrics(SM_CYSCREEN);

        // MEMZ-Tunneleffekt
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, sw, sh);
        SelectObject(memDC, memBitmap);
        BitBlt(memDC, 0, 0, sw, sh, hdc, 0, 0, SRCCOPY);
        StretchBlt(hdc, 30, 30, sw - 60, sh - 60, memDC, 0, 0, sw, sh, SRCCOPY);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        // Zufällige Linien
        int x1 = rand() % sw;
        int y1 = rand() % sh;
        int x2 = rand() % sw;
        int y2 = rand() % sh;

        HPEN pen = CreatePen(PS_SOLID, 3, RGB(rand() % 256, rand() % 256, rand() % 256));
        SelectObject(hdc, pen);
        MoveToEx(hdc, x1, y1, NULL);
        LineTo(hdc, x2, y2);
        DeleteObject(pen);

        // Bildschirm invertieren
        if (frame % 60 == 0) {
            InvertScreenColors(hwnd);
        }

        frame++;
        Sleep(30);
    }

cleanup:
    ReleaseDC(hwnd, hdc);
    return 0;
}
