#include "Hasher.h"
#include "MurmurHash3.h"

#include "InternalOutput.h"


namespace EDUtilities
{
	// Value used to seed the MurmurHash hashing algorithm
	#define MAGIC_HASHING_SEED 42

	#ifdef _DEBUG
		// Determins if values should be dumped to the InternalOutput whenever hashed
		#define HASHER_DUMP_HASHES 0
		#define HASHER_DEBUG 1
	#else
		#define HASHER_DEBUG 0
	#endif

	std::map<unsigned int, std::string> &GetRegisteredHashes()
	{
		/// Map of hashes registered to strings. Used to detect collisions.
		static std::map<unsigned int, std::string> registeredHashes;
		return registeredHashes;
	}

	void Hasher::DebugCheckHash(const std::string &eventName, unsigned int hash)
	{
		/// Map of hashes registered to strings that were made lowercase. Used to see if a hash only differes
		///  based on case.
		static std::map<unsigned int, std::pair<std::string, std::string>> registeredHashesAllLower;

		std::map<unsigned int, std::string> &registeredHashes = GetRegisteredHashes();

		if(registeredHashes.count(hash) != 0)
		{
			if(registeredHashes[hash] != eventName)
			{
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";
				EDUtilities::InternalOutput::GetReference() << " ***** Warning: Hash Collision! \n" << registeredHashes[hash] << " == " << eventName << "\n";
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";

				MessageBoxA(0, "Hash collision detected. Check output for more details.", "Hashing Error", MB_ICONERROR | MB_OK);
			}

			return;
		}

		// This is a new hash - Check the lowercase version of this eventName to see if it already exists. This may be an
		//  eventName that only differs by case and is likely a user error
		std::string eventNameLower = eventName;
		unsigned int hashLower = 0;

		// registeredHashesAllLower[...].first = lowercase eventname
		// registeredHashesAllLower[...].second = original eventname
		std::transform(eventNameLower.begin(), eventNameLower.end(), eventNameLower.begin(), tolower);

		MurmurHash3_x86_32(eventNameLower.c_str(), static_cast<int>(eventNameLower.length()), MAGIC_HASHING_SEED, &hashLower);

		if(registeredHashesAllLower.count(hashLower) != 0 && registeredHashesAllLower[hashLower].first == eventNameLower)
		{
			if(registeredHashesAllLower[hashLower].second != eventName)
			{
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";
				EDUtilities::InternalOutput::GetReference() << " ** Warning: Multiple events differing only by case: \n  '" << registeredHashesAllLower[hashLower].second << "' VS. '" << eventName << "'\n";
				EDUtilities::InternalOutput::GetReference() << " ----------------------------------------------------\n";

				MessageBoxA(0, "Multiple events with same name differing only by case. Check output for more details.", "Hashing Error", MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			registeredHashesAllLower[hashLower] = std::make_pair<std::string, std::string>(
				std::string(eventNameLower), std::string(eventName));
		}

		registeredHashes[hash] = eventName;

	#if HASHER_DUMP_HASHES
		EDUtilities::InternalOutput::GetReference() << "Hash Debug: '" << eventName << "' = " << hash << "\n";
	#endif
	}

	unsigned int Hasher::HashString(const std::string &stringToHash)
	{
		unsigned int hash = 0;

		MurmurHash3_x86_32(stringToHash.c_str(), static_cast<int>(stringToHash.length()), MAGIC_HASHING_SEED, &hash);
#if HASHER_DEBUG
		DebugCheckHash(stringToHash, hash);
#endif

		return hash;
	}

	unsigned int Hasher::HashString(const char *stringToHash)
	{
		unsigned int hash = 0;

		MurmurHash3_x86_32(stringToHash, static_cast<int>(strlen(stringToHash)), MAGIC_HASHING_SEED, &hash);
#if HASHER_DEBUG
		DebugCheckHash(stringToHash, hash);
#endif

		return hash;
	}

	const std::string Hasher::ReverseHash(unsigned int hash)
	{
		static std::string InvalidResult = "Failed to find hash";

		std::map<unsigned int, std::string> &registeredHashes = GetRegisteredHashes();

		auto iter = registeredHashes.find(hash);
		if(iter != registeredHashes.end())
		{
			return iter->second;
		}

		return InvalidResult;
	}

	void Hasher::DumpHashes()
	{
		std::map<unsigned int, std::string> &registeredHashes = GetRegisteredHashes();

		EDUtilities::InternalOutput::GetReference() << "--------------------------------\n";
		EDUtilities::InternalOutput::GetReference() << "Hash Dump:\n";
		for(auto iter = registeredHashes.begin(); iter != registeredHashes.end(); ++iter)
		{
			EDUtilities::InternalOutput::GetReference() << "    '" <<ReverseHash(iter->first) << "' = " << iter->first << "\n";
		}
		EDUtilities::InternalOutput::GetReference() << "--------------------------------\n";
	}
}