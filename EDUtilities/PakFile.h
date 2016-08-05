#ifndef _PAK_FILE_H_
#define _PAK_FILE_H_

// simple pak file format based on quake implementation

namespace EDUtilities
{
	// signature - must be 'PACK' to signify its a valid pak file
	// offset - the absolute position in bytes of the directory
	// length - the size in bytes of all the file headers
	struct MainHeaderPak
	{
		char signature[4];
		int  directory_offset;
		int	 directory_length;
	};

	// filename - relative path of file
	// offset - the absolute position in bytes of the file
	// length - the size in bytes of the file
	struct FileHeaderPak
	{
		char filename[56];
		int	 file_offset;
		int	 file_length;
	};

	class PakFile
	{
	private:

		//static PakFile* instance;
		PakFile();
		~PakFile();

		std::unordered_map<std::string, FileHeaderPak> pak_directory;
		std::ifstream pak_file;

		bool LoadAssetSolution(const char* _asset_name, char** _buffer, unsigned int& _file_size);
		void ClearPakSolution();
		void LoadDirectorySolution(const char* _pak_file);
		bool IsInPakSolution(const char* _file_name);
		void FindAssetNameSolution(const char* _raw_name, std::string& _final_name);
		void ExtractAssetSolution(const char* _asset_name);

	public:

		static PakFile* GetInstance();

		// loads an asset from the PAK to a buffer, invoker of this function must deallocate buffer
		// IN _asset_name - name of the asset to look for
		// OUT _buffer - address of the buffer to store the asset, must be allocated
		// OUT _file_size - reference to the size of the file
		// RETURN - if the load was a success or not
		bool LoadAsset(const char* _asset_name, char** _buffer, unsigned int& _file_size);

		// opens the pak file and loads its directory into memory
		// IN _pak_file - name of the pak file to load
		void LoadDirectory(const char* _pak_file);

		// checks the loaded directory to see if an asset is in it
		// IN _file_name - name of the asset to look for
		// RETURN - if the asset is in the pak file or not
		bool IsInPak(const char* _file_name);

		// clears the loaded directory and closes the pak file
		void ClearPak();

		// strips a name of relative paths for searching or loading
		// IN _raw_name - name with relative path
		// OUT _final_name - name stripped of paths
		void FindAssetName(const char* _raw_name, std::string& _final_name);

		// a debug funcion that extracts an asset from the pak to the file system
		// IN _asset_name - name of the asset to extract to the file system
		void ExtractAsset(const char* _asset_name);
	};
}

#endif
