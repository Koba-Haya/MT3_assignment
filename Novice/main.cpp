#include <Novice.h>
#include <math.h>

const char kWindowTitle[] = "LE2B_10_コバヤシ_ハヤト_MT3_00_01";

// 三次元構造体
struct Vector3 {
	float x, y, z;
};

/// <summary>
/// 加算
/// </summary>
/// <param name="v1">加算するベクトル１</param>
/// <param name="v2">加算するベクトル２</param>
/// <returns>加算合計ベクトル</returns>
Vector3 Add(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 減算
/// </summary>
/// <param name="v1">減算するベクトル１</param>
/// <param name="v2">減算するベクトル２</param>
/// <returns>減算合計ベクトル</returns>
Vector3 Subtract(const Vector3& v1, const Vector3& v2);

/// <summary>
/// スカラー倍
/// </summary>
/// <param name="scalar">スカラー</param>
/// <param name="v">ベクトル</param>
/// <returns>合計された値</returns>
Vector3 Multiply(float scalar, const Vector3& v);

/// <summary>
/// 内積
/// </summary>
/// <param name="v1">計算されるベクトル１</param>
/// <param name="v2">計算されるベクトル２</param>
/// <returns>合計値</returns>
float Dot(const Vector3& v1, const Vector3& v2);

/// <summary>
/// 長さ（ノルム）
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>長さ</returns>
float Length(const Vector3& v);

/// <summary>
/// 正規化
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>正規化された値</returns>
Vector3 Normalize(const Vector3& v);

static const int kColummnWidth = 60;

/// <summary>
/// 描画関数
/// </summary>
/// <param name="x">X座標</param>
/// <param name="y">Y座標</param>
/// <param name="vector">描画する計算された値</param>
/// <param name="label">計算の種類</param>
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColummnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColummnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColummnWidth * 3, y, "%s", label);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 v1{1.0f, 3.0f, -5.0f};
	Vector3 v2{4.0f, -1.0f, 2.0f};
	float k = {4.0f};
	int kRowHeight = 20;

	Vector3 resultAdd = Add(v1, v2);
	Vector3 resultSubtract = Subtract(v1, v2);
	Vector3 resultMultiply = Multiply(k, v1);
	float resultDot = Dot(v1, v2);
	float resultLength = Length(v1);
	Vector3 resultNormalize = Normalize(v2);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, resultAdd, " : Add");
		VectorScreenPrintf(0, kRowHeight, resultSubtract, " : Subtract");
		VectorScreenPrintf(0, kRowHeight * 2, resultMultiply, " : Multiply");
		Novice::ScreenPrintf(0, kRowHeight * 3, "%.02f : Dot", resultDot);
		Novice::ScreenPrintf(0, kRowHeight * 4, "%.02f : Length", resultLength);
		VectorScreenPrintf(0, kRowHeight * 5, resultNormalize, " : Normalize");

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;
	return result;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;
	return result;
}

Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 result;
	result = {scalar * v.x, scalar * v.y, scalar * v.z};
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}

float Length(const Vector3& v) {
	float result;
	result = {sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2))};
	return result;
}

Vector3 Normalize(const Vector3& v) {
	Vector3 result;
	result = {v.x / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)), v.y / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2)), v.z / sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2))};
	return result;
}
