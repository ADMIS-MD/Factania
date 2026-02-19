#pragma once

#include "Math.h"

class Camera
{
public:
	Camera();
	~Camera();

	Vec2 GetPos() const;
	fixed GetScale() const;
	Vec2 WorldToCamera() const;
	Vec2 ScreenSpaceFactor() const;
	Vec2 ScreenSpaceExtent() const;
	Vec2 ScreenToWorld(Vec2 screen_pos) const;
	Vec2 WorldToCamera(Vec2 world_pos) const;

	void SetPos(Vec2 pos);
	void MoveRight(fixed x);
	void MoveLeft(fixed x);
	void MoveUp(fixed y);
	void MoveDown(fixed y);
	void SetScale(fixed scale);

private:
	void UpdateCache();

	Vec2 m_eye = { 0.0f , 0.0f };
	Vec2 m_cachedWorldToCamera = { 0.0f, 0.0f };
	// Essentially the grid size - defines scale between world position and screen space
	fixed m_scaleFactor = 16.f;
	Vec2 m_cachedScreenToWorld = { FINT(1) / m_scaleFactor, FINT(1) / m_scaleFactor };
	Vec2 m_cachedScreenExtent = m_cachedScreenToWorld *  Vec2(FINT(SCREEN_WIDTH), FINT(SCREEN_HEIGHT));
};