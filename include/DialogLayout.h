#pragma once

#include "stdafx.h"
#include <map>
#include <string>

/**
 * @brief 各要素が扱うデータ型
 */
enum class DialogDataType
{
    String,          // 文字列
    StringArray,     // 文字列の配列
    StringPairArray, // タイトルと文字列の配列
    Boolean,         // 真偽値
};

/**
 * @brief ダイアログ内の各セクション（要素）のプロパティ
 */
struct DialogElement
{
    std::string Title;                                // タイトル
    DialogDataType DataType = DialogDataType::String; // データ型（デフォルトを設定）

    bool IsEditable = true;       // 編集可能か（テキストボックスのReadOnly等）
    bool HasEditButton = false;   // 編集ボタンの有無
    bool HasSelectButton = false; // 選択ボタンの有無
    bool HasAddButton = false;    // 追加ボタンの有無

    // 右クリックメニュー設定
    bool HasContextMenu = false;
};

/**
 * @brief 自動生成されたレイアウトとウィジェット群を保持する構造体
 */
struct GeneratedDialogUI
{
    class QVBoxLayout *MainLayout = nullptr;

    class QPushButton *HelpButton = nullptr;
    class QPushButton *OkButton = nullptr;
    class QPushButton *CancelButton = nullptr;

    // IDをキーにして、生成された各ウィジェットのポインタを保持
    // QLineEdit, QListWidget, QCheckBox, QTableWidget(StringPairArray用) など
    std::map<int, class QWidget *> Inputs;
    std::map<int, class QPushButton *> EditButtons;   // 「編集」ボタン
    std::map<int, class QPushButton *> SelectButtons; // 「選択」ボタン
    std::map<int, class QPushButton *> AddButtons;    // 「追加」ボタン
};

/**
 * @brief ダイアログ全体のレイアウト構成を管理するデータクラス
 */
class DialogLayout
{
public:
    DialogLayout() = default;
    ~DialogLayout() = default;

    static DialogLayout Create(const std::string &title, bool hasHelpButton, bool hasOkButton, bool hasCancelButton);

    // ==========================================
    // 各要素の個別設定用セッター群
    // ==========================================

    void SetTitle(int id, const std::string &title);
    void SetDataType(int id, DialogDataType dataType);
    void SetIsEditable(int id, bool isEditable);
    void SetHasEditButton(int id, bool hasEditButton);
    void SetHasSelectButton(int id, bool hasSelectButton);
    void SetHasAddButton(int id, bool hasAddButton);
    void SetHasContextMenu(int id, bool hasContextMenu);

    void GenerateLayout(class QWidget *parent);
    void Clear(const int id);
    std::vector<QLineEdit *> AddLine(const int id, const std::vector<std::string> &values, const std::vector<int> &widths);

    const GeneratedDialogUI &GetUI() const;

private:
    // ==========================================
    // 全体設定（主要なボタンの有無）
    // ==========================================

    std::string MainTitle = "";
    bool HasHelpButton = true;
    bool HasOkButton = true;
    bool HasCancelButton = true;

    // ==========================================
    // 各要素のマップ (ID -> 要素プロパティ)
    // ==========================================

    std::map<int, DialogElement> Elements;
    GeneratedDialogUI UI;
};