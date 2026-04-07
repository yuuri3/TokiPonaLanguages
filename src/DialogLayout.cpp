#include "DialogLayout.h"
#include "Utility.h"

/**
 * @brief 全体設定を指定してインスタンスを生成する
 * @param title メインタイトル
 * @param hasHelpButton ヘルプボタンの有無
 * @param hasOkButton OKボタンの有無
 * @param hasCancelButton キャンセルボタンの有無
 * @return DialogLayout 生成されたレイアウトインスタンス
 */
DialogLayout DialogLayout::Create(const std::string &title, bool hasHelpButton, bool hasOkButton, bool hasCancelButton)
{
    DialogLayout layout;
    layout.MainTitle_ = title;
    layout.HasHelpButton_ = hasHelpButton;
    layout.HasOKButton_ = hasOkButton;
    layout.HasCancelButton_ = hasCancelButton;
    return layout;
}

/**
 * @brief 指定したIDの要素にタイトルを設定する
 * @param id 要素のID
 * @param title 設定するタイトル文字列
 */
void DialogLayout::SetTitle(int id, const std::string &title)
{
    Elements_[id].Title = title;
}

/**
 * @brief 指定したIDの要素にデータ型を設定する
 * @param id 要素のID
 * @param dataType 設定するデータ型
 */
void DialogLayout::SetDataType(int id, DialogDataType dataType)
{
    Elements_[id].DataType = dataType;

    // データなしの場合は編集やメニューを無効化する
    if (dataType == DialogDataType::NoData)
    {
        Elements_[id].IsEditable = false;
        Elements_[id].HasContextMenu = false;
    }
}

/**
 * @brief 指定したIDの要素の編集可否を設定する
 * @param id 要素のID
 * @param isEditable 編集可能にする場合は true
 */
void DialogLayout::SetIsEditable(int id, bool isEditable)
{
    if (Elements_[id].DataType == DialogDataType::NoData)
    {
        return;
    }
    Elements_[id].IsEditable = isEditable;
}

/**
 * @brief 指定したIDの要素にボタンを設定する
 *
 * @param id 要素のID
 * @param buttonName ボタン名
 */
void DialogLayout::SetButton(int id, std::string buttonName)
{
    Elements_[id].HasButton = true;
    Elements_[id].ButtonName = buttonName;
}

/**
 * @brief 指定したIDの要素の右クリックメニュー設定
 * @param id 要素のID
 * @param hasContextMenu メニューを含める場合は true
 */
void DialogLayout::SetHasContextMenu(int id, bool hasContextMenu)
{
    if (Elements_[id].DataType == DialogDataType::NoData)
    {
        return;
    }
    Elements_[id].HasContextMenu = hasContextMenu;
}

/**
 * @brief 設定されたデータに基づいてレイアウトを生成する
 * @param parent 親ウィジェット
 */
void DialogLayout::GenerateLayout(QWidget *parent)
{
    if (!MainTitle_.empty())
    {
        parent->setWindowTitle(QString::fromStdString(MainTitle_));
    }
    UI_.MainLayout = new QVBoxLayout(parent);

    // ==========================================
    // 1. ヘルプボタン (最上部・右寄せ)
    // ==========================================
    if (HasHelpButton_)
    {
        auto *helpLayout = new QHBoxLayout();
        UI_.HelpButton = new QPushButton("ヘルプ", parent);
        UI_.HelpButton->setObjectName("HelpButton");
        helpLayout->addStretch();
        helpLayout->addWidget(UI_.HelpButton);
        UI_.MainLayout->addLayout(helpLayout);
    }

    // ==========================================
    // スクロールエリアの設定
    // ==========================================
    auto *scrollArea = new QScrollArea(parent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    UI_.MainLayout->addWidget(scrollArea);

    auto *scrollContent = new QWidget(scrollArea);
    auto *contentLayout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // ==========================================
    // 2. 各要素の生成 (map なので ID の昇順で処理される)
    // ==========================================
    for (const auto &[id, element] : Elements_)
    {
        // 真偽値（チェックボックス）の場合はタイトルと入力を兼ねるため特別扱い
        if (element.DataType == DialogDataType::Boolean)
        {
            auto *checkBox = new QCheckBox(QString::fromStdString(element.Title), scrollContent);
            checkBox->setEnabled(element.IsEditable);
            checkBox->setObjectName(QString("CheckBox_%1").arg(id));
            contentLayout->addWidget(checkBox);
            UI_.Inputs[id] = checkBox; // 構造体に保存
            continue;                  // 次の要素へ
        }

        // --- それ以外のデータ型（文字列・配列・データなし）---

        // タイトルと付随ボタンのレイアウト
        auto *titleLayout = new QHBoxLayout();
        if (!element.Title.empty())
        {
            titleLayout->addWidget(new QLabel(QString::fromStdString(element.Title), scrollContent));
        }

        if (element.HasButton)
        {
            UI_.Buttons[id] = new QPushButton(QString::fromStdString(element.ButtonName), scrollContent);
            UI_.Buttons[id]->setObjectName(QString("AddButton_%1").arg(id));
            UI_.Buttons[id]->setFixedWidth(50);
            titleLayout->addWidget(UI_.Buttons[id]);
        }
        titleLayout->addStretch();
        contentLayout->addLayout(titleLayout);

        // 入力ウィジェットの生成
        if (element.DataType == DialogDataType::NoData)
        {
            // データなし（ボタンのみ等）の場合は入力ウィジェットを生成しない
        }
        else if (element.DataType == DialogDataType::String)
        {
            auto *lineEdit = new QLineEdit(scrollContent);
            lineEdit->setReadOnly(!element.IsEditable);
            lineEdit->setObjectName(QString("LineEdit_%1").arg(id));
            contentLayout->addWidget(lineEdit);
            UI_.Inputs[id] = lineEdit;
        }
        else if (element.DataType == DialogDataType::StringArray)
        {
            auto *listWidget = new QListWidget(scrollContent);
            listWidget->setObjectName(QString("ListWidget_%1").arg(id));

            if (element.HasContextMenu)
            {
                listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            }
            contentLayout->addWidget(listWidget);
            UI_.Inputs[id] = listWidget;
        }
        else if (element.DataType == DialogDataType::StringPairArray)
        {
            // 2列（複数列）のテキストボックスを動的に並べるためのコンテナを作成
            auto *containerWidget = new QWidget(scrollContent);
            containerWidget->setObjectName(QString("ContainerWidget_%1").arg(id));

            auto *containerLayout = new QVBoxLayout(containerWidget);
            containerLayout->setContentsMargins(0, 0, 0, 0); // 外枠の余白をなくす
            containerLayout->setSpacing(5);                  // 行間のスペース

            // 右クリックメニュー等が必要な場合はコンテナ自体、
            // もしくは後で追加する QLineEdit 個別に設定します
            if (element.HasContextMenu)
            {
                containerWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            }

            contentLayout->addWidget(containerWidget);
            UI_.Inputs[id] = containerWidget; // Translations_ などの QWidget* に代入される
        }
        else if (element.DataType == DialogDataType::Table)
        {
            auto table = new QTableWidget(scrollContent);
            contentLayout->addWidget(table);

            if (element.HasContextMenu)
            {
                table->setContextMenuPolicy(Qt::CustomContextMenu);
            }
            UI_.Inputs[id] = table;
        }
    }

    contentLayout->addStretch(); // コンテンツを上に寄せる

    // ==========================================
    // 3. 下部ボタン (OK / キャンセル)
    // ==========================================
    if (HasOKButton_ || HasCancelButton_)
    {
        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();

        if (HasOKButton_)
        {
            UI_.OkButton = new QPushButton("OK", parent);
            UI_.OkButton->setObjectName("OkButton");
            buttonLayout->addWidget(UI_.OkButton);
        }
        if (HasCancelButton_)
        {
            UI_.CancelButton = new QPushButton("キャンセル", parent);
            UI_.CancelButton->setObjectName("CancelButton");
            buttonLayout->addWidget(UI_.CancelButton);
        }
        UI_.MainLayout->addLayout(buttonLayout);
    }

    parent->setLayout(UI_.MainLayout);
}

/**
 * @brief ヘルプボタンが有効か
 */
bool DialogLayout::HasHelpButton() const
{
    return HasHelpButton_;
}

/**
 * @brief OKボタンが有効か
 */
bool DialogLayout::HasOKButton() const
{
    return HasOKButton_;
}

/**
 * @brief ボタンが有効か
 *
 * @param id ID
 */
bool DialogLayout::HasButton(const int id) const
{
    if (Elements_.count(id) == 0)
    {
        return false;
    }
    return Elements_.at(id).HasButton;
}

/**
 * @brief 指定したIDのウィジェットで現在選択されている行番号を取得する
 * @param id 要素ID
 * @return int 選択されている行番号。選択されていない場合や非対応の型は -1
 */
int DialogLayout::GetCurrentRow(const int id) const
{
    // IDが存在しない場合は -1 を返す
    if (UI_.Inputs.count(id) == 0)
        return -1;

    auto *widget = UI_.Inputs.at(id);

    // QListWidget (StringArray) の場合
    if (auto *listWidget = qobject_cast<QListWidget *>(widget))
    {
        return listWidget->currentRow();
    }
    // QTableWidget (Table) などの場合
    else if (auto *tableWidget = qobject_cast<QTableWidget *>(widget))
    {
        return tableWidget->currentRow();
    }

    return -1;
}

/**
 * @brief ローカル座標から有効なセルの情報（行番号、場所、グローバル座標）を取得する
 *
 * @param id ID
 * @param pos ローカル座標
 * @return std::optional<CellInfo> セルの情報
 */
std::optional<CellInfo> DialogLayout::GetCellInfo(const int id, const QPoint &pos) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
    {
        return std::nullopt;
    }

    auto *widget = UI_.Inputs.at(id);
    if (auto *tableWidget = qobject_cast<QTableWidget *>(widget))
    {
        QTableWidgetItem *item = tableWidget->itemAt(pos);
        if (!item)
        {
            return std::nullopt;
        }

        const int row = tableWidget->row(item);
        const int column = tableWidget->column(item);

        QPoint globalPos = tableWidget->viewport()->mapToGlobal(pos);

        return CellInfo{row, column, globalPos};
    }
    return std::nullopt;
}

/**
 * @brief 指定した行にデータが存在するかを確認する
 *
 * @param id ID
 * @param row 行番号
 * @return bool データが存在する場合は true
 */
bool DialogLayout::HasDataInRow(const int id, const int row) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
        return false;

    if (Elements_.at(id).DataType == DialogDataType::Table)
    {
        auto *tableWidget = qobject_cast<QTableWidget *>(UI_.Inputs.at(id));
        for (int c = 0; c < tableWidget->columnCount(); ++c)
        {
            QTableWidgetItem *cellItem = tableWidget->item(row, c);
            if (cellItem && !cellItem->text().isEmpty())
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief 指定した列にデータが存在するかを確認する
 *
 * @param id ID
 * @param column 列番号
 * @return bool データが存在する場合は true
 */
bool DialogLayout::HasDataInColumn(const int id, const int column) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
        return false;

    if (Elements_.at(id).DataType == DialogDataType::Table)
    {
        auto *tableWidget = qobject_cast<QTableWidget *>(UI_.Inputs.at(id));
        for (int r = 0; r < tableWidget->rowCount(); ++r)
        {
            QTableWidgetItem *cellItem = tableWidget->item(r, column);
            if (cellItem && !cellItem->text().isEmpty())
            {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief ウィジェットをクリア
 *
 * @param id ID
 */
void DialogLayout::Clear(const int id)
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0 || Elements_.at(id).DataType == DialogDataType::NoData)
    {
        return;
    }

    auto *widget = UI_.Inputs.at(id);
    if (!widget)
    {
        return;
    }

    const auto type = Elements_.at(id).DataType;
    switch (type)
    {
    case DialogDataType::String:
        if (auto *lineEdit = qobject_cast<QLineEdit *>(widget))
        {
            lineEdit->clear();
        }
        break;

    case DialogDataType::StringArray:
        if (auto *listWidget = qobject_cast<QListWidget *>(widget))
        {
            listWidget->clear();
        }
        break;

    case DialogDataType::StringPairArray:
        // コンテナウィジェットのレイアウト内にある行ウィジェットをすべて削除
        ClearLayout(widget->layout());
        break;

    case DialogDataType::Boolean:
        if (auto *checkBox = qobject_cast<QCheckBox *>(widget))
        {
            checkBox->setChecked(false);
        }
        break;

    case DialogDataType::Table:
        if (auto *tableWidget = qobject_cast<QTableWidget *>(widget))
        {
            tableWidget->setRowCount(0);
            tableWidget->clearContents();
        }
        break;

    default:
        break;
    }
}

/**
 * @brief 値セット
 *
 * @param id ID
 * @param values 入力値
 */
void DialogLayout::SetData(const int id, const std::vector<std::string> &values)
{
    if (Elements_.count(id) == 0 || Elements_[id].DataType == DialogDataType::NoData)
    {
        return;
    }
    const auto type = Elements_.at(id).DataType;

    if (type == DialogDataType::StringArray)
    {
        Clear(id);
        // 対象のウィジェットを QListWidget として取得する
        auto listWidget = qobject_cast<QListWidget *>(UI_.Inputs.at(id));
        if (!listWidget)
        {
            return;
        }

        // 渡された文字列の数だけリストアイテムを生成して追加
        for (const auto &value : values)
        {
            auto *item = new QListWidgetItem(QString::fromStdString(value));
            if (Elements_[id].IsEditable)
            {
                // ダブルクリックで直接テキストを編集できるようにフラグを設定
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }

            listWidget->addItem(item);
        }
    }
}

/**
 * @brief 値セット
 *
 * @param id ID
 * @param values 入力値
 * @param isEdit 編集可能か
 */
void DialogLayout::SetData(const int id, const std::vector<std::vector<std::string>> &values, bool isEdit)
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0 || Elements_.at(id).DataType == DialogDataType::NoData)
    {
        return;
    }
    const auto type = Elements_.at(id).DataType;

    if (Elements_[id].DataType == DialogDataType::StringPairArray)
    {
        Clear(id);
        for (const auto &lineData : values)
        {
            const std::vector<int> widths = {50, 200};
            auto widget = UI_.Inputs.at(id);
            if (!widget)
            {
                return;
            }
            if (!widget->layout())
            {
                widget->setLayout(new QVBoxLayout(widget));
                widget->layout()->setContentsMargins(0, 0, 0, 0);
            }

            QWidget *rowContainer = new QWidget(widget);
            QHBoxLayout *subLayout = new QHBoxLayout(rowContainer);
            subLayout->setContentsMargins(0, 0, 0, 0);
            subLayout->setAlignment(Qt::AlignTop); // 複数行入力時に左側のボックスが上に寄るように設定

            for (size_t i = 0; i < lineData.size(); i++)
            {
                {
                    auto line = new QLineEdit(rowContainer);
                    line->setText(QString::fromStdString(lineData[i]));
                    line->setFixedWidth(widths[i]);
                    line->setContextMenuPolicy(Qt::CustomContextMenu);
                    subLayout->addWidget(line);
                }
            }

            widget->layout()->addWidget(rowContainer);
        }
    }
    else if (type == DialogDataType::Table)
    {
        if (auto *tableWidget = qobject_cast<QTableWidget *>(UI_.Inputs.at(id)))
        {
            tableWidget->blockSignals(true);
            DisplayTable(tableWidget, values, isEdit);
            tableWidget->blockSignals(false);
        }
    }
}

/**
 * @brief 指定されたIDの要素から行の文字列リストを取得
 *
 * @param id 要素のID
 * @return const std::vector<std::string> 行の文字列リスト
 */
const std::vector<std::string> DialogLayout::GetLine(const int id)
{
    std::vector<std::string> resultList;

    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0 || Elements_.at(id).DataType == DialogDataType::NoData)
    {
        return resultList;
    }

    auto *widget = UI_.Inputs.at(id);
    const auto dataType = Elements_.at(id).DataType;

    if (dataType == DialogDataType::StringArray)
    {
        if (auto *listWidget = qobject_cast<QListWidget *>(widget))
        {
            for (int i = 0; i < listWidget->count(); ++i)
            {
                resultList.push_back(listWidget->item(i)->text().toStdString());
            }
        }
    }
    else if (dataType == DialogDataType::StringPairArray)
    {
        if (auto *mainLayout = widget->layout())
        {
            for (int i = 0; i < mainLayout->count(); ++i)
            {
                if (auto *rowWidget = mainLayout->itemAt(i)->widget())
                {
                    if (auto *rowLayout = rowWidget->layout())
                    {
                        std::string combinedString = "";
                        for (int j = 0; j < rowLayout->count(); ++j)
                        {
                            if (auto *lineEdit = qobject_cast<QLineEdit *>(rowLayout->itemAt(j)->widget()))
                            {
                                if (!combinedString.empty())
                                {
                                    combinedString += "\t";
                                }
                                combinedString += lineEdit->text().toStdString();
                            }
                        }
                        if (!combinedString.empty())
                        {
                            resultList.push_back(combinedString);
                        }
                    }
                }
            }
        }
    }
    else if (dataType == DialogDataType::String)
    {
        if (auto *lineEdit = qobject_cast<QLineEdit *>(widget))
        {
            resultList.push_back(lineEdit->text().toStdString());
        }
    }

    return resultList;
}

/**
 * @brief 行数を取得する
 *
 * @param id ID
 * @return const int 行数
 */
const int DialogLayout::GetLineCount(const int id) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0 || Elements_.at(id).DataType == DialogDataType::NoData)
    {
        return 0;
    }

    auto *widget = UI_.Inputs.at(id);
    const auto dataType = Elements_.at(id).DataType;

    if (dataType == DialogDataType::StringArray)
    {
        if (auto *listWidget = qobject_cast<QListWidget *>(widget))
        {
            return listWidget->count();
        }
    }
    else if (dataType == DialogDataType::StringPairArray)
    {
        if (auto *layout = widget->layout())
        {
            return layout->count();
        }
    }
    else if (dataType == DialogDataType::Table)
    {
        if (auto *tableWidget = qobject_cast<QTableWidget *>(widget))
        {
            return tableWidget->rowCount();
        }
    }
    else if (dataType == DialogDataType::String)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief 選択された行を1つ上へ移動する
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::MoveUp(const int id, const int lineIndex)
{
    if (Elements_.count(id) == 0 || Elements_[id].DataType == DialogDataType::NoData)
        return;

    if (Elements_[id].DataType == DialogDataType::StringArray)
    {
        auto widget = qobject_cast<QListWidget *>(UI_.Inputs.at(id));
        if (!widget)
            return;

        if (lineIndex > 0)
        {
            // アイテムをリストから一度取り外す（削除はされない）
            QListWidgetItem *currentItem = widget->takeItem(lineIndex);

            // 1つ上のインデックスに挿入し直す
            widget->insertItem(lineIndex - 1, currentItem);

            // 移動した後のアイテムを再び選択状態にする
            widget->setCurrentRow(lineIndex - 1);
        }
    }
}

/**
 * @brief 選択された行を1つ下へ移動する
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::MoveDown(const int id, const int lineIndex)
{
    if (Elements_.count(id) == 0 || Elements_[id].DataType == DialogDataType::NoData)
        return;

    if (Elements_[id].DataType == DialogDataType::StringArray)
    {
        auto widget = qobject_cast<QListWidget *>(UI_.Inputs.at(id));
        if (!widget)
            return;

        // 何も選択されていない(-1)場合や、すでに一番下にある場合は何もしない
        if (lineIndex >= 0 && lineIndex < widget->count() - 1)
        {
            // アイテムをリストから一度取り外す（削除はされない）
            QListWidgetItem *currentItem = widget->takeItem(lineIndex);

            // 1つ下のインデックスに挿入し直す
            widget->insertItem(lineIndex + 1, currentItem);

            // 移動した後のアイテムを再び選択状態にする
            widget->setCurrentRow(lineIndex + 1);
        }
    }
}

/**
 * @brief 行追加
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::AddLine(const int id, const int lineIndex)
{
    if (Elements_.count(id) == 0 || Elements_[id].DataType == DialogDataType::NoData)
        return;

    if (Elements_[id].DataType == DialogDataType::StringArray)
    {
        auto widget = qobject_cast<QListWidget *>(UI_.Inputs.at(id));
        if (!widget)
            return;

        if (lineIndex >= 0 && lineIndex <= widget->count())
        {
            auto *item = new QListWidgetItem("");
            if (Elements_[id].IsEditable)
            {
                item->setFlags(item->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
            }
            widget->insertItem(lineIndex, item);
        }
    }
    else if (Elements_[id].DataType == DialogDataType::StringPairArray)
    {
        auto widget = UI_.Inputs.at(id);
        if (!widget)
            return;

        auto layout = qobject_cast<QVBoxLayout *>(widget->layout());
        if (!layout)
            return;

        if (lineIndex >= 0 && lineIndex <= layout->count())
        {
            const std::vector<int> widths = {50, 200};

            QWidget *rowContainer = new QWidget(widget);
            QHBoxLayout *subLayout = new QHBoxLayout(rowContainer);
            subLayout->setContentsMargins(0, 0, 0, 0);
            subLayout->setAlignment(Qt::AlignTop);

            for (size_t i = 0; i < widths.size(); i++)
            {
                auto line = new QLineEdit(rowContainer);
                line->setText("");
                line->setFixedWidth(widths[i]);
                line->setContextMenuPolicy(Qt::CustomContextMenu);
                subLayout->addWidget(line);
            }

            layout->insertWidget(lineIndex, rowContainer);
        }
    }
}

/**
 * @brief 選択された行を削除する
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::DeleteLine(const int id, const int lineIndex)
{
    if (Elements_.count(id) == 0 || Elements_[id].DataType == DialogDataType::NoData)
        return;

    if (Elements_[id].DataType == DialogDataType::StringArray)
    {
        auto widget = qobject_cast<QListWidget *>(UI_.Inputs.at(id));
        if (!widget)
            return;

        // 指定された行が存在するかチェック
        if (lineIndex >= 0 && lineIndex < widget->count())
        {
            // アイテムをリストから取り外す
            QListWidgetItem *item = widget->takeItem(lineIndex);

            // 取り外したアイテムのメモリを解放して完全に削除する
            delete item;
        }
    }
}

/**
 * @brief テキストをセット
 *
 * @param id ID
 * @param text テキスト
 */
void DialogLayout::SetText(const int id, const std::string text)
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0 || Elements_.at(id).DataType == DialogDataType::NoData)
    {
        return;
    }
    const auto type = Elements_.at(id).DataType;
    if (type == DialogDataType::String)
    {
        qobject_cast<QLineEdit *>(UI_.Inputs.at(id))->setText(QString::fromStdString(text));
    }
}

/**
 * @brief ボタンの有効状態を変化
 *
 * @param id ID
 * @param isActivete ボタンを有効にするか
 */
void DialogLayout::ActivateButton(const int id, const int isActivete)
{
    if (id == HELP_BUTTON_ID && UI_.HelpButton)
    {
        UI_.HelpButton->setEnabled(isActivete);
    }
    else if (id == OK_BUTTON_ID && UI_.OkButton)
    {
        UI_.OkButton->setEnabled(isActivete);
    }
    else if (id == CANCEL_BUTTON_ID && UI_.CancelButton)
    {
        UI_.CancelButton->setEnabled(isActivete);
    }
    else if (UI_.Buttons.count(id))
    {
        UI_.Buttons.at(id)->setEnabled(isActivete);
    }
}