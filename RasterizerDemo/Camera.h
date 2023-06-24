#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

struct CameraPos
{
	XMFLOAT3 cameraPos;
	float filler = 0.0f;

	CameraPos(const XMFLOAT3& camPos)
	{
		cameraPos = camPos;
	}
};

class Camera
{
public:
	Camera(ID3D11Device* device, float aspectRatio, XMFLOAT3 position = XMFLOAT3(0.0f, 5.0f, -5.0f), XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f), float fovDegrees = 90.0f, float nearZ = 0.1f, float farZ = 1000.0f);
	~Camera();
	void SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ);

	ID3D11Buffer* cameraPosConstantBuffer;
	CameraPos camPos =
	{
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	};

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;

	const XMFLOAT3& GetPositionFloat3() const;
	const XMFLOAT3& GetRotationFloat3() const;

	void SetPosition(XMFLOAT3 position);
	void AdjustPosition(const XMVECTOR& pos);
	void AdjustPosition(float x, float y, float z);
	void SetRotation(XMFLOAT3 rotation);
	void AdjustRotation(float x, float y, float z);
	void SetLookAtPos(XMFLOAT3 lookAtPos);
	const XMVECTOR& GetForwardVector();
	const XMVECTOR& GetRightVector();
	const XMVECTOR& GetBackwardVector();
	const XMVECTOR& GetLeftVector();
	const XMVECTOR& GetUpVector();
private:
	void UpdateViewMatrix();
	XMVECTOR posVector;
	XMVECTOR rotVector;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
	const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR vec_forward;
	XMVECTOR vec_left;
	XMVECTOR vec_right;
	XMVECTOR vec_backward;
	XMVECTOR vec_up;
};
