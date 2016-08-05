#include "PakFile.h"

namespace EDUtilities
{
	PakFile::PakFile()
	{
	}

	PakFile::~PakFile()
	{
		ClearPak();
	}

	PakFile* PakFile::GetInstance()
	{
		static PakFile pak_file;
		return &pak_file;
	}

	void PakFile::FindAssetName(const char* _raw_name, std::string& _final_name)
	{
		// TODO - comment out solution version and write your own implementation
		FindAssetNameSolution(_raw_name, _final_name);
	}

	bool PakFile::LoadAsset(const char* _asset_name, char** _buffer, unsigned int& _file_size)
	{
		// TODO - comment out solution version and write your own implementation
		return LoadAssetSolution(_asset_name, _buffer, _file_size);
	}

	void PakFile::ClearPak()
	{
		// TODO - comment out solution version and write your own implementation
		ClearPakSolution();
	}

	bool PakFile::IsInPak(const char* _file_name)
	{
		// TODO - comment out solution version and write your own implementation
		return IsInPakSolution(_file_name);
	}

	void PakFile::LoadDirectory(const char* _pak_file)
	{
		// TODO - comment out solution version and write your own implementation
		LoadDirectorySolution(_pak_file);
	}

	void PakFile::ExtractAsset(const char* _asset_name)
	{
		// TODO - comment out solution version and write your own implementation
		ExtractAssetSolution(_asset_name);
	}
}
