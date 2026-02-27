#include "EditPeriodDialog.h"
#include "UnimplementedDialog.h"

EditPeriodDialog::EditPeriodDialog(QWidget *parent)
{
    setWindowTitle("時間軸編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // TODO ボタン名は検討
    AddUpButton_ = new QPushButton("上に追加", this);
    connect(AddUpButton_, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(AddUpButton_);

    AddDownButton_ = new QPushButton("下に追加", this);
    connect(AddDownButton_, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(AddDownButton_);

    RemoveButton_ = new QPushButton("削除", this);
    connect(RemoveButton_, &QPushButton::clicked, this, &EditPeriodDialog::Unimplemented);
    layout->addWidget(RemoveButton_);
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditPeriodDialog::SetPlace(const std::string &place)
{
    Place_ = place;
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditPeriodDialog::SetPeriod(const int period)
{
    Period_ = period;
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