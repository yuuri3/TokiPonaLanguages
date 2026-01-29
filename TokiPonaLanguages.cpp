#include <windows.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <fstream>
#include "Evolution.h"

// コントロールID
enum
{
    ID_BTN_OLD_CSV = 101,
    ID_BTN_PHON_CSV,
    ID_BTN_MAP_CSV,
    ID_BTN_OUT_PATH,
    ID_BTN_RUN
};

// 入力欄のハンドルを保持する構造体
struct AppControls
{
    HWND hN_Bollow, hPSound, hPRemoveS, hPMeaning, hMaxMeaning, hPRemoveW, hPCreateW;
    HWND hOldPath, hPhonPath, hMapPath, hOutPath;
} ctrl;

// ファイル選択ダイアログを開く
void SelectFile(HWND owner, HWND targetEdit)
{
    wchar_t szFile[260] = {0};
    OPENFILENAMEW ofn = {sizeof(ofn)};
    ofn.hwndOwner = owner;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"CSV Files\0*.csv\0All Files\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameW(&ofn))
        SetWindowTextW(targetEdit, szFile);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        int y = 10, h = 25;

        // ラベル、テキストボックス、(任意で)参照ボタンを追加するラムダ関数
        auto AddRow = [&](const wchar_t *label, const wchar_t *text, HWND &hEdit, int btnId = 0)
        {
            // ラベル
            CreateWindowW(L"STATIC", label, WS_CHILD | WS_VISIBLE, 10, y, 180, h, hwnd, NULL, NULL, NULL);

            // テキストボックス (ファイルパス用に少し幅を調整)
            hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", text,
                                    WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, 200, y, 200, h, hwnd, NULL, NULL, NULL);

            // btnId が指定されている場合のみ、参照ボタンを作る
            if (btnId != 0)
            {
                CreateWindowW(L"BUTTON", L"...", WS_CHILD | WS_VISIBLE,
                              405, y, 35, h, hwnd, (HMENU)(INT_PTR)btnId, NULL, NULL);
            }
            y += 30; // 次の行へ
        };

        // パラメータ行（ボタンなし: 第3引数を省略）
        AddRow(L"N_BOLLOW", L"4", ctrl.hN_Bollow);
        AddRow(L"P_SOUND_CHANGE", L"0.3", ctrl.hPSound);
        AddRow(L"P_REMOVE_SOUND", L"0.3", ctrl.hPRemoveS);
        AddRow(L"P_MEANING_CHANGE", L"0.0", ctrl.hPMeaning);
        AddRow(L"MAX_MEANING_RATE", L"0.0", ctrl.hMaxMeaning);
        AddRow(L"P_REMOVE_WORD", L"0.0", ctrl.hPRemoveW);
        AddRow(L"P_CREATE_WORD", L"0.0", ctrl.hPCreateW);

        y += 10; // 少し隙間を空ける

        // ファイルパス行（ボタンあり: IDを指定）
        AddRow(L"OLD_TOKI_PONA (.csv)", L"OldTokiPona.csv", ctrl.hOldPath, ID_BTN_OLD_CSV);
        AddRow(L"PHONETICS (.csv)", L"Phonetics.csv", ctrl.hPhonPath, ID_BTN_PHON_CSV);
        AddRow(L"MAP (.csv)", L"Map.csv", ctrl.hMapPath, ID_BTN_MAP_CSV);
        AddRow(L"OUTPUT PATH", L"ignore/Output.csv", ctrl.hOutPath, ID_BTN_OUT_PATH);

        // 実行ボタン
        CreateWindowW(L"BUTTON", L"START",
                      WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 150, y + 20, 150, 40, hwnd, (HMENU)ID_BTN_RUN, NULL, NULL);
        break;
    }
    case WM_COMMAND:
    {
        // ...（ファイル選択ボタンの処理）...
        int wmId = LOWORD(wp);
        switch (wmId)
        {
        case ID_BTN_OLD_CSV:
            SelectFile(hwnd, ctrl.hOldPath);
            break;
        case ID_BTN_PHON_CSV:
            SelectFile(hwnd, ctrl.hPhonPath);
            break;
        case ID_BTN_MAP_CSV:
            SelectFile(hwnd, ctrl.hMapPath);
            break;
        case ID_BTN_OUT_PATH:
            SelectFile(hwnd, ctrl.hOutPath);
            break;

        case ID_BTN_RUN:

            // 各エディタコントロールから文字列を取得して書き込む関数
            auto ConvertToInt = [&](HWND hEdit)
            {
                wchar_t buffer[1024];
                GetWindowTextW(hEdit, buffer, sizeof(buffer));
                return std::stoi(buffer);
            };

            auto ConvertToDouble = [&](HWND hEdit)
            {
                wchar_t buffer[1024];
                GetWindowTextW(hEdit, buffer, sizeof(buffer));
                return std::stod(buffer);
            };

            auto ConvertToStr = [&](HWND hEdit) -> std::wstring
            {
                const int BUF_SIZE = 1024;
                wchar_t buffer[BUF_SIZE] = {0};

                int length = GetWindowTextW(hEdit, buffer, BUF_SIZE);

                if (length <= 0)
                    return L"";

                for (int i = 0; i < length; ++i)
                {
                    if (buffer[i] == L'\\')
                    {
                        buffer[i] = L'/';
                    }
                }

                return std::wstring(buffer, length);
            };

            MessageBoxW(hwnd, L"start simulation", L"Success", MB_OK);

            evolution(
                ConvertToInt(ctrl.hN_Bollow),
                ConvertToDouble(ctrl.hPSound),
                ConvertToDouble(ctrl.hPRemoveS),
                ConvertToDouble(ctrl.hPMeaning),
                ConvertToDouble(ctrl.hMaxMeaning),
                ConvertToDouble(ctrl.hPRemoveW),
                ConvertToDouble(ctrl.hPCreateW),
                ConvertToStr(ctrl.hOldPath),
                ConvertToStr(ctrl.hPhonPath),
                ConvertToStr(ctrl.hMapPath),
                ConvertToStr(ctrl.hOutPath));

            break;
        }
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nShow)
{
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"ParamWin";
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowW(L"ParamWin", L"TokiPonaLanguage Setting", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX, 100, 100, 480, 480, NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, nShow);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}