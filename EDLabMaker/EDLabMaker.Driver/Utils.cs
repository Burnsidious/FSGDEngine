using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using LabChecker;
using System.Text.RegularExpressions;

namespace EDLabMaker
{
	public class Utils
	{
		/// <summary>
		/// Pattern to match the beginning tag indicating the start of a solution
		/// </summary>
		private static Regex patternMatchSolutionBegin = new Regex(@"^[ \t]*(//|--)[ \t]*SOLUTION_BEGIN[ \t]*$", RegexOptions.IgnoreCase);
		/// <summary>
		/// Pattern to match the ending tag indicating the end of a solution
		/// </summary>
		private static Regex patternMatchSolutionEnd = new Regex(@"^[ \t]*(//|--)[ \t]*SOLUTION_END[ \t]*$", RegexOptions.IgnoreCase);

		/// <summary>
		/// Determins if the specified file contains solution tags
		/// </summary>
		/// <param name="filePath">Path of file to check</param>
		/// <returns>True if file contains solution tags</returns>
		public static bool FileContainsSolution(string filePath)
		{
			List<string> lines = File.ReadAllLines(filePath).ToList();

			for (int currentLine = 0; currentLine < lines.Count; ++currentLine)
			{
				if (patternMatchSolutionBegin.IsMatch(lines[currentLine]) || patternMatchSolutionEnd.IsMatch(lines[currentLine]))
				{
					return true;
				}
			}

			return false;
		}

		/// <summary>
		/// Removes all solution blocks from specified file. All lines in range 
		/// [SOLUTION_BEGIN, SOLUTION_END] will be removed from files.
		/// EX: lines 2-4 will be removed in the following example
		/// 1. Non Solution Code
		/// 2. // SOLUTION_BEGIN
		/// 3. Solution code to be removed
		/// 4. // SOLUTION_END
		/// 5. Non Solution Code
		/// </summary>
		/// <param name="filePath">Path to remove solution code from</param>
		public static int RemoveMarkedSolutionsFromFile(string filePath)
		{
			List<string> lines = File.ReadAllLines(filePath).ToList();
			int stubCount = 0;
			bool isStubFile = false;

			for (int currentLine = 0; currentLine < lines.Count; ++currentLine)
			{
				// Found SOLUTION_BEGIN
				if (patternMatchSolutionBegin.IsMatch(lines[currentLine]))
				{
					bool foundEndSolutionTag = false;
					int begin = currentLine;
					int count = 1;
					isStubFile = true;

					//  begin looking for SOLUTION_END
					while (++currentLine < lines.Count)
					{
						++count;
						if (patternMatchSolutionEnd.IsMatch(lines[currentLine]))
						{
							foundEndSolutionTag = true;
							break;
						}
					}

					if (!foundEndSolutionTag)
					{
						throw new ApplicationException("Failed to find matching SOLUTION_END tag");
					}

					// Remove all lines in range of [SOLUTION_BEGIN, SOLUTION_END]
					lines.RemoveRange(begin, count);

					currentLine -= count;
					++stubCount;
				}
			}

			if (isStubFile)
			{
				File.WriteAllLines(filePath, lines);
			}

			return stubCount;
		}

		/// <summary>
		/// Attempts to get the path to git.exe
		/// </summary>
		/// <returns>Path to git.exe, or string.empty on failure</returns>
		public static string GetGitPath()
		{
			string[] paths = Environment.GetEnvironmentVariable("path").Split(new char[] { ';' });

			for (int i = 0; i < paths.Length; i++)
			{
				try
				{
					string testPath = Path.GetFullPath(paths[i].Trim() + @"\..\bin\git.exe");
					if (File.Exists(testPath))
					{
						return testPath;
					}
				}
				catch (Exception)
				{
				}
			}

			return string.Empty;
		}

		/// <summary>
		/// Attempts to get the path to visual studio's devenv.exe
		/// </summary>
		/// <returns>Path to devenv.exe, or string.empty on failure</returns>
		public static string GetDevEnvPath()
		{
			string vsComnTools = Environment.GetEnvironmentVariable("VS110COMNTOOLS");

			if (vsComnTools == null)
			{
				return string.Empty;
			}

			try
			{
				return Path.GetFullPath(vsComnTools + "../IDE/devenv.exe");
			}
			catch (Exception)
			{
				return string.Empty;
			}
		}

		/// <summary>
		/// Reads command line arguments into a map of key,value pairs. Valid arguments are strings
		/// starting with a '-' character followed by one or more characters. key,value pairs are
		/// extracted from arguments by splitting the arguement at the '=' character and having the
		/// text prior to the '=' character as the key and the text after the '=' as the value.
		/// ex: -foo=bar => key="foo", value="bar" | -baz => key="baz" value=""
		/// </summary>
		/// <param name="args">Array of command line arguments</param>
		/// <returns>Map of command line arguments</returns>
		public static Dictionary<string, string> ReadArguments(string[] args)
		{
			Dictionary<string, string> argMap = new Dictionary<string, string>();

			char[] delimters = new char[] { '=' };
			for (int argIndex = 0; argIndex < args.Length; ++argIndex)
			{
				if (args[argIndex].Length <= 1)
					continue;

				if (args[argIndex][0] != '-')
					continue;

				string[] splitArgs = args[argIndex].Split(delimters);
				if (splitArgs.Length > 2)
					continue;

				if (string.IsNullOrWhiteSpace(splitArgs[0]) || splitArgs[0].Length <= 1)
					continue;

				string key = splitArgs[0].Trim().Substring(1);
				string value = string.Empty;

				if (splitArgs.Length > 1)
					value = splitArgs[1].Trim();

				argMap[key] = value;
			}

			return argMap;
		}
	}
}
