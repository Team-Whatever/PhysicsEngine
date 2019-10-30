#pragma once
struct Color
{
	float r;
	float g;
	float b;
	float a;

	Color(float _r = 1, float _g = 1, float _b = 1 , float _a = 1) 
		: r(_r), g(_g), b(_b), a(_a) { }
};