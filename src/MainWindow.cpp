#include "MainWindow.h"
#include "Constants.h"
#include "UnimplementedDialog.h"
#include "SimulationDialog.h"
#include "Utility.h"
#include <QMenuBar>
#include <QTableWidget>
#include <QHeaderView>

namespace
{
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
    simulationMenu = menuBar->addMenu("シミュレーション");

    //     * シミュレーション
    simulateAction = new QAction("シミュレーション", this);
    simulationMenu->addAction(simulateAction);
    connect(simulateAction, &QAction::triggered, this, &MainWindow::Simulate);

    //     * 新規作成
    newFileAction = new QAction("新規作成", this);
    fileMenu->addAction(newFileAction);
    connect(newFileAction, &QAction::triggered, this, &MainWindow::Unimplemented);

    //     * ファイルを開く
    openFileAction = new QAction("ファイルを開く", this);
    fileMenu->addAction(openFileAction);
    connect(openFileAction, &QAction::triggered, this, &MainWindow::Unimplemented);

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

void MainWindow::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

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

void MainWindow::DisplayLanguageFamily()
{
    const auto table = simulator->ToString();
    DisplayTable(this, table);
}