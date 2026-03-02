#pragma once

#include "stdafx.h"
#include "LanguageFamily.h"

class EditGeometryDialog : public QDialog
{
    Q_OBJECT

public:
    EditGeometryDialog(QWidget *parent = nullptr);
    ~EditGeometryDialog() = default;
    void SetLanguages(LanguageFamily *languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void OnItemChanged(QTableWidgetItem *item);

private:
    LanguageFamily *Languages_;
    std::string Place_;
    int Period_;

    QTableWidget *MainTable_;

    void UpdateTable();
};