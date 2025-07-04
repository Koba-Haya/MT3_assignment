#include <Novice.h>
#include <assert.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include "Vector3.h"
#include <algorithm>
#include <imgui.h>
#include <math.h>

// vLeLs = vWe 動く床などに疑似的に親子関係を結ぶ
// vLhLeLs = vWh

const char kWindowTitle[] = "LE2B_10_コバヤシ_ハヤト_MT3_03_02";

struct Vector3 {
	float x; // X座標
	float y; // Y座標
	float z; // Z座標
};

struct Matrix4x4 {
	float m[4][4]; // 4x4行列
};

struct Sphere {
	Vector3 center; // 中心点
	float radius;   // 半径
};

struct Plane {
	Vector3 normal; // 法線
	float distance; // 距離
};

struct Segment {
	Vector3 origin; // 原点
	Vector3 diff;   // 終点への差分ベクトル
};

struct Triangle {
	Vector3 vertices[3]; // 原点
};

struct AABB {
	Vector3 min; // 最小点
	Vector3 max; // 最大点
};

struct OBB {
	Vector3 center;          // 中心点
	Vector3 orientations[3]; // 座標軸。正規化・直行必要
	Vector3 size;            // 中心点から面までの距離
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

Vector3 Multiply(float& s, const Vector3& v);

/// <summary>
/// 透視投影行列
/// </summary>
/// <param name="fovY">画角Y</param>
/// <param name="aspectRatio">アスペクト比</param>
/// <param name="nearClip">近平面への距離</param>
/// <param name="farClip">遠平面への距離</param>
/// <returns>切り取る範囲</returns>
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

/// <summary>
/// ビューポート変換行列
/// </summary>
/// <param name="left">左座標</param>
/// <param name="top">上座標</param>
/// <param name="width">幅</param>
/// <param name="height">高さ</param>
/// <param name="minDepth">最小深度値</param>
/// <param name="maxDepth">最大深度値</param>
/// <returns>スクリーン座標系</returns>
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

/// <summary>
/// 3次元アフィン変換行列
/// </summary>
/// <param name="scale">拡縮</param>
/// <param name="rotate">回転</param>
/// <param name="translate">移動</param>
/// <returns>変換結果</returns>
Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate);

/// <summary>
/// 変換行列
/// </summary>
/// <param name="vector">次元</param>
/// <param name="matrix">行列</param>
/// <returns>変換後行列</returns>
Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix);

/// <summary>
/// 逆行列
/// </summary>
/// <param name="m">変換される行列</param>
/// <returns>変換結果</returns>
Matrix4x4 Inverse(const Matrix4x4& m);

/// <summary>
/// 行列の和
/// </summary>
/// <param name="m1">計算される行列1</param>
/// <param name="m2">計算される行列2</param>
/// <returns>計算結果</returns>
Matrix4x4 MatrixAdd(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 行列の差
/// </summary>
/// <param name="m1">計算される行列1</param>
/// <param name="m2">計算される行列2</param>
/// <returns>計算結果</returns>
Matrix4x4 MatrixSubtract(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// 行列の積
/// </summary>
/// <param name="m1">計算される行列1</param>
/// <param name="m2">計算される行列2</param>
/// <returns>計算結果</returns>
Matrix4x4 MatrixMultiply(const Matrix4x4& m1, const Matrix4x4& m2);

/// <summary>
/// グリッド描画関数
/// </summary>
/// <param name="viewProjectionMatrix">ビュー・射影行列</param>
/// <param name="viewportMatrix">ビューポート変換行列</param>
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

/// <summary>
/// スフィア描画関数
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="radius">半径</param>
/// <param name="viewProjectionMatrix">ビュー・射影行列</param>
/// <param name="viewportMatrix">ビューポート変換行列</param>
/// <param name="color">色</param>
void DrawSegment(const Vector3& origin, const Vector3& diff, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

/// <summary>
/// 長さ（ノルム）
/// </summary>
/// <param name="v">ベクトル</param>
/// <returns>長さ</returns>
float Length(const Vector3& v);

/// <summary>
/// 内積
/// </summary>
/// <param name="v1">計算されるベクトル１</param>
/// <param name="v2">計算されるベクトル２</param>
/// <returns>合計値</returns>
float Dot(const Vector3& v1, const Vector3& v2);

// 球とOBBの当たり判定関数
bool IsCollision(const AABB& aabb, const Segment& segment);

bool IsCollisionOBBLine(const OBB& obb, const Matrix4x4& obbWorldMatrix, const Segment& worldsegment);

Vector3 Perpendicular(const Vector3& vector);

Vector3 Normalize(const Vector3& v);

Vector3 Cross(const Vector3& v1, const Vector3& v2) { return {v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x}; }

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void DrawAABB(const AABB& aabb, const Matrix4x4& viewprojectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

/// <summary>
/// スフィア描画関数
/// </summary>
/// <param name="center">中心座標</param>
/// <param name="radius">半径</param>
/// <param name="viewProjectionMatrix">ビュー・射影行列</param>
/// <param name="viewportMatrix">ビューポート変換行列</param>
/// <param name="color">色</param>
void DrawSphere(const Vector3& center, float radius, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

//=== OBB描画関数 ===//
void DrawOBB(const Vector3& size, const Matrix4x4& worldMatrix, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

void UpdateCamera(Vector3& cameraTranslate, Vector3& cameraRotate, const char* keys);

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

/*------------------２項演算子----------------------*/
Vector3& operator+(const Vector3& v1, const Vector3& v2) {
	Vector3 result = Add(v1, v2);
	return result;
}

Vector3& operator-(const Vector3& v1, const Vector3& v2) {
	Vector3 result = Subtract(v1, v2);
	return result;
}

Vector3& operator*(float& s, const Vector3& v) {
	Vector3 result = Multiply(s, v);
	return result;
}

Vector3& operator*(const Vector3& v, float& s) { return s * v; }

Vector3& operator/(const Vector3& v, float& s) {
	float num = 1.0f / s;
	Vector3 result = Multiply(num, v);
	return result;
}

Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = MatrixAdd(m1, m2);
	return result;
}

Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = MatrixSubtract(m1, m2);
	return result;
}

Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = MatrixMultiply(m1, m2);
	return result;
}
/*-------------------------------------------------------*/

/*-------------------------単項演算子-------------------------------*/
Vector3 operator-(const Vector3& v) { return {-v.x, -v.y, -v.z}; }

Vector3 operator+(const Vector3& v) { return v; }
/*----------------------------------------------------------------*/

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate = {0.0f, 1.9f, -6.49f};
	Vector3 cameraRotate = {0.26f, 0.0f, 0.0f};

	// uint32_t color = BLUE;

	Vector3 a = {0.2f, 1.0f, 0.0f};
	Vector3 b = {2.4f, 3.1f, 1.2f};
	Vector3 c = a + b;
	Vector3 d = a - b;
	Vector3 e = a * 2.4f;
	Vector3 rotate{0.4f, 1.43f, -0.8f};
	Matrix4x4 rotateXMatrix = ;

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

		// ImGui操作
		ImGui::Begin("Window");
		ImGui::DragFloat3("translate[0]", &translate[0].x, 0.01f);
		ImGui::DragFloat3("rotate[0]", &rotate[0].x, 0.01f);
		ImGui::DragFloat3("scale[0]", &scale[0].x, 0.01f);
		ImGui::DragFloat3("translate[1]", &translate[1].x, 0.01f);
		ImGui::DragFloat3("rotate[1]", &rotate[1].x, 0.01f);
		ImGui::DragFloat3("scale[1]", &scale[1].x, 0.01f);
		ImGui::DragFloat3("translate[2]", &translate[2].x, 0.01f);
		ImGui::DragFloat3("rotate[2]", &rotate[2].x, 0.01f);
		ImGui::DragFloat3("scale[2]", &scale[2].x, 0.01f);
		ImGui::End();

		UpdateCamera(cameraTranslate, cameraRotate, keys);

		// 各種行列計算
		Matrix4x4 cameraMatrix = MakeAffineMatrix({1.0f, 1.0f, 1.0f}, {cameraRotate}, {cameraTranslate});
		Matrix4x4 viewMatrix = Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
		Matrix4x4 viewProjectionMatrix = MatrixMultiply(viewMatrix, projectionMatrix);
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280, 720, 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(viewProjectionMatrix, viewportMatrix);

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

Vector3 Multiply(float& s, const Vector3& v) {
	Vector3 result = {v.x * s, v.y * s, v.z * s};
	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result;
	result = {(1.0f / aspectRatio) * (1.0f / std::tanf(fovY / 2.0f)), 0, 0, 0, 0, (1.0f / std::tanf(fovY / 2.0f)), 0, 0, 0, 0, farClip / (farClip - nearClip), 1, 0, 0,
	          (-nearClip * farClip) / (farClip - nearClip),           0};
	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;
	result = {width / 2.0f, 0, 0, 0, 0, -height / 2.0f, 0, 0, 0, 0, maxDepth - minDepth, 0, left + (width / 2.0f), top + (height / 2.0f), minDepth, 1};
	return result;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result;
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	float determinant;
	determinant =
	    m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] - m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] + m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];
	Matrix4x4 result;
	result = {
	    (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] + m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] - m.m[1][2] * m.m[2][1] * m.m[3][3] -
	     m.m[1][1] * m.m[2][3] * m.m[3][2]) /
	        determinant,
	    (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] - m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] + m.m[0][2] * m.m[2][1] * m.m[3][3] +
	     m.m[0][1] * m.m[2][3] * m.m[3][2]) /
	        determinant,
	    (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] + m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] - m.m[0][2] * m.m[1][1] * m.m[3][3] -
	     m.m[0][1] * m.m[1][3] * m.m[3][2]) /
	        determinant,
	    (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] - m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] + m.m[0][2] * m.m[1][1] * m.m[2][3] +
	     m.m[0][1] * m.m[1][3] * m.m[2][2]) /
	        determinant,
	    (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] - m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] + m.m[1][2] * m.m[2][0] * m.m[3][3] +
	     m.m[1][0] * m.m[2][3] * m.m[3][2]) /
	        determinant,
	    (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] + m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] - m.m[0][2] * m.m[2][0] * m.m[3][3] -
	     m.m[0][0] * m.m[2][3] * m.m[3][2]) /
	        determinant,
	    (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] - m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] + m.m[0][2] * m.m[1][0] * m.m[3][3] +
	     m.m[0][0] * m.m[1][3] * m.m[3][2]) /
	        determinant,
	    (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] + m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] - m.m[0][2] * m.m[1][0] * m.m[2][3] -
	     m.m[0][0] * m.m[1][3] * m.m[2][2]) /
	        determinant,
	    (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] + m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] - m.m[1][1] * m.m[2][0] * m.m[3][3] -
	     m.m[1][0] * m.m[2][3] * m.m[3][1]) /
	        determinant,
	    (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] - m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] + m.m[0][1] * m.m[2][0] * m.m[3][3] +
	     m.m[0][0] * m.m[2][3] * m.m[3][1]) /
	        determinant,
	    (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] + m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] - m.m[0][1] * m.m[1][0] * m.m[3][3] -
	     m.m[0][0] * m.m[1][3] * m.m[3][1]) /
	        determinant,
	    (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] - m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] + m.m[0][1] * m.m[1][0] * m.m[2][3] +
	     m.m[0][0] * m.m[1][3] * m.m[2][1]) /
	        determinant,
	    (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] - m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] + m.m[1][1] * m.m[2][0] * m.m[3][2] +
	     m.m[1][0] * m.m[2][2] * m.m[3][1]) /
	        determinant,
	    (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] + m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] - m.m[0][1] * m.m[2][0] * m.m[3][2] -
	     m.m[0][0] * m.m[2][2] * m.m[3][1]) /
	        determinant,
	    (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] - m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] + m.m[0][1] * m.m[1][0] * m.m[3][2] +
	     m.m[0][0] * m.m[1][2] * m.m[3][1]) /
	        determinant,
	    (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] + m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] - m.m[0][1] * m.m[1][0] * m.m[2][2] -
	     m.m[0][0] * m.m[1][2] * m.m[2][1]) /
	        determinant};
	return result;
}

Matrix4x4 MatrixAdd(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	result.m[0][0] = m1.m[0][0] + m2.m[0][0];
	result.m[0][1] = m1.m[0][1] + m2.m[0][1];
	result.m[0][2] = m1.m[0][2] + m2.m[0][2];
	result.m[0][3] = m1.m[0][3] + m2.m[0][3];
	result.m[1][0] = m1.m[0][0] + m2.m[0][0];
	result.m[1][1] = m1.m[0][1] + m2.m[0][1];
	result.m[1][2] = m1.m[0][2] + m2.m[0][2];
	result.m[1][3] = m1.m[0][3] + m2.m[0][3];
	result.m[2][0] = m1.m[0][0] + m2.m[0][0];
	result.m[2][1] = m1.m[0][1] + m2.m[0][1];
	result.m[2][2] = m1.m[0][2] + m2.m[0][2];
	result.m[2][3] = m1.m[0][3] + m2.m[0][3];
	result.m[3][0] = m1.m[0][0] + m2.m[0][0];
	result.m[3][1] = m1.m[0][1] + m2.m[0][1];
	result.m[3][2] = m1.m[0][2] + m2.m[0][2];
	result.m[3][3] = m1.m[0][3] + m2.m[0][3];
	return result;
}

Matrix4x4 MatrixSubtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	result.m[0][0] = m1.m[0][0] - m2.m[0][0];
	result.m[0][1] = m1.m[0][1] - m2.m[0][1];
	result.m[0][2] = m1.m[0][2] - m2.m[0][2];
	result.m[0][3] = m1.m[0][3] - m2.m[0][3];
	result.m[1][0] = m1.m[0][0] - m2.m[0][0];
	result.m[1][1] = m1.m[0][1] - m2.m[0][1];
	result.m[1][2] = m1.m[0][2] - m2.m[0][2];
	result.m[1][3] = m1.m[0][3] - m2.m[0][3];
	result.m[2][0] = m1.m[0][0] - m2.m[0][0];
	result.m[2][1] = m1.m[0][1] - m2.m[0][1];
	result.m[2][2] = m1.m[0][2] - m2.m[0][2];
	result.m[2][3] = m1.m[0][3] - m2.m[0][3];
	result.m[3][0] = m1.m[0][0] - m2.m[0][0];
	result.m[3][1] = m1.m[0][1] - m2.m[0][1];
	result.m[3][2] = m1.m[0][2] - m2.m[0][2];
	result.m[3][3] = m1.m[0][3] - m2.m[0][3];
	return result;
}

Matrix4x4 MatrixMultiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	result.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0];
	result.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1];
	result.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2];
	result.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3];
	result.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0];
	result.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1];
	result.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2];
	result.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3];
	result.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0];
	result.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1];
	result.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2];
	result.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3];
	result.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0];
	result.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1];
	result.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2];
	result.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3];
	return result;
}

Matrix4x4 MakeAffineMatrix(Vector3 scale, Vector3 rotate, Vector3 translate) {
	//====================
	// 拡縮の行列の作成
	//====================
	Matrix4x4 scaleMatrix4x4;
	scaleMatrix4x4.m[0][0] = scale.x;
	scaleMatrix4x4.m[0][1] = 0.0f;
	scaleMatrix4x4.m[0][2] = 0.0f;
	scaleMatrix4x4.m[0][3] = 0.0f;

	scaleMatrix4x4.m[1][0] = 0.0f;
	scaleMatrix4x4.m[1][1] = scale.y;
	scaleMatrix4x4.m[1][2] = 0.0f;
	scaleMatrix4x4.m[1][3] = 0.0f;

	scaleMatrix4x4.m[2][0] = 0.0f;
	scaleMatrix4x4.m[2][1] = 0.0f;
	scaleMatrix4x4.m[2][2] = scale.z;
	scaleMatrix4x4.m[2][3] = 0.0f;

	scaleMatrix4x4.m[3][0] = 0.0f;
	scaleMatrix4x4.m[3][1] = 0.0f;
	scaleMatrix4x4.m[3][2] = 0.0f;
	scaleMatrix4x4.m[3][3] = 1.0f;

	//===================
	// 回転の行列の作成
	//===================
	// Xの回転行列
	Matrix4x4 rotateMatrixX;
	rotateMatrixX.m[0][0] = 1.0f;
	rotateMatrixX.m[0][1] = 0.0f;
	rotateMatrixX.m[0][2] = 0.0f;
	rotateMatrixX.m[0][3] = 0.0f;

	rotateMatrixX.m[1][0] = 0.0f;
	rotateMatrixX.m[1][1] = cosf(rotate.x);
	rotateMatrixX.m[1][2] = sinf(rotate.x);
	rotateMatrixX.m[1][3] = 0.0f;

	rotateMatrixX.m[2][0] = 0.0f;
	rotateMatrixX.m[2][1] = -sinf(rotate.x);
	rotateMatrixX.m[2][2] = cosf(rotate.x);
	rotateMatrixX.m[2][3] = 0.0f;

	rotateMatrixX.m[3][0] = 0.0f;
	rotateMatrixX.m[3][1] = 0.0f;
	rotateMatrixX.m[3][2] = 0.0f;
	rotateMatrixX.m[3][3] = 1.0f;

	// Yの回転行列
	Matrix4x4 rotateMatrixY;
	rotateMatrixY.m[0][0] = cosf(rotate.y);
	rotateMatrixY.m[0][1] = 0.0f;
	rotateMatrixY.m[0][2] = -sinf(rotate.y);
	rotateMatrixY.m[0][3] = 0.0f;

	rotateMatrixY.m[1][0] = 0.0f;
	rotateMatrixY.m[1][1] = 1.0f;
	rotateMatrixY.m[1][2] = 0.0f;
	rotateMatrixY.m[1][3] = 0.0f;

	rotateMatrixY.m[2][0] = sinf(rotate.y);
	rotateMatrixY.m[2][1] = 0.0f;
	rotateMatrixY.m[2][2] = cosf(rotate.y);
	rotateMatrixY.m[2][3] = 0.0f;

	rotateMatrixY.m[3][0] = 0.0f;
	rotateMatrixY.m[3][1] = 0.0f;
	rotateMatrixY.m[3][2] = 0.0f;
	rotateMatrixY.m[3][3] = 1.0f;

	// Zの回転行列
	Matrix4x4 rotateMatrixZ;
	rotateMatrixZ.m[0][0] = cosf(rotate.z);
	rotateMatrixZ.m[0][1] = sinf(rotate.z);
	rotateMatrixZ.m[0][2] = 0.0f;
	rotateMatrixZ.m[0][3] = 0.0f;

	rotateMatrixZ.m[1][0] = -sinf(rotate.z);
	rotateMatrixZ.m[1][1] = cosf(rotate.z);
	rotateMatrixZ.m[1][2] = 0.0f;
	rotateMatrixZ.m[1][3] = 0.0f;

	rotateMatrixZ.m[2][0] = 0.0f;
	rotateMatrixZ.m[2][1] = 0.0f;
	rotateMatrixZ.m[2][2] = 1.0f;
	rotateMatrixZ.m[2][3] = 0.0f;

	rotateMatrixZ.m[3][0] = 0.0f;
	rotateMatrixZ.m[3][1] = 0.0f;
	rotateMatrixZ.m[3][2] = 0.0f;
	rotateMatrixZ.m[3][3] = 1.0f;

	// 回転行列の作成
	Matrix4x4 rotateMatrix4x4;

	rotateMatrix4x4 = MatrixMultiply(rotateMatrixX, MatrixMultiply(rotateMatrixY, rotateMatrixZ));

	//==================
	// 移動の行列の作成
	//==================
	Matrix4x4 translateMatrix4x4;
	translateMatrix4x4.m[0][0] = 1.0f;
	translateMatrix4x4.m[0][1] = 0.0f;
	translateMatrix4x4.m[0][2] = 0.0f;
	translateMatrix4x4.m[0][3] = 0.0f;

	translateMatrix4x4.m[1][0] = 0.0f;
	translateMatrix4x4.m[1][1] = 1.0f;
	translateMatrix4x4.m[1][2] = 0.0f;
	translateMatrix4x4.m[1][3] = 0.0f;

	translateMatrix4x4.m[2][0] = 0.0f;
	translateMatrix4x4.m[2][1] = 0.0f;
	translateMatrix4x4.m[2][2] = 1.0f;
	translateMatrix4x4.m[2][3] = 0.0f;

	translateMatrix4x4.m[3][0] = translate.x;
	translateMatrix4x4.m[3][1] = translate.y;
	translateMatrix4x4.m[3][2] = translate.z;
	translateMatrix4x4.m[3][3] = 1.0f;

	//====================
	// アフィン行列の作成
	//====================
	// 上で作った行列からアフィン行列を作る
	// アフィン行列の作成（スケール→回転→移動の順）
	Matrix4x4 affineMatrix4x4;
	affineMatrix4x4 = MatrixMultiply(scaleMatrix4x4, MatrixMultiply(rotateMatrix4x4, translateMatrix4x4));

	return affineMatrix4x4;
}

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / static_cast<float>(kSubdivision);

	for (uint32_t i = 0; i <= kSubdivision; ++i) {
		float offset = -kGridHalfWidth + i * kGridEvery;

		// 色を決定（中央線だけ黒、それ以外は灰色）
		uint32_t color = (offset == 0.0f) ? 0x000000FF : 0xAAAAAAFF;

		// Z方向（X軸に平行）
		Vector3 start = {-kGridHalfWidth, 0.0f, offset};
		Vector3 end = {kGridHalfWidth, 0.0f, offset};
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);

		// X方向（Z軸に平行）
		start = {offset, 0.0f, -kGridHalfWidth};
		end = {offset, 0.0f, kGridHalfWidth};
		start = Transform(Transform(start, viewProjectionMatrix), viewportMatrix);
		end = Transform(Transform(end, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
	}
}

void DrawSegment(const Vector3& origin, const Vector3& diff, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 start = Transform(Transform(origin, viewProjectionMatrix), viewportMatrix);
	Vector3 end = Transform(Transform(Add(origin, diff), viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(static_cast<int>(start.x), static_cast<int>(start.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
}

float Length(const Vector3& v) {
	float result;
	result = {sqrtf(powf(v.x, 2) + powf(v.y, 2) + powf(v.z, 2))};
	return result;
}

float Dot(const Vector3& v1, const Vector3& v2) {
	float result;
	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	return result;
}

bool IsCollision(const AABB& aabb, const Segment& segment) {
	float tMin = 0.0f;
	float tMax = 1.0f;

	Vector3 p = segment.origin;
	Vector3 d = segment.diff;

	for (int i = 0; i < 3; ++i) {
		float start = (&p.x)[i];
		float direction = (&d.x)[i];
		float minVal = (&aabb.min.x)[i];
		float maxVal = (&aabb.max.x)[i];

		if (fabsf(direction) < 1e-6f) {
			// 平行な場合：内側にあるか
			if (start < minVal || start > maxVal) {
				return false;
			}
		} else {
			float t1 = (minVal - start) / direction;
			float t2 = (maxVal - start) / direction;
			if (t1 > t2)
				std::swap(t1, t2);
			tMin = (std::max)(tMin, t1);
			tMax = (std::min)(tMax, t2);
			if (tMin > tMax)
				return false;
		}
	}
	return true;
}

bool IsCollisionOBBLine(const OBB& obb, const Matrix4x4& obbWorldMatrix, const Segment& worldsegment) {
	Matrix4x4 obbInverse = Inverse(obbWorldMatrix);

	Vector3 localOrigin = Transform(worldsegment.origin, obbInverse);
	Vector3 localEnd = Transform(Add(worldsegment.origin, worldsegment.diff), obbInverse);

	Segment localLine;
	localLine.origin = localOrigin;
	localLine.diff = Subtract(localEnd, localOrigin);

	AABB localAABB = {
	    {-obb.size.x, -obb.size.y, -obb.size.z},
        {+obb.size.x, +obb.size.y, +obb.size.z}
    };

	return IsCollision(localAABB, localLine);
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return {-vector.y, vector.x, 0.0f};
	}
	return {0.0f, -vector.z, vector.y};
}

Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	return {v.x / len, v.y / len, v.z / len};
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 平面の中心点（法線方向に distance だけ離れた位置）
	Vector3 center = {plane.normal.x * plane.distance, plane.normal.y * plane.distance, plane.normal.z * plane.distance};

	// 平面と垂直な2つのベクトルを作る
	Vector3 tangent = Normalize(Perpendicular(plane.normal));
	Vector3 bitangent = Normalize(Cross(plane.normal, tangent));

	// 大きめのサイズで四角を作成（調整可）
	float halfSize = 2.0f;
	Vector3 corners[4];
	corners[0] = Add(Add(center, {tangent.x * halfSize, tangent.y * halfSize, tangent.z * halfSize}), {bitangent.x * halfSize, bitangent.y * halfSize, bitangent.z * halfSize});
	corners[1] = Add(Add(center, {-tangent.x * halfSize, -tangent.y * halfSize, -tangent.z * halfSize}), {bitangent.x * halfSize, bitangent.y * halfSize, bitangent.z * halfSize});
	corners[2] = Add(Add(center, {-tangent.x * halfSize, -tangent.y * halfSize, -tangent.z * halfSize}), {-bitangent.x * halfSize, -bitangent.y * halfSize, -bitangent.z * halfSize});
	corners[3] = Add(Add(center, {tangent.x * halfSize, tangent.y * halfSize, tangent.z * halfSize}), {-bitangent.x * halfSize, -bitangent.y * halfSize, -bitangent.z * halfSize});

	// 画面座標に変換
	for (int i = 0; i < 4; i++) {
		corners[i] = Transform(Transform(corners[i], viewProjectionMatrix), viewportMatrix);
	}

	// 線で四角形を描く
	for (int i = 0; i < 4; i++) {
		int next = (i + 1) % 4;
		Novice::DrawLine(static_cast<int>(corners[i].x), static_cast<int>(corners[i].y), static_cast<int>(corners[next].x), static_cast<int>(corners[next].y), color);
	}
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	for (int i = 0; i < 3; ++i) {
		Vector3 p1 = Transform(Transform(triangle.vertices[i], viewProjectionMatrix), viewportMatrix);
		Vector3 p2 = Transform(Transform(triangle.vertices[(i + 1) % 3], viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y), color);
	}
}

void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 8頂点を求める
	Vector3 corners[8] = {
	    {aabb.min.x, aabb.min.y, aabb.min.z},
        {aabb.max.x, aabb.min.y, aabb.min.z},
        {aabb.min.x, aabb.max.y, aabb.min.z},
        {aabb.max.x, aabb.max.y, aabb.min.z},
	    {aabb.min.x, aabb.min.y, aabb.max.z},
        {aabb.max.x, aabb.min.y, aabb.max.z},
        {aabb.min.x, aabb.max.y, aabb.max.z},
        {aabb.max.x, aabb.max.y, aabb.max.z},
	};

	// 各頂点をスクリーン座標に変換
	for (int i = 0; i < 8; ++i) {
		corners[i] = Transform(Transform(corners[i], viewProjectionMatrix), viewportMatrix);
	}

	// 線を引く（12本）
	const int edges[12][2] = {
	    {0, 1},
        {1, 3},
        {3, 2},
        {2, 0}, // 底面
	    {4, 5},
        {5, 7},
        {7, 6},
        {6, 4}, // 上面
	    {0, 4},
        {1, 5},
        {2, 6},
        {3, 7}  // 側面
	};

	for (int i = 0; i < 12; ++i) {
		const Vector3& p1 = corners[edges[i][0]];
		const Vector3& p2 = corners[edges[i][1]];
		Novice::DrawLine(static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y), color);
	}
}

void DrawSphere(const Vector3& center, float radius, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 10;
	const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubdivision);
	const float kLonEvery = static_cast<float>(2.0f * M_PI) / static_cast<float>(kSubdivision);

	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = kLonEvery * lonIndex;

			// 緯線
			Vector3 a = {center.x + radius * cosf(lat) * cosf(lon), center.y + radius * sinf(lat), center.z + radius * cosf(lat) * sinf(lon)};
			Vector3 b = {center.x + radius * cosf(lat + kLatEvery) * cosf(lon), center.y + radius * sinf(lat + kLatEvery), center.z + radius * cosf(lat + kLatEvery) * sinf(lon)};
			// 経線
			Vector3 c = {center.x + radius * cosf(lat) * cosf(lon + kLonEvery), center.y + radius * sinf(lat), center.z + radius * cosf(lat) * sinf(lon + kLonEvery)};

			a = Transform(Transform(a, viewProjectionMatrix), viewportMatrix);
			b = Transform(Transform(b, viewProjectionMatrix), viewportMatrix);
			c = Transform(Transform(c, viewProjectionMatrix), viewportMatrix);

			Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y), static_cast<int>(b.x), static_cast<int>(b.y), color);
			Novice::DrawLine(static_cast<int>(a.x), static_cast<int>(a.y), static_cast<int>(c.x), static_cast<int>(c.y), color);
		}
	}
}

void UpdateCamera(Vector3& cameraTranslate, Vector3& cameraRotate, const char* keys) {
	const float kMoveSpeed = 0.03f;
	const float kRotateSpeed = 0.005f;

	// 回転から前方向・右方向を計算（Y軸回転のみ考慮）
	float sinY = sinf(cameraRotate.y);
	float cosY = cosf(cameraRotate.y);

	Vector3 forward = {sinY, 0.0f, cosY}; // Z+方向が前
	Vector3 right = {cosY, 0.0f, -sinY};  // X+方向が右

	// キーによる移動
	if (keys[DIK_W]) {
		cameraTranslate.x += forward.x * kMoveSpeed;
		cameraTranslate.z += forward.z * kMoveSpeed;
	}
	if (keys[DIK_S]) {
		cameraTranslate.x -= forward.x * kMoveSpeed;
		cameraTranslate.z -= forward.z * kMoveSpeed;
	}
	if (keys[DIK_D]) {
		cameraTranslate.x += right.x * kMoveSpeed;
		cameraTranslate.z += right.z * kMoveSpeed;
	}
	if (keys[DIK_A]) {
		cameraTranslate.x -= right.x * kMoveSpeed;
		cameraTranslate.z -= right.z * kMoveSpeed;
	}
	if (keys[DIK_E]) {
		cameraTranslate.y += kMoveSpeed;
	}
	if (keys[DIK_Q]) {
		cameraTranslate.y -= kMoveSpeed;
	}

	// 回転（矢印キー）
	if (keys[DIK_UP]) {
		cameraRotate.x -= kRotateSpeed;
	}
	if (keys[DIK_DOWN]) {
		cameraRotate.x += kRotateSpeed;
	}
	if (keys[DIK_LEFT]) {
		cameraRotate.y -= kRotateSpeed;
	}
	if (keys[DIK_RIGHT]) {
		cameraRotate.y += kRotateSpeed;
	}

	// マウス回転（右ドラッグ）
	int mouseX, mouseY;
	Novice::GetMousePosition(&mouseX, &mouseY);
	static int prevMouseX = mouseX;
	static int prevMouseY = mouseY;

	if (Novice::IsPressMouse(1)) {
		int dx = mouseX - prevMouseX;
		int dy = mouseY - prevMouseY;
		cameraRotate.y += dx * 0.01f;
		cameraRotate.x += dy * 0.01f;
	}

	prevMouseX = mouseX;
	prevMouseY = mouseY;

	// 上下回転は制限（90度超えないように）
	cameraRotate.x = std::clamp(cameraRotate.x, -1.57f, 1.57f);
}

//=== OBB描画関数 ===//
void DrawOBB(const Vector3& size, const Matrix4x4& worldMatrix, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 8頂点をローカル空間で定義
	Vector3 localCorners[8] = {
	    {-size.x, -size.y, -size.z},
        {size.x,  -size.y, -size.z},
        {-size.x, size.y,  -size.z},
        {size.x,  size.y,  -size.z},
	    {-size.x, -size.y, size.z },
        {size.x,  -size.y, size.z },
        {-size.x, size.y,  size.z },
        {size.x,  size.y,  size.z },
	};

	// ワールド空間→スクリーン空間
	for (int i = 0; i < 8; ++i) {
		localCorners[i] = Transform(localCorners[i], worldMatrix);
		localCorners[i] = Transform(localCorners[i], viewProjectionMatrix);
		localCorners[i] = Transform(localCorners[i], viewportMatrix);
	}

	// 辺を描画
	const int indices[12][2] = {
	    {0, 1},
        {1, 3},
        {3, 2},
        {2, 0},
        {4, 5},
        {5, 7},
        {7, 6},
        {6, 4},
        {0, 4},
        {1, 5},
        {2, 6},
        {3, 7}
    };

	for (int i = 0; i < 12; ++i) {
		Novice::DrawLine(
		    static_cast<int>(localCorners[indices[i][0]].x), static_cast<int>(localCorners[indices[i][0]].y), static_cast<int>(localCorners[indices[i][1]].x),
		    static_cast<int>(localCorners[indices[i][1]].y), color);
	}
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 point1;
	Vector3 point2;
	point1 = {t * v1.x, t * v1.y, t * v1.z};
	point2 = {(1.0f - t) * v2.x, (1.0f - t) * v2.y, (1.0f - t) * v2.z};
	return {point1.x + point2.x, point1.y + point2.y, point1.z + point2.z};
}

Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {
	Vector3 point01;
	Vector3 point12;

	point01 = Lerp(p0, p1, t);
	point12 = Lerp(p1, p2, t);

	return Lerp(point01, point12, t);
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 bezier1;
	Vector3 bezier2;
	Vector3 p1;
	Vector3 p2;
	float divide = 32.0f;

	for (float index = 0.0f; index < divide; index++) {
		float t0 = index / divide;
		float t1 = (index + 1.0f) / divide;

		bezier1 = Bezier(controlPoint0, controlPoint1, controlPoint2, t0);
		bezier2 = Bezier(controlPoint0, controlPoint1, controlPoint2, t1);
		p1 = Transform(Transform(bezier1, viewProjectionMatrix), viewportMatrix);
		p2 = Transform(Transform(bezier2, viewProjectionMatrix), viewportMatrix);
		Novice::DrawLine(static_cast<int>(p1.x), static_cast<int>(p1.y), static_cast<int>(p2.x), static_cast<int>(p2.y), color);
	}
}
