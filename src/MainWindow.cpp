#include "MainWindow.h"
#include "UnimplementedDialog.h"
#include "SimulationDialog.h"
#include "Utility.h"
#include "stdafx.h"
#include "EditLanguageWindow.h"

/**
 * @brief Construct a new Main Window:: Main Window object
 *
 * @param parent nullptr
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    const auto appName = QFileInfo(QCoreApplication::applicationFilePath()).completeBaseName();
    setWindowTitle(appName);

    // * メニューバー
    menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    menuBar->setStyleSheet("QMenuBar { border-bottom: 1px solid #B0B0B0; }");
    QHBoxLayout *menuLayout = new QHBoxLayout(menuBar);

    //   * ファイルメニュー
    fileMenu = menuBar->addMenu("ファイル");

    //     * 新規作成
    newFileAction = new QAction("新規作成", this);
    fileMenu->addAction(newFileAction);
    connect(newFileAction, &QAction::triggered, this, &MainWindow::NewFile);

    //     * ファイルを開く
    openFileAction = new QAction("ファイルを開く", this);
    fileMenu->addAction(openFileAction);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::OpenFile);

    //     * ファイル保存
    saveFileAction = new QAction("ファイル保存", this);
    fileMenu->addAction(saveFileAction);
    connect(saveFileAction, &QAction::triggered, this, &MainWindow::SaveFile);

    //   * 編集メニュー
    editMenu = menuBar->addMenu("編集");

    //     * 音韻変化
    phonologicalChangeAction = new QAction("音韻変化", this);
    editMenu->addAction(phonologicalChangeAction);
    connect(phonologicalChangeAction, &QAction::triggered, this, &MainWindow::Unimplemented);

    //     * 借用
    loanwordAction = new QAction("借用", this);
    editMenu->addAction(loanwordAction);
    connect(loanwordAction, &QAction::triggered, this, &MainWindow::Unimplemented);

    //   * シミュレーションメニュー
    simulationMenu = menuBar->addMenu("シミュレーション");

    //     * シミュレーション
    simulateAction = new QAction("シミュレーション", this);
    simulationMenu->addAction(simulateAction);
    connect(simulateAction, &QAction::triggered, this, &MainWindow::Simulate);

    //   * ヘルプメニュー
    helpMenu = menuBar->addMenu("ヘルプ");

    //     * ヘルプ
    helpAction = new QAction("ヘルプ", this);
    helpMenu->addAction(helpAction);
    connect(helpAction, &QAction::triggered, this, &MainWindow::Unimplemented);

    // * セントラル
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    //   * メインテーブル
    mainTable = new QTableWidget(this);
    layout->addWidget(mainTable);

    mainTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mainTable, &QTableWidget::customContextMenuRequested,
            this, &MainWindow::ShowContextMenu);

    // レイアウト調整
    constexpr int BUTTON_HEIGHT = 30;
    constexpr int BUTTON_WIDTH = 90;
    constexpr int MARGIN = 20;
    constexpr int BUTTON_SPACE = 5;
    constexpr int WINDOW_HEIGHT = 300;
    constexpr int WINDOW_WIDTH = 400;

    layout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    layout->setSpacing(BUTTON_SPACE);

    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

MainWindow::~MainWindow()
{
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void MainWindow::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief シミュレート
 *
 */
void MainWindow::Simulate()
{
    WarningUnsaveFile();
    SimulationDialog sub(this);
    sub.exec();
    simulator = sub.GetSimulator();
    if (simulator)
    {
        DisplayLanguageFamily();
    }
}

/**
 * @brief 語族をウィンドウに表示
 *
 */
void MainWindow::DisplayLanguageFamily()
{
    const auto table = simulator->ToStringLanguageFamily();
    DisplayTable(mainTable, table);
}

/**
 * @brief 編集結果を保存
 *
 */
void MainWindow::SaveFile()
{
    if (simulator)
    {
        QString fileName = QFileDialog::getSaveFileName(
            this,
            "保存先を設定",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
            "CSV Files (*.log);;All Files (*)");

        simulator->LanguageFamily_.Export(fileName.toStdString());
    }
    else
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "保存するファイルがありません。");
    }
}

/**
 * @brief ファイルを開く
 *
 */
void MainWindow::OpenFile()
{
    WarningUnsaveFile();
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "ファイルを選択",
        "C:/",              // 初期表示フォルダ
        "CSV Files (*.log)" // フィルタ
    );
    LanguageFamily languageFamily;
    languageFamily.Import(fileName.toStdString());
    simulator = LanguageFamilySimulator::Create(languageFamily);
    DisplayLanguageFamily();
}

void MainWindow::NewFile()
{
    WarningUnsaveFile();
    simulator = LanguageFamilySimulator::Create();
    DisplayLanguageFamily();
}

/**
 * @brief 未保存のファイルを警告
 *
 */
void MainWindow::WarningUnsaveFile()
{
    if (simulator)
    {
        const auto reply = QMessageBox::warning(
            this,
            "",
            "現在開いているファイルを保存しますか。",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QString fileName = QFileDialog::getSaveFileName(
                this,
                "保存先を設定",
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                "CSV Files (*.log);;All Files (*)");

            simulator->LanguageFamily_.Export(fileName.toStdString());
        }
    }
}

/**
 * @brief メインテーブル右クリック時
 *
 */
void MainWindow::ShowContextMenu(const QPoint &pos)
{
    // クリックされた位置のアイテムを取得
    QTableWidgetItem *item = mainTable->itemAt(pos);
    if (!item)
        return; // セルのない場所なら何もしない

    QMenu menu(this);
    QAction *editAction = menu.addAction("個別言語編集");

    // メニューを表示し、選ばれたアクションを取得
    QAction *selectedAction = menu.exec(mainTable->viewport()->mapToGlobal(pos));

    if (selectedAction == editAction)
    {
        const int row = mainTable->currentRow();
        const int column = mainTable->currentColumn();
        const std::string place = mainTable->item(0, column)->text().toStdString();
        const int period = row;
        EditLanguage(place, period);
    }
}

/**
 * @brief アプリクローズ時イベント
 *
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!simulator)
    {
        event->accept();
        return;
    }

    // カスタムダイアログの作成
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("うなぎエディタ");
    msgBox.setText("変更が保存されていません。");
    msgBox.setInformativeText("終了する前に保存しますか？");

    // ボタンの追加
    QPushButton *saveButton = msgBox.addButton("保存して終了", QMessageBox::ActionRole);
    QPushButton *discardButton = msgBox.addButton("保存せずに終了", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("キャンセル", QMessageBox::RejectRole);

    msgBox.setDefaultButton(saveButton);
    msgBox.exec();

    if (msgBox.clickedButton() == saveButton)
    {
        SaveFile();
        event->accept();
    }
    else if (msgBox.clickedButton() == discardButton)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

/**
 * @brief 言語編集ウィンドウを開く
 *
 * @param place 地域
 * @param period 時代
 */
void MainWindow::EditLanguage(const std::string place, const int period)
{
    EditLanguageWindow subWindow(this);
    subWindow.SetLanguages(std::make_shared<LanguageFamily>(simulator->LanguageFamily_));
    subWindow.SetPlace(place);
    subWindow.SetPeriod(period);
    subWindow.exec();
}