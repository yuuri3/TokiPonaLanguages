#include "UnimplementedDialog.h"

UnimplementedDialog::UnimplementedDialog(QWidget *parent)
{
    const auto appName = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName();
    setWindowTitle(appName);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *warningText = new QLabel("機能未実装です", this);
    layout->addWidget(warningText);

    OKButton_ = new QPushButton("OK", this);
    layout->addWidget(OKButton_);
    connect(OKButton_, &QPushButton::clicked, this, &UnimplementedDialog::OkButtonClicked);

    // レイアウト調整
    constexpr int BUTTON_HEIGHT = 30;
    constexpr int BUTTON_WIDTH = 90;
    constexpr int MARGIN = 20;
    constexpr int BUTTON_SPACE = 5;

    layout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    layout->setSpacing(BUTTON_SPACE);

    OKButton_->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    this->setFixedSize(sizeHint());
}

/**
 * @brief OKボタンクリック時イベント
 *
 */
void UnimplementedDialog::OkButtonClicked()
{
    this->close();
}