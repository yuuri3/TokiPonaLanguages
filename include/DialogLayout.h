#pragma once

#include "stdafx.h"

constexpr int HELP_BUTTON_ID = -1;
constexpr int OK_BUTTON_ID = -2;
constexpr int CANCEL_BUTTON_ID = -3;

/**
 * @brief 各要素が扱うデータ型
 */
enum class DialogDataType
{
    String,          // 文字列
    StringArray,     // 文字列の配列
    StringPairArray, // タイトルと文字列の配列
    Boolean,         // 真偽値
    Table,           // 表
    NoData,          // データなし（ボタンのみなど）
};

/**
 * @brief ダイアログ内の各セクション（要素）のプロパティ
 */
struct DialogElement
{
    std::string Title;                                // タイトル
    DialogDataType DataType = DialogDataType::String; // データ型（デフォルトを設定）

    bool IsEditable = true; // 編集可能か（テキストボックスのReadOnly等）
    std::string ButtonName; // ボタン名
    bool HasButton = false; // ボタンの有無

    bool HasContextMenu = false; // 右クリックメニュー設定
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
    std::map<int, class QPushButton *> Buttons; // ボタン
};

/**
 * @brief セルの情報を保持する構造体
 */
struct CellInfo
{
    int row;
    int column;
    std::string place;
    QPoint globalPos;
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
    // 各要素のセッター（レイアウト生成前）
    // ==========================================

    void SetTitle(int id, const std::string &title);
    void SetDataType(int id, DialogDataType dataType);
    void SetIsEditable(int id, bool isEditable);
    void SetButton(int id, std::string buttonName);
    void SetHasContextMenu(int id, bool hasContextMenu);

    // ==========================================
    // レイアウト生成
    // ==========================================

    void GenerateLayout(class QWidget *parent);

    // ==========================================
    // ゲッタ
    // ==========================================

    bool HasHelpButton() const;
    bool HasOKButton() const;
    bool HasButton(const int id) const;
    int GetCurrentRow(const int id) const;
    std::optional<CellInfo> GetCellInfo(const int id, const QPoint &pos) const;
    bool HasDataInRow(const int id, const int row) const;
    bool HasDataInColumn(const int id, const int column) const;

    // ==========================================
    // 各ウィジェット操作（レイアウト生成後）
    // ==========================================

    void Clear(const int id);

    void SetData(const int id, const std::vector<std::string> &values);
    void SetData(const int id, const std::vector<std::vector<std::string>> &values, bool isEdit = false);

    const std::vector<std::string> GetLine(const int id);
    const int GetLineCount(const int id) const;

    void MoveUp(const int id, const int lineIndex);
    void MoveDown(const int id, const int lineIndex);
    void AddLine(const int id, const int lineIndex);
    void DeleteLine(const int id, const int lineIndex);

    void SetText(const int id, const std::string text);
    void ActivateButton(const int id, const int isActivete);

    // ==========================================
    // イベントの接続
    // ==========================================

    template <typename Receiver, typename Slot>
    void ConnectButtonClicked(const int id, Receiver *receiver, Slot slot) const;
    template <typename Receiver, typename Slot>
    void ConnectClicked(const int id, Receiver *receiver, Slot slot) const;
    template <typename Receiver, typename Slot>
    void ConnectRightClicked(const int id, Receiver *receiver, Slot slot) const;
    template <typename Receiver, typename Slot>
    void AddLineAndConnectRightClicked(const int id, const std::vector<std::string> &values, const std::vector<int> &widths, Receiver *receiver, Slot slot);
    template <typename Receiver, typename Slot>
    void ConnectContextMenu(const int id, Receiver *receiver, Slot slot) const;
    template <typename Receiver, typename Slot>
    void ConnectItemChanged(const int id, Receiver *receiver, Slot slot) const;

private:
    // ==========================================
    // 全体設定（主要なボタンの有無）
    // ==========================================

    std::string MainTitle_ = "";
    bool HasHelpButton_ = true;
    bool HasOKButton_ = true;
    bool HasCancelButton_ = true;

    // ==========================================
    // 各要素のマップ (ID -> 要素プロパティ)
    // ==========================================

    std::map<int, DialogElement> Elements_;
    GeneratedDialogUI UI_;
};

#include "DialogLayout.inl"