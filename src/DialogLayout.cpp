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
    layout.MainTitle = title;
    layout.HasHelpButton = hasHelpButton;
    layout.HasOkButton = hasOkButton;
    layout.HasCancelButton = hasCancelButton;
    return layout;
}

/**
 * @brief 指定したIDの要素にタイトルを設定する
 * @param id 要素のID
 * @param title 設定するタイトル文字列
 */
void DialogLayout::SetTitle(int id, const std::string &title)
{
    Elements[id].Title = title;
}

/**
 * @brief 指定したIDの要素にデータ型を設定する
 * @param id 要素のID
 * @param dataType 設定するデータ型
 */
void DialogLayout::SetDataType(int id, DialogDataType dataType)
{
    Elements[id].DataType = dataType;
}

/**
 * @brief 指定したIDの要素の編集可否を設定する
 * @param id 要素のID
 * @param isEditable 編集可能にする場合は true
 */
void DialogLayout::SetIsEditable(int id, bool isEditable)
{
    Elements[id].IsEditable = isEditable;
}

/**
 * @brief 指定したIDの要素に編集ボタンの有無を設定する
 * @param id 要素のID
 * @param hasEditButton 編集ボタンを持たせる場合は true
 */
void DialogLayout::SetHasEditButton(int id, bool hasEditButton)
{
    Elements[id].HasEditButton = hasEditButton;
}

/**
 * @brief 指定したIDの要素に選択ボタンの有無を設定する
 *
 * @param id 要素のID
 * @param hasSelectButton 選択ボタンを持たせる場合は true
 */
void DialogLayout::SetHasSelectButton(int id, bool hasSelectButton)
{
    Elements[id].HasSelectButton = hasSelectButton;
}

/**
 * @brief 指定したIDの要素に追加ボタンの有無を設定する
 * @param id 要素のID
 * @param hasAddButton 追加ボタンを持たせる場合は true
 */
void DialogLayout::SetHasAddButton(int id, bool hasAddButton)
{
    Elements[id].HasAddButton = hasAddButton;
}

/**
 * @brief 指定したIDの要素の右クリックメニュー設定
 * @param id 要素のID
 * @param hasContextDelete メニューに「削除」を含める場合は true
 */
void DialogLayout::SetHasContextMenu(int id, bool hasContextMenu)
{
    Elements[id].HasContextMenu = hasContextMenu;
}

/**
 * @brief 設定されたデータに基づいてレイアウトを生成する
 * @param parent 親ウィジェット
 */
void DialogLayout::GenerateLayout(QWidget *parent)
{
    parent->setWindowTitle(QString::fromStdString(MainTitle));
    UI.MainLayout = new QVBoxLayout(parent);

    // ==========================================
    // 1. ヘルプボタン (最上部・右寄せ)
    // ==========================================
    if (HasHelpButton)
    {
        auto *helpLayout = new QHBoxLayout();
        UI.HelpButton = new QPushButton("ヘルプ", parent);
        UI.HelpButton->setObjectName("HelpButton");
        helpLayout->addStretch();
        helpLayout->addWidget(UI.HelpButton);
        UI.MainLayout->addLayout(helpLayout);
    }

    // ==========================================
    // スクロールエリアの設定
    // ==========================================
    auto *scrollArea = new QScrollArea(parent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    UI.MainLayout->addWidget(scrollArea);

    auto *scrollContent = new QWidget(scrollArea);
    auto *contentLayout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // ==========================================
    // 2. 各要素の生成 (map なので ID の昇順で処理される)
    // ==========================================
    for (const auto &[id, element] : Elements)
    {
        // 真偽値（チェックボックス）の場合はタイトルと入力を兼ねるため特別扱い
        if (element.DataType == DialogDataType::Boolean)
        {
            auto *checkBox = new QCheckBox(QString::fromStdString(element.Title), scrollContent);
            checkBox->setEnabled(element.IsEditable);
            checkBox->setObjectName(QString("CheckBox_%1").arg(id));
            contentLayout->addWidget(checkBox);
            UI.Inputs[id] = checkBox; // 構造体に保存
            continue;                 // 次の要素へ
        }

        // --- それ以外のデータ型（文字列・配列）---

        // タイトルと付随ボタンのレイアウト
        auto *titleLayout = new QHBoxLayout();
        titleLayout->addWidget(new QLabel(QString::fromStdString(element.Title), scrollContent));

        if (element.HasEditButton)
        {
            UI.SelectButtons[id] = new QPushButton("編集", scrollContent);
            UI.SelectButtons[id]->setObjectName(QString("SelectButton_%1").arg(id));
            UI.SelectButtons[id]->setFixedWidth(50);
            titleLayout->addWidget(UI.SelectButtons[id]);
        }
        if (element.HasSelectButton)
        {
            UI.EditButtons[id] = new QPushButton("選択", scrollContent);
            UI.EditButtons[id]->setObjectName(QString("EditButton_%1").arg(id));
            UI.EditButtons[id]->setFixedWidth(50);
            titleLayout->addWidget(UI.EditButtons[id]);
        }
        if (element.HasAddButton)
        {
            UI.AddButtons[id] = new QPushButton("追加", scrollContent);
            UI.AddButtons[id]->setObjectName(QString("AddButton_%1").arg(id));
            UI.AddButtons[id]->setFixedWidth(50);
            titleLayout->addWidget(UI.AddButtons[id]);
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
            UI.Inputs[id] = lineEdit;
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
            UI.Inputs[id] = listWidget;
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
            UI.Inputs[id] = containerWidget; // Translations_ などの QWidget* に代入される
        }
    }

    contentLayout->addStretch(); // コンテンツを上に寄せる

    // ==========================================
    // 3. 下部ボタン (OK / キャンセル)
    // ==========================================
    if (HasOkButton || HasCancelButton)
    {
        auto *buttonLayout = new QHBoxLayout();
        buttonLayout->addStretch();

        if (HasOkButton)
        {
            UI.OkButton = new QPushButton("OK", parent);
            UI.OkButton->setObjectName("OkButton");
            buttonLayout->addWidget(UI.OkButton);
        }
        if (HasCancelButton)
        {
            UI.CancelButton = new QPushButton("キャンセル", parent);
            UI.CancelButton->setObjectName("CancelButton");
            buttonLayout->addWidget(UI.CancelButton);
        }
        UI.MainLayout->addLayout(buttonLayout);
    }
}

/**
 * @brief ウィジェットをクリア
 *
 * @param id ID
 */
void DialogLayout::Clear(const int id)
{
    auto widget = UI.Inputs.at(id);
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
std::vector<QLineEdit *> DialogLayout::AddLine(const int id, const std::vector<std::string> &values, const std::vector<int> &widths)
{
    std::vector<QLineEdit *> result;
    auto widget = UI.Inputs.at(id);
    if (!widget)
    {
        return {};
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
        // 自由記述(Contents_)セクションの2番目のボックスのみ QTextEdit を使用
        // if (widget == LayoutData_.UI.Inputs[3] && i == 1)
        // {
        //     auto textEdit = new QTextEdit(rowContainer);
        //     textEdit->setPlainText(QString::fromStdString(values[i]));
        //     textEdit->setFixedWidth(widths[i]);
        //     textEdit->setFixedHeight(80); // 複数行用に高さを確保
        //     textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
        //     connect(textEdit, &QTextEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
        //     subLayout->addWidget(textEdit);
        // }
        // else
        {
            auto line = new QLineEdit(rowContainer);
            line->setText(QString::fromStdString(values[i]));
            line->setFixedWidth(widths[i]);
            line->setContextMenuPolicy(Qt::CustomContextMenu);
            // connect(line, &QLineEdit::customContextMenuRequested, this, &EditWordDialog::ClickLine);
            subLayout->addWidget(line);
            result.emplace_back(line);
        }
    }

    widget->layout()->addWidget(rowContainer);
    return result;
}

/**
 * @brief UI情報を取得
 *
 * @return const GeneratedDialogUI&
 */
const GeneratedDialogUI &DialogLayout::GetUI() const
{
    return UI;
}