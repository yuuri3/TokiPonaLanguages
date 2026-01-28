#include "Random.h"
#include <random>
#include <algorithm>

namespace
{
    static std::random_device rd;
    static std::mt19937 gen(rd() + std::rand());
}

int getRandomInt(int min, int max)
{
    if (min == max)
    {
        return min;
    }

    // [min, max] の範囲で一様分布させる設定
    std::uniform_int_distribution<int> dist(min, max);

    auto hoge = dist(gen);
    return hoge;
}

double getRandomDouble(double min, double max)
{

    // [min, max] の範囲で一様分布させる設定
    std::uniform_real_distribution<double> dist(min, max);

    return dist(gen);
}

bool getWithProbability(double p)
{
    // pが範囲外の場合のガード
    if (p <= 0.0)
        return false;
    if (p >= 1.0)
        return true;

    // ベルヌーイ分布オブジェクトの作成
    std::bernoulli_distribution dist(p);

    return dist(gen);
}

void moveRandomOnTable(int &A, int &B, const std::vector<std::vector<std::string>> &table)
{
    if (table.empty())
        return;

    // 1. 移動方向の定義 {dA, dB}
    // 上(-1, 0), 下(1, 0), 左(0, -1), 右(0, 1)
    std::vector<std::pair<int, int>> directions = {
        {-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    // 2. 方向をランダムにシャッフルする
    std::shuffle(directions.begin(), directions.end(), gen);

    // 3. 各方向について試行
    for (const auto &dir : directions)
    {
        int nextA = A + dir.first;
        int nextB = B + dir.second;

        // 指定方向に突き進むループ
        while (true)
        {
            // 範囲外チェック
            if (nextA < 0 || nextA >= (int)table.size() ||
                nextB < 0 || nextB >= (int)table[nextA].size())
            {
                break; // この方向はこれ以上進めない（次の方向へ）
            }

            // 空文字チェック
            if (table[nextA][nextB] != "")
            {
                // 有効なセルが見つかったら A, B を更新して終了
                A = nextA;
                B = nextB;
                return;
            }

            // table[nextA][nextB] == "" の場合は、同じ方向にさらに進む
            nextA += dir.first;
            nextB += dir.second;
        }
    }

    // どの方向にも有効なセルが見つからなかった場合、A, B は変更されない
}