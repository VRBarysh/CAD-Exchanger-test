#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <string>
#include <memory>
#include <vector>

class c3DPoint
{
public:
	c3DPoint(float new_x, float new_y, float new_z);

	float x, y, z;
};

class cParametrized3DCurve
{
public:
	virtual c3DPoint Coordinate(float t) = 0;
	virtual c3DPoint Derivative(float t) = 0;
	virtual std::string Report(std::vector<float>& values) = 0;
};

class acHorizontalCircle :
	public cParametrized3DCurve
{
public:
	float radius, x0, y0;
};

class cHorizontalCircle :
	public acHorizontalCircle
{
public:
	cHorizontalCircle(float new_x0, float new_y0, float new_radius);
	virtual c3DPoint Coordinate(float t) override;
	virtual c3DPoint Derivative(float t) override;
	virtual std::string Report(std::vector<float>& values) override;
};

class acHorizontalEllipse :
	public cParametrized3DCurve
{
public:
	float radius_x, radius_y, x0, y0;
};

class cHorizontalEllipse :
	public acHorizontalEllipse
{
public:
	cHorizontalEllipse(float new_x0, float new_y0, float new_radius_x, float new_radius_y);
	virtual c3DPoint Coordinate(float t) override;
	virtual c3DPoint Derivative(float t) override;
	virtual std::string Report(std::vector<float>& values) override;
};

class acVertical3DHelix :
	public cParametrized3DCurve
{
public:
	float radius, step, x0, y0;
};

class cVertical3DHelix :
	public acVertical3DHelix
{
public:
	cVertical3DHelix(float new_x0, float new_y0, float new_radius, float new_step);
	virtual c3DPoint Coordinate(float t) override;
	virtual c3DPoint Derivative(float t) override;
	virtual std::string Report(std::vector<float>& values) override;
};



