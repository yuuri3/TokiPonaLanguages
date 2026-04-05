#include "EditPeriodDialog.h"
#include "UnimplementedDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"

constexpr int ARRAY_ID = 1;

EditPeriodDialog::EditPeriodDialog(QWidget *parent)
    : QDialog(parent)
{
    Layout_ = DialogLayout::Create("時間軸編集", true, true, true);
    Layout_.SetDataType(ARRAY_ID, DialogDataType::Table);
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
        CurrentLanguages_ = Languages_->ToString();
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
        Layout_.Clear(ARRAY_ID);
        Layout_.SetDataToTable(ARRAY_ID, CurrentLanguages_, false);
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

    const int row = cellInformation->row - 1;
    const QPoint globalPos = cellInformation->globalPos;

    QAction *selectedAction = menu.exec(globalPos);

    if (selectedAction == addUpAction)
    {
        PeriodDifference difference = PeriodDifference::CreateAddPeriodAboveOperation(row);
        PeriodDifferences_.push_back(difference);
        ApplyDifference(difference);
        UpdateList();
    }
    else if (selectedAction == addDownAction)
    {
        PeriodDifference difference = PeriodDifference::CreateAddPeriodBelowOperation(row);
        PeriodDifferences_.push_back(difference);
        ApplyDifference(difference);
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

        PeriodDifference difference = PeriodDifference::CreateRemovePeriodOperation(row);
        PeriodDifferences_.push_back(difference);
        ApplyDifference(difference);
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
    if (Languages_)
    {
        Languages_->EditPeriod(PeriodDifferences_);
    }
    QDialog::accept();
}

void EditPeriodDialog::ApplyDifference(const PeriodDifference &difference)
{
    const auto targetIndex = difference.GetTargetPeriod() + 1;
    switch (difference.GetOperationType())
    {
    case PeriodOperationType::AddPeriodAbove:
        CurrentLanguages_.insert(CurrentLanguages_.begin() + targetIndex, CurrentLanguages_[targetIndex]);
        break;

    case PeriodOperationType::AddPeriodBelow:
        CurrentLanguages_.insert(CurrentLanguages_.begin() + targetIndex + 1, CurrentLanguages_[targetIndex]);
        break;

    case PeriodOperationType::RemovePeriod:
        if (targetIndex < static_cast<int>(CurrentLanguages_.size()))
        {
            CurrentLanguages_.erase(CurrentLanguages_.begin() + targetIndex);
        }
        break;
    }
}