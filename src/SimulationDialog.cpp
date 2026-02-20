#include "SimulationDialog.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace
{
    QSpinBox *CreateNSpin(SimulationDialog *dialog)
    {
        QSpinBox *spin = new QSpinBox(dialog);
        spin->setRange(1, 10000);
        spin->setValue(3);
        return spin;
    }

    QDoubleSpinBox *CreatePSpin(SimulationDialog *dialog, const double startP)
    {
        QDoubleSpinBox *spin = new QDoubleSpinBox(dialog);
        spin->setRange(0, 1);
        spin->setValue(startP);
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        return spin;
    }

    QLineEdit *CreateLineEdit(SimulationDialog *dialog, const std::string startP)
    {
        QLineEdit *lineEdit = new QLineEdit(dialog);
        lineEdit->setText(QString::fromStdString(startP));
        return lineEdit;
    }
}

SimulationDialog::SimulationDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("シミュレーション設定");
    setMinimumWidth(320);

    // --- レイアウト構成 ---
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    // 借用回数
    nLoanwordSpin = CreateNSpin(this);
    formLayout->addRow("借用回数:", nLoanwordSpin);

    // 音韻変化率
    pPhonologicalChangeSpin = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻変化率:", pPhonologicalChangeSpin);

    // 音韻脱落率
    pPhonologicalLossSpin = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻脱落率:", pPhonologicalLossSpin);

    // 単語消失率
    pObsoleteWordSpin = CreatePSpin(this, 0.0);
    formLayout->addRow("単語消失率:", pObsoleteWordSpin);

    // 単語生成率
    pCompoundSpin = CreatePSpin(this, 0.0);
    formLayout->addRow("単語生成率:", pCompoundSpin);

    // 祖語ファイルパス
    protoLanguagePath = CreateLineEdit(this, "ProtoLanguage.csv");
    formLayout->addRow("祖語ファイルパス:", protoLanguagePath);

    // 音素表ファイルパス
    phonemeTablePath = CreateLineEdit(this, "Phoneme.csv");
    formLayout->addRow("音素表ファイルパス:", phonemeTablePath);

    // 地理ファイルパス
    geometryPath = CreateLineEdit(this, "Geometry.csv");
    formLayout->addRow("地理ファイルパス:", geometryPath);

    mainLayout->addLayout(formLayout);

    // スペーサー（ボタンを下に押し下げる）
    mainLayout->addStretch();

    // --- ボタンエリア ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    cancelButton = new QPushButton("キャンセル", this);
    runButton = new QPushButton("実行開始", this);
    runButton->setDefault(true); // Enterキー対応

    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(runButton);
    mainLayout->addLayout(buttonLayout);

    // --- シグナルとスロットの接続 ---
    connect(cancelButton, &QPushButton::clicked, this, &SimulationDialog::reject);
    connect(runButton, &QPushButton::clicked, this, &SimulationDialog::accept);
}

void SimulationDialog::reject()
{
    QDialog::reject();
}

void SimulationDialog::accept()
{
    simulator = evolution(nLoanwordSpin->value(),
                          pPhonologicalChangeSpin->value(),
                          pPhonologicalLossSpin->value(),
                          0.0,
                          0.0,
                          pObsoleteWordSpin->value(),
                          pCompoundSpin->value(),
                          protoLanguagePath->text().toStdString(),
                          phonemeTablePath->text().toStdString(),
                          geometryPath->text().toStdString(),
                          "ignore/hoge.csv");
    if (!simulator)
    {
        // エラー
    }
    else
    {
        //
    }
    QDialog::accept();
}

std::optional<LanguageFamilySimulator> SimulationDialog::GetSimulator()
{
    return simulator;
}