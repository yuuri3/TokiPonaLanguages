#include "HelpDialog.h"

/**
 * @brief ヘルプダイアログのコンストラクタ
 * * @param parent 親ウィジェット
 */
HelpDialog::HelpDialog(QWidget *parent)
{
    setWindowTitle("ヘルプ");

    // メインレイアウト
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // スクロールエリアの設定
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);       // 中身のサイズに合わせて伸縮させる
    scrollArea->setFrameShape(QFrame::NoFrame); // 枠線を消してダイアログになじませる
    mainLayout->addWidget(scrollArea);

    // スクロールエリアの中身となるメインウィジェット
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // 説明テキスト表示エリア
    QTextEdit *helpText = new QTextEdit(scrollContent);
    helpText->setReadOnly(scrollContent); // 編集不可に設定
    helpText->setPlainText(
        "【文面】");
    layout->addWidget(helpText);

    // ボタンの配置用レイアウト（右寄せにするため）
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *closeButton = new QPushButton("閉じる", scrollContent);
    closeButton->setFixedWidth(BUTTON_WIDTH);

    // ダイアログを閉じるスロットに接続
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);

    buttonLayout->addWidget(closeButton);
    layout->addLayout(buttonLayout);
}