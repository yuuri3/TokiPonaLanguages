#include "MainWindow.h"
#include "UnimplementedDialog.h"
#include "Utility.h"
#include "stdafx.h"
#include "EditLanguageDialog.h"
#include "EditPeriodDialog.h"
#include "EditGeometryDialog.h"
#include "HelpDialog.h"
#include "EditPhonologicalChangeDialog.h"
#include "EditLoanwordDialog.h"

/**
 * @brief Construct a new Main Window:: Main Window object
 *
 * @param parent nullptr
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Languages_ = std::make_shared<LanguageFamily>();
    IsLanguagesSaved_ = true;

    setWindowTitle(QString::fromStdString(APPLICATION_NAME));

    // * メニューバー
    MenuBar_ = new QMenuBar(this);
    setMenuBar(MenuBar_);
    MenuBar_->setStyleSheet("QMenuBar { border-bottom: 1px solid #B0B0B0; }");
    QHBoxLayout *menuLayout = new QHBoxLayout(MenuBar_);

    //   * ファイルメニュー
    FileMenu_ = MenuBar_->addMenu("ファイル");

    //     * 新規作成
    NewFileAction_ = new QAction("新規作成", this);
    FileMenu_->addAction(NewFileAction_);
    connect(NewFileAction_, &QAction::triggered, this, &MainWindow::NewFile);

    //     * ファイルを開く
    OpenFileAction_ = new QAction("ファイルを開く", this);
    FileMenu_->addAction(OpenFileAction_);
    connect(OpenFileAction_, &QAction::triggered, this, &MainWindow::OpenFile);

    //     * ファイル保存
    SaveFileAction_ = new QAction("ファイル保存", this);
    FileMenu_->addAction(SaveFileAction_);
    connect(SaveFileAction_, &QAction::triggered, this, &MainWindow::SaveFile);

    //   * 編集メニュー
    EditMenu_ = MenuBar_->addMenu("編集");

    //     * 音韻変化
    PhonologicalChangeAction_ = new QAction("音韻変化", this);
    EditMenu_->addAction(PhonologicalChangeAction_);
    connect(PhonologicalChangeAction_, &QAction::triggered, this, &MainWindow::EditPhonologicalChange);

    //     * 借用
    LoanwordAction_ = new QAction("借用", this);
    EditMenu_->addAction(LoanwordAction_);
    connect(LoanwordAction_, &QAction::triggered, this, &MainWindow::EditLoanword);

    //     * 地理編集
    EditGeometry_ = new QAction("地理編集", this);
    EditMenu_->addAction(EditGeometry_);
    connect(EditGeometry_, &QAction::triggered, this, &MainWindow::EditGeometryFromMenu);

    //     * 時間軸編集
    EditPeriod_ = new QAction("時間軸編集", this);
    EditMenu_->addAction(EditPeriod_);
    connect(EditPeriod_, &QAction::triggered, this, &MainWindow::EditPeriodFromMenu);

    //   * ヘルプメニュー
    HelpMenu_ = MenuBar_->addMenu("ヘルプ");

    //     * ヘルプ
    HelpAction_ = new QAction("ヘルプ", this);
    HelpMenu_->addAction(HelpAction_);
    connect(HelpAction_, &QAction::triggered, this, &MainWindow::ShowHelp);

    //     * バージョン情報
    VersionAction_ = new QAction("バージョン情報", this);
    HelpMenu_->addAction(VersionAction_);
    connect(VersionAction_, &QAction::triggered, this, &MainWindow::ShowVersion);

    //     * Qt ライセンス
    QtAction_ = new QAction("Qt ライセンス", this);
    HelpMenu_->addAction(QtAction_);
    connect(QtAction_, &QAction::triggered, this, &MainWindow::ShowQtLicense);

    // * セントラル
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    LayoutData_ = DialogLayout::Create("", false, false, false);
    LayoutData_.SetDataType(MAIN_TABLE_ID, DialogDataType::Table);
    LayoutData_.SetHasContextMenu(MAIN_TABLE_ID, true);

    LayoutData_.GenerateLayout(centralWidget);

    LayoutData_.ConnectContextMenu(MAIN_TABLE_ID, this, &MainWindow::ShowContextMenu);

    DisplayLanguageFamily(Languages_);

    // レイアウト調整
    constexpr int MARGIN = 20;
    constexpr int BUTTON_SPACE = 5;
    constexpr int WINDOW_HEIGHT = 300;
    constexpr int WINDOW_WIDTH = 400;

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
 * @brief 語族をウィンドウに表示
 *
 */
void MainWindow::DisplayLanguageFamily(const std::shared_ptr<LanguageFamily> languages)
{
    LanguageNames_ = languages->GetLanguageNames();
    if (!LanguageNames_)
    {
        return;
    }
    LayoutData_.SetData(MAIN_TABLE_ID, LanguageNames_->Fill());

    if (LanguageNames_->Header.empty() || LanguageNames_->Body.empty())
    {
        SaveFileAction_->setEnabled(false);
        PhonologicalChangeAction_->setEnabled(false);
        LoanwordAction_->setEnabled(false);
        EditGeometry_->setEnabled(false);
        EditPeriod_->setEnabled(false);
    }
    else
    {
        SaveFileAction_->setEnabled(true);
        PhonologicalChangeAction_->setEnabled(true);
        LoanwordAction_->setEnabled(true);
        EditGeometry_->setEnabled(true);
        EditPeriod_->setEnabled(true);
    }
}

/**
 * @brief 編集結果を保存
 *
 */
void MainWindow::SaveFile()
{
    if (!Languages_->Empty())
    {
        QString fileName = QFileDialog::getSaveFileName(
            this,
            "保存先を設定",
            QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
            "ULang Files (*.ulng)");

        if (fileName.isEmpty())
        {
            return;
        }
        Languages_->Export(fileName.toStdString());
    }
    else
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "保存するファイルがありません。");
    }
    IsLanguagesSaved_ = true;
}

/**
 * @brief 音韻変化編集
 *
 */
void MainWindow::EditPhonologicalChange()
{
    if (!LanguageNames_)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "編集するファイルがありません。");
        return;
    }
    EditPhonologicalChangeDialog subWindow(this);
    subWindow.SetLanguages(Languages_);
    subWindow.SetLanguageNames(*LanguageNames_);
    subWindow.exec();
    IsLanguagesSaved_ = false;
}

/**
 * @brief 借用編集
 *
 */
void MainWindow::EditLoanword()
{
    if (!LanguageNames_)
    {
        QMessageBox::critical(
            this,
            "実行エラー",
            "編集するファイルがありません。");
        return;
    }

    EditLoanwordDialog subWindow(this);
    subWindow.SetLanguages(Languages_);
    subWindow.SetLanguageNames(*LanguageNames_);
    subWindow.exec();

    IsLanguagesSaved_ = false;
}

/**
 * @brief 地理編集
 *
 */
void MainWindow::EditGeometryFromMenu()
{
    EditGeometryDialog subWindow(this);
    subWindow.Set(Languages_);
    subWindow.exec();

    DisplayLanguageFamily(Languages_);
    IsLanguagesSaved_ = false;
}

/**
 * @brief 時間軸編集
 *
 */
void MainWindow::EditPeriodFromMenu()
{
    EditPeriodDialog subWindow(this);
    subWindow.SetLanguages(Languages_);
    subWindow.exec();

    DisplayLanguageFamily(Languages_);
    IsLanguagesSaved_ = false;
}

/**
 * @brief ファイルを開く
 *
 */
void MainWindow::OpenFile()
{
    if (IsLanguagesSaved_ || WarningUnsaveFile())
    {
        QString fileName = QFileDialog::getOpenFileName(
            this,
            "ファイルを選択",
            "C:/",                            // 初期表示フォルダ
            "Supported Files (*.ulng *.json)" // フィルタ
        );

        // ファイル選択がキャンセルされた場合は何もしない
        if (fileName.isEmpty())
        {
            return;
        }

        // 拡張子による分岐処理
        bool isOpenFile = false;
        if (fileName.endsWith(".json", Qt::CaseInsensitive))
        {
            // .json 読み込み
            *Languages_ = LanguageFamily();
            isOpenFile = Languages_->ImportJson(fileName.toStdString());
        }
        else if (fileName.endsWith(".ulng", Qt::CaseInsensitive))
        {
            // 従来の .ulng 読み込み処理
            *Languages_ = LanguageFamily();
            isOpenFile = Languages_->Import(fileName.toStdString());
        }

        if (!isOpenFile)
        {
            QMessageBox::critical(
                this,
                "実行エラー",
                "ファイルを開けません");
        }
        else
        {
            DisplayLanguageFamily(Languages_);
            IsLanguagesSaved_ = true;
        }
    }
}

void MainWindow::NewFile()
{
    if (IsLanguagesSaved_ || WarningUnsaveFile())
    {
        *Languages_ = LanguageFamily();
        DisplayLanguageFamily(Languages_);

        IsLanguagesSaved_ = true;
    }
}

/**
 * @brief 未保存のファイルを警告
 * * @return 保存処理が完了、または保存せずに続行する場合は true。キャンセルされた場合は false。
 */
bool MainWindow::WarningUnsaveFile()
{
    if (!Languages_->Empty())
    {
        const auto reply = QMessageBox::warning(
            this,
            "",
            "現在開いているファイルを保存しますか。",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

        if (reply == QMessageBox::Yes)
        {
            QString fileName = QFileDialog::getSaveFileName(
                this,
                "保存先を設定",
                QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                "CSV Files (*.log);;All Files (*)");

            if (fileName.isEmpty())
            {
                // 保存ダイアログでキャンセルされた場合
                return false;
            }

            Languages_->Export(fileName.toStdString());
            return true;
        }
        else if (reply == QMessageBox::Cancel)
        {
            // キャンセルボタンまたは×ボタンで終了した場合
            return false;
        }
    }

    // Languages_ が空、または「いいえ（保存しない）」を選択した場合
    return true;
}

/**
 * @brief メインテーブル右クリック時
 *
 */
void MainWindow::ShowContextMenu(const QPoint &pos)
{
    if (!LanguageNames_)
    {
        return;
    }

    auto cellInfo = LayoutData_.GetCellInfo(MAIN_TABLE_ID, pos);
    if (!cellInfo || cellInfo->row < 0)
        return;

    const int row = cellInfo->row;
    const int column = cellInfo->column;
    if (row < 0 || row >= LanguageNames_->Body.size())
    {
        return;
    }
    if (column < 0 || column >= LanguageNames_->Body.at(0).size())
    {
        return;
    }
    const int place = column;
    const int period = row;

    QMenu menu(this);
    QAction *editAction = menu.addAction("個別言語編集");
    if (LanguageNames_->Body.at(period).at(column).empty())
    {
        editAction->setEnabled(false);
    }
    QAction *editPhonologicalChange = menu.addAction("音韻変化編集");
    if (LanguageNames_->Body.at(period).at(column).empty() || row < 1)
    {
        editPhonologicalChange->setEnabled(false);
    }
    QAction *editPeriod = menu.addAction("時間軸編集");
    QAction *editGeography = menu.addAction("地理編集");

    // メニューを表示し、選ばれたアクションを取得
    QAction *selectedAction = menu.exec(cellInfo->globalPos);

    if (selectedAction == editAction)
    {
        EditLanguage(place, period);
    }
    else if (selectedAction == editPhonologicalChange)
    {
        EditPhonologicalChangeWithIndex(row, column);
    }
    else if (selectedAction == editPeriod)
    {
        EditPeriod(place, period);
    }
    else if (selectedAction == editGeography)
    {
        EditGeometry(place, period);
    }
}

/**
 * @brief ヘルプ起動
 *
 */
void MainWindow::ShowHelp()
{
    HelpDialogContent contents;
    // ■ ファイル
    contents.AddHeader("ファイル");
    contents.AddContent("新規作成", "新しく空のプロジェクト（語族データ）を作成します。");
    contents.AddContent("ファイルを開く", "保存済みの語族データ（.ulng）や、個別言語のデータ（.json）を読み込みます。");
    contents.AddContent("プロジェクトを保存", "編集中の語族データ全体（構成する全言語を含む）を .ulng ファイルに書き出します。");
    contents.AddContent("個別言語をエクスポート", "現在選択している特定の言語データのみを .json ファイルとして保存します。");

    // layout->addSpacing(10);

    // ■ 編集
    contents.AddHeader("編集");
    contents.AddContent("音韻変化", "設定した音韻変化規則に基づき、単語の語形を世代ごとに一斉更新します。");
    contents.AddContent("借用", "隣接する他言語の語彙を参照し、新しい単語として自言語に取り込みます。");
    contents.AddContent("地理編集", "言語が話されている地域（セル）の追加・削除や、その接続関係を編集します。");
    contents.AddContent("時間軸編集", "言語変化の単位となる時代の追加・削除を行います。");
    contents.AddContent("個別言語編集", "特定の時代・場所...の詳細な編集を行います。");

    // layout->addSpacing(10);

    // // ■ シミュレーション
    contents.AddHeader("シミュレーション");
    contents.AddContent("シミュレーション", "変化の頻度や伝播率などのパラメータを入力し、計算された言語変遷の結果を表示します。");

    HelpDialog subWindow(this);
    subWindow.SetContents(contents);
    subWindow.exec();
}

/**
 * @brief アプリクローズ時イベント
 *
 * @param event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (Languages_->Empty() || IsLanguagesSaved_)
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
        if (IsLanguagesSaved_)
        {
            event->accept();
        }
        else
        {
            event->ignore();
        }
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
void MainWindow::EditLanguage(const int place, const int period)
{
    if (!Languages_->Empty())
    {
        EditLanguageDialog subWindow(this);
        subWindow.Set(Languages_,place,period);
        subWindow.exec();

        IsLanguagesSaved_ = false;
    }
}

/**
 * @brief 音韻変化編集ダイアログを開く
 *
 * @param row 行
 * @param column 列
 */
void MainWindow::EditPhonologicalChangeWithIndex(const int row, const int column)
{
    EditPhonologicalChangeDialog subWindow(this);
    subWindow.SetLanguages(Languages_);
    subWindow.SetLanguageNames(*LanguageNames_);
    subWindow.SetPlaceAndPeriod(row, column);
    subWindow.exec();
    IsLanguagesSaved_ = false;
}

/**
 * @brief バージョン表示イベント
 *
 */
void MainWindow::ShowVersion()
{
    QMessageBox::about(this, "バージョン情報",
                       QString::fromStdString(APPLICATION_NAME + APPLICATION_VERSION));
}

/**
 * @brief Qt ライセンス表示
 *
 */
void MainWindow::ShowQtLicense()
{
    QApplication::aboutQt();
}

/**
 * @brief 時間軸編集
 *
 * @param place 地域
 * @param period 時代
 */
void MainWindow::EditPeriod(const int place, const int period)
{
    EditPeriodDialog subWindow(this);
    subWindow.SetLanguages(Languages_);
    subWindow.SetPlace(place);
    subWindow.SetPeriod(period);
    subWindow.exec();

    DisplayLanguageFamily(Languages_);
    IsLanguagesSaved_ = false;
}

/**
 * @brief 地理編集
 *
 * @param place 地域
 * @param period 時代
 */
void MainWindow::EditGeometry(const int place, const int period)
{
    EditGeometryDialog subWindow(this);
    subWindow.Set(Languages_);
    subWindow.exec();

    DisplayLanguageFamily(Languages_);
    IsLanguagesSaved_ = false;
}