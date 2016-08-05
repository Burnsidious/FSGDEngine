using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Xml.Serialization;
using System.Reflection;

namespace EDLabMaker
{
	public class Config
	{
		#region Singleton
		private static readonly Config instance = new Config();

		/// <summary>
		/// Gets the one and only one instance of the Config
		/// </summary>
		public static Config Instance
		{
			get { return instance; }
		}
		#endregion

		/// <summary>
		/// Path to the engine
		/// </summary>
		public string PathToEngine { get; set; }
		/// <summary>
		/// Path to where the student engine will be created at
		/// </summary>
		public string PathToStudentEngine { get; set; }
		/// <summary>
		/// Path to batch file that will setup the student engine files. Should just xcopy a bunch
		/// of stuff from engine.
		/// </summary>
		public string PathToBatchFile { get; set; }
		/// <summary>
		/// Path to batch file to execute after student solution is complete. Cleanup/etc.
		/// </summary>
		public string PathToPostBatchFile { get; set; }
		/// <summary>
		/// Path to git.exe
		/// </summary>
		public string PathToGit { get; set; }
		/// <summary>
		/// Path to Visual Studio's Devenv.exe
		/// </summary>
		public string PathToDevEnv { get; set; }
		/// <summary>
		/// Address of remote git repository
		/// </summary>
		public string RemoteEngineRepository { get; set; }
		/// <summary>
		/// Determines if the engine should be cloned/pulled/updated from remote git server
		/// </summary>
		public bool IsPullingFromServer { get; set; }
		/// <summary>
		/// Determines if debug|win32 version of engine should be built
		/// </summary>
		public bool IsBuildingEngine { get; set; }
		/// <summary>
		/// Determines if All versions of engine should be built
		/// </summary>
		public bool IsBuildingEngineFull { get; set; }
		/// <summary>
		/// Determines if debug|win32 version of student engine should be built
		/// </summary>
		public bool IsBuildingStudentEngine { get; set; }
		/// <summary>
		/// Determiens if all versions of student engine should be built
		/// </summary>
		public bool IsBuildingStudentEngineFull { get; set; }
		/// <summary>
		/// Names of projects that should be removed from student engine sln
		/// </summary>
		public HashSet<string> NamesOfProjectsToRemove { get; set; }
		/// <summary>
		/// Build config names for entire solution. Visual Studio doesn't appear to support a
		/// /build ALL argument so individual build configs must be known ahead of time.
		/// </summary>
		public HashSet<string> SolutionConfigNames { get; set; }
		/// <summary>
		/// Additional project properties to add to solution. Eg: additional libraries
		/// </summary>
		public List<PropertyData> PropertyDataToAdd { get; set; }
		/// <summary>
		/// Name of student engine sln file, eg: FSGDGame_Student.sln
		/// </summary>
		public string StudentEngineName { get; set; }
		/// <summary>
		/// Name of engine sln file, eg: FSGDGame.sln
		/// </summary>
		public string EngineName { get; set; }
		/// <summary>
		/// Name of branch to checkout.
		/// </summary>
		public string GitBranchToCheckout { get; set; }
		/// <summary>
		/// Project to build when building the engine.
		/// </summary>
		public string ProjectToBuild { get; set; }
		/// <summary>
		/// Determines if solution code should be removed. Solution code are all lines between
		/// the // SOLUTION_BEGIN and // SOLUTION_END comments. These comments need to be on their
		/// own line.
		/// EX: lines 2-4 will be removed in the following example
		/// 1. Non Solution Code
		/// 2. // SOLUTION_BEGIN
		/// 3. Solution code to be removed
		/// 4. // SOLUTION_END
		/// 5. Non Solution Code
		/// </summary>
		public bool IsRemovingSolutionCode { get; set; }
		/// <summary>
		/// Set of all file extensions that should be checked for solution code if IsRemovingSolutionCode
		/// flag is set.
		/// </summary>
		public HashSet<string> ExtensionsToRemoveSolutionsFrom { get; set; }

		/// <summary>
		/// Writes configuration file
		/// </summary>
		/// <param name="configFile">The configuration file path</param>
		public void WriteConfig(string configFile)
		{
			XmlSerializer serializer = new XmlSerializer(this.GetType());

			using (FileStream outStream = new FileStream(configFile, FileMode.Create))
			{
				serializer.Serialize(outStream, this);
				outStream.Flush();
			}
		}

		/// <summary>
		/// Reads a configuration file.
		/// </summary>
		/// <param name="configFile">The configuration file path</param>
		public bool ReadConfig(string configFile)
		{
			XmlSerializer serializer = new XmlSerializer(typeof(Config));
			Config tempConfig;

			using (FileStream inStream = new FileStream(configFile, FileMode.Open))
			{
				tempConfig = serializer.Deserialize(inStream) as Config;
			}

			PropertyInfo[] propertyInfo = this.GetType().GetProperties();

			// Copy all properties over from tempConfig to this config
			foreach (var property in propertyInfo)
			{
				// We don't want static stuff (eg: Instance)
				if (property.GetGetMethod().IsStatic)
					continue;

				property.SetValue(this, property.GetValue(tempConfig, null), null);
			}

			// PathToStudentEngine and PathToEngine must end in a trailing slash (for the batch files)
			if(!this.PathToStudentEngine.EndsWith("\\") && !this.PathToStudentEngine.EndsWith("/"))
				this.PathToStudentEngine += '\\';
			if (!this.PathToEngine.EndsWith("\\") && !this.PathToEngine.EndsWith("/"))
				this.PathToEngine += '\\';

			return true;
		}

		#region DefaultConfigData
		/// <summary>
		/// Restores config to default
		/// TODO: Temporary function, remove later
		/// </summary>
		public void ResetConfigToDefaults()
		{
			this.PathToStudentEngine = @"..\..\FSGDEngine-Student\";
			this.PathToBatchFile = @".\makeStudentEngine.bat";
			this.PathToPostBatchFile = @".\postBuildOperation.bat";
			this.PathToEngine = @"..\";
			this.RemoteEngineRepository = String.Empty;
			this.IsPullingFromServer = false;
			this.PathToGit = Utils.GetGitPath();
			this.PathToDevEnv = Utils.GetDevEnvPath();
			this.IsBuildingEngine = true;
			this.IsBuildingEngineFull = true;
			this.IsBuildingStudentEngine = false;
			this.IsBuildingStudentEngineFull = false;
			this.StudentEngineName = "FSGDGame_Student.sln";
			this.EngineName = "FSGDGame.sln";
			this.GitBranchToCheckout = "development";
			this.ProjectToBuild = "FSGDGame";
			this.IsRemovingSolutionCode = true;

			SolutionConfigNames = new HashSet<string>()
			{
				"Debug|x86",
				"Debug|x64",
				"Release|x86",
				"Release|x64",
			};

			NamesOfProjectsToRemove = new HashSet<string>()
			{
				"edheapsolution",
				"edsolutions",
				"tools",
				"edtree",
				"edcollision",
				"ainavigationmanager",
				"ainavigationmesh",
				"aipathplanner",
			};

			ExtensionsToRemoveSolutionsFrom = new HashSet<string>()
			{
				".c",
				".cpp",
				".hpp",
				".fx",
				".cxx",
				".h",
				".lua",
				".hlsl",
				".hlsli",
				".inc"
			};

			PropertyDataToAdd = new List<PropertyData>()
			{
				new PropertyData()
				{
					SolutionName =  @"FSGDGame\FSGDGame.vcxproj",
					Config = "*",
					Rule = "Link",
					Property = "AdditionalDependencies",
					DataToAdd = "EDCollision.lib;EDSolutions.lib;EDTree.lib",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName =  @"EDMemoryManager\EDMemoryManager.vcxproj",
					Config = "*",
					Rule = "Link",
					Property = "AdditionalDependencies",
					DataToAdd = "EDHeapSolution.lib",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName =  @"FSGDGame\FSGDGame.vcxproj",
					Config = "Debug|Win32",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\DebugLibsWin32",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName =  @"FSGDGame\FSGDGame.vcxproj",
					Config = "Debug|x64",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\DebugLibsx64",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName =  @"FSGDGame\FSGDGame.vcxproj",
					Config = "Release|Win32",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\ReleaseLibsWin32",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName =  @"FSGDGame\FSGDGame.vcxproj",
					Config = "Release|x64",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\ReleaseLibsx64",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName = @"EDMemoryManager\EDMemoryManager.vcxproj",
					Config = "Debug|Win32",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\DebugLibsWin32",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName = @"EDMemoryManager\EDMemoryManager.vcxproj",
					Config = "Debug|x64",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\DebugLibsx64",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName = @"EDMemoryManager\EDMemoryManager.vcxproj",
					Config = "Release|Win32",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\ReleaseLibsWin32",
					IsPrependingData = true
				},
				new PropertyData()
				{
					SolutionName = @"EDMemoryManager\EDMemoryManager.vcxproj",
					Config = "Release|x64",
					Rule = "Link",
					Property = "AdditionalLibraryDirectories",
					DataToAdd = @"..\ReleaseLibsx64",
					IsPrependingData = true
				},
			};
		}
		#endregion
	}
}
