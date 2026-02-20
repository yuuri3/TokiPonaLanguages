#include "SimulationDialog.h"
#include <QFormLayout>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

namespace
{
    /**
     * @brief スピンボックス作成
     *
     * @param dialog ダイアログ
     * @param start 初期値
     * @return QSpinBox*
     */
    QSpinBox *CreateNSpin(SimulationDialog *dialog, const int start)
    {
        QSpinBox *spin = new QSpinBox(dialog);
        spin->setRange(1, 10000);
        spin->setValue(start);
        return spin;
    }

    /**
     * @brief 数値ボックス作成
     *
     * @param dialog ダイアログ
     * @param start 初期値
     * @return QDoubleSpinBox*
     */
    QDoubleSpinBox *CreatePSpin(SimulationDialog *dialog, const double start)
    {
        QDoubleSpinBox *spin = new QDoubleSpinBox(dialog);
        spin->setRange(0, 1);
        spin->setValue(start);
        spin->setButtonSymbols(QAbstractSpinBox::NoButtons);
        return spin;
    }

    /**
     * @brief テキストボックス作成
     *
     * @param dialog ダイアログ
     * @param start 初期値
     * @return QLineEdit*
     */
    QLineEdit *CreateLineEdit(SimulationDialog *dialog, const std::string start)
    {
        QLineEdit *lineEdit = new QLineEdit(dialog);
        lineEdit->setText(QString::fromStdString(start));
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
    nLoanwordSpin = CreateNSpin(this, 3);
    formLayout->addRow("借用回数:", nLoanwordSpin);

    // 音韻変化率
    pPhonologicalChangeSpin = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻変化率:", pPhonologicalChangeSpin);

    // 音韻脱落率
    pPhonologicalLossSpin = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻脱落率:", pPhonologicalLossSpin);

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

/**
 * @brief キャンセルボタンクリック時イベント
 *
 */
void SimulationDialog::reject()
{
    QDialog::reject();
}

/**
 * @brief 実行開始ボタンクリック時イベント
 *
 */
void SimulationDialog::accept()
{
    simulator = evolution(nLoanwordSpin->value(),
                          pPhonologicalChangeSpin->value(),
                          pPhonologicalLossSpin->value(),
                          0.0,
                          0.0,
                          protoLanguagePath->text().toStdString(),
                          phonemeTablePath->text().toStdString(),
                          geometryPath->text().toStdString(),
                          "ignore/hoge.csv");
    if (!simulator)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "シミュレーションの実行に失敗しました。\nファイルパスや設定値を確認してください。");
    }
    QDialog::accept();
}

/**
 * @brief シミュレータを取得
 *
 * @return std::optional<LanguageFamilySimulator>
 */
std::optional<LanguageFamilySimulator> SimulationDialog::GetSimulator()
{
    return simulator;
}