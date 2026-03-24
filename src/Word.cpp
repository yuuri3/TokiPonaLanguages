#include "Language.h"
#include "PhonologicalChange.h"
#include "PhonemeTable.h"

namespace
{
    /**
     * @brief 音韻変化を適用する
     *
     * @param wordForm 変換前の語形（音素IDの配列）
     * @param changedWordForm 変換後の語形（音素IDの配列）
     * @param phonologicalChange 音韻変化の定義
     * @param phonemeTable 音素マスタ情報
     * @return bool 変化が発生したかどうか
     */
    bool ChangeWordSound(const std::vector<int> &wordForm, std::vector<int> &changedWordForm, const PhonologicalChange &phonologicalChange, const PhonemeTable &phonemeTable)
    {
        changedWordForm.clear();
        // わずかに余裕を持ってリザーブ（挿入による拡張を考慮）
        changedWordForm.reserve(wordForm.size() + 2);
        bool isChanged = false;

        const auto &beforeItems = phonologicalChange.BeforePhoneticItems_;
        const auto &afterItems = phonologicalChange.AfterPhoneticItems_;
        const auto &envConditions = phonologicalChange.PhoneticEnvironment_;

        const size_t wordSize = wordForm.size();
        const size_t beforeSize = beforeItems.size();

        for (size_t soundPosition = 0; soundPosition < wordSize;)
        {
            bool isMatch = true;

            // 1. 変化前パターン(Before)の照合（シーケンスチェック）
            if (soundPosition + beforeSize > wordSize)
            {
                isMatch = false;
            }
            else
            {
                for (size_t i = 0; i < beforeSize; ++i)
                {
                    if (!phonemeTable.Matches(wordForm[soundPosition + i], beforeItems[i]))
                    {
                        isMatch = false;
                        break;
                    }
                }
            }

            // 2. 変化条件(Environment)の照合
            if (isMatch)
            {
                for (const auto &env : envConditions)
                {
                    const int relativeOffset = env.first;
                    const PhoneticItem &conditionItem = env.second;

                    // 安全なインデックス計算（アンダーフロー/オーバーフロー防止）
                    int baseIndex = (relativeOffset < 0)
                                        ? static_cast<int>(soundPosition)
                                        : static_cast<int>(soundPosition + beforeSize) - 1;

                    int targetIndex = baseIndex + relativeOffset;

                    // 境界チェック
                    if (targetIndex < 0 || targetIndex >= static_cast<int>(wordSize))
                    {
                        isMatch = false;
                        break;
                    }

                    if (!phonemeTable.Matches(wordForm[static_cast<size_t>(targetIndex)], conditionItem))
                    {
                        isMatch = false;
                        break;
                    }
                }
            }

            // 3. 置換処理の実行
            if (isMatch)
            {
                isChanged = true;
                // Beforeに該当する部分を飛ばし、Afterの内容を追加する
                for (const auto &afterItem : afterItems)
                {
                    // Afterが具体的な音素IDを持つ場合はそれを追加
                    if (afterItem.Type_ == PhoneticItemType::Phoneme)
                    {
                        changedWordForm.push_back(afterItem.ID_);
                    }
                    // 素性変更などの場合は、元の音素に基づいてPhonemeTableから新音素を特定
                    else
                    {
                        // 素性変更などの未実装箇所でも、元の音素を維持するか明示的な処理が必要
                        // ここではロジック維持のため元の要素をコピーする例として記述
                        changedWordForm.push_back(wordForm[soundPosition]);
                    }
                }

                // 無限ループ防止: beforeSizeが0の場合でも必ず1つは進める
                soundPosition += (beforeSize > 0) ? beforeSize : 1;
            }
            else
            {
                // マッチしなかった場合は現在の音素をそのままコピーして次へ
                changedWordForm.push_back(wordForm[soundPosition]);
                soundPosition++;
            }
        }

        return isChanged;
    }
}

/**
 * @brief 複合語を生成
 *
 * @param word 単語
 * @return 複合語
 */
Word Word::Add(const Word &word) const
{
    Word result;
    result.Form_.reserve(Form_.size() + word.Form_.size());
    result.Form_.insert(result.Form_.end(), Form_.begin(), Form_.end());
    result.Form_.insert(result.Form_.end(), word.Form_.begin(), word.Form_.end());

    return result;
}

/**
 * @brief インスタンス作成
 *
 * @param form
 */
Word Word::Create(const std::vector<int> &form)
{
    Word word;
    word.Form_ = form;
    return word;
}

/**
 * @brief json オブジェクトから作成
 *
 * @param obj json オブジェクト
 * @return Word
 */
Word Word::CreateFromJsonObject(const QJsonObject &obj, const PhonemeTable &table)
{
    Word word;

    // 1. entry (ID, form)
    QJsonObject entryObj = obj["entry"].toObject();
    word.ID = entryObj["id"].toInt();

    // form 文字列を std::vector<Phoneme> に変換
    // ※プロジェクト内の既存の変換関数（PhonemeConverterなど）を利用
    QString formStr = entryObj["form"].toString();
    word.Form_ = table.ConvertToPhoneme(formStr.toStdString());

    // 2. tags
    QJsonArray tagsArray = obj["tags"].toArray();
    int tagID = 0;
    for (const auto &tag : tagsArray)
    {
        word.Tags_[tagID] = tag.toString().toStdString();
        tagID++;
    }

    // 3. translations (title -> forms のリスト)
    QJsonArray transArray = obj["translations"].toArray();
    int partId = 0;
    for (const auto &transValue : transArray)
    {
        QJsonObject transObj = transValue.toObject();
        std::string title = transObj["title"].toString().toStdString();
        word.SetPart(partId, title);

        std::vector<std::string> forms;
        QJsonArray formsArray = transObj["forms"].toArray();
        for (const auto &f : formsArray)
        {
            forms.push_back(f.toString().toStdString());
        }
        int id = 0;
        for (const auto &form : forms)
        {
            word.SetTranslation(partId, id, form);
            id++;
        }
        partId++;
    }

    // 4. contents (title -> text)
    QJsonArray contentsArray = obj["contents"].toArray();
    int contentID = 0;
    for (const auto &contValue : contentsArray)
    {
        QJsonObject contObj = contValue.toObject();
        std::string title = contObj["title"].toString().toStdString();
        std::string text = contObj["text"].toString().toStdString();
        word.Contents_[contentID] = {title, text};
        contentID++;
    }

    // 5. variations (title -> Phonemeリスト)
    QJsonArray variationsArray = obj["variations"].toArray();
    int variationID = 0;
    for (const auto &varValue : variationsArray)
    {
        QJsonObject varObj = varValue.toObject();
        std::string title = varObj["title"].toString().toStdString();
        std::string form = varObj["form"].toString().toStdString();
        word.Variations_[variationID] = {title, table.ConvertToPhoneme(form)};
        variationID++;
    }

    // 6. relations (index -> <title, entry ID>)
    QJsonArray relationsArray = obj["relations"].toArray();
    int relationId = 0;
    for (const auto &relValue : relationsArray)
    {
        QJsonObject relObj = relValue.toObject();
        std::string title = relObj["title"].toString().toStdString();
        int entryId = relObj["entry"].toObject()["id"].toInt();

        word.Relations_[relationId] = std::make_pair(title, entryId);
        relationId++;
    }

    return word;
}

/**
 * @brief 語形をゲット
 *
 * @return const std::vector<Phoneme>
 */
const std::vector<int> Word::GetForm() const
{
    return Form_;
}

/**
 * @brief 品詞IDをゲット
 *
 * @return const std::vector<int>
 */
const std::vector<int> Word::GetPartIDs() const
{
    std::vector<int> result;
    for (const auto &[partID, _] : Translations_)
    {
        result.emplace_back(partID);
    }
    return result;
}

/**
 * @brief 訳語IDをゲット
 *
 * @param partID 品詞ID
 * @return const std::vector<int>
 */
const std::vector<int> Word::GetTranslationIDs(const int partID) const
{
    std::vector<int> result;
    if (Translations_.count(partID) == 0)
    {
        return result;
    }
    for (const auto &[translationID, _] : Translations_.at(partID).second)
    {
        result.emplace_back(translationID);
    }
    return result;
}

/**
 * @brief 品詞をゲット
 *
 * @param partID 品詞ID
 * @return const std::string
 */
const std::string Word::GetPart(const int partID) const
{
    if (Translations_.count(partID) == 0)
    {
        return "";
    }
    return Translations_.at(partID).first;
}

/**
 * @brief 訳語をゲット
 *
 * @param partID 品詞ID
 * @param translationID 訳語ID
 * @return const std::string
 */
const std::string Word::GetTranslation(const int partID, const int translationID) const
{
    if (Translations_.count(partID) == 0)
    {
        return "";
    }
    if (Translations_.at(partID).second.count(translationID) == 0)
    {
        return "";
    }
    return Translations_.at(partID).second.at(translationID);
}

/**
 * @brief 品詞をセット
 *
 * @param partID 品詞ID
 * @param part 品詞
 */
void Word::SetPart(const int partID, const std::string &part)
{
    Translations_[partID].first = part;
}

/**
 * @brief 品詞を削除
 *
 * @param partID 品詞ID
 */
void Word::DeletePart(const int partID)
{
    Translations_.erase(partID);
}

/**
 * @brief 訳語をセット
 *
 * @param partID 品詞ID
 * @param translationID 訳語ID
 * @param translation 訳語
 */
void Word::SetTranslation(const int partID, const int translationID, const std::string &translation)
{
    if (Translations_.count(partID) == 1)
    {
        Translations_[partID].second[translationID] = translation;
    }
}

/**
 * @brief 訳語を削除
 *
 * @param partID 品詞ID
 * @param translationID 訳語ID
 */
void Word::DeleteTranslation(const int partID, const int translationID)
{
    if (Translations_.count(partID) == 1)
    {
        Translations_[partID].second.erase(translationID);
    }
}

/**
 * @brief タグを編集
 *
 * @param tagID タグID
 * @param tag タグ
 */
void Word::SetTag(const int tagID, const std::string &tag)
{
    Tags_[tagID] = tag;
}

/**
 * @brief タグを削除
 *
 * @param tagID タグID
 */
void Word::DeleteTag(const int tagID)
{
    Tags_.erase(tagID);
}

/**
 * @brief タグIDをゲット
 *
 * @return const std::vector<int>
 */
const std::vector<int> Word::GetTagIDs() const
{
    std::vector<int> result;
    for (const auto &[tagID, _] : Tags_)
    {
        result.emplace_back(tagID);
    }
    return result;
}

/**
 * @brief タグをゲット
 *
 * @param tagID タグID
 * @return const std::string
 */
const std::string Word::GetTag(const int tagID) const
{
    if (Tags_.count(tagID) == 0)
    {
        return "";
    }
    return Tags_.at(tagID);
}

/**
 * @brief 自由記述IDをゲット
 *
 * @return const std::vector<int>
 */
const std::vector<int> Word::GetContentIDs() const
{
    std::vector<int> result;
    for (const auto &[contentID, _] : Contents_)
    {
        result.emplace_back(contentID);
    }
    return result;
}

/**
 * @brief 自由記述タイトルをゲット
 *
 * @param contentID 自由記述ID
 * @return const std::vector
 */
const std::string Word::GetContentTitle(const int contentID) const
{
    if (Contents_.count(contentID) == 0)
    {
        return "";
    }
    return Contents_.at(contentID).first;
}

/**
 * @brief 自由記述をゲット
 *
 * @param contentID 自由記述ID
 * @return const std::vector
 */
const std::string Word::GetContent(const int contentID) const
{
    if (Contents_.count(contentID) == 0)
    {
        return "";
    }
    return Contents_.at(contentID).second;
}

/**
 * @brief 自由記述をセット
 *
 * @param contentID 自由記述ID
 * @param title タイトル
 * @param content 自由記述
 */
void Word::SetContent(const int contentID, const std::string &title, const std::string &content)
{
    Contents_[contentID] = {title, content};
}

/**
 * @brief 自由記述削除
 *
 * @param contentID 自由記述ID
 */
void Word::DeleteContent(const int contentID)
{
    Contents_.erase(contentID);
}

/**
 * @brief 変化形IDゲット
 *
 * @return const std::vector<int>
 */
const std::vector<int> Word::GetVariationIDs() const
{
    std::vector<int> result;
    for (const auto &[variationID, _] : Variations_)
    {
        result.emplace_back(variationID);
    }
    return result;
}

/**
 * @brief 変化形タイトルゲット
 *
 * @param contentID 変化形ID
 * @return const std::string
 */
const std::string Word::GetVariationTitle(const int variationID) const
{
    if (Variations_.count(variationID) == 0)
    {
        return "";
    }
    return Variations_.at(variationID).first;
}

/**
 * @brief 変化形ゲット
 *
 * @param contentID 変化形ID
 * @return const std::vector<Phoneme>
 */
const std::vector<int> Word::GetVariation(const int variationID) const
{
    if (Variations_.count(variationID) == 0)
    {
        return {};
    }
    return Variations_.at(variationID).second;
}

/**
 * @brief バリエーション（語形変化など）を設定・更新する
 * @param variationID バリエーションの識別ID
 * @param title バリエーションのタイトル（例: "過去形", "複数形"）
 * @param content 変化後の音素列
 */
void Word::SetVariation(const int variationID, const std::string &title, const std::vector<int> &content)
{
    Variations_[variationID] = std::make_pair(title, content);
}

/**
 * @brief バリエーションを削除する
 * @param variationID 削除対象のバリエーションID
 */
void Word::DeleteVariation(const int variationID)
{
    Variations_.erase(variationID);
}

/**
 * @brief 関連単語のIDリストを取得
 */
const std::vector<int> Word::GetRelationIDs() const
{
    std::vector<int> ids;
    for (const auto &[id, _] : Relations_)
    {
        ids.emplace_back(id);
    }
    return ids;
}

/**
 * @brief 関連単語のタイトル（ラベル）を取得
 */
const std::string Word::GetRelationTitle(const int relationID) const
{
    auto it = Relations_.find(relationID);
    if (it != Relations_.end())
    {
        return it->second.first;
    }
    return std::string();
}

/**
 * @brief 関連先の単語IDを取得
 */
const int Word::GetRelationWordID(const int relationID) const
{
    auto it = Relations_.find(relationID);
    if (it != Relations_.end())
    {
        return it->second.second;
    }
    return -1;
}

/**
 * @brief 関連語を設定（新規追加・更新）する
 * @param relationID 関連ID
 * @param title 関連のタイトル（ラベル）
 * @param targetWordID 参照先の単語ID
 */
void Word::SetRelation(const int relationID, const std::string &title, const int targetWordID)
{
    Relations_[relationID] = std::make_pair(title, targetWordID);
}

/**
 * @brief 関連語を削除する
 * @param relationID 削除対象の関連ID
 */
void Word::DeleteRelation(const int relationID)
{
    Relations_.erase(relationID);
}

/**
 * @brief 訳語の集合を取得
 *
 * @return const std::vector<std::string>
 */
const std::vector<std::string> Word::GetAllTranslations() const
{
    std::vector<std::string> result;
    for (const auto &[_, pair] : Translations_)
    {
        const auto &[__, translations] = pair;
        for (const auto &[___, translation] : translations)
        {
            result.emplace_back(translation);
        }
    }
    return result;
}

/**
 * @brief 音韻変化
 *
 * @param phon
 * @param phonemeTable
 */
void Word::ChangeSound(PhonologicalChange phon, const PhonemeTable &phonemeTable)
{
    std::vector<int> changedWordForm;
    if (!ChangeWordSound(Form_, changedWordForm, phon, phonemeTable))
        return;

    Form_ = changedWordForm;
}