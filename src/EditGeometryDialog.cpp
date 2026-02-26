#include "EditGeometryDialog.h"
#include "UnimplementedDialog.h"

EditGeometryDialog::EditGeometryDialog(QWidget *parent)
{
    setWindowTitle("地理編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    mainTable = new QTableWidget(this);
    layout->addWidget(mainTable);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditGeometryDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditGeometryDialog::SetPlace(const std::string &place)
{
    Place = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditGeometryDialog::SetPeriod(const int period)
{
    Period = period;
    UpdateTable();
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditGeometryDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief 表更新
 *
 */
void EditGeometryDialog::UpdateTable()
{
    if (Languages && Place && Period)
    {
        DisplayTable(mainTable, Languages->Geography);
    }
}