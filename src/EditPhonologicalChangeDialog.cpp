#include "EditPhonologicalChangeDialog.h"

EditPhonologicalChangeDialog::EditPhonologicalChangeDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("音韻変化編集"));
    resize(400, 500);

    auto *mainLayout = new QVBoxLayout(this);

    // ==========================================
    // ヘルプボタン
    // ==========================================
    auto *helpLayout = new QHBoxLayout();
    helpButton_ = new QPushButton(tr("ヘルプ"), this);
    connect(helpButton_, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    helpLayout->addStretch();
    helpLayout->addWidget(helpButton_);
    mainLayout->addLayout(helpLayout);

    // ==========================================
    // 言語名セクション
    // ==========================================
    auto *langLabelLayout = new QHBoxLayout();
    langLabelLayout->addWidget(new QLabel(tr("言語名:"), this));
    selectLanguageButton_ = new QPushButton(tr("言語選択"), this);
    connect(selectLanguageButton_, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    langLabelLayout->addWidget(selectLanguageButton_);
    langLabelLayout->addStretch();
    mainLayout->addLayout(langLabelLayout);

    languageNameEdit_ = new QLineEdit(this);
    languageNameEdit_->setReadOnly(true); // 編集不可
    languageNameEdit_->setPlaceholderText(tr("未選択"));
    mainLayout->addWidget(languageNameEdit_);

    // ==========================================
    // 音韻変化セクション
    // ==========================================
    auto *ruleLabelLayout = new QHBoxLayout();
    ruleLabelLayout->addWidget(new QLabel(tr("音韻変化:"), this));
    addRuleButton_ = new QPushButton(tr("追加"), this);
    connect(addRuleButton_, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    ruleLabelLayout->addWidget(addRuleButton_);
    ruleLabelLayout->addStretch();
    mainLayout->addLayout(ruleLabelLayout);

    // 複数のテキストボックスの代わりとして QListWidget を使用
    rulesListWidget_ = new QListWidget(this);
    rulesListWidget_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(rulesListWidget_, &QListWidget::customContextMenuRequested, this, &EditPhonologicalChangeDialog::ShowContextMenu);

    // UI確認用のダミーアイテム（ダブルクリック等で編集可能）
    auto *dummyItem = new QListWidgetItem(tr("a > b / _c"));
    dummyItem->setFlags(dummyItem->flags() | Qt::ItemIsEditable);
    rulesListWidget_->addItem(dummyItem);

    mainLayout->addWidget(rulesListWidget_);

    // ==========================================
    // 音節構造セクション
    // ==========================================
    auto *syllableLayout = new QHBoxLayout();
    syllableLayout->addWidget(new QLabel(tr("音節構造:"), this));
    syllableStructureEdit_ = new QLineEdit(this);
    syllableLayout->addWidget(syllableStructureEdit_);
    mainLayout->addLayout(syllableLayout);

    // ==========================================
    // 同音語を許容するボタン
    // ==========================================
    // 状態を持つため QCheckBox を使用（QPushButton をトグル化しても可）
    allowHomophonesCheckBox_ = new QCheckBox(tr("同音語を許容する"), this);
    mainLayout->addWidget(allowHomophonesCheckBox_);

    // ==========================================
    // 下部ボタンセクション
    // ==========================================
    auto *buttonLayout = new QHBoxLayout();
    okButton_ = new QPushButton(tr("OK"), this);
    cancelButton_ = new QPushButton(tr("キャンセル"), this);

    connect(okButton_, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::Unimplemented);
    connect(cancelButton_, &QPushButton::clicked, this, &EditPhonologicalChangeDialog::reject);

    buttonLayout->addStretch();
    buttonLayout->addWidget(okButton_);
    buttonLayout->addWidget(cancelButton_);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
}

void EditPhonologicalChangeDialog::Unimplemented()
{
    // 未実装であることをユーザーに通知
    QMessageBox::information(this, tr("未実装"), tr("この機能は現在未実装です。"));
}

void EditPhonologicalChangeDialog::ShowContextMenu(const QPoint &pos)
{
    // 右クリックメニューの構築
    QMenu menu(this);

    QAction *moveUpAction = menu.addAction(tr("上へ移動"));
    QAction *moveDownAction = menu.addAction(tr("下へ移動"));
    menu.addSeparator();
    QAction *deleteAction = menu.addAction(tr("削除"));

    // 各アクションを未実装スロットに接続
    connect(moveUpAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);
    connect(moveDownAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);
    connect(deleteAction, &QAction::triggered, this, &EditPhonologicalChangeDialog::Unimplemented);

    // リストウィジェット上のグローバル座標にメニューを表示
    menu.exec(rulesListWidget_->mapToGlobal(pos));
}