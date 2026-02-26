#include "EditPeriodDialog.h"
#include "UnimplementedDialog.h"

EditPeriodDialog::EditPeriodDialog(QWidget *parent)
{
    setWindowTitle("時間軸編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // TODO ボタン名は検討
    addUpButton = new QPushButton("上に追加", this);
    connect(addUpButton, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(addUpButton);

    addDownButton = new QPushButton("下に追加", this);
    connect(addDownButton, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(addDownButton);

    removeButton = new QPushButton("削除", this);
    connect(removeButton, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(removeButton);
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditPeriodDialog::SetPlace(const std::string &place)
{
    Place = place;
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditPeriodDialog::SetPeriod(const int period)
{
    Period = period;
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditPeriodDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}