#include "MainWindow.h"
#include "Constants.h"
#include "UnimplementedDialog.h"
#include "SimulationDialog.h"
#include "Utility.h"
#include "stdafx.h"

namespace
{
    /**
     * @brief 文字列の配列をウィンドウに表示
     *
     * @param window ウィンドウ
     * @param data 文字列の配列
     */
    void DisplayTable(MainWindow *window, const std::vector<std::vector<std::string>> &data)
    {
        QWidget *centralWidget = new QWidget(window);
        window->setCentralWidget(centralWidget);
        QHBoxLayout *layout = new QHBoxLayout(centralWidget);

        QTableWidget *table = new QTableWidget(window);

        if (!data.empty())
        {
            int rows = data.size();
            int cols = data[0].size();
            table->setRowCount(rows);
            table->setColumnCount(cols);

            // 3. データの流し込み
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    // std::string から QString へ変換してセット
                    QString content = QString::fromStdString(data[i][j]);
                    table->setItem(i, j, new QTableWidgetItem(content));
                }
            }
        }

        table->verticalHeader()->setVisible(false);
        table->horizontalHeader()->setVisible(false);
        table->resizeColumnsToContents();
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

        layout->addWidget(table);
    }
}

/**
 * @brief Construct a new Main Window:: Main Window object
 *
 * @param parent nullptr
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(MyConst::Name);

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

    //   * シミュレーションメニュー
    simulationMenu = menuBar->addMenu("シミュレーション");

    //     * シミュレーション
    simulateAction = new QAction("シミュレーション", this);
    simulationMenu->addAction(simulateAction);
    connect(simulateAction, &QAction::triggered, this, &MainWindow::Simulate);

    // * セントラル
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

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
    const auto table = simulator->ToString();
    DisplayTable(this, table);
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
    simulator = LanguageFamilySimulator::Create();
}