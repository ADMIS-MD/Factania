#pragma once

#include "Math.h"

class Camera
{
public:
	Camera();
	~Camera();

	Vec2 GetPos() const;
	Vec2 WorldToCamera() const;

	void SetPos(Vec2 pos);
	void MoveRight(fixed x);
	void MoveLeft(fixed x);
	void MoveUp(fixed y);
	void MoveDown(fixed y);

private:
	Vec2 m_eye = { 0.0f , 0.0f };
};