#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"

class EditGeometryDialog : public QDialog
{
    Q_OBJECT

public:
    EditGeometryDialog(QWidget *parent = nullptr);
    ~EditGeometryDialog() = default;
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);

private:
    std::shared_ptr<LanguageFamily> Languages_;
    std::optional<std::string> Place_;
    std::optional<int> Period_;

    QTableWidget *MainTable_;

    void UpdateTable();
};