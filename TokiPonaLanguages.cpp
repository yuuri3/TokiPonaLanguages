#include "Evolution.h"

/**
 * @brief ウィンドウ表示タイプ
 *
 */
enum WindowType
{
    // ホーム
    Home,
    // 言語変化シミュレート
    Simulation,
    // 終了
    Quit,
    // エラー
    Error,
};

/**
 * @brief ウィンドウ表示
 *
 * @param type ウィンドウ表示タイプ
 * @return 遷移後のタイプ
 */
WindowType DisplayWindow(WindowType type)
{
    switch (type)
    {
    case WindowType::Home:
    {
        std::cout << "=============================================\n";
        std::cout << ">\n";
        std::cout << "0 : Simulate\n";
        std::cout << "q : Quit\n";
        std::string input;
        std::cin >> input;
        if (input == "0")
        {
            return WindowType::Simulation;
        }
        else if (input == "q")
        {
            return WindowType::Quit;
        }
        else
        {
            return WindowType::Home;
        }
    }

    case WindowType::Simulation:
    {
        std::cout << "=============================================\n";
        std::cout << "> Simulate\n";
        std::cout << "N_BORROW                = 4\n";
        std::cout << "P_SOUND_CHANGE          = 0.3\n";
        std::cout << "P_SOUND_LOSS            = 0.3\n";
        std::cout << "P_SEMANTIC_SHIFT        = 0\n";
        std::cout << "MAX_SEMANTIC_SHIFT_RATE = 0\n";
        std::cout << "P_WORD_LOSS             = 0\n";
        std::cout << "P_WORD_BIRTH            = 0\n";
        std::cout << "PROTO_LANGUAGE_PATH     = OldTokiPona.csv\n";
        std::cout << "PHONEME_TABLE_PATH      = Phonetics.csv\n";
        std::cout << "MAP_PATH                = Map.csv\n";
        std::cout << "OUTPUT_PATH             = ignore\\Output.csv\n";
        evolution(4, 0.3, 0.3, 0, 0, 0, 0, L"OldTokiPona.csv", L"Phonetics.csv", L"Map.csv", L"ignore\\Output.csv");
        std::cout << "Simulation Complete\n";
        std::cout << "Press Any Button\n";
        std::string input;
        std::cin >> input;
        return WindowType::Home;
    }

    default:
        return WindowType::Error;
    }
    return WindowType::Error;
}

int main()
{
    // 起動時ウィンドウ
    WindowType type = WindowType::Home;
    for (int i = 0; i < 100; i++)
    {
        type = DisplayWindow(type);
        if (type == WindowType::Quit || type == WindowType::Error)
        {
            break;
        }
    }

    return 0;
}