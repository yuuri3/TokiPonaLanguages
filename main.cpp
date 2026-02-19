#include "include\\Evolution.h"
#include <windows.h>

namespace
{
    // ウィンドウ幅
    constexpr int window_width = 96;

    // 世代あたりの系全体での借用回数
    int n_loanword = 4;
    // 1世代である言語が音韻変化を起こす確率
    double p_phonological_change = 0.3;
    // 音韻変化を起こしたときに音素の脱落が起こる確率
    double p_phonological_change_loss = 0.3;
    // 1世代である言語が意味変化を起こす確率
    int p_semantic_shift = 0;
    // 意味の最大変化率
    int max_semantic_shift_rate = 0;
    // 1世代である言語の単語が脱落する確率
    int p_obsolete_word = 0;
    // 1世代である言語の単語が生成される確率
    int p_compound = 0;
    // 祖語ファイルパス
    std::string proto_language_path = "ProtoLanguage.csv";
    // 音素表ファイルパス
    std::string phoneme_table_path = "Phoneme.csv";
    // 地理データファイルパス
    std::string geometry_path = "Geometry.csv";
    // 出力ファイルパス
    std::string output_path = "ignore\\Output.csv";

    // 生成した語族データ
    std::optional<LanguageFamilySimulator> simulator;

    // 選択した地域名
    std::string selected_place;
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
    // 言語変化シミュレート/結果
    SimulationDisplay,
    // 言語変化シミュレート/結果/単語
    SimulationDisplayWord,
    // ファイル選択
    SellectFile,
    // ファイル選択/結果
    SellectFileDisplay,
    // ファイル選択/結果/単語
    SellectFileDisplayWord,
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
 * @brief 複数列表示
 *
 * @param strs 表示文字列
 */
void DisplayMulti(std::vector<std::string> strs)
{
    size_t width = 0;
    for (const auto str : strs)
    {
        width = std::max(width, str.size());
    }
    width += 8;
    int nColumn = window_width / width;
    for (int i = 0; i < strs.size(); i++)
    {
        std::cout << i << " : " << strs[i];
        for (int j = 0; j < width - strs[i].length() - std::to_string(i).length() - 3; j++)
        {
            std::cout << " ";
        }
        if ((i + 1) % nColumn == 0)
        {
            std::cout << "\n";
        }
    }
    std::cout << "\n";
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
        std::cout << "1 : ファイル選択\n";
        std::cout << "q : 終了\n";
        std::string input;
        std::cin >> input;
        if (input == "0")
        {
            return WindowType::Simulation;
        }
        if (input == "1")
        {
            return WindowType::SellectFile;
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
            std::cout << "0 : N_LOANWORD              =" << n_loanword << "\n";
            std::cout << "1 : P_PHONOLOGICAL_CHANGE   =" << p_phonological_change << "\n";
            std::cout << "2 : P_PHONOLOGICAL_LOSS     =" << p_phonological_change_loss << "\n";
            std::cout << "3 : P_SEMANTIC_SHIFT        =" << p_semantic_shift << "\n";
            std::cout << "4 : MAX_SEMANTIC_SHIFT_RATE =" << max_semantic_shift_rate << "\n";
            std::cout << "5 : P_OBSOLETE_WORD         =" << p_obsolete_word << "\n";
            std::cout << "6 : P_COMPOUND              =" << p_compound << "\n";
            std::cout << "7 : PROTO_LANGUAGE_PATH     =" << proto_language_path << "\n";
            std::cout << "8 : PHONEME_TABLE_PATH      =" << phoneme_table_path << "\n";
            std::cout << "9 : GEOMETRY_PATH           =" << geometry_path << "\n";
            std::cout << "10 : OUTPUT_PATH            =" << output_path << "\n";
            std::cout << "e : 実行\n";
            std::cout << "q : 戻る\n";

            std::string input;
            std::cin >> input;

            if (input == "0")
            {
                n_loanword = std::stoi(InputParameter("N_LOANWORD"));
            }
            else if (input == "1")
            {
                p_phonological_change = std::stod(InputParameter("P_PHONOLOGICAL_CHANGE"));
            }
            else if (input == "2")
            {
                p_phonological_change_loss = std::stod(InputParameter("P_PHONOLOGICAL_LOSS"));
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
                p_obsolete_word = std::stod(InputParameter("P_OBSOLETE_WORD"));
            }
            else if (input == "6")
            {
                p_compound = std::stod(InputParameter("P_COMPOUND"));
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
                geometry_path = InputParameter("GEOMETRY_PATH");
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
        simulator = evolution(
            n_loanword,
            p_phonological_change,
            p_phonological_change_loss,
            p_semantic_shift,
            max_semantic_shift_rate,
            p_obsolete_word,
            p_compound,
            proto_language_path,
            phoneme_table_path,
            geometry_path,
            output_path);
        if (simulator)
        {
            std::cout << "シミュレート完了\n";
            std::cout << "任意のキーを押してください\n";
            std::string input2;
            std::cin >> input2;
            return WindowType::SimulationDisplay;
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
    case WindowType::SimulationDisplay:
    {
        std::cout << "=============================================\n";
        std::cout << "> 言語変化シミュレート > 表示\n";

        const auto geometry = getNonEmptyStrings(simulator->LanguageFamily_.Geography);
        DisplayMulti(geometry);

        std::cout << "q : 戻る\n";
        std::string input;
        std::cin >> input;

        if (input == "q")
        {
            return WindowType::Home;
        }
        else if (std::stoi(input) < geometry.size())
        {
            selected_place = geometry[std::stoi(input)];
            return WindowType::SimulationDisplayWord;
        }
    }
    case WindowType::SimulationDisplayWord:
    {
        std::cout << "=============================================\n";
        std::cout << "> 言語変化シミュレート > 表示 > 個別言語\n";

        const auto words = simulator->GetWords(selected_place);
        DisplayMulti(words);

        std::cout << "q : 戻る\n";
        std::string input;
        std::cin >> input;

        if (input == "q")
        {
            return WindowType::SimulationDisplay;
        }
    }
    case WindowType::SellectFile:
    {
        std::cout << "=============================================\n";
        std::cout << "> ファイル選択\n";
        std::cout << "表示するファイルパスを入力してください。\n";

        std::string input;
        std::cin >> input;

        bool hasImport = true;
        LanguageFamily languageFamily;
        if (!languageFamily.Import(input))
        {
            hasImport = false;
        }
        simulator = LanguageFamilySimulator::Create(languageFamily);

        if (simulator && hasImport)
        {
            return WindowType::SellectFileDisplay;
        }
        else
        {
            std::cout << "ファイル読み込み失敗\n";
            std::cout << "任意のキーを押してください\n";
            std::string input2;
            std::cin >> input2;
            return WindowType::Simulation;
        }
    }
    case WindowType::SellectFileDisplay:
    {
        std::cout << "=============================================\n";
        std::cout << "> ファイル選択 > 結果\n";

        const auto geometry = getNonEmptyStrings(simulator->LanguageFamily_.Geography);
        DisplayMulti(geometry);

        std::cout << "q : 戻る\n";
        std::string input;
        std::cin >> input;

        if (input == "q")
        {
            return WindowType::Home;
        }
        else if (std::stoi(input) < geometry.size())
        {
            selected_place = geometry[std::stoi(input)];
            return WindowType::SellectFileDisplayWord;
        }
    }
    case WindowType::SellectFileDisplayWord:
    {
        std::cout << "=============================================\n";
        std::cout << "> ファイル選択 > 結果 > 個別言語\n";

        const auto words = simulator->GetWords(selected_place);
        DisplayMulti(words);

        std::cout << "q : 戻る\n";
        std::string input;
        std::cin >> input;

        if (input == "q")
        {
            return WindowType::SellectFileDisplay;
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