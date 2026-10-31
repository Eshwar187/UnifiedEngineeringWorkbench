#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>
#include "thermal/thermal_solver.hpp"

#pragma comment(lib, "comctl32.lib")

static HWND g_output = nullptr;

static void append_text(const std::string& s) {
    int len = GetWindowTextLengthA(g_output);
    SendMessageA(g_output, EM_SETSEL, len, len);
    SendMessageA(g_output, EM_REPLACESEL, FALSE, (LPARAM)s.c_str());
}

static void run_thermal() {
    try {
        std::filesystem::create_directories("results");
        std::ostringstream report;
        report << "THERMAL FEA — 1D steady-state rod\n"
               << "L=1 m | A=1e-4 m^2 | k=10 W/(m K) | q=10000 W/m^3\n"
               << "Boundary conditions: T(0)=100 C, T(L)=20 C\n\n";
        for (int N : {10, 20, 40, 80}) {
            auto result = thermal::solve_uniform_rod(N);
            const std::string path = "results/thermal_N" + std::to_string(N) + ".csv";
            const bool saved = thermal::write_csv(result, path);
            report << "Elements: " << N
                   << " | max midpoint error: " << result.max_midpoint_error
                   << " C | CSV: " << (saved ? path : "EXPORT FAILED") << "\r\n";
        }
        report << "\r\nFEA calculation completed.\r\n";
        append_text(report.str());
    } catch (const std::exception& e) {
        append_text(std::string("ERROR: ") + e.what() + "\r\n");
    }
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        CreateWindowA("STATIC", "UNIFIED ENGINEERING WORKBENCH",
            WS_CHILD | WS_VISIBLE, 20, 15, 500, 30, hwnd, nullptr, nullptr, nullptr);
        CreateWindowA("BUTTON", "Run Thermal FEA (10 / 20 / 40 / 80 elements)",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 55, 390, 35,
            hwnd, (HMENU)1001, nullptr, nullptr);
        g_output = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_AUTOVSCROLL |
            ES_READONLY | WS_VSCROLL,
            20, 105, 740, 400, hwnd, nullptr, nullptr, nullptr);
        append_text("Prototype ready.\r\nSelect Run Thermal FEA to execute the C++ solver.\r\n");
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(wp) == 1001) { run_thermal(); return 0; }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int show) {
    const char CLASS_NAME[] = "UEWMainWindow";
    WNDCLASSA wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = instance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    if (!RegisterClassA(&wc)) return 1;

    HWND hwnd = CreateWindowA(CLASS_NAME, "Unified Engineering Workbench",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 580,
        nullptr, nullptr, instance, nullptr);
    if (!hwnd) return 2;
    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return static_cast<int>(msg.wParam);
}
