using System.Collections.Generic;
using System.Text.RegularExpressions;

namespace LabChecker
{
	public interface ILabChecker
	{
		/// <summary>
		/// List of expressions to determin if this lab checker is capable of handling a lab submission
		/// based on submission filename.
		/// </summary>
		List<Regex> Patterns { get; set; }

		/// <summary>
		/// Initialize the module. Will be called multiple times.
		/// </summary>
		/// <returns>True on success</returns>
		bool Init();
		/// <summary>
		/// Modify freshly extracted files to either add, remove or modify files.
		/// </summary>
		/// <param name="projectFiles">List of files attached to this project. All files will be deleted.</param>
		void ModifyExtractedFiles(string extractionPath, ref List<string> projectFiles);
		/// <summary>
		/// Determines if the compiled solution passes all tests
		/// </summary>
		/// <param name="pathToExecutable">Path to executable being tested</param>
		/// <returns>true when output is correct for all test cases</returns>
		bool IsCorrect(string pathToExecutable);
		/// <summary>
		/// Determines if the specified file can be processed by this lab
		/// </summary>
		/// <param name="archiveFile">Properly named lab submission (eg: doe.john.lab4.zip).</param>
		/// <returns>True if this lab can handle the specified submission</returns>
		bool IsAbleToProcessFile(string filePath);
	}
}
