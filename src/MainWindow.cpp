#include "MainWindow.h"
#include "Constants.h"
#include "UnimplementedDialog.h"

/**
 * @brief Construct a new Main Window:: Main Window object
 *
 * @param parent nullptr
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(MyConst::Name);
    resize(400, 300);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    // シミュレーション
    simulateButton = new QPushButton("シミュレーション", this);
    layout->addWidget(simulateButton);
    connect(simulateButton, &QPushButton::clicked, this, &MainWindow::Unimplemented);

    // 新規作成
    newButton = new QPushButton("新規作成", this);
    layout->addWidget(newButton);
    connect(newButton, &QPushButton::clicked, this, &MainWindow::Unimplemented);

    // ファイルを開く
    openFileButton = new QPushButton("ファイルを開く", this);
    layout->addWidget(openFileButton);
    connect(openFileButton, &QPushButton::clicked, this, &MainWindow::Unimplemented);

    // レイアウト調整
    constexpr int BUTTON_HEIGHT = 30;
    constexpr int BUTTON_WIDTH = 90;
    constexpr int MARGIN = 20;
    constexpr int BUTTON_SPACE = 5;

    layout->setContentsMargins(MARGIN, MARGIN, MARGIN, MARGIN);
    layout->setSpacing(BUTTON_SPACE);

    simulateButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);
    newButton->setFixedSize(BUTTON_WIDTH + 5, BUTTON_HEIGHT);
    openFileButton->setFixedSize(BUTTON_WIDTH, BUTTON_HEIGHT);

    this->setFixedSize(sizeHint());
}

MainWindow::~MainWindow()
{
}

void MainWindow::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}