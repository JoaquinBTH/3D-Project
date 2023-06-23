#include "Camera.h"

Camera::Camera(ID3D11Device* device, float aspectRatio, XMFLOAT3 position, XMFLOAT3 rotation, float fovDegrees, float nearZ, float farZ)
{
	this->SetPosition(position);
	this->SetRotation(rotation);
	this->SetProjectionValues(fovDegrees, aspectRatio, nearZ, farZ);

	this->camPos.cameraPos = this->pos;

	//Buffer description
	D3D11_BUFFER_DESC cameraPosBufferDesc{};
	cameraPosBufferDesc.ByteWidth = sizeof(CameraPos);
	cameraPosBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraPosBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraPosBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraPosBufferDesc.MiscFlags = 0;
	cameraPosBufferDesc.StructureByteStride = 0;

	//Initial data in the buffer
	D3D11_SUBRESOURCE_DATA cameraPosData;
	cameraPosData.pSysMem = &camPos;
	cameraPosData.SysMemPitch = 0;
	cameraPosData.SysMemSlicePitch = 0;

	//Create the buffer
	device->CreateBuffer(&cameraPosBufferDesc, &cameraPosData, &cameraPosConstantBuffer);

	//Update the view matrix
	this->UpdateViewMatrix();
}

Camera::~Camera()
{
	cameraPosConstantBuffer->Release();
}

void Camera::SetProjectionValues(float fovDegrees, float aspectRatio, float nearZ, float farZ)
{
	float fovRadians = (fovDegrees / 360.0f) * XM_2PI;
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearZ, farZ);
}

const XMMATRIX& Camera::GetViewMatrix() const
{
	return this->viewMatrix;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMFLOAT3& Camera::GetPositionFloat3() const
{
	return this->pos;
}

const XMFLOAT3& Camera::GetRotationFloat3() const
{
	return this->rot;
}

void Camera::SetPosition(XMFLOAT3 position)
{
	this->pos = position;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(const XMVECTOR& pos)
{
	this->posVector += pos;
	XMStoreFloat3(&this->pos, this->posVector);
	this->UpdateViewMatrix();
}

void Camera::AdjustPosition(float x, float y, float z)
{
	this->pos.x += x;
	this->pos.y += y;
	this->pos.z += z;
	this->posVector = XMLoadFloat3(&this->pos);
	this->UpdateViewMatrix();
}

void Camera::SetRotation(XMFLOAT3 rotation)
{
	this->rot = rotation;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::AdjustRotation(float x, float y, float z)
{
	this->rot.x += x;
	if (this->rot.x > XM_2PI || this->rot.x < -XM_2PI)
	{
		this->rot.x = 0.0f;
	}
	this->rot.y += y;
	if (this->rot.y > XM_2PI || this->rot.y < -XM_2PI)
	{
		this->rot.y = 0.0f;
	}
	this->rot.z += z;
	this->rotVector = XMLoadFloat3(&this->rot);
	this->UpdateViewMatrix();
}

void Camera::SetLookAtPos(XMFLOAT3 lookAtPos)
{
	if (lookAtPos.x == this->pos.x && lookAtPos.y == this->pos.y && lookAtPos.z == this->pos.z)
	{
		return;
	}

	lookAtPos.x = this->pos.x - lookAtPos.x;
	lookAtPos.y = this->pos.y - lookAtPos.y;
	lookAtPos.z = this->pos.z - lookAtPos.z;

	float pitch = 0.0f;
	if (lookAtPos.y != 0.0f)
	{
		const float distance = sqrtf(lookAtPos.x * lookAtPos.x + lookAtPos.z * lookAtPos.z);
		pitch = atanf(lookAtPos.y / distance);
	}

	float yaw = 0.0f;
	if (lookAtPos.x != 0.0f)
	{
		yaw = atanf(lookAtPos.x / lookAtPos.z);
	}
	if (lookAtPos.z > 0)
		yaw += XM_PI;

	this->SetRotation(XMFLOAT3(pitch, yaw, 0.0f));
}

const XMVECTOR& Camera::GetForwardVector()
{
	return this->vec_forward;
}

const XMVECTOR& Camera::GetRightVector()
{
	return this->vec_right;
}

const XMVECTOR& Camera::GetBackwardVector()
{
	return this->vec_backward;
}

const XMVECTOR& Camera::GetLeftVector()
{
	return this->vec_left;
}

const XMVECTOR& Camera::GetUpVector()
{
	return this->vec_up;
}

void Camera::UpdateViewMatrix() //Update view matrix and move vectors
{
	//Update camPos
	camPos.cameraPos = this->pos;

	XMMATRIX camRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);
	XMVECTOR camTarget = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, camRotationMatrix);
	camTarget += this->posVector;
	this->vec_up = XMVector3TransformCoord(this->DEFAULT_UP_VECTOR, camRotationMatrix);
	this->viewMatrix = XMMatrixLookAtLH(this->posVector, camTarget, vec_up);

	XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, 0.0f);
	this->vec_forward = XMVector3TransformCoord(this->DEFAULT_FORWARD_VECTOR, vecRotationMatrix);
	this->vec_backward = XMVector3TransformCoord(this->DEFAULT_BACKWARD_VECTOR, vecRotationMatrix);
	this->vec_left = XMVector3TransformCoord(this->DEFAULT_LEFT_VECTOR, vecRotationMatrix);
	this->vec_right = XMVector3TransformCoord(this->DEFAULT_RIGHT_VECTOR, vecRotationMatrix);
}