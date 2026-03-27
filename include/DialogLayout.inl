// =================================================================
// DialogLayout.inl
// テンプレート関数の実装ファイル（DialogLayout.h の末尾でインクルードされます）
// =================================================================

template <typename Receiver, typename Slot>
void DialogLayout::ConnectButtonClicked(int id, Receiver *receiver, Slot slot) const
{
    if (id == HELP_BUTTON_ID && UI_.HelpButton)
    {
        QObject::connect(UI_.HelpButton, &QPushButton::clicked, receiver, slot);
    }
    else if (id == OK_BUTTON_ID && UI_.OkButton)
    {
        QObject::connect(UI_.OkButton, &QPushButton::clicked, receiver, slot);
    }
    else if (id == CANCEL_BUTTON_ID && UI_.CancelButton)
    {
        QObject::connect(UI_.CancelButton, &QPushButton::clicked, receiver, slot);
    }
    if (UI_.Buttons.count(id))
    {
        QObject::connect(UI_.Buttons.at(id), &QPushButton::clicked, receiver, slot);
    }
}
template <typename Receiver, typename Slot>
void DialogLayout::ConnectClicked(const int id, Receiver *receiver, Slot slot) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
        return;

    const auto type = Elements_.at(id).DataType;
    auto *widget = UI_.Inputs.at(id);

    switch (type)
    {
    case DialogDataType::Boolean:
        // スロットが「boolを受け取れる」または「引数なし」の場合のみコンパイルする
        if constexpr (std::is_invocable_v<Slot, Receiver *, bool> || std::is_invocable_v<Slot, Receiver *>)
        {
            if (auto *checkBox = qobject_cast<QCheckBox *>(widget))
            {
                QObject::connect(checkBox, &QCheckBox::clicked, receiver, slot);
            }
        }
        break;

    case DialogDataType::StringArray:
        // スロットが「QListWidgetItem*を受け取れる」または「引数なし」の場合のみコンパイルする
        if constexpr (std::is_invocable_v<Slot, Receiver *, QListWidgetItem *> || std::is_invocable_v<Slot, Receiver *>)
        {
            if (auto *listWidget = qobject_cast<QListWidget *>(widget))
            {
                QObject::connect(listWidget, &QListWidget::itemClicked, receiver, slot);
            }
        }
        break;

    case DialogDataType::Table:
        // スロットが「int, intを受け取れる」または「引数なし」の場合のみコンパイルする
        if constexpr (std::is_invocable_v<Slot, Receiver *, int, int> || std::is_invocable_v<Slot, Receiver *>)
        {
            if (auto *tableWidget = qobject_cast<QTableWidget *>(widget))
            {
                QObject::connect(tableWidget, &QTableWidget::cellClicked, receiver, slot);
            }
        }
        break;

    default:
        break;
    }
}
template <typename Receiver, typename Slot>
void DialogLayout::ConnectRightClicked(const int id, Receiver *receiver, Slot slot) const
{
    // 要素IDが登録されていない、またはウィジェットが生成されていない場合は何もしない
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
    {
        return;
    }

    // 全ての入力部品は QWidget を継承しているため、そのまま共通のシグナルに接続可能
    auto *widget = UI_.Inputs.at(id);
    QObject::connect(widget, &QWidget::customContextMenuRequested, receiver, slot);
}
template <typename Receiver, typename Slot>
void DialogLayout::AddLineAndConnectRightClicked(const int id, const std::vector<std::string> &values, const std::vector<int> &widths, Receiver *receiver, Slot slot)
{
    if (Elements_[id].DataType == DialogDataType::StringPairArray)
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
                QObject::connect(qobject_cast<QLineEdit *>(line), &QLineEdit::customContextMenuRequested, receiver, slot);
            }
        }

        widget->layout()->addWidget(rowContainer);
    }
}

/**
 * @brief コンテキストメニュー要求シグナルをスロットに接続
 *
 * @param id 要素ID
 * @param receiver レシーバーオブジェクトポインタ
 * @param slot 実行されるスロット関数
 */
template <typename Receiver, typename Slot>
void DialogLayout::ConnectContextMenu(const int id, Receiver *receiver, Slot slot) const
{
    if (Elements_.count(id) == 0 || UI_.Inputs.count(id) == 0)
    {
        return;
    }

    auto *widget = UI_.Inputs.at(id);

    // シグナルを発火させるためにコンテキストメニューのポリシーを設定
    widget->setContextMenuPolicy(Qt::CustomContextMenu);

    QObject::connect(widget, &QWidget::customContextMenuRequested, receiver, slot);
}