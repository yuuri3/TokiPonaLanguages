#include "Evolution.h"
#include <windows.h>

namespace
{
    // 世代あたりの系全体での借用回
    int n_borrow = 4;
    // 1世代である言語が音韻変化を起こす確率
    double p_sound_change = 0.3;
    // 音韻変化を起こしたときに音素の脱落が起こる確率
    double p_sound_loss = 0.3;
    // 1世代である言語が意味変化を起こす確率
    int p_semantic_shift = 0;
    // 意味の最大変化率
    int max_semantic_shift_rate = 0;
    // 1世代である言語の単語が脱落する確率
    int p_word_loss = 0;
    // 1世代である言語の単語が生成される確率
    int p_word_birth = 0;
    // 祖語ファイルパス
    std::string proto_language_path = "OldTokiPona.csv";
    // 音素表ファイルパス
    std::string phoneme_table_path = "Phonetics.csv";
    // 地理データファイルパス
    std::string map_path = "Map.csv";
    // 出力ファイルパス
    std::string output_path = "ignore\\Output.csv";

    std::optional<LanguageSystem> language_system;
}

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
    // 言語変化シミュレート/実行
    SimulationExecute,
    // 終了
    Quit,
    // エラー
    Error,
};

/**
 * @brief パラメータ設定
 *
 * @param paramName パラメータ名
 * @return std::string 入力
 */
std::string InputParameter(std::string paramName)
{
    std::cout << "=============================================\n";
    std::cout << paramName << " を入力してください\n";
    std::string input;
    std::cin >> input;
    return input;
}

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
        std::cout << "0 : 言語変化シミュレート\n";
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
        while (true)
        {
            std::cout << "=============================================\n";
            std::cout << "> 言語変化シミュレート\n";
            std::cout << "0 : N_BORROW                =" << n_borrow << "\n";
            std::cout << "1 : P_SOUND_CHANGE          =" << p_sound_change << "\n";
            std::cout << "2 : P_SOUND_LOSS            =" << p_sound_loss << "\n";
            std::cout << "3 : P_SEMANTIC_SHIFT        =" << p_semantic_shift << "\n";
            std::cout << "4 : MAX_SEMANTIC_SHIFT_RATE =" << max_semantic_shift_rate << "\n";
            std::cout << "5 : P_WORD_LOSS             =" << p_word_loss << "\n";
            std::cout << "6 : P_WORD_BIRTH            =" << p_word_birth << "\n";
            std::cout << "7 : PROTO_LANGUAGE_PATH     =" << proto_language_path << "\n";
            std::cout << "8 : PHONEME_TABLE_PATH      =" << phoneme_table_path << "\n";
            std::cout << "9 : MAP_PATH                =" << map_path << "\n";
            std::cout << "10 : OUTPUT_PATH            =" << output_path << "\n";
            std::cout << "e : 実行\n";
            std::cout << "q : 戻る\n";

            std::string input;
            std::cin >> input;

            if (input == "0")
            {
                n_borrow = std::stoi(InputParameter("N_BORROW"));
            }
            else if (input == "1")
            {
                p_sound_change = std::stod(InputParameter("P_SOUND_CHANGE"));
            }
            else if (input == "2")
            {
                p_sound_loss = std::stod(InputParameter("P_SOUND_LOSS"));
            }
            else if (input == "3")
            {
                p_semantic_shift = std::stod(InputParameter("P_SEMANTIC_SHIFT"));
            }
            else if (input == "4")
            {
                max_semantic_shift_rate = std::stod(InputParameter("MAX_SEMANTIC_SHIFT_RATE"));
            }
            else if (input == "5")
            {
                p_word_loss = std::stod(InputParameter("P_WORD_LOSS"));
            }
            else if (input == "6")
            {
                p_word_birth = std::stod(InputParameter("P_WORD_BIRTH"));
            }
            else if (input == "7")
            {
                proto_language_path = InputParameter("PROTO_LANGUAGE_PATH");
            }
            else if (input == "8")
            {
                phoneme_table_path = InputParameter("PHONEME_TABLE_PATH");
            }
            else if (input == "9")
            {
                map_path = InputParameter("MAP_PATH");
            }
            else if (input == "10")
            {
                output_path = InputParameter("OUTPUT_PATH");
            }
            else if (input == "e")
            {
                return WindowType::SimulationExecute;
            }
            else if (input == "q")
            {
                return WindowType::Home;
            }
        }
        return WindowType::Home;
    }
    case WindowType::SimulationExecute:
    {
        std::cout << "=============================================\n";
        std::cout << "> 言語変化シミュレート > 実行\n";
        language_system = evolution(
            n_borrow,
            p_sound_change,
            p_sound_loss,
            p_semantic_shift,
            max_semantic_shift_rate,
            p_word_loss,
            p_word_birth,
            proto_language_path,
            phoneme_table_path,
            map_path,
            output_path);
        if (language_system)
        {
            std::cout << "シミュレート完了\n";
            std::cout << "任意のキーを押してください\n";
            std::string input2;
            std::cin >> input2;
            return WindowType::Simulation;
        }
        else
        {
            std::cout << "シミュレート失敗\n";
            std::cout << "任意のキーを押してください\n";
            std::string input2;
            std::cin >> input2;
            return WindowType::Simulation;
        }
    }

    default:
        return WindowType::Error;
    }
    return WindowType::Error;
}

int main()
{
    // コンソールの文字コードを UTF-8 (65001) に設定
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001); // 入力側も UTF-8 に設定
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