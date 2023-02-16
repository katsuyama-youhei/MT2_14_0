#include"Matrix.h"
#include"World.h"
#include <Novice.h>

const char kWindowTitle[] = "LC1A_06_カツヤマヨウヘイ_確認課題";

float Length(const Vector2& v) { return sqrtf(v.x * v.x + v.y * v.y); }

struct Box {
	Vector2 position;
	Vector2 size;
	Vector2 velocity;
	Vector2 acceleration;
	float mass;
	unsigned int color;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	const int kWindowWidth = 1280;
	const int kWindowHeight = 720;
	Novice::Initialize(kWindowTitle, kWindowWidth, kWindowHeight);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Box box{
		{50.0f,50.0f},
		{100.0f,100.0f},
		{0.0f,0.0f},
		{0.0f,0.0f},
		1.0f,
		WHITE,
	};

	Vector2 leftTopLocal = { box.position.x - box.size.x / 2, box.position.y + box.size.y / 2 };
	Vector2 rightTopLocal = { box.position.x + box.size.x / 2, box.position.y + box.size.y / 2 };
	Vector2 leftBottomLocal = { box.position.x - box.size.x / 2, box.position.y - box.size.y / 2 };
	Vector2 rightBottomLocal = { box.position.x + box.size.x / 2, box.position.y - box.size.y / 2 };

	Vector2 cameraPosition = { 480.0f,200.0f };
	Vector2 orthoLeftTop = { -640.0f,360.0f };
	Vector2 orthoRightBottom = { 640.0f,-360.0f };
	Vector2 viewportLeftTop = { 0.0f,0.0f };
	Vector2 viewportSize = { kWindowWidth,kWindowHeight };

	const Vector2 kAxisXStart = { -10000.0f,0.0f };
	const Vector2 kAxisXEnd = { 10000.0f,0.0f };
	const Vector2 kAxisYStart = { 0.0f, -10000.0f };
	const Vector2 kAxisYEnd = { 0.0f, 10000.0f };

	const Vector2 kGravity = { 0.0f,-9.8f };
	const float miu = 0.4f;

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

		if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
			box.velocity.x = 70.0f;
		}

		Matrix3x3 cameraMatrix = MakeAffineMatrix(Vector2{ 1.0f,1.0f }, 0.0f, cameraPosition);
		Matrix3x3 viewMatrix = Inverse(cameraMatrix);
		Matrix3x3 orthoMatrix = MakeOrthographicMatrix(
			orthoLeftTop.x, orthoLeftTop.y, orthoRightBottom.x, orthoRightBottom.y);
		Matrix3x3 viewportMatrix = MakeViewPortMatrix(0, 0, viewportSize.x, viewportSize.y);
		Matrix3x3 vpVpMatrix = Multiply(viewMatrix, orthoMatrix);
		vpVpMatrix = Multiply(vpVpMatrix, viewportMatrix);

		if (box.velocity.x != 0.0f || box.velocity.y != 0.0f) {
			float magnitude = miu * Length({ -box.mass * kGravity.x,-box.mass * kGravity.y });
			Vector2 direction{ 0.0f < box.velocity.x ? -1.0f : 1.0f,0.0f };
			Vector2 frictionalForce{ magnitude * direction.x,magnitude * direction.y };
			box.acceleration.x = frictionalForce.x / box.mass;

			if (fabs(box.acceleration.x / 60.0f) > fabs(box.velocity.x)) {
				box.acceleration.x = box.velocity.x * 60.0f;
			}
		}
		else {
			box.acceleration.x = 0.0f;
		}

		box.velocity.x += (box.acceleration.x / 60.0f);
		box.velocity.x = fabs(box.velocity.x) < 0.01f ? 0.0f : box.velocity.x;
		box.position.x += (box.velocity.x / 60.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Vector2 axisXStart = Transform(kAxisXStart, vpVpMatrix);
		Vector2 axisXEnd = Transform(kAxisXEnd, vpVpMatrix);
		Vector2 axisYStart = Transform(kAxisYStart, vpVpMatrix);
		Vector2 axisYEnd = Transform(kAxisYEnd, vpVpMatrix);

		Novice::DrawLine(axisXStart.x, axisXStart.y, axisXEnd.x, axisXEnd.y, RED);
		Novice::DrawLine(axisYStart.x, axisYStart.y, axisYEnd.x, axisYEnd.y, GREEN);

		Vector2 boxCenter = Transform(box.position, vpVpMatrix);

		Novice::DrawBox(
			boxCenter.x - (box.size.x / 2.0f), boxCenter.y - (box.size.y / 2.0f),
			box.size.x, box.size.y,
			0.0f, box.color, kFillModeSolid
		);

		Novice::ScreenPrintf(30, 30, "%f", box.velocity.x);

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
