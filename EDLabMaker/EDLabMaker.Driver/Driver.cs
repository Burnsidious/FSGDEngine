using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using LabChecker;
using EnvDTE80;
using EnvDTE;
using Microsoft.VisualStudio.VCProjectEngine;
using System.Text.RegularExpressions;

namespace EDLabMaker
{
	public class Driver
	{
		/// <summary>
		/// Function to send all output to
		/// </summary>
		public Action<string> outputFunction;

		/// <summary>
		/// Builds solution 'solutionName' in the 'enginePath' directory with the specified build config.
		/// </summary>
		/// <param name="enginePath">Full path to engine (EG: C:\Workspace\FSGDEngine-Instructor\)</param>
		/// <param name="solutionName">Name of the solution we will be building (EG: FSGDGame.sln)</param>
		/// <param name="config">Config to build (EG: Debug|x86)</param>
		/// <returns>True on success</returns>
		public bool BuildSolution(string enginePath, string solutionName, string config)
		{
			string DevEnvPath = Config.Instance.PathToDevEnv;
			string buildArgs = solutionName + " /build \"" + config + "\" /project " + Config.Instance.ProjectToBuild;

			// Redirect visual studio output to stdout
			Environment.SetEnvironmentVariable("vsconsoleoutput", "1");

			OutputLine("Building project...\n");

			int exitCode = 0;
			try
			{
				exitCode = Common.GetProgramOutput(DevEnvPath, buildArgs, enginePath, OutputLine);
			}
			catch (Exception ex)
			{
				OutputLine("Error: Build failed: " + ex.Message);
				return false;
			}

			if (exitCode != 0)
			{
				OutputLine("Error: Build failed");
				return false;
			}

			return true;
		}

		/// <summary>
		/// Function used to output messages
		/// </summary>
		/// <param name="message">Message to output</param>
		private void Output(string message)
		{
			Action<string> tempOutputFunction = outputFunction;
			if (tempOutputFunction != null)
			{
				tempOutputFunction(message);
			}
		}
		/// <summary>
		/// Function used to output messages
		/// </summary>
		/// <param name="message">Message to output</param>
		private void OutputLine(string message)
		{
			Output(message + '\n');
		}

		/// <summary>
		/// Traverse specified solution and preforms an action on each project
		/// </summary>
		/// <param name="solution">Solution to traverse</param>
		/// <param name="actionToDo">Action to preform on each project or solution folder (eg: tools)</param>
		private void TraverseSolution(Solution2 solution, Action<Project> actionToDo)
		{
			foreach (Project project in solution.Projects)
			{
				TraverseProject(project, actionToDo);
			}
		}

		/// <summary>
		/// Traverse specified project and preforms an action on the project and each of it's sub projects
		/// </summary>
		/// <param name="project">Project to traverse</param>
		/// <param name="actionToDo">Action to preform on each project or solution folder (eg: tools)</param>
		private void TraverseProject(Project project, Action<Project> actionToDo)
		{
			actionToDo(project);

			// If it's a folder then it might have more projects
			if (project.Kind == EnvDTE.Constants.vsProjectKindSolutionItems)
			{
				foreach (ProjectItem item in project.ProjectItems)
				{
					TraverseProject(item.SubProject, actionToDo);
				}
			}
		}

		/// <summary>
		/// Removes specified projects from solution
		/// </summary>
		/// <param name="solution">Solution to remove projects from</param>
		/// <param name="projectsToRemove">Projects to remove</param>
		private void RemoveProjectsFromSolution(Solution2 solution, List<Project> projectsToRemove)
		{
			foreach (var project in projectsToRemove)
			{
				OutputLine("Removing project '" + project.Name + "'...");
				solution.Remove(project);
			}
		}

		/// <summary>
		/// Applies changes found in Config.PropertyDataToAdd to solution
		/// </summary>
		/// <param name="solution">Solution to apply changes to</param>
		/// <param name="projectConfigs">List of configs for each project. Used for applying properties to all configs</param>
		private void ApplyChangesToProjects(Solution2 solution, Dictionary<string, List<string>> projectConfigs)
		{
			// Apply config changes to solution
			foreach (PropertyData property in Config.Instance.PropertyDataToAdd)
			{
				List<string> configsToApply = new List<string>();

				// Config type of '*' means we should apply this change to all configs
				if (property.Config == "*")
				{
					configsToApply.AddRange(projectConfigs[property.SolutionName.ToLower()]);
				}
				else
				{
					configsToApply.Add(property.Config);
				}

				foreach (string configName in configsToApply)
				{
					OutputLine(property.ToString());
					if (property.IsPrependingData)
					{
						ProjectTools.AddToProperty(solution, property.SolutionName, configName, property.Rule, property.Property, property.DataToAdd);
					}
					else
					{
						ProjectTools.SetProperty(solution, property.SolutionName, configName, property.Rule, property.Property, property.DataToAdd);
					}
				}
			}
		}

		/// <summary>
		/// Loads and edits the specified Visual Studio solution with values from Config. This function
		/// will load visual studio and interact with it through Visual Studio's automation interface.
		/// If it fails then it's likely that the user will be left with a copy of DevEnv.exe running
		/// in the background.
		/// </summary>
		/// <param name="studentSolutionPath">Path to student version of the sln</param>
		/// <param name="solution">solution</param>
		private void ProcessSolution(string studentSolutionPath, Solution2 solution)
		{
			List<Project> projectsToRemove = new List<Project>();

			// Map of config types for each project.
			// EG: "EDMemoryManager" -> {"Release|x86","Debug|x86","Release|x64","Debug|x64"}
			Dictionary<string, List<string>> projectConfigs = new Dictionary<string, List<string>>();

			OutputLine("Opening solution '" + studentSolutionPath + Config.Instance.StudentEngineName + "'...");
			solution.Open(studentSolutionPath + Config.Instance.StudentEngineName);

			try
			{
				TraverseSolution(solution, new Action<Project>(currentProject =>
				{
					string currentProjectName = currentProject.Name.ToLower().Trim();
					if (currentProject.ConfigurationManager != null)
					{
						string uniqueProjectName = currentProject.UniqueName.ToLower();

						Array configRowNames = (Array)currentProject.ConfigurationManager.ConfigurationRowNames;
						Array platformRowNames = (Array)currentProject.ConfigurationManager.PlatformNames;

						if (!projectConfigs.ContainsKey(uniqueProjectName))
						{
							projectConfigs.Add(uniqueProjectName, new List<string>());
						}
						foreach (string configName in configRowNames)
						{
							foreach (string platformName in platformRowNames)
							{
								projectConfigs[uniqueProjectName].Add(configName + "|" + platformName);
							}
						}
					}
					if (Config.Instance.NamesOfProjectsToRemove.Contains(currentProjectName))
					{
						projectsToRemove.Add(currentProject);
					}
					else
					{
						//ProjectTools.RemoveCustomBuildToolsFromFxFiles(currentProject);
						ProjectTools.DisableHlslFiles(currentProject, Output);
					}
				}));

				RemoveProjectsFromSolution(solution, projectsToRemove);
				ApplyChangesToProjects(solution, projectConfigs);
			}
			catch (Exception)
			{
				throw;
			}
			finally
			{
				solution.Close(true);
			}
		}



		/// <summary>
		/// Loads and edits the specified Visual Studio solution with values from Config. This function
		/// will load visual studio and interact with it through Visual Studio's automation interface.
		/// If it fails then it's likely that the user will be left with a copy of DevEnv.exe running
		/// in the background.
		/// </summary>
		/// <param name="studentSolutionPath">Path to student version of the sln</param>
		public void ProcessSolution(string studentSolutionPath)
		{
			OutputLine("Creating instance of VisualStudio...");
			System.Type dteType = System.Type.GetTypeFromProgID("VisualStudio.DTE.11.0", true);
			EnvDTE80.DTE2 devEnvInstance = (EnvDTE80.DTE2)System.Activator.CreateInstance(dteType, true);

			MessageFilter.Register();
			//devEnvInstance.MainWindow.Activate();

			ProcessSolution(studentSolutionPath, devEnvInstance.Solution as Solution2);

			devEnvInstance.Quit();
			MessageFilter.Revoke();
		}

		/// <summary>
		/// Connects to a remote repository and pulls the latest developement branch changes. If local
		/// repository doesn't exist then it will clone the entre repository from the server.
		/// </summary>
		/// <param name="localEngineRepository">Local path to where git will update or clone the remote repository</param>
		/// <param name="remoteEngineRepository">Address of remote repository, the server address</param>
		public void CloneAndPullEngine(string localEngineRepository, string remoteEngineRepository)
		{
			string argClone = string.Format("clone --progress -v \"{0}\" \"{1}\"", remoteEngineRepository, localEngineRepository);
			string argCheckout = "checkout " + Config.Instance.GitBranchToCheckout;
			string argPull = "pull -v --progress  \"origin\" " + Config.Instance.GitBranchToCheckout;
			string gitPath = Utils.GetGitPath();

			if (gitPath == null)
			{
				Output("Failed to find git path\n");
				return;
			}
			else
			{
				Output("Git path: " + gitPath + "\n");
			}

			Directory.CreateDirectory(localEngineRepository);

			// Clone
			Common.GetProgramOutput(
				gitPath,
				argClone,
				localEngineRepository,
				OutputLine);

			// Checkout development branch
			Common.GetProgramOutput(
				gitPath,
				argCheckout,
				localEngineRepository,
				OutputLine);

			// Pull from server
			Common.GetProgramOutput(
				gitPath,
				argPull,
				localEngineRepository,
				OutputLine);
		}


		/// <summary>
		/// Removes all solution blocks from all files inspecified directory and sub directories.
		/// All lines in range [SOLUTION_BEGIN, SOLUTION_END] will be removed from files.
		/// EX: lines 2-4 will be removed in the following example
		/// 1. Non Solution Code
		/// 2. // SOLUTION_BEGIN
		/// 3. Solution code to be removed
		/// 4. // SOLUTION_END
		/// 5. Non Solution Code
		/// </summary>
		/// <param name="directoryPath">Path containing files to remove solution stubs from</param>
		/// <returns>True on success</returns>
		private bool RemoveMarkedSolutionsFromAllFiles(string directoryPath)
		{
			string[] filesToProcess = Directory.GetFiles(directoryPath, "*.*", SearchOption.AllDirectories);

			foreach (string fileName in filesToProcess)
			{
				string extension = Path.GetExtension(fileName).ToLower();

				if (Config.Instance.ExtensionsToRemoveSolutionsFrom.Contains(extension))
				{
					try
					{
						int stubCount = Utils.RemoveMarkedSolutionsFromFile(fileName);
						if (stubCount > 0)
						{
							OutputLine("Wrote " + stubCount + " stub(s) to " + fileName);
						}
					}
					catch (Exception ex)
					{
						OutputLine("Failed: " + ex.Message);
						return false;
					}
				}
			}

			return true;
		}



		/// <summary>
		/// Primary logic for controlling the lab maker automation.
		/// </summary>
		/// <param name="invoker">
		/// Optional action that may be called to preform some logic on the primary thread. Action
		/// is an action that takes a single delegate. This delegate will be executed on the primary
		/// thread while the calling thread is suspended until execution of delgate is completed.
		/// </param>
		public void DriverThreadEntrypoint(object invoker)
		{
			Config config = Config.Instance;

			if (!RunPullFromGitServer())
			{
				return;
			}

			if (!RunBuildEngine())
			{
				return;
			}

			if (!RunPreBatchFile())
			{
				return;
			}

			if (!RunProcessStudentSolution(invoker))
			{
				return;
			}

			if (Config.Instance.IsRemovingSolutionCode)
			{
				OutputLine("Removing solutions from files");
				if (!RemoveMarkedSolutionsFromAllFiles(Path.GetFullPath(Config.Instance.PathToStudentEngine)))
				{
					return;
				}
			}

			if (!RunBuildStudentSolution())
			{
				return;
			}

			if (!RunPostBatchFile())
			{
				return;
			}

			OutputLine("\nAll done.");
			OutputLine("Student solution can be found at: " + Path.GetFullPath(Config.Instance.PathToStudentEngine));
		}

		/// <summary>
		/// Pulls from the git server if needed
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunPullFromGitServer()
		{
			Config config = Config.Instance;

			if (config.IsPullingFromServer)
			{
				try
				{
					CloneAndPullEngine(Path.GetFullPath(config.PathToEngine), config.RemoteEngineRepository);
				}
				catch (Exception ex)
				{
					Output("Failed to pull from repository: " + ex.Message);
					return false;
				}
			}

			return true;
		}

		/// <summary>
		/// Builds the engine if needed
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunBuildEngine()
		{
			Config config = Config.Instance;

			// Build all solutions
			if (config.IsBuildingEngine || config.IsBuildingEngineFull)
			{
				foreach (string configName in config.SolutionConfigNames)
				{
					if (!BuildSolution(Path.GetFullPath(config.PathToEngine), Config.Instance.EngineName, configName))
						return false;

					// Only build the first config if we're not doing a full build
					if (config.IsBuildingEngine && !config.IsBuildingEngineFull)
						break;
				}
			}

			return true;
		}

		/// <summary>
		/// Runs the pre batch file that setups the student engine files by copying the source from
		/// the engine to the student engine directory.
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunPreBatchFile()
		{
			Config config = Config.Instance;
			string batchFileArgs = string.Format("\"{0}\" \"{1}\"", Path.GetFullPath(config.PathToStudentEngine), Path.GetFullPath(config.PathToEngine));
			try
			{
				Common.GetProgramOutput(
					Path.GetFullPath(config.PathToBatchFile),
					batchFileArgs,
					OutputLine
				);
			}
			catch (Exception ex)
			{
				Output("Failed to run batch file: " + ex.Message);
				return false;
			}

			return true;
		}

		/// <summary>
		/// Builds the student engine if needed
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunBuildStudentSolution()
		{
			Config config = Config.Instance;

			// Build all student solutions
			if (config.IsBuildingStudentEngine || config.IsBuildingStudentEngineFull)
			{
				OutputLine("Buliding student engine");
				foreach (string configName in config.SolutionConfigNames)
				{
					if (!BuildSolution(Path.GetFullPath(config.PathToStudentEngine), Config.Instance.StudentEngineName, configName))
						return false;

					// Only build the first config if we're not doing a full build
					if (config.IsBuildingStudentEngine && !config.IsBuildingStudentEngineFull)
						break;
				}
			}

			return true;
		}

		/// <summary>
		/// Fixes up the student engine code by removing unneeded projects and modifying project
		/// properties.
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunProcessStudentSolution(object invoker)
		{
			Config config = Config.Instance;

			try
			{
				if (invoker != null)
				{
					Action<Delegate> InvokerFunction = (Action<Delegate>)invoker;

					Output("Executing logic on main thread, please wait...\n");
					// Open solution and remove solutions/edit linker options/etc
					InvokerFunction(new Action(() =>
					{
						ProcessSolution(Path.GetFullPath(config.PathToStudentEngine));
					}));
					Output("Logic back on it's ownthread\n");
				}
				else
				{
					ProcessSolution(Path.GetFullPath(config.PathToStudentEngine));
				}
			}
			catch (Exception ex)
			{
				Output("Failed to modify student engine: " + ex.Message + "\n");
				return false;
			}

			return true;
		}

		/// <summary>
		/// Runs the post batch file that should cleanup any files that shouldn't be included in the student engine
		/// </summary>
		/// <returns>True on success, false on failure</returns>
		private bool RunPostBatchFile()
		{
			Config config = Config.Instance;
			string batchFileArgs = string.Format("\"{0}\" \"{1}\"", Path.GetFullPath(config.PathToStudentEngine), Path.GetFullPath(config.PathToEngine));

			try
			{
				Common.GetProgramOutput(
					Path.GetFullPath(config.PathToPostBatchFile),
					batchFileArgs,
					OutputLine);
			}
			catch (Exception ex)
			{
				Output("Failed to run post batch file: " + ex.Message);
				return false;
			}

			return true;
		}

	}
}
