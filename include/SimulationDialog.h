#pragma once

#include "stdafx.h"
#include "Evolution.h"

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
    QLineEdit *protoLanguagePath;
    QLineEdit *phonemeTablePath;
    QLineEdit *geometryPath;

    std::optional<LanguageFamilySimulator> simulator;

    QPushButton *runButton;
    QPushButton *cancelButton;
};