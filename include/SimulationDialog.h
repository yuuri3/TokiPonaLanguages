#pragma once

#include <QDialog>
#include <QDoubleSpinBox>
#include "Evolution.h"

// クラスの前方宣言（コンパイル速度向上のため）
class QSpinBox;
class QLineEdit;
class QPushButton;

class SimulationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationDialog(QWidget *parent = nullptr);
    std::optional<LanguageFamilySimulator> GetSimulator();
    void reject() override;
    void accept() override;

private:
    QSpinBox *nLoanwordSpin;
    QDoubleSpinBox *pPhonologicalChangeSpin;
    QDoubleSpinBox *pPhonologicalLossSpin;
    QDoubleSpinBox *pObsoleteWordSpin;
    QDoubleSpinBox *pCompoundSpin;
    QLineEdit *protoLanguagePath;
    QLineEdit *phonemeTablePath;
    QLineEdit *geometryPath;

    std::optional<LanguageFamilySimulator> simulator;

    QPushButton *runButton;
    QPushButton *cancelButton;
};