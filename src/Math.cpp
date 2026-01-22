//
// Created by willb on 1/20/2026.
//

#include "Math.h"
#include "generated_defines.h"

#define FIXED_POINT_LOC 12

fixed::fixed()
{
}

fixed::fixed(int32 value) : m_value(intToFixed(value, FIXED_POINT_LOC))
{
}

fixed::fixed(float value) : m_value(floatToFixed(value, FIXED_POINT_LOC))
{
}


fixed::operator int32() const
{
    return fixedToInt(m_value, FIXED_POINT_LOC);
}

fixed::operator float() const
{
    return fixedToFloat(m_value, FIXED_POINT_LOC);
}

int32 fixed::GetInt() const
{
    return int32{*this};
}

int32 const& fixed::GetFixed() const
{
    return m_value;
}

int32& fixed::GetFixed()
{
    return m_value;
}

fixed fixed::FromFixed(int32 f)
{
    fixed fp{};
    fp.m_value = f;
    return fp;
}

void fixed::DivideAsynch(fixed other) const
{
    divf32_asynch(m_value, other.m_value);
}

fixed fixed::DivideAsynchResult()
{
    return FromFixed(divf32_result());
}

fixed fixed::Sqrt()
{
    return FromFixed(sqrtf32(m_value));
}

void fixed::SqrtAsynch()
{
    sqrtf32_asynch(m_value);
}

fixed fixed::SqrtAsynchResult()
{
    return FromFixed(sqrtf32_result());
}

fixed& fixed::operator+=(const fixed& f)
{
    m_value += f.m_value;
    return *this;
}

fixed& fixed::operator-=(const fixed& f)
{
    m_value -= f.m_value;
    return *this;
}

fixed& fixed::operator*=(const fixed& f)
{
    m_value = mulf32(m_value, f.m_value);
    return *this;
}

fixed& fixed::operator/=(const fixed& f)
{
    m_value = divf32(m_value, f.m_value);
    return *this;
}

fixed fixed::operator-()
{
    return FromFixed(-m_value);
}

Vec2::Vec2()
{
}

Vec2::Vec2(fixed x, fixed y) : arr{x, y}
{
}

Vec2::Vec2(const Vec2& other)
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
}

Vec2::Vec2(Vec2&& other) noexcept
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
}

Vec2& Vec2::operator=(const Vec2& other)
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
    return *this;
}

Vec2& Vec2::operator=(Vec2&& other) noexcept
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
    return *this;
}

bool operator==(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.arr[0] == rhs.arr[0] && lhs.arr[1] == rhs.arr[1];
}

bool operator!=(const Vec2& lhs, const Vec2& rhs)
{
    return !(lhs == rhs);
}

Vec2& Vec2::operator+=(const Vec2& other)
{
    X() += other.X();
    Y() += other.Y();
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    X() -= other.X();
    Y() -= other.Y();
    return *this;
}

Vec2& Vec2::operator*=(const Vec2& other)
{
    X() *= other.X();
    Y() *= other.Y();
    return *this;
}

Vec2& Vec2::operator*=(fixed other)
{
    X() *= other;
    Y() *= other;
    return *this;
}

Vec2& Vec2::operator/=(fixed other)
{
    X() /= other;
    Y() /= other;
    return *this;
}

Vec2 Vec2::operator-()
{
    return {-X(), -Y()};
}

Vec2 operator+(const Vec2& a, const Vec2& b)
{
    Vec2 a1(a);
    return a1 += b;
}

Vec2 operator-(const Vec2& a, const Vec2& b)
{
    Vec2 a1(a);
    return a1 -= b;
}

Vec2 operator*(const Vec2& a, const Vec2& b)
{
    Vec2 a1(a);
    return a1 *= b;
}

Vec2 operator*(const Vec2& a, float b)
{
    Vec2 a1(a);
    return a1 *= b;
}

Vec2 operator*(float a, const Vec2& b)
{
    Vec2 b1(b);
    return b1 *= a;
}

Vec2 operator/(const Vec2& a, float b)
{
    Vec2 a1(a);
    return a1 /= b;
}

fixed& Vec2::operator[](int v)
{
    return arr[v];
}

fixed const& Vec2::operator[](int v) const
{
    return arr[v];
}

fixed& Vec2::X()
{
    return arr[0];
}

fixed& Vec2::Y()
{
    return arr[1];
}

fixed const& Vec2::X() const
{
    return arr[0];
}

fixed const& Vec2::Y() const
{
    return arr[1];
}

Vec3::Vec3() : arr{int32{0}, int32{0}, int32{0}}
{
}

Vec3::Vec3(fixed x, fixed y, fixed z)
{
    arr[0] = x;
    arr[1] = y;
    arr[2] = z;
}

Vec3& Vec3::operator*=(fixed other)
{
    arr[0] *= other;
    arr[1] *= other;
    arr[2] *= other;

    return *this;
}

Vec3& Vec3::operator/=(fixed other)
{
    arr[0] / other;
    arr[1] / other;
    arr[2] / other;

    return *this;
}

Vec3 Vec3::operator-()
{
    return {-X(), -Y(), -Z()};
}

fixed& Vec3::X()
{
    return arr[0];
}

fixed& Vec3::Y()
{
    return arr[1];
}

fixed& Vec3::Z()
{
    return arr[2];
}

fixed const& Vec3::X() const
{
    return arr[0];
}

fixed const& Vec3::Y() const
{
    return arr[1];
}

fixed const& Vec3::Z() const
{
    return arr[2];
}

Vec3::Vec3(const Vec3& other)
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
    arr[2] = other.arr[2];
}

Vec3::Vec3(Vec3&& other) noexcept
{
    arr[0] = other.arr[0];
    arr[1] = other.arr[1];
    arr[2] = other.arr[2];
}

Vec3& Vec3::operator=(const Vec3& other)
{
    X() = other.X();
    Y() = other.Y();
    Z() = other.Z();
    return *this;
}

Vec3& Vec3::operator=(Vec3&& other) noexcept
{
    X() = other.X();
    Y() = other.Y();
    Z() = other.Z();
    return *this;
}

Vec3& Vec3::operator+=(const Vec3& other)
{
    X() += other.X();
    Y() += other.Y();
    Z() += other.Z();
    return *this;
}

Vec3& Vec3::operator-=(const Vec3& other)
{
    X() -= other.X();
    Y() -= other.Y();
    Z() -= other.Z();
    return *this;
}

Vec3& Vec3::operator*=(const Vec3& other)
{
    X() *= other.X();
    Y() *= other.Y();
    Z() *= other.Z();
    return *this;
}

fixed Vec3::Dot(Vec3& other)
{
    return dotf32(&arr->GetFixed(), &other.arr->GetFixed());
}

Vec3 Vec3::Cross(Vec3& other)
{
    Vec3 out;
    crossf32(&arr->GetFixed(), &other.arr->GetFixed(), &out.arr->GetFixed());
    return out;
}

fixed& Vec3::operator[](int v)
{
    return arr[v];
}

fixed const& Vec3::operator[](int v) const
{
    return arr[v];
}


fixed operator+(fixed a, fixed b)
{
    return a += b;
}

bool operator==(fixed a, fixed b)
{
    return a.m_value == b.m_value;
}

bool operator!=(fixed a, fixed b)
{
    return !(a == b);
}

bool operator<(const fixed& lhs, const fixed& rhs)
{
    return lhs.m_value < rhs.m_value;
}

bool operator<=(const fixed& lhs, const fixed& rhs)
{
    return !(rhs < lhs);
}

bool operator>(const fixed& lhs, const fixed& rhs)
{
    return rhs < lhs;
}

bool operator>=(const fixed& lhs, const fixed& rhs)
{
    return !(lhs < rhs);
}

bool operator==(const Vec3& lhs, const Vec3& rhs)
{
    return lhs.X() == rhs.X()
        && lhs.Y() == rhs.Y();
}

bool operator!=(const Vec3& lhs, const Vec3& rhs)
{
    return !(lhs == rhs);
}

Vec3 operator+(const Vec3& a, const Vec3& b)
{
    Vec3 a1(a);
    return a1 += b;
}

Vec3 operator-(const Vec3& a, const Vec3& b)
{
    Vec3 a1(a);
    return a1 -= b;
}

Vec3 operator*(const Vec3& a, const Vec3& b)
{
    Vec3 a1(a);
    return a1 *= b;
}

Vec3 operator*(const Vec3& a, float b)
{
    Vec3 a1(a);
    return a1 *= b;
}

Vec3 operator*(float a, const Vec3& b)
{
    Vec3 b1(b);
    return b1 *= a;
}

Vec3 operator/(const Vec3& a, float b)
{
    Vec3 a1(a);
    return a1 /= b;
}
