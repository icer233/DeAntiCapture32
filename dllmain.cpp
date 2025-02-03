#include <Windows.h>
#include <stdio.h>

HANDLE g_hThread = NULL;
BOOL g_bRunning = FALSE; 

DWORD WINAPI MonitorThread(LPVOID lpParam) {
    // 目标窗口类名和标题
    const char* TARGET_CLASS = "Chrome_WidgetWin_1";
    const char* TARGET_TITLE = "爱问云";

    while (g_bRunning) {
        // 查找目标窗口
        HWND hWnd = FindWindowA(TARGET_CLASS, TARGET_TITLE);
        if (!hWnd) {
            Sleep(1000); // 窗口未找到，等待后重试
            continue;
        }

        // 获取当前窗口的显示亲缘性
        DWORD dwAffinity = 0;
        if (!GetWindowDisplayAffinity(hWnd, &dwAffinity)) {
            DWORD error = GetLastError();
            char buf[100];
            sprintf_s(buf, "获取亲缘性失败，错误码：%d", error);
            MessageBoxA(NULL, buf, "反反截屏工具 作者：ICER233", MB_OK);
            continue;
        }

        // 果亲缘性不是WDA_NONE，则强制重置
        if (dwAffinity != WDA_NONE) {
            if (SetWindowDisplayAffinity(hWnd, WDA_NONE)) {
                MessageBoxA(NULL, "已重置亲缘性为WDA_NONE！", "反反截屏工具 作者：ICER233", MB_OK);
            }
            else {
                DWORD error = GetLastError();
                char buf[100];
                sprintf_s(buf, "重置失败，错误码：%d", error);
                MessageBoxA(NULL, buf, "反反截屏工具 作者：ICER233", MB_OK);
            }
        }

        // 间隔1秒后再次检查
        Sleep(1000);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        MessageBoxA(NULL, "反反截屏工具注入成功！", "反反截屏工具 作者：ICER233", MB_OK);
        // 启动监控线程
        g_bRunning = TRUE;
        g_hThread = CreateThread(NULL, 0, MonitorThread, NULL, 0, NULL);
        if (g_hThread == NULL) {
            MessageBoxA(NULL, "线程创建失败！", "反反截屏工具 作者：ICER233", MB_OK);
        }
        break;

    case DLL_PROCESS_DETACH:
        // 通知线程退出并等待
        g_bRunning = FALSE;
        if (g_hThread) {
            WaitForSingleObject(g_hThread, 5000); // 等待线程退出
            CloseHandle(g_hThread);
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}