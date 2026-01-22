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

void Camera::SetPos(Vec2 pos)
{
    m_eye = pos;
}

void Camera::MoveRight(fixed x)
{
    m_eye.X()  += x;
}

void Camera::MoveLeft(fixed x)
{
    m_eye.X() -= x;
}

void Camera::MoveUp(fixed y)
{
    m_eye.Y() += y;
}

void Camera::MoveDown(fixed y)
{
    m_eye.Y() -= y;
}