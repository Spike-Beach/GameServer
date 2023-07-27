#pragma once

template <typename T>
class Singleton
{
public:
	Singleton(const Singleton&);
	Singleton& operator = (const Singleton&);

	static T& Instance()
	{
		static T instance;
		return instance;
	}
protected:
	Singleton() {};
	~Singleton() {};
};