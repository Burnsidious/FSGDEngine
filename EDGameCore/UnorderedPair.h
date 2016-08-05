#pragma once

template<typename T>
struct UnorderedPair
{
	T first;
	T second;

	UnorderedPair(const T& a, const T& b)
	{
		if( a < b )
		{
			first = a;
			second = b;
		}
		else
		{
			first = b;
			second = a;
		}
	}

	bool operator<(const UnorderedPair& rhs)const
	{
		return	first < rhs.first ||
			(first == rhs.first && second < rhs.second);
	}
};