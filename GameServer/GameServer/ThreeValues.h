#pragma once

#pragma pack(push,1)
class ThreeValues
{
public:
	float x;
	float y;
	float z;
	ThreeValues() : x(0), y(0), z(0) {}
	ThreeValues(float setX, float setY, float setZ) : x(setX), y(setY), z(setZ) {}
	ThreeValues& operator=(const ThreeValues& other);
	ThreeValues operator+(const ThreeValues& other) const;
	ThreeValues operator-(const ThreeValues& other) const;
	ThreeValues& operator+=(const ThreeValues& other);
	ThreeValues& operator-=(const ThreeValues& other);
	virtual void ScalarAdd(const float scalar);
	virtual void ScalarSub(const float scalar);
	virtual void ScalarMul(const float scalar);
	virtual bool ScalarDiv(const float scalar);
	virtual std::vector<char> Serialize();
	virtual size_t Deserialize(char* buf, size_t len);
};
#pragma pack(pop)
