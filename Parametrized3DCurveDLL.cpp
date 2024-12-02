#include "pch.h"
#include "Parametrized3DCurveDLL.h"

extern "C" __declspec(dllexport) acHorizontalCircle *CreateHorizontalCircle
						(float new_x0, float new_y0, float new_radius) {
	return new cHorizontalCircle(new_x0, new_y0, new_radius);
}

extern "C" __declspec(dllexport) acHorizontalEllipse *CreateHorizontalEllipse(
						float new_x0, float new_y0, float new_radius_x, float new_radius_y) {
	return new cHorizontalEllipse(new_x0, new_y0, new_radius_x, new_radius_y);
}

extern "C" __declspec(dllexport) acVertical3DHelix *CreateVertical3DHelix(
						float new_x0, float new_y0, float new_radius_x, float new_radius_y) {
	return new cVertical3DHelix(new_x0, new_y0, new_radius_x, new_radius_y);
}

c3DPoint::c3DPoint(float new_x, float new_y, float new_z)
{
	x = new_x;
	y = new_y;
	z = new_z;
}

cHorizontalCircle::cHorizontalCircle(float new_x0, float new_y0, float new_radius)
{
	radius = new_radius > 0 ? new_radius : -new_radius;
	x0 = new_x0;
	y0 = new_y0;
}

c3DPoint cHorizontalCircle::Coordinate(float t)
{
	return c3DPoint(x0 + radius * cosf(t), y0 + radius * sinf(t), 0);
}

c3DPoint cHorizontalCircle::Derivative(float t)
{
	return c3DPoint(-radius * sinf(t), radius * cosf(t), 0);
}

std::string cHorizontalCircle::Report(std::vector<float>& values)
{
	values.push_back(x0);
	values.push_back(y0);
	values.push_back(radius);
	return std::string("Circle");
}

cHorizontalEllipse::cHorizontalEllipse(float new_x0, float new_y0, float new_radius_x, float new_radius_y)
{
	radius_x = new_radius_x > 0 ? new_radius_x : -new_radius_x;
	radius_y = new_radius_y > 0 ? new_radius_y : -new_radius_y;
	x0 = new_x0;
	y0 = new_y0;
}

c3DPoint cHorizontalEllipse::Coordinate(float t)
{
	return c3DPoint(x0 + radius_x * cosf(t), y0 + radius_y * sinf(t), 0);
}

c3DPoint cHorizontalEllipse::Derivative(float t)
{
	return c3DPoint(-radius_x * sinf(t), radius_y * cosf(t), 0);
}

std::string cHorizontalEllipse::Report(std::vector<float>& values)
{
	values.push_back(x0);
	values.push_back(y0);
	values.push_back(radius_x);
	values.push_back(radius_y);
	return std::string("Ellipse");
}

cVertical3DHelix::cVertical3DHelix(float new_x0, float new_y0, float new_radius, float new_step)
{
	radius = new_radius > 0 ? new_radius : -new_radius;
	step = new_step;
	x0 = new_x0;
	y0 = new_y0;
}

c3DPoint cVertical3DHelix::Coordinate(float t)
{
	return c3DPoint(x0 + radius * cosf(t), y0 + radius * sinf(t), step * t * M_1_PI * 0.5);
}

c3DPoint cVertical3DHelix::Derivative(float t)
{
	return c3DPoint(-radius * sinf(t), radius * cosf(t), step * M_1_PI * 0.5);
}

std::string cVertical3DHelix::Report(std::vector<float>& values)
{
	values.push_back(x0);
	values.push_back(y0);
	values.push_back(radius);
	values.push_back(step);
	return std::string("3D Helix");
}
