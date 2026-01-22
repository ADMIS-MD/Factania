//
// Created by willb on 1/20/2026.
//

#pragma once
#include <utility>

#include "nds/ndstypes.h"
#include "nds/arm9/videoGL.h"

// 20.12 fixed point, use this instead of float
//  - To create from int literal use `static_cast<int32>(1337)`
class fixed
{
public:
    fixed();
    fixed(int32 value);
    fixed(float value);
    explicit operator int32() const;
    explicit operator float() const;

    int32 GetInt() const;
    int32 const& GetFixed() const;
    int32& GetFixed();
    static fixed FromFixed(int32 f);
    void DivideAsynch(fixed other) const;
    static fixed DivideAsynchResult();
    fixed Sqrt();
    void SqrtAsynch();
    static fixed SqrtAsynchResult();

    fixed& operator+=(const fixed& f);
    fixed& operator-=(const fixed& f);
    fixed& operator*=(const fixed& f);
    fixed& operator/=(const fixed& f);
    fixed operator-() const;
    friend fixed operator+(fixed a, fixed b);
    friend fixed operator-(fixed a, fixed b);
    friend fixed operator*(fixed a, fixed b);
    friend fixed operator/(fixed a, fixed b);
    friend bool operator==(fixed a, fixed b);
    friend bool operator!=(fixed a, fixed b);
    friend bool operator<(const fixed& lhs, const fixed& rhs);
    friend bool operator<=(const fixed& lhs, const fixed& rhs);
    friend bool operator>(const fixed& lhs, const fixed& rhs);
    friend bool operator>=(const fixed& lhs, const fixed& rhs);

private:
    int32 m_value;
};


struct Vec2
{
    Vec2();
    Vec2(fixed x, fixed y);
    Vec2(const Vec2& other);
    Vec2(Vec2&& other) noexcept;

    Vec2& operator=(const Vec2& other);
    Vec2& operator=(Vec2&& other) noexcept;
    friend bool operator==(const Vec2& lhs, const Vec2& rhs);
    friend bool operator!=(const Vec2& lhs, const Vec2& rhs);

    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(const Vec2& other); // Dot product
    Vec2& operator*=(fixed other);
    // Avoid this function!
    Vec2& operator/=(fixed other);
    Vec2 operator-() const;
    friend Vec2 operator+(const Vec2& a, const Vec2& b);
    friend Vec2 operator-(const Vec2& a, const Vec2& b);
    friend Vec2 operator*(const Vec2& a, const Vec2& b);
    friend Vec2 operator*(const Vec2& a, float b);
    friend Vec2 operator*(float a, const Vec2& b);
    // Avoid this function!
    friend Vec2 operator/(const Vec2& a, float b);

    fixed& operator[](int v);
    fixed const& operator[](int v) const;
    fixed& X();
    fixed& Y();
    fixed const& X() const;
    fixed const& Y() const;

    fixed arr[2];
};

struct Vec3
{
    Vec3();
    Vec3(fixed x, fixed y, fixed z);
    Vec3(const Vec3& other);
    Vec3(Vec3&& other) noexcept;

    Vec3& operator=(const Vec3& other);
    Vec3& operator=(Vec3&& other) noexcept;
    friend bool operator==(const Vec3& lhs, const Vec3& rhs);
    friend bool operator!=(const Vec3& lhs, const Vec3& rhs);

    Vec3& operator+=(const Vec3& other);
    Vec3& operator-=(const Vec3& other);
    Vec3& operator*=(const Vec3& other); // Dot product
    Vec3& operator*=(fixed other);
    // Avoid this function!
    Vec3& operator/=(fixed other);
    Vec3 operator-() const;
    friend Vec3 operator+(const Vec3& a, const Vec3& b);
    friend Vec3 operator-(const Vec3& a, const Vec3& b);
    friend Vec3 operator*(const Vec3& a, const Vec3& b);
    friend Vec3 operator*(const Vec3& a, float b);
    friend Vec3 operator*(float a, const Vec3& b);
    // Avoid this function!
    friend Vec3 operator/(const Vec3& a, float b);

    fixed Dot(Vec3& other);
    Vec3 Cross(Vec3& other);
    // Normalizes self
    Vec3& Normalize();

    fixed& operator[](int v);
    fixed const& operator[](int v) const;
    fixed& X();
    fixed& Y();
    fixed& Z();
    fixed const& X() const;
    fixed const& Y() const;
    fixed const& Z() const;

    fixed arr[3];
};
