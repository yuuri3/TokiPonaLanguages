#include "HelpDialog.h"

void HelpDialogContent::AddHeader(const QString &header)
{
    Headers.emplace_back(header);
}
void HelpDialogContent::AddContent(const QString &title, const QString &content)
{
    if (Contents.size() < Headers.size())
    {
        std::vector<std::pair<QString, QString>> cont = {{title, content}};
        Contents.emplace_back(cont);
    }
    else
    {
        Contents.back().emplace_back(title, content);
    }
}

/**
 * @brief ヘルプダイアログのコンストラクタ
 *
 * @param parent 親ウィジェット
 */
HelpDialog::HelpDialog(QWidget *parent)
{
    setWindowTitle("ヘルプ");
    setMinimumSize(500, 500);
}

/**
 * @brief レイアウト構築
 *
 * @param contents 内容
 */
void HelpDialog::SetContents(const HelpDialogContent &contents)
{
    // メインレイアウト
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // スクロールエリアの設定
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(scrollArea);

    // スクロールエリアの中身となるメインウィジェット
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // ラベル作成用のヘルパー処理（背景色付き見出し）
    auto addHeader = [&](const QString &text)
    {
        QLabel *label = new QLabel(text, scrollContent);
        label->setStyleSheet("background-color: #e0e0e0; font-weight: bold; padding: 5px;");
        layout->addWidget(label);
    };

    // 小項目と説明を追加する処理
    auto addContent = [&](const QString &title, const QString &desc)
    {
        layout->addWidget(new QLabel(QString("<b>【%1】</b>").arg(title), scrollContent));
        QLabel *descLabel = new QLabel(QString("　%1").arg(desc), scrollContent);
        descLabel->setWordWrap(true);
        layout->addWidget(descLabel);
        layout->addSpacing(5);
    };

    // --- コンテンツ構成 ---

    for (int i = 0; i < contents.Headers.size(); i++)
    {
        if (i != 0)
        {
            layout->addSpacing(10);
        }
        addHeader(contents.Headers[i]);
        for (const auto &[title, cont] : contents.Contents[i])
        {
            addContent(title, cont);
        }
    }

    // 下部に余白を追加して上寄せにする
    layout->addStretch();

    // --- 固定エリア（閉じるボタン） ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *closeButton = new QPushButton("閉じる", this);
    closeButton->setFixedWidth(BUTTON_WIDTH);
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);

    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
}