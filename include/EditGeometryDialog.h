#pragma once

#include "stdafx.h"
#include "GeometryDifference.h"

class LanguageFamily;

class EditGeometryDialog : public QDialog
{
    Q_OBJECT

public:
    EditGeometryDialog(QWidget *parent = nullptr);
    ~EditGeometryDialog() = default;
    void SetLanguages(std::shared_ptr<LanguageFamily> languages);
    void SetPlace(const std::string &place);
    void SetPeriod(const int period);
    void accept() override;

private slots:
    void Unimplemented();
    void ShowContextMenu(const QPoint &pos);
    void OnItemChanged(QTableWidgetItem *item);
    void ShowHelp();

private:
    std::shared_ptr<LanguageFamily> Languages_;
    std::string Place_;
    int Period_;

    QTableWidget *MainTable_;
    std::vector<GeometryDifference> GeometryDifferences_;

    // 現在の地理データの状態を保持するメンバ
    std::vector<std::vector<std::string>> CurrentGeometryTable_;

    void UpdateTable();
    void ApplyDifference(const GeometryDifference &difference);
};