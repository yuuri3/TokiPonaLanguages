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
}

/**
 * @brief 指定したIDの要素の編集可否を設定する
 * @param id 要素のID
 * @param isEditable 編集可能にする場合は true
 */
void DialogLayout::SetIsEditable(int id, bool isEditable)
{
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
 * @param hasContextDelete メニューに「削除」を含める場合は true
 */
void DialogLayout::SetHasContextMenu(int id, bool hasContextMenu)
{
    Elements_[id].HasContextMenu = hasContextMenu;
}

/**
 * @brief 設定されたデータに基づいてレイアウトを生成する
 * @param parent 親ウィジェット
 */
void DialogLayout::GenerateLayout(QWidget *parent)
{
    parent->setWindowTitle(QString::fromStdString(MainTitle_));
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

        // --- それ以外のデータ型（文字列・配列）---

        // タイトルと付随ボタンのレイアウト
        auto *titleLayout = new QHBoxLayout();
        titleLayout->addWidget(new QLabel(QString::fromStdString(element.Title), scrollContent));

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
        if (element.DataType == DialogDataType::String)
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

            table->setContextMenuPolicy(Qt::CustomContextMenu);
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
 * @brief ウィジェットをクリア
 *
 * @param id ID
 */
void DialogLayout::Clear(const int id)
{
    auto widget = UI_.Inputs.at(id);
    if (!widget)
    {
        return;
    }
    ClearWidget(widget);
}

/**
 * @brief 行追加
 *
 * @param widget 親ウィジェット
 * @param values 入力値（[0]: タイトル, [1]: 内容）
 * @param widths 幅のリスト
 */
void DialogLayout::AddLine(const int id, const std::vector<std::string> &values, const std::vector<int> &widths)
{
    if (Elements_[id].DataType == DialogDataType::StringArray)
    {
        if (Elements_[id].DataType == DialogDataType::StringArray)
        {
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
    else if (Elements_[id].DataType == DialogDataType::StringPairArray)
    {
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

        for (size_t i = 0; i < values.size(); i++)
        {
            {
                auto line = new QLineEdit(rowContainer);
                line->setText(QString::fromStdString(values[i]));
                line->setFixedWidth(widths[i]);
                line->setContextMenuPolicy(Qt::CustomContextMenu);
                subLayout->addWidget(line);
            }
        }

        widget->layout()->addWidget(rowContainer);
    }
}

/**
 * @brief 選択された行を1つ上へ移動する
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::MoveUp(const int id, const int lineIndex)
{
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
 * @brief 選択された行を削除する
 *
 * @param id 要素ID
 * @param lineIndex 行
 */
void DialogLayout::DeleteLine(const int id, const int lineIndex)
{
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
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
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
 * @brief 表にデータをセット
 *
 * @param id
 * @param data
 */
void DialogLayout::SetDataToTable(const int id, const std::vector<std::vector<std::string>> &data)
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
    {
        return;
    }
    const auto type = Elements_.at(id).DataType;
    if (type == DialogDataType::Table)
    {
        DisplayTable(qobject_cast<QTableWidget *>(UI_.Inputs.at(id)), data);
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