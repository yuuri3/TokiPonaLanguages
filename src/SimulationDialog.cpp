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
    NLoanwordSpin_ = CreateNSpin(this, 3);
    formLayout->addRow("借用回数:", NLoanwordSpin_);

    // 音韻変化率
    PPhonologicalChangeSpin_ = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻変化率:", PPhonologicalChangeSpin_);

    // 音韻脱落率
    PPhonologicalLossSpin_ = CreatePSpin(this, 0.3);
    formLayout->addRow("音韻脱落率:", PPhonologicalLossSpin_);

    // 祖語ファイルパス
    ProtoLanguagePath_ = CreateLineEdit(this, "data/ProtoLanguage.csv");
    formLayout->addRow("祖語ファイルパス:", ProtoLanguagePath_);

    // 音素表ファイルパス
    PhonemeTablePath_ = CreateLineEdit(this, "data/Phoneme.csv");
    formLayout->addRow("音素表ファイルパス:", PhonemeTablePath_);

    // 地理ファイルパス
    GeometryPath_ = CreateLineEdit(this, "data/Geometry.csv");
    formLayout->addRow("地理ファイルパス:", GeometryPath_);

    mainLayout->addLayout(formLayout);

    // スペーサー（ボタンを下に押し下げる）
    mainLayout->addStretch();

    // --- ボタンエリア ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    CancelButton_ = new QPushButton("キャンセル", this);
    RunButton_ = new QPushButton("実行開始", this);
    RunButton_->setDefault(true); // Enterキー対応

    buttonLayout->addStretch();
    buttonLayout->addWidget(CancelButton_);
    buttonLayout->addWidget(RunButton_);
    mainLayout->addLayout(buttonLayout);

    // --- シグナルとスロットの接続 ---
    connect(CancelButton_, &QPushButton::clicked, this, &SimulationDialog::reject);
    connect(RunButton_, &QPushButton::clicked, this, &SimulationDialog::accept);
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
    Simulator_ = evolution(NLoanwordSpin_->value(),
                           PPhonologicalChangeSpin_->value(),
                           PPhonologicalLossSpin_->value(),
                           ProtoLanguagePath_->text().toStdString(),
                           PhonemeTablePath_->text().toStdString(),
                           GeometryPath_->text().toStdString(),
                           "ignore/hoge.csv");
    if (!Simulator_)
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
    return Simulator_;
}