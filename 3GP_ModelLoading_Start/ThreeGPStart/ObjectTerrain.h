#pragma once
#include "Object.h"
class ObjectTerrain :public Object
{
private:

	int m_numCellsX{ 0 };
	int m_numCellsZ{ 0 };
	float m_sizeX{ 0 };
	float m_sizeZ{ 0 };


public:

	ObjectTerrain(int numCellsX, int numCellsZ, float sizeX, float sizeZ);
	bool Initialise() override final;
	float CosineLerp(float a, float b, float x);
	float Noise(int x, int y);
	float KenPerlin(float xPos, float zPos);
};

