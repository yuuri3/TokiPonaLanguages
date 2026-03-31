#include "EditLanguageDialog.h"
#include "UnimplementedDialog.h"
#include "EditWordDialog.h"
#include "LanguageFamily.h"
#include "Utility.h"

EditLanguageDialog::EditLanguageDialog(QWidget *parent)
{
    setWindowTitle("個別言語編集");

    QVBoxLayout *layout = new QVBoxLayout(this);

    //   * 単語表示
    MainTable_ = new QTableWidget(this);
    layout->addWidget(MainTable_);

    MainTable_->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(MainTable_, &QTableWidget::customContextMenuRequested,
            this, &EditLanguageDialog::ShowContextMenu);

    //   * 検索バー
    QHBoxLayout *searchLayout = new QHBoxLayout();

    SearchLineEdit_ = new QLineEdit(this);
    SearchLineEdit_->setPlaceholderText("検索ワードを入力...");

    SearchButton_ = new QPushButton("検索", this);

    connect(SearchButton_, &QPushButton::clicked, this, &EditLanguageDialog::Unimplemented);

    searchLayout->addWidget(SearchLineEdit_);
    searchLayout->addWidget(SearchButton_);

    layout->addLayout(searchLayout);

    //   * 単語追加ボタン
    AddWordButton_ = new QPushButton("単語追加", this);

    connect(AddWordButton_, &QPushButton::clicked, this, &EditLanguageDialog::Unimplemented);
    layout->addWidget(AddWordButton_);
}

/**
 * @brief 語族セッタ
 *
 * @param languages
 */
void EditLanguageDialog::SetLanguages(std::shared_ptr<LanguageFamily> languages)
{
    Languages_ = languages;
    UpdateTable();
}

/**
 * @brief 地理セッタ
 *
 * @param place
 */
void EditLanguageDialog::SetPlace(const std::string &place)
{
    Place_ = place;
    UpdateTable();
}

/**
 * @brief 時代セッタ
 *
 * @param period
 */
void EditLanguageDialog::SetPeriod(const int period)
{
    Period_ = period;
    UpdateTable();
}

/**
 * @brief 表更新
 *
 */
void EditLanguageDialog::UpdateTable()
{
    if (Languages_ && Place_ && Period_)
    {
        auto language = Languages_->CalculateLanguage(*Place_, *Period_);
        if (!language)
        {
            return;
        }

        std::vector<std::vector<std::string>> wordData;
        std::vector<std::string> line;

        line.emplace_back("単語");
        line.emplace_back("訳語");
        wordData.emplace_back(line);
        line.clear();

        for (int i = 0; i < language->CountWord(); i++)
        {
            const auto &[_, word] = language->GetNthWord(i);

            line.emplace_back(Languages_->GetPhonemeTable().ConvertToString(word.GetForm()));

            const auto translations = word.GetAllTranslations();
            line.emplace_back(JoinStrs(translations, ","));

            wordData.emplace_back(line);
            line.clear();
        }

        DisplayTable(MainTable_, wordData);
    }
}

/**
 * @brief 未実装な機能へアクセスしたときの処理
 *
 */
void EditLanguageDialog::Unimplemented()
{
    UnimplementedDialog sub(this);
    sub.exec();
}

/**
 * @brief 単語編集メニュー表示
 *
 */
void EditLanguageDialog::ShowContextMenu(const QPoint &pos)
{
    if (Languages_ && Place_ && Period_)
    {
        // クリックされた位置のアイテムを取得
        QTableWidgetItem *item = MainTable_->itemAt(pos);
        if (!item)
            return; // セルのない場所なら何もしない

        QMenu menu(this);
        QAction *editAction = menu.addAction("編集");

        // メニューを表示し、選ばれたアクションを取得
        QAction *selectedAction = menu.exec(MainTable_->viewport()->mapToGlobal(pos));

        if (selectedAction == editAction)
        {
            std::optional<Language> language;
            if (Language_)
            {
                language = Language_;
            }
            else
            {
                language = Languages_->CalculateLanguage(*Place_, *Period_);
                Language_ = language;
                if (!language)
                {
                    return;
                }
            }

            const int row = MainTable_->currentRow();
            const int column = MainTable_->currentColumn();

            if (row - 1 >= language->CountWord())
            {
                return;
            }
            const auto &[wordID, _] = language->GetNthWord(row - 1);

            EditWordDialog subWindow(this);
            subWindow.Set(Languages_, std::make_shared<Language>(Language_.value()), *Place_, *Period_, wordID);
            subWindow.exec();
        }
    }
}