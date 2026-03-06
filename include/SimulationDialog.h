#pragma once

#include "stdafx.h"
#include "LanguageFamilySimulator.h"

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
    QSpinBox *NLoanwordSpin_;
    QDoubleSpinBox *PPhonologicalChangeSpin_;
    QDoubleSpinBox *PPhonologicalLossSpin_;
    QLineEdit *ProtoLanguagePath_;
    QLineEdit *PhonemeTablePath_;
    QLineEdit *GeometryPath_;

    std::optional<LanguageFamilySimulator> Simulator_;

    QPushButton *RunButton_;
    QPushButton *CancelButton_;
};