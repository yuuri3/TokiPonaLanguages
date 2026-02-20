#pragma once

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>

class UnimplementedDialog : public QDialog
{
    Q_OBJECT

public:
    UnimplementedDialog(QWidget *parent = nullptr);
    ~UnimplementedDialog() = default;

private slots:
    void OkButtonClicked();

private:
    QPushButton *OKButton;
};