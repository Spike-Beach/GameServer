#include "pch.h"
#include "Velocity.h"

Velocity& Velocity::operator=(const Velocity& other)
{
	ThreeValues::operator=(other);
	return *this;
}

Velocity Velocity::operator+(const Velocity& other) const
{
	Velocity temp(x + other.x, y + other.y, z + other.z);
	return temp;
}

Velocity Velocity::operator-(const Velocity& other) const
{
	Velocity temp(x - other.x, y - other.y, z - other.z);
	return temp;
}

Velocity& Velocity::operator+=(const Velocity& other)
{
	(*this) = (*this) + other;
	return *this;
}

Velocity& Velocity::operator-=(const Velocity& other)
{
	(*this) = (*this) - other;
	return *this;
}

Velocity Velocity::operator*(const float& scalar) const
{
	Velocity temp(x * scalar, y * scalar, z * scalar);
	return temp;
}

Velocity Velocity::operator/(const float& scalar) const
{
	Velocity temp;
	if (scalar == 0)
	{
		g_logger.Log(LogLevel::ERR, "Velocity::operator/", "Divide 0");
		throw std::exception("Divide 0");
	}
	temp.x /= scalar;
	temp.y /= scalar;
	temp.z /= scalar;
	return true;
}

//std::optional<Velocity> Velocity::GetNomalVel()
//{
//	Velocity temp(*this);
//	if (x == 0 && y == 0 && z == 0)
//	{
//		return std::nullopt;
//	}
//	else
//	{
//		temp.ScalarDiv(GetMagnitude());
//		return temp;
//	}
//}

void Velocity::ApplyBrakes(float decelerate, float elapsedSec)
{
	x -= decelerate * elapsedSec;
	y -= decelerate * elapsedSec;
	z -= decelerate * elapsedSec;
	if (x < 0) { x = 0; }
	if (y < 0) { y = 0; }
	if (z < 0) { z = 0; }
}

void Velocity::UpdateVelocity(Acceleration acc, float elapsedSec)
{
	Velocity temp(acc.x * elapsedSec, acc.y * elapsedSec, acc.z * elapsedSec);
	*this += temp;
}

void Velocity::AdjustToMaxMagnitude(float maxVelMagnitude, const Velocity& oldVel)
{
	if (maxVelMagnitude == 0)
	{
		if (oldVel.x * x < 0) { x = 0; }
		if (oldVel.y * y < 0) { y = 0; }
		if (oldVel.z * z < 0) { z = 0; }
	}
	else if (ThreeValues::GetMagnitude() > maxVelMagnitude)
	{
		auto optNomal = GetNomal();
		if (optNomal.has_value())
		{
			Velocity nomal = optNomal.value();
			nomal.ScalarMul(maxVelMagnitude);
			this->operator=(nomal);
		}
	}
}

float Velocity::CalMaxVelApprochElapseSec(Acceleration acc, float maxVelMagnitude)
{
	if (acc.IsZero())
	{
		g_logger.Log(LogLevel::CRITICAL, "Velocity::CalMaxVelApprochElapseSec()", "Divide Zero");
		throw std::runtime_error("Divide Zero");
	}
	float elapsedSec = (maxVelMagnitude - ThreeValues::GetMagnitude()) / acc.GetMagnitude();
	return elapsedSec;
}

std::vector<char> Velocity::Serialize()
{
	return ThreeValues::Serialize();
}


size_t Velocity::Deserialize(char* buf, size_t len)
{
	return ThreeValues::Deserialize(buf, len);
}