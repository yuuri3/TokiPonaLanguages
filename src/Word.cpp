#include "Language.h"
#include "PhonologicalChange.h"
#include "PhonemeConverter.h"

namespace
{
    /**
     * @brief 音韻の制限
     *
     * @param changedWordForm
     * @return true
     * @return false
     */
    bool CheckSoundDuplication(const std::vector<Phoneme> &changedWordForm)
    {
        // 子音と母音の境界
        constexpr int MAX_CONSONANT_MANNER = 3;

        bool isSoundDuplication = false;

        std::vector<std::vector<Phoneme>> wordForms;
        std::vector<Phoneme> wordForm;
        for (const auto &phoneme : changedWordForm)
        {
            if (phoneme.IsSpace())
            {
                wordForms.emplace_back(wordForm);
                wordForm.clear();
            }
            else
            {
                wordForm.emplace_back(phoneme);
            }
        }
        wordForms.emplace_back(wordForm);

        for (const auto &w : wordForms)
        {
            if (w.empty())
                isSoundDuplication = true;
            else if (w.size() == 1)
            {
                if (w[0].IsConsonant())
                    isSoundDuplication = true;
            }
            else
            {
                // 境界条件のチェック
                if ((w[0].IsConsonant() && w[1].IsConsonant()) ||
                    (w.back().IsConsonant() && w[w.size() - 2].IsConsonant()))
                {
                    isSoundDuplication = true;
                }
                else
                {
                    // 3連続のチェック
                    for (size_t j = 0; j + 2 < w.size(); ++j)
                    {
                        bool isConsonant = (w[j].IsConsonant() &&
                                            w[j + 1].IsConsonant() &&
                                            w[j + 2].IsConsonant());
                        bool isVowel = (w[j].IsVowel() &&
                                        w[j + 1].IsVowel() &&
                                        w[j + 2].IsVowel());
                        if (isConsonant || isVowel)
                        {
                            isSoundDuplication = true;
                            break;
                        }
                    }
                }
            }
        }
        return isSoundDuplication;
    }

    /**
     * @brief 音韻変化を適用する
     *
     * @param wordForm 語形
     * @param changedWordForm 変化語の語形
     * @param phonologicalChange 音韻変化
     */
    bool ChangeWordSound(const std::vector<Phoneme> &wordForm, std::vector<Phoneme> &changedWordForm, const PhonologicalChange &phonologicalChange)
    {
        changedWordForm.reserve(wordForm.size());
        bool isChanged = false;

        for (size_t soundPosition = 0; soundPosition < wordForm.size(); ++soundPosition)
        {
            const auto &sound = wordForm[soundPosition];

            // 変化条件の判定
            bool isSoundEqualToBeforePhoneme = (sound == phonologicalChange.BeforePhoneme_);
            if (isSoundEqualToBeforePhoneme)
            {
                if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::Start && !(soundPosition == 0 || wordForm[soundPosition - 1].IsSpace()))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::End && !(soundPosition == wordForm.size() - 1 || wordForm[soundPosition + 1].IsSpace()))
                    isSoundEqualToBeforePhoneme = false;
                else if (phonologicalChange.PhoneticEnvironment_ == PhoneticEnvironment::Middle && (soundPosition == 0 || wordForm[soundPosition - 1].IsSpace() || soundPosition == wordForm.size() - 1 || wordForm[soundPosition + 1].IsSpace()))
                    isSoundEqualToBeforePhoneme = false;
            }

            if (isSoundEqualToBeforePhoneme)
            {
                isChanged = true;
                if (!phonologicalChange.IsRemove_)
                {
                    changedWordForm.push_back(phonologicalChange.AfterPhoneme_);
                }
            }
            else
            {
                changedWordForm.push_back(sound);
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
Word Word::Create(const std::vector<Phoneme> &form)
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
Word Word::CreateFromJsonObject(const QJsonObject &obj, const PhonemeConverter &converter)
{
    Word word;

    // 1. entry (ID, form)
    QJsonObject entryObj = obj["entry"].toObject();
    word.ID = entryObj["id"].toInt();

    // form 文字列を std::vector<Phoneme> に変換
    // ※プロジェクト内の既存の変換関数（PhonemeConverterなど）を利用
    QString formStr = entryObj["form"].toString();
    word.Form_ = converter.ConvertToPhoneme(formStr.toStdString());

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
    for (const auto &varValue : variationsArray)
    {
        QJsonObject varObj = varValue.toObject();
        std::string title = varObj["title"].toString().toStdString();
        std::string form = varObj["form"].toString().toStdString();
        word.Variations_[title] = converter.ConvertToPhoneme(form);
    }

    // 6. relations (title -> entry ID)
    QJsonArray relationsArray = obj["relations"].toArray();
    for (const auto &relValue : relationsArray)
    {
        QJsonObject relObj = relValue.toObject();
        std::string title = relObj["title"].toString().toStdString();
        int entryId = relObj["entry"].toObject()["id"].toInt();
        word.Relations_[title] = entryId;
    }

    return word;
}

/**
 * @brief 語形をゲット
 *
 * @return const std::vector<Phoneme>
 */
const std::vector<Phoneme> Word::GetForm() const
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
 * @brief 変化形をゲット
 *
 * @return const std::map<std::string, std::vector<Phoneme>>
 */
const std::map<std::string, std::vector<Phoneme>> Word::GetVariations() const
{
    return Variations_;
}

/**
 * @brief 関連語をゲット
 *
 * @return const std::map<std::string, int>
 */
const std::map<std::string, int> Word::GetRealtions() const
{
    return Relations_;
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
 */
void Word::ChangeSound(PhonologicalChange phon, const bool isProhibitSoundDuplication)
{
    std::vector<Phoneme> changedWordForm;
    if (!ChangeWordSound(Form_, changedWordForm, phon))
        return;

    // 子音・母音の重複禁止チェック (isSoundDuplication)
    if (isProhibitSoundDuplication)
    {
        if (CheckSoundDuplication(changedWordForm))
            return; // 違反していればこの単語の変化は破棄
    }
    Form_ = changedWordForm;
}