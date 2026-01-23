#include <vector>
#include <string>

/**
 * CSVファイルを読み込んで2次元ベクトルで返す関数
 */
std::vector<std::vector<std::string>> readCSV(const std::string &filename);

/**
 * 2次元ベクトルをCSVファイルに出力する関数
 * @return 保存に成功したらtrue、失敗したらfalse
 */
bool writeCSV(const std::string &filename, const std::vector<std::vector<std::string>> &data);

/**
 * 2次元データから隣接関係（横・縦）を抽出する関数
 * @param data 2次元文字列ベクトル
 * @return 隣接する要素のペアのリスト
 */
std::vector<std::pair<std::string, std::string>> getAdjacencies(const std::vector<std::vector<std::string>> &data);

/**
 * 2次元データから空ではない文字列のみを抽出して1次元のリストにする
 * @param data 2次元文字列ベクトル
 */
std::vector<std::string> getNonEmptyStrings(const std::vector<std::vector<std::string>> &data);