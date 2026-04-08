#include "EditGeometryDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"

constexpr int TABLE_ID = 1;

EditGeometryDialog::EditGeometryDialog(QWidget *parent)
    : QDialog(parent)
{
    Layout_ = DialogLayout::Create("地理編集", true, true, true);
    Layout_.SetDataType(TABLE_ID, DialogDataType::Table);
    Layout_.SetHasContextMenu(TABLE_ID, true);

    Layout_.GenerateLayout(this);

    Layout_.ConnectButtonClicked(HELP_BUTTON_ID, this, &EditGeometryDialog::ShowHelp);
    Layout_.ConnectButtonClicked(OK_BUTTON_ID, this, &EditGeometryDialog::accept);
    Layout_.ConnectButtonClicked(CANCEL_BUTTON_ID, this, &EditGeometryDialog::reject);

    Layout_.ConnectContextMenu(TABLE_ID, this, &EditGeometryDialog::ShowContextMenu);
    Layout_.ConnectItemChanged(TABLE_ID, this, &EditGeometryDialog::OnItemChanged);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditGeometryDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    if (Languages_)
    {
        CurrentGeometryTable_ = Languages_->GetGeometryTable().GetData();
    }
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
        Layout_.SetData(TABLE_ID, CurrentGeometryTable_, true);
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

    auto cellInformation = Layout_.GetCellInfo(TABLE_ID, pos);
    if (!cellInformation.has_value())
    {
        return; // セルのない場所なら何もしない
    }

    QMenu menu(this);
    QAction *addUpRow = menu.addAction("上に行追加");
    QAction *addDownRow = menu.addAction("下に行追加");
    QAction *deleteRow = menu.addAction("行削除");
    QAction *addRightColumn = menu.addAction("右に列追加");
    QAction *addLeftColumn = menu.addAction("左に列追加");
    QAction *deleteColumn = menu.addAction("列削除");

    const int row = cellInformation->row;
    const int column = cellInformation->column;
    const QPoint globalPos = cellInformation->globalPos;

    // メニューを表示し、選ばれたアクションを取得
    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == addUpRow)
    {
        GeometryDifference difference = GeometryDifference::CreateRowOperation(GeometryOperationType::AddRowAbove, row);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateTable();
    }
    else if (selectedAction == addDownRow)
    {
        GeometryDifference difference = GeometryDifference::CreateRowOperation(GeometryOperationType::AddRowBelow, row);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateTable();
    }
    else if (selectedAction == deleteRow)
    {
        if (Layout_.HasDataInRow(TABLE_ID, row))
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

        GeometryDifference difference = GeometryDifference::CreateRowOperation(GeometryOperationType::DeleteRow, row);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateTable();
    }
    else if (selectedAction == addRightColumn)
    {
        GeometryDifference difference = GeometryDifference::CreateColumnOperation(GeometryOperationType::AddColumnRight, column);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateTable();
    }
    else if (selectedAction == addLeftColumn)
    {
        GeometryDifference difference = GeometryDifference::CreateColumnOperation(GeometryOperationType::AddColumnLeft, column);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateTable();
    }
    else if (selectedAction == deleteColumn)
    {
        if (Layout_.HasDataInColumn(TABLE_ID, column))
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

        GeometryDifference difference = GeometryDifference::CreateColumnOperation(GeometryOperationType::DeleteColumn, column);
        GeometryDifferences_.push_back(difference);
        ApplyDifference(difference);
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
            UpdateTable(); // Yes以外の場合は変更前のデータで再描画する
            return;
        }
    }

    GeometryDifference difference = GeometryDifference::CreateChangePlaceNameOperation(row, column, name);
    GeometryDifferences_.push_back(difference);
    ApplyDifference(difference);
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

/**
 * @brief 差分を内部保持しているテーブルデータに適用する
 *
 * @param difference 適用する操作
 */
void EditGeometryDialog::ApplyDifference(const GeometryDifference &difference)
{
    switch (difference.GetOperationType())
    {
    case GeometryOperationType::ChangePlaceName:
        if (difference.GetTargetRow() < static_cast<int>(CurrentGeometryTable_.Body.size()) &&
            difference.GetTargetColumn() < static_cast<int>(CurrentGeometryTable_.Body[difference.GetTargetRow()].size()))
        {
            CurrentGeometryTable_.Body[difference.GetTargetRow()][difference.GetTargetColumn()] = difference.GetPlaceName();
        }
        break;

    case GeometryOperationType::AddRowAbove:
    {
        int columnCount = CurrentGeometryTable_.Body.empty() ? 0 : static_cast<int>(CurrentGeometryTable_.Body[0].size());
        CurrentGeometryTable_.Body.insert(CurrentGeometryTable_.Body.begin() + difference.GetTargetRow(), std::vector<std::string>(columnCount, ""));
        break;
    }

    case GeometryOperationType::AddRowBelow:
    {
        int columnCount = CurrentGeometryTable_.Body.empty() ? 0 : static_cast<int>(CurrentGeometryTable_.Body[0].size());
        CurrentGeometryTable_.Body.insert(CurrentGeometryTable_.Body.begin() + difference.GetTargetRow() + 1, std::vector<std::string>(columnCount, ""));
        break;
    }

    case GeometryOperationType::DeleteRow:
        if (difference.GetTargetRow() < static_cast<int>(CurrentGeometryTable_.Body.size()))
        {
            CurrentGeometryTable_.Body.erase(CurrentGeometryTable_.Body.begin() + difference.GetTargetRow());
        }
        break;

    case GeometryOperationType::AddColumnLeft:
        for (auto &rowVector : CurrentGeometryTable_.Body)
        {
            rowVector.insert(rowVector.begin() + difference.GetTargetColumn(), "");
        }
        break;

    case GeometryOperationType::AddColumnRight:
        for (auto &rowVector : CurrentGeometryTable_.Body)
        {
            rowVector.insert(rowVector.begin() + difference.GetTargetColumn() + 1, "");
        }
        break;

    case GeometryOperationType::DeleteColumn:
        for (auto &rowVector : CurrentGeometryTable_.Body)
        {
            if (difference.GetTargetColumn() < static_cast<int>(rowVector.size()))
            {
                rowVector.erase(rowVector.begin() + difference.GetTargetColumn());
            }
        }
        break;
    }
}