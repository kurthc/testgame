#include "Math.h"



vec2::vec2()
{
	this->X = 0;
	this->Y = 0;
}

vec2::vec2(float X, float Y)
{
	this->X = X;
	this->Y = Y;
}


void vec2::SetXY(float x, float y)
{
	this->X = x;
	this->Y = y;
}

vec2 operator+(vec2 v1, vec2 v2)
{
	vec2 Result;
	Result.X = v1.X + v2.X;
	Result.Y = v1.Y + v2.Y;
	return Result;
}

vec2 & operator+=(vec2 &v1, vec2 v2)
{
	v1 = v1 + v2;
	return v1;
}

vec2 operator-(vec2 v)
{
	vec2 Result;
	Result.X = -v.X;
	Result.Y = -v.Y;
	return Result;
}

vec2 operator-(vec2 v1, vec2 v2)
{
	vec2 Result;
	Result.X = v1.X - v2.X;
	Result.Y = v1.Y - v2.Y;
	return Result;
}

vec2 & operator-=(vec2 &v1, vec2 v2)
{
	v1 = v1 - v2;
	return v1;
}

vec2 operator*(float a, vec2 v)
{
	vec2 Result;
	Result.X = a*v.X;
	Result.Y = a*v.Y;
	return Result;
}

vec2 operator*(vec2 v, float a)
{
	vec2 Result;
	Result.X = a*v.X;
	Result.Y = a*v.Y;
	return Result;
}

