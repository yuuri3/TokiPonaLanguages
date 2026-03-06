#include "HelpDialog.h"

/**
 * @brief ヘルプダイアログのコンストラクタ
 *
 * @param parent 親ウィジェット
 */
HelpDialog::HelpDialog(QWidget *parent)
{
    setWindowTitle("ヘルプ");
    setMinimumSize(500, 500);

    // メインレイアウト
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // スクロールエリアの設定
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(scrollArea);

    // スクロールエリアの中身となるメインウィジェット
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    scrollArea->setWidget(scrollContent);

    // ラベル作成用のヘルパー処理（背景色付き見出し）
    auto addHeader = [&](const QString &text)
    {
        QLabel *label = new QLabel(text, scrollContent);
        label->setStyleSheet("background-color: #e0e0e0; font-weight: bold; padding: 5px;");
        layout->addWidget(label);
    };

    // 小項目と説明を追加する処理
    auto addContent = [&](const QString &title, const QString &desc)
    {
        layout->addWidget(new QLabel(QString("<b>【%1】</b>").arg(title), scrollContent));
        QLabel *descLabel = new QLabel(QString("　%1").arg(desc), scrollContent);
        descLabel->setWordWrap(true);
        layout->addWidget(descLabel);
        layout->addSpacing(5);
    };

    // --- コンテンツ構成 ---

    // ■ ファイル
    addHeader("ファイル");
    addContent("新規作成", "新しく空のプロジェクト（語族データ）を作成します。");
    addContent("ファイルを開く", "保存済みの語族データ（.ulng）や、個別言語のデータ（.json）を読み込みます。");
    addContent("プロジェクトを保存", "編集中の語族データ全体（構成する全言語を含む）を .ulng ファイルに書き出します。");
    addContent("個別言語をエクスポート", "現在選択している特定の言語データのみを .json ファイルとして保存します。");

    layout->addSpacing(10);

    // ■ 編集
    addHeader("編集");
    addContent("音韻変化", "設定した音韻変化規則に基づき、単語の語形を世代ごとに一斉更新します。");
    addContent("借用", "隣接する他言語の語彙を参照し、新しい単語として自言語に取り込みます。");
    addContent("地理編集", "言語が話されている地域（セル）の追加・削除や、その接続関係を編集します。");
    addContent("時間軸編集", "言語変化の単位となる時代の追加・削除を行います。");
    addContent("個別言語編集", "特定の時代・場所...の詳細な編集を行います。");

    layout->addSpacing(10);

    // ■ シミュレーション
    addHeader("シミュレーション");
    addContent("シミュレーション", "変化の頻度や伝播率などのパラメータを入力し、計算された言語変遷の結果を表示します。");

    // 下部に余白を追加して上寄せにする
    layout->addStretch();

    // --- 固定エリア（閉じるボタン） ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    QPushButton *closeButton = new QPushButton("閉じる", this);
    closeButton->setFixedWidth(BUTTON_WIDTH);
    connect(closeButton, &QPushButton::clicked, this, &HelpDialog::accept);

    buttonLayout->addWidget(closeButton);
    mainLayout->addLayout(buttonLayout);
}