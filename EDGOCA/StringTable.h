#pragma once

#include <string>
#include <map>
#include <unordered_map>

namespace EDGOCA
{
	class StringTable
	{
		std::map< size_t, std::string > UIntStringMap;

	public:

		size_t AddString(const char* str);

		void RemoveString(const char* str);

		size_t GetStringKey(const char* str);

		const char* GetKeyString(size_t key);
	};
}