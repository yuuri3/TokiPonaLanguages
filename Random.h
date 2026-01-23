#include <vector>
#include <string>

/**
 * @brief 指定した範囲 [min, max] の整数をランダムに生成する。
 * @param min 生成する乱数の下限値
 * @param max 生成する乱数の上限値
 * @return 生成された整数
 */
int getRandomInt(int min, int max);

/**
 * @brief 指定した範囲 [min, max] の実数をランダムに生成する。
 * @param min 生成する乱数の下限値
 * @param max 生成する乱数の上限値
 * @return 生成された実数
 */
double getRandomDouble(double min, double max);

/**
 * @brief 確率 p で true を返す。
 * @param p true になる確率 (0.0 <= p <= 1.0)
 * @return true または false
 */
bool getWithProbability(double p);

/**
 * @brief 座標 A, B をランダムに ±1 移動させ、有効なセル（空文字でない）が見つかるまで同方向に進む。
 * @param A 現在の行インデックス（参照渡し、更新される）
 * @param B 現在の列インデックス（参照渡し、更新される）
 * @param table 探索対象の2次元テーブル
 */
void moveRandomOnTable(int &A, int &B, const std::vector<std::vector<std::string>> &table);