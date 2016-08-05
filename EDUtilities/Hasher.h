#pragma once


namespace EDUtilities
{
	class Hasher
	{
	public:
		/// Computes the hash value of a string
		/// \param stringToHash - String to hash
		/// \return Hash value of the string.
		static unsigned int HashString(const char *stringToHash);
		/// Computes the hash value of a string
		/// \param stringToHash - String to hash
		/// \return Hash value of the string.
		static unsigned int HashString(const std::string &stringToHash);
		/// Retrieves the string used to produce the specified hash. Only works on hash values
		/// that have been computed with HashString in debug mode.
		/// \param hash - Hash value of a string already hashed by the system
		/// \return String used to acquire the specified hash.
		static const std::string ReverseHash(unsigned int hash);
		/// Dumps all hashed values and strings to the InternalOutput
		static void DumpHashes();

	private:
		/// Used to debug various aspects of the hashing system. Can detect collisions and possible
		///  errors in hash names, such as multiple hashes that differ only by CaSe.
		/// \param stringBeingHashed - String that is being hashed
		/// \param hash - Hash value of the string.
		static void DebugCheckHash(const std::string &stringBeingHashed, unsigned int hash);
	};
}