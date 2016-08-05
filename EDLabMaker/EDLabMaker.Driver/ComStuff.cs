using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;
using EnvDTE80;
using Microsoft.VisualStudio.VCProjectEngine;
using EnvDTE;
using System.IO;

namespace EDLabMaker
{
	public class ProjectTools
	{
		/// <summary>
		/// Excludes all stubbed hlsl files from building
		/// </summary>
		/// <param name="project">Project to exclude </param>
		public static void DisableHlslFiles(Project project, Action<string> outputFunction = null)
		{
			if (!(project.Object is VCProject))
			{
				return;
			}

			VCProject vcProject = project.Object as VCProject;

			foreach (var item in vcProject.Files)
			{
				if (item is VCFile)
				{
					VCFile file = item as VCFile;

					string fileExtension = Path.GetExtension(file.Name.ToLower());
					if (fileExtension == null || (fileExtension != ".hlsl" && fileExtension != ".hlsli"))
					{
						continue;
					}

					if (!Utils.FileContainsSolution(file.FullPath))
					{
						continue;
					}

					if (outputFunction != null)
					{
						outputFunction("Setting item type to None for: " + file.Name + "\n");
					}

					file.ItemType = "None";
				}
			}
		}

		/// <summary>
		/// Removes custom build data from .fx files. It actually just sets the tool command line
		/// and outputs to an empty string.
		/// </summary>
		/// <param name="project">Project to remove custom build stuff from</param>
		public static void RemoveCustomBuildToolsFromFxFiles(Project project)
		{
			if (!(project.Object is VCProject))
			{
				return;
			}

			VCProject vcProject = project.Object as VCProject;

			foreach (var item in vcProject.Files)
			{
				if (item is VCFile)
				{
					VCFile file = item as VCFile;
					if (!Path.HasExtension(file.Name) || Path.GetExtension(file.Name) != ".fx")
					{
						continue;
					}

					foreach (VCFileConfiguration fileConfig in file.FileConfigurations)
					{
						if (fileConfig.Tool is VCCustomBuildTool)
						{
						    VCCustomBuildTool customTool = fileConfig.Tool as VCCustomBuildTool;
							if (customTool == null)
							{
								continue;
							}

							try
							{
								customTool.Outputs = String.Empty;
								customTool.CommandLine = String.Empty;
							}
							catch (Exception)
							{
								continue;
							}
						}
					}
				}
			}
		}

		public static IVCRulePropertyStorage GetPropertyStorage(Solution2 solution, string projectName, string buildConfig, string ruleName, string property)
		{
			VCProject fsgdGameProject = solution.Projects.Item(projectName).Object as VCProject;
			VCConfiguration config = fsgdGameProject.Configurations.Item(buildConfig) as VCConfiguration;
			IVCRulePropertyStorage propertyStorage = config.Rules.Item(ruleName);

			return propertyStorage;
		}

		public static void AddToProperty(Solution2 solution, string projectName, string buildConfig, string ruleName, string property, string additionalData)
		{
			IVCRulePropertyStorage rule = GetPropertyStorage(solution, projectName, buildConfig, ruleName, property);
			string previousDependencies = rule.GetUnevaluatedPropertyValue(property);

			rule.SetPropertyValue(property, additionalData + ';' + previousDependencies);
		}

		public static void SetProperty(Solution2 solution, string projectName, string buildConfig, string ruleName, string property, string additionalData)
		{
			IVCRulePropertyStorage rule = GetPropertyStorage(solution, projectName, buildConfig, ruleName, property);

			rule.SetPropertyValue(property, additionalData);
		}

		public static string GetUnevaluatedProperty(Solution2 solution, string projectName, string buildConfig, string ruleName, string property, string additionalData)
		{
			IVCRulePropertyStorage rule = GetPropertyStorage(solution, projectName, buildConfig, ruleName, property);

			return rule.GetUnevaluatedPropertyValue(property);
		}

		public static VCLinkerTool GetLinkerTool(Solution2 solution, string projectName, string buildConfig)
		{
			VCProject fsgdGameProject = solution.Projects.Item(projectName).Object as VCProject;
			VCConfiguration debug32 = fsgdGameProject.Configurations.Item(buildConfig) as VCConfiguration;
			VCLinkerTool tool = debug32.Tools.Item("VCLinkerTool");

			return tool;
		}

		public static VCCLCompilerTool GetCompilerTool(Solution2 solution, string projectName, string buildConfig)
		{
			VCProject fsgdGameProject = solution.Projects.Item(solution).Object as VCProject;
			VCConfiguration debug32 = fsgdGameProject.Configurations.Item(buildConfig) as VCConfiguration;
			VCCLCompilerTool tool = debug32.Tools.Item("VCCLCompilerTool");

			return tool;
		}
	}

	// http://msdn.microsoft.com/en-us/library/ms228772%28v=vs.80%29.aspx
	public class MessageFilter : IOleMessageFilter
	{
		//
		// Class containing the IOleMessageFilter
		// thread error-handling functions.

		// Start the filter.
		public static void Register()
		{
			IOleMessageFilter newFilter = new MessageFilter();
			IOleMessageFilter oldFilter = null;
			CoRegisterMessageFilter(newFilter, out oldFilter);
		}

		// Done with the filter, close it.
		public static void Revoke()
		{
			IOleMessageFilter oldFilter = null;
			CoRegisterMessageFilter(null, out oldFilter);
		}

		//
		// IOleMessageFilter functions.
		// Handle incoming thread requests.
		int IOleMessageFilter.HandleInComingCall(int dwCallType,
		  System.IntPtr hTaskCaller, int dwTickCount, System.IntPtr
		  lpInterfaceInfo)
		{
			//Return the flag SERVERCALL_ISHANDLED.
			return 0;
		}

		// Thread call was rejected, so try again.
		int IOleMessageFilter.RetryRejectedCall(System.IntPtr
		  hTaskCallee, int dwTickCount, int dwRejectType)
		{
			if (dwRejectType == 2)
			// flag = SERVERCALL_RETRYLATER.
			{
				// Retry the thread call immediately if return >=0 & 
				// <100.
				return 99;
			}
			// Too busy; cancel call.
			return -1;
		}

		int IOleMessageFilter.MessagePending(System.IntPtr hTaskCallee,
		  int dwTickCount, int dwPendingType)
		{
			//Return the flag PENDINGMSG_WAITDEFPROCESS.
			return 2;
		}

		// Implement the IOleMessageFilter interface.
		[DllImport("Ole32.dll")]
		private static extern int
		  CoRegisterMessageFilter(IOleMessageFilter newFilter, out 
		  IOleMessageFilter oldFilter);
	}

	// http://msdn.microsoft.com/en-us/library/ms228772%28v=vs.80%29.aspx
	[ComImport(), Guid("00000016-0000-0000-C000-000000000046"),
	InterfaceTypeAttribute(ComInterfaceType.InterfaceIsIUnknown)]
	interface IOleMessageFilter
	{
		[PreserveSig]
		int HandleInComingCall(
			int dwCallType,
			IntPtr hTaskCaller,
			int dwTickCount,
			IntPtr lpInterfaceInfo);

		[PreserveSig]
		int RetryRejectedCall(
			IntPtr hTaskCallee,
			int dwTickCount,
			int dwRejectType);

		[PreserveSig]
		int MessagePending(
			IntPtr hTaskCallee,
			int dwTickCount,
			int dwPendingType);
	}
}
