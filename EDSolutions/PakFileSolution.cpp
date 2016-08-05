#include "precompiled.h"

#include "../EDUtilities/PakFile.h"
#include "../EDUtilities/PrintConsole.h"
#include "../EDMath/float3.h"

using namespace EDUtilities;
using namespace EDMath;

namespace EDUtilities
{
	void PakFile::ClearPakSolution()
	{

		pak_directory.clear();
		pak_file.close();
		pak_file.clear();
	}

	void PakFile::LoadDirectorySolution(const char* _pak_file)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Pak File", Float3(1.0f, 0.0f, 0.0f), L"PakFile::LoadDirectorySolution");
			added = true;
		}

		MainHeaderPak main_header;
		FileHeaderPak file_header;
		int num_of = 0;

		pak_file.open(_pak_file, std::ios_base::in | std::ios_base::binary);

		if (pak_file.is_open())
		{
			// get meta information from main header, # of files and director location
			pak_file.read((char*)&main_header, sizeof(MainHeaderPak));
			num_of = main_header.directory_length / sizeof(FileHeaderPak);
			pak_file.seekg(main_header.directory_offset, std::ios_base::beg);

			// add each file entry to the map
			for(int i = 0; i < num_of; i++)
			{
				pak_file.read((char*)&file_header, sizeof(FileHeaderPak));
				std::string name_key(file_header.filename);
				pak_directory[name_key] = file_header;
			}
		}
		else
		{
			EDUtilities::InternalOutput::GetReference() << "Pak file not found, loading all assets from file system.\n";
		}
	}

	void PakFile::FindAssetNameSolution(const char* _raw_name, std::string& _final_name)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Pak File", Float3(1.0f, 0.0f, 0.0f), L"PakFile::FindAssetNameSolution");
			added = true;
		}

		char file_name[128];
		char* short_name =  nullptr;
		char* context = nullptr;

		// xml files reference file only by name, no path, so we have to strip that out
		strcpy_s(file_name, 128, _raw_name);
		short_name = strtok_s(file_name, "/", &context);
		// parse until the next character is a null, at that point the path should be gone
		while(context[0] != '\0')
		{
			short_name = strtok_s(nullptr, "/", &context);
		}

		// packer adds slashes to name to resolve pathing issues, add them here for searching purposes
		_final_name.assign("\\");
		_final_name.append(short_name);
	}

	bool PakFile::IsInPakSolution(const char* _file_name)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Pak File", Float3(1.0f, 0.0f, 0.0f), L"PakFile::IsInPakSolution");
			added = true;
		}

		std::string name_key;
		bool found = true;

		FindAssetNameSolution(_file_name, name_key);

		auto iter = PakFile::pak_directory.find(name_key);

		if (iter == pak_directory.end())
			found = false;

		return found;
	}

	bool PakFile::LoadAssetSolution(const char* _asset_name, char** _buffer, unsigned int& _file_size)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Pak File", Float3(1.0f, 0.0f, 0.0f), L"PakFile::LoadAssetSolution");
			added = true;
		}

		bool success = false;
		std::string name_key;

		(*_buffer) = nullptr;
		_file_size = 0;

		// if the file is open then we can attempto find the loaded directory entry and load the file
		if (pak_file.is_open() && IsInPakSolution(_asset_name))
		{
			FindAssetNameSolution(_asset_name, name_key);
			FileHeaderPak& file_header = pak_directory[name_key];

			_file_size = file_header.file_length;
			(*_buffer) = new char[_file_size];
			pak_file.seekg(file_header.file_offset, std::ios_base::beg);
			pak_file.read((*_buffer), _file_size);

			success = true;
		}

		return success;
	}

	void PakFile::ExtractAssetSolution(const char* _asset_name)
	{
		static bool added = false;
		if(!added)
		{
			PrintConsole::GetReference().AddSolution(L"ED1", "Pak File", Float3(1.0f, 0.0f, 0.0f), L"PakFile::ExtractAssetSolution");
			added = true;
		}

		const int BUFFER_SIZE = 64000;
		std::string file_name;
		std::ofstream output_file;
		char buffer[BUFFER_SIZE];
		int bytes_read = 0;

		if (pak_file.is_open() && IsInPak(_asset_name))
		{
			output_file.open(_asset_name, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
			FindAssetName(_asset_name, file_name);
			FileHeaderPak& file_header = pak_directory[file_name];

			// if gcount reports less than the requested size then we must be at the end
			pak_file.seekg(file_header.file_offset, std::ios_base::beg);
			do
			{
				pak_file.read(buffer, BUFFER_SIZE);
				bytes_read = static_cast<int>(pak_file.gcount());
				output_file.write(buffer, bytes_read);
			} while (bytes_read >= BUFFER_SIZE);
			
			output_file.close();
			// must clear, since I read a set amount every loop I will eventually read past eof
			pak_file.clear();
		}
	}
}
