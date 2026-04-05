#include "EditPeriodDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"

constexpr int ARRAY_ID = 1;

EditPeriodDialog::EditPeriodDialog(QWidget *parent)
    : QDialog(parent)
{
    Layout_ = DialogLayout::Create("時間軸編集", true, true, true);
    Layout_.SetDataType(ARRAY_ID, DialogDataType::StringArray);
    Layout_.SetHasContextMenu(ARRAY_ID, true);

    Layout_.GenerateLayout(this);

    Layout_.ConnectButtonClicked(HELP_BUTTON_ID, this, &EditPeriodDialog::ShowHelp);
    Layout_.ConnectButtonClicked(OK_BUTTON_ID, this, &EditPeriodDialog::accept);
    Layout_.ConnectButtonClicked(CANCEL_BUTTON_ID, this, &EditPeriodDialog::reject);

    Layout_.ConnectContextMenu(ARRAY_ID, this, &EditPeriodDialog::ShowContextMenu);
}

void EditPeriodDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    if (Languages_)
    {
        // TODO: 実際のメソッド名に合わせて変更してください
        // CurrentPeriodArray_ = Languages_->GetPeriods();
    }
    UpdateList();
}

void EditPeriodDialog::SetPlace(const std::string &place)
{
    Place_ = place;
    UpdateList();
}

void EditPeriodDialog::SetPeriod(const int period)
{
    Period_ = period;
    UpdateList();
}

void EditPeriodDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

void EditPeriodDialog::UpdateList()
{
    if (Languages_)
    {
        // TODO: DialogLayoutにStringArrayのデータを一括設定するメソッドがあれば変更してください
        // Layout_.Clear(ARRAY_ID);
        // for(const auto& period : CurrentPeriodArray_) { ... }
    }
}

void EditPeriodDialog::ShowContextMenu(const QPoint &pos)
{
    if (!Languages_)
    {
        return;
    }

    auto cellInformation = Layout_.GetCellInfo(ARRAY_ID, pos);
    if (!cellInformation.has_value())
    {
        return;
    }

    QMenu menu(this);
    QAction *addUpAction = menu.addAction("上に追加");
    QAction *addDownAction = menu.addAction("下に追加");
    QAction *deleteAction = menu.addAction("削除");

    const int row = cellInformation->row;
    const QPoint globalPos = cellInformation->globalPos;

    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == addUpAction)
    {
        // TODO: EditGeometryDialogのように差分を内部保持する仕様であれば変更してください
        Languages_->AddPeriodAbove(row);
        UpdateList();
    }
    else if (selectedAction == addDownAction)
    {
        // TODO: EditGeometryDialogのように差分を内部保持する仕様であれば変更してください
        Languages_->AddPeriodBelow(row);
        UpdateList();
    }
    else if (selectedAction == deleteAction)
    {
        auto ret = QMessageBox::warning(
            this,
            "",
            "時代を削除しますか？ 紐づいている言語も削除されます。",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (ret != QMessageBox::Yes)
        {
            return;
        }

        // TODO: EditGeometryDialogのように差分を内部保持する仕様であれば変更してください
        Languages_->RemovePeriod(row);
        UpdateList();
    }
}

void EditPeriodDialog::ShowHelp()
{
    QMessageBox::information(this, "ヘルプ - 時間軸編集",
                             "【時間軸編集ダイアログの機能】\n\n"
                             "■ 時代の操作\n"
                             "リストの任意の項目を右クリックするとメニューが表示され、以下の操作が可能です。\n"
                             "・時代の追加（上/下）と削除\n");
}

void EditPeriodDialog::accept()
{
    // TODO: 差分を一括適用する仕様であれば、ここで適用処理を実装します
    QDialog::accept();
}