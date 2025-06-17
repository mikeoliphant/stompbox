#pragma once

#include <atomic>

template <typename T>
class DoubleBufferedResource
{
public:
	T& GetRead()
	{
		return *currentResource;
	}

	T& GetWriteLock()
	{
		return (currentResource == &resource1) ? resource2 : resource1;
	}

	void FinishWrite()
	{
		//Swap();
	}

private:
	void Swap()
	{
		currentResource.exchange((currentResource == &resource1) ? &resource2 : &resource1);
	}

	T resource1;
	T resource2;
	std::atomic<T*> currentResource = &resource1;
};