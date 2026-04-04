#include "EditGeometryDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"
#include <QDialogButtonBox>

EditGeometryDialog::EditGeometryDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("地理編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    // ヘルプボタンの追加
    QHBoxLayout *topLayout = new QHBoxLayout();
    QPushButton *helpButton = new QPushButton("ヘルプ", this);
    topLayout->addStretch();
    topLayout->addWidget(helpButton);
    layout->addLayout(topLayout);

    connect(helpButton, &QPushButton::clicked, this, &EditGeometryDialog::ShowHelp);

    MainTable_ = new QTableWidget(this);
    layout->addWidget(MainTable_);

    // OK / キャンセルボタンの追加
    QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &EditGeometryDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &EditGeometryDialog::reject);

    MainTable_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(MainTable_, &QTableWidget::customContextMenuRequested,
            this, &EditGeometryDialog::ShowContextMenu);
    connect(MainTable_, &QTableWidget::itemChanged,
            this, &EditGeometryDialog::OnItemChanged);
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
    if (Languages_)
    {
        MainTable_->blockSignals(true);
        DisplayTable(MainTable_, Languages_->GetGeography(), true);
        MainTable_->blockSignals(false);
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
        GeometryDifferences_.push_back(GeometryDifference::CreateRowOperation(GeometryOperationType::AddRowAbove, row));
        UpdateTable();
    }
    else if (selectedAction == addDownRow)
    {
        GeometryDifferences_.push_back(GeometryDifference::CreateRowOperation(GeometryOperationType::AddRowBelow, row));
        UpdateTable();
    }
    else if (selectedAction == deleteRow)
    {
        bool hasData = false;
        for (int c = 0; c < MainTable_->columnCount(); ++c)
        {
            QTableWidgetItem *cellItem = MainTable_->item(row, c);
            if (cellItem && !cellItem->text().isEmpty())
            {
                hasData = true;
                break;
            }
        }

        if (hasData)
        {
            auto ret = QMessageBox::warning(
                this,
                "",
                "行を削除しますか？ 紐づいている言語も削除されます。",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (ret != QMessageBox::Yes)
            {
                return;
            }
        }

        GeometryDifferences_.push_back(GeometryDifference::CreateRowOperation(GeometryOperationType::DeleteRow, row));
        UpdateTable();
    }
    else if (selectedAction == addRightColumn)
    {
        GeometryDifferences_.push_back(GeometryDifference::CreateColumnOperation(GeometryOperationType::AddColumnRight, column));
        UpdateTable();
    }
    else if (selectedAction == addLeftColumn)
    {
        GeometryDifferences_.push_back(GeometryDifference::CreateColumnOperation(GeometryOperationType::AddColumnLeft, column));
        UpdateTable();
    }
    else if (selectedAction == deleteColumn)
    {
        bool hasData = false;
        for (int r = 0; r < MainTable_->rowCount(); ++r)
        {
            QTableWidgetItem *cellItem = MainTable_->item(r, column);
            if (cellItem && !cellItem->text().isEmpty())
            {
                hasData = true;
                break;
            }
        }

        if (hasData)
        {
            auto ret = QMessageBox::warning(
                this,
                "",
                "列を削除しますか？ 紐づいている言語も削除されます。",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

            if (ret != QMessageBox::Yes)
            {
                return;
            }
        }

        GeometryDifferences_.push_back(GeometryDifference::CreateColumnOperation(GeometryOperationType::DeleteColumn, column));
        UpdateTable();
    }
}

/**
 * @brief セル変更時イベント
 *
 * @param item
 */
void EditGeometryDialog::OnItemChanged(QTableWidgetItem *item)
{
    const int row = item->row();
    const int column = item->column();
    const std::string name = item->text().toStdString();

    if (name.empty())
    {
        auto ret = QMessageBox::warning(
            this,
            "",
            "地域を削除しますか？ 紐づいている言語も削除されます。",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (ret != QMessageBox::Yes)
        {
            UpdateTable(); // Yes以外の場合はモデルからデータを再取得して変更を破棄する
            return;
        }
    }

    GeometryDifferences_.push_back(GeometryDifference::CreateChangePlaceNameOperation(row, column, name));
}

/**
 * @brief ヘルプ表示
 *
 */
void EditGeometryDialog::ShowHelp()
{
    QMessageBox::information(this, "ヘルプ - 地理編集",
                             "【地理編集ダイアログの機能】\n\n"
                             "■ 場所名の編集\n"
                             "表の各セルをダブルクリックしてテキストを編集することで、場所の名前を変更できます。\n\n"
                             "■ 行・列の操作\n"
                             "表の任意のセルを右クリックするとメニューが表示され、以下の操作が可能です。\n"
                             "・行の追加（上/下）と削除\n"
                             "・列の追加（左/右）と削除\n");
}

/**
 * @brief OKボタン押下時の処理
 *
 */
void EditGeometryDialog::accept()
{
    if (Languages_)
    {
        Languages_->EditGeometry(GeometryDifferences_);
    }
    QDialog::accept();
}