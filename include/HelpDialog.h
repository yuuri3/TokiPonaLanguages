#pragma once

#include "stdafx.h"

struct HelpDialogContent
{
    std::vector<QString> Headers;
    std::vector<std::vector<std::pair<QString, QString>>> Contents;
    void AddHeader(const QString &header);
    void AddContent(const QString &title, const QString &content);
};

class HelpDialog : public QDialog
{
    Q_OBJECT

public:
    HelpDialog(QWidget *parent = nullptr);
    ~HelpDialog() = default;
    void SetContents(const HelpDialogContent &contents);

private slots:

private:
    QPushButton *CloseButton;

    const int BUTTON_WIDTH = 50;
};