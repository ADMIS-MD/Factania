//-----------------------------------------------------------------------------
//
//	File:   Camera.cpp
//  Author: Nicholas Brennan
//  Date:   01/22/2026
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//	Includes
//-----------------------------------------------------------------------------

#include "Camera.h"

//-----------------------------------------------------------------------------
//	Methods
//-----------------------------------------------------------------------------

Camera::Camera()
{

}

Camera::~Camera()
{

}

Vec2 Camera::GetPos() const
{
	return m_eye;
}

fixed Camera::GetScale() const
{
    return m_scaleFactor;
}

Vec2 Camera::WorldToCamera() const
{
    return m_cachedWorldToCamera;
}

Vec2 Camera::ScreenSpaceFactor() const
{
    return m_cachedScreenToWorld;
}

Vec2 Camera::ScreenSpaceExtent() const
{
    return m_cachedScreenExtent;
}

Vec2 Camera::ScreenToWorld(Vec2 screen_pos) const
{
    Vec2 v = m_cachedScreenToWorld * screen_pos;
    v.Y() = -v.Y();
    return v + GetPos();
}

Vec2 Camera::WorldToCamera(Vec2 world_pos) const
{
    return m_cachedWorldToCamera + world_pos * m_scaleFactor;
}

void Camera::SetPos(Vec2 pos)
{
    m_eye = pos;
    UpdateCache();
}

void Camera::MoveRight(fixed x)
{
    m_eye.X() += x;
    UpdateCache();
}

void Camera::MoveLeft(fixed x)
{
    m_eye.X() -= x;
    UpdateCache();
}

void Camera::MoveUp(fixed y)
{
    m_eye.Y() += y;
    UpdateCache();
}

void Camera::MoveDown(fixed y)
{
    m_eye.Y() -= y;
    UpdateCache();
}

void Camera::SetScale(fixed scale)
{
    m_scaleFactor = scale;
    UpdateCache();
}

void Camera::UpdateCache()
{
    Vec2 v = this->GetPos();
    v = -v;
    v *= m_scaleFactor;
    m_cachedWorldToCamera = v;

    m_cachedScreenToWorld = { FINT(1) / m_scaleFactor, FINT(1) / m_scaleFactor };
    m_cachedScreenExtent = m_cachedScreenToWorld * Vec2(FINT(SCREEN_WIDTH), FINT(SCREEN_HEIGHT));
}
