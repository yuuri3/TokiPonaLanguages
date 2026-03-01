#include "EditGeometryDialog.h"
#include "UnimplementedDialog.h"

EditGeometryDialog::EditGeometryDialog(QWidget *parent)
{
    setWindowTitle("地理編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    MainTable_ = new QTableWidget(this);
    layout->addWidget(MainTable_);

    MainTable_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(MainTable_, &QTableWidget::customContextMenuRequested,
            this, &EditGeometryDialog::ShowContextMenu);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditGeometryDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditGeometryDialog::SetPlace(const std::string &place)
{
    Place_ = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditGeometryDialog::SetPeriod(const int period)
{
    Period_ = period;
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
    if (Languages_ && Place_ && Period_)
    {
        DisplayTable(MainTable_, Languages_->GetGeography(), true);
    }
}

/**
 * @brief 表右クリック時イベント
 *
 */
void EditGeometryDialog::ShowContextMenu(const QPoint &pos)
{
    if (!Languages_)
    {
        return;
    }
    // クリックされた位置のアイテムを取得
    QModelIndex index = MainTable_->indexAt(pos);
    if (!index.isValid())
        return; // セルのない場所なら何もしない

    QMenu menu(this);
    QAction *addUpRow = menu.addAction("上に行追加");
    QAction *addDownRow = menu.addAction("下に行追加");
    QAction *deleteRow = menu.addAction("行削除");
    QAction *addRightColumn = menu.addAction("右に列追加");
    QAction *addLeftColumn = menu.addAction("左に列追加");
    QAction *deleteColumn = menu.addAction("列削除");

    // メニューを表示し、選ばれたアクションを取得
    QAction *selectedAction = menu.exec(MainTable_->viewport()->mapToGlobal(pos));

    const int row = index.row();
    const int column = index.column();

    if (selectedAction == addUpRow)
    {
        Languages_->AddGeomgraphicRowAbove(row);
        UpdateTable();
    }
    else if (selectedAction == addDownRow)
    {
        Unimplemented();
    }
    else if (selectedAction == deleteRow)
    {
        Unimplemented();
    }
    else if (selectedAction == addRightColumn)
    {
        Unimplemented();
    }
    else if (selectedAction == addLeftColumn)
    {
        Unimplemented();
    }
    else if (selectedAction == deleteColumn)
    {
        Unimplemented();
    }
}