#include "EditPeriodDialog.h"
#include "UnimplementedDialog.h"

EditPeriodDialog::EditPeriodDialog(QWidget *parent)
{
    setWindowTitle("時間軸編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // TODO ボタン名は検討
    AddUpButton_ = new QPushButton("上に追加", this);
    connect(AddUpButton_, &QPushButton::clicked, this, &EditPeriodDialog::AddAbove);
    layout->addWidget(AddUpButton_);

    AddDownButton_ = new QPushButton("下に追加", this);
    connect(AddDownButton_, &QPushButton::clicked, this, &EditPeriodDialog::AddBelow);
    layout->addWidget(AddDownButton_);

    RemoveButton_ = new QPushButton("削除", this);
    connect(RemoveButton_, &QPushButton::clicked, this, &EditPeriodDialog::Remove);
    layout->addWidget(RemoveButton_);
}

/**
 * @brief 語族をセット
 *
 * @param languages
 */
void EditPeriodDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
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

/**
 * @brief 上に時代を追加
 *
 */
void EditPeriodDialog::AddAbove()
{
    if (Languages_ && Period_)
    {
        Languages_->AddPeriodAbove(*Period_);
    }
    close();
}

/**
 * @brief 下に時代を追加
 *
 */
void EditPeriodDialog::AddBelow()
{
    if (Languages_ && Period_)
    {
        Languages_->AddPeriodBelow(*Period_);
    }
    close();
}

/**
 * @brief 削除
 *
 */
void EditPeriodDialog::Remove()
{
    if (Languages_ && Period_)
    {
        Languages_->RemovePeriod(*Period_);
    }
    close();
}