using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading;

namespace EDLabMaker
{
	public struct TestData
	{
		public string TestDescription;
		public string TestArgs;
		public int PointsWorth;
	}

	public class Common
	{
		/// <summary>
		/// Extracts the specified resoruce from the calling assembly to the specified path
		/// </summary>
		/// <param name="resource">Path of embeded resource (E.G: Lab6.MAR1Lib.lib)</param>
		/// <param name="outputPath">Path to extract resource to</param>
		/// <returns>True, throws an Exception on failure</returns>
		public static bool ExtractEmbededResource(string resource, string outputPath)
		{
			byte[] resourceBytes;

			using (Stream resourceStream = Assembly.GetCallingAssembly().GetManifestResourceStream(resource))
			{
				resourceBytes = new byte[resourceStream.Length];
				resourceStream.Read(resourceBytes, 0, (int)resourceStream.Length);
			}

			File.WriteAllBytes(outputPath, resourceBytes);

			return true;
		}

		/// <summary>
		/// Reads all lines from specified embeded resource of calling assembly into list of strings
		/// </summary>
		/// <param name="resourcePath">Path to text resource to read</param>
		/// <returns>List of lines contained in resource</returns>
		public static List<string> ReadEmbededTextLines(string resourcePath)
		{
			List<string> output = new List<string>();

			using (Stream resourceStream = Assembly.GetCallingAssembly().GetManifestResourceStream(resourcePath))
			{
				StreamReader sr = new StreamReader(resourceStream);
				while (!sr.EndOfStream)
				{
					output.Add(sr.ReadLine());
				}
			}

			return output;
		}

		/// <summary>
		/// Reads all text from specified embeded resource of calling assembly
		/// </summary>
		/// <param name="resourcePath">Path to text resource to read</param>
		/// <returns>Contents of embeded resource</returns>
		public static string ReadEmbededText(string resourcePath)
		{
			using (Stream resourceStream = Assembly.GetCallingAssembly().GetManifestResourceStream(resourcePath))
			{
				StreamReader sr = new StreamReader(resourceStream);
				return sr.ReadToEnd();
			}
		}

		/// <summary>
		/// Runs the specified executable file, passing the optional input to the standard input stream and stores all the standard
		/// output and errors in output
		/// </summary>
		/// <param name="filePath">Path to executable file to execute</param>
		/// <param name="args">Arguments to use when executing process</param>
		/// <param name="workingDirectory">Directory to execute the program in</param>
		/// <param name="timeout">Time to wait in seconds before aborting or 0 for no timeout</param>
		/// <param name="outputFunction">Function to send all program output to</param>
		/// <param name="input">List of input to send to process</param>
		/// <returns>Error code returned from program's main entrypoint or -2 when process times out. Success is 0, everything else is failure.</returns>
		public static int GetProgramOutput(string filePath, string args, string workingDirectory, Action<string> outputFunction = null, int timeout = 0, ICollection<string> input = null)
		{
			outputFunction(string.Format("GetProgramOutput:\n  workingDirectory = {0}\n  args = {1}\n  filePath = {2}", workingDirectory, args, filePath));

			bool isEndOfStream = false;

			// Handler that processes any output from stdout or stderr 
			var dataReceivedHandler = new DataReceivedEventHandler((sender, e) =>
			{
				// null is the last data sent when stream is closed.
				if (e.Data != null)
				{
					if (outputFunction != null)
					{
						outputFunction(e.Data);
					}
				}
				else
				{
					isEndOfStream = true;
				}
			});

			ProcessStartInfo startInfo = new ProcessStartInfo();
			startInfo.UseShellExecute = false;
			startInfo.FileName = filePath;
			startInfo.CreateNoWindow = true;
			startInfo.WorkingDirectory = workingDirectory;
			startInfo.RedirectStandardInput = true;
			startInfo.RedirectStandardOutput = true;
			startInfo.RedirectStandardError = true;
			startInfo.ErrorDialog = false;
			startInfo.Arguments = args;

			Process testProcA = new Process();
			testProcA.StartInfo = startInfo;
			testProcA.OutputDataReceived += dataReceivedHandler;
			testProcA.ErrorDataReceived += dataReceivedHandler;
			testProcA.Start();
			testProcA.BeginOutputReadLine();
			testProcA.BeginErrorReadLine();

			// Write all input lines to process
			if (input != null)
			{
				foreach (var item in input)
				{
					testProcA.StandardInput.WriteLine(item);
				}
				testProcA.StandardInput.Write("\n");
			}

			// Wait for process to complete or timeout
			if (timeout > 0)
			{
				for (int i = 0; i < timeout; i++)
				{
					if (i >= timeout - 7)
					{
						if (outputFunction != null)
						{
							outputFunction(" *** Timeout in " + (timeout - i) + " seconds...\n");
						}
					}

					if (testProcA.WaitForExit(1000))
					{
						break;
					}
				}
			}
			else
			{
				testProcA.WaitForExit();
			}
			
			// If process timed out then fail
			if (!testProcA.HasExited)
			{
				if (outputFunction != null)
				{
					outputFunction(" *** Process not complete after " + timeout + " seconds, killing process...\n");
				}
				testProcA.Kill();
				testProcA.WaitForExit(2000);
				return -2;
			}

			// Wait for completion of process's output, times out after about 5 seconds
			for (int i = 0; !isEndOfStream && i < 500; i++)
			{
				Thread.Sleep(100);
			}
			if (!isEndOfStream)
			{
				if (outputFunction != null)
				{
					outputFunction(" *** Output not processed after ~5 seconds\n");
				}
			}

			return testProcA.ExitCode;
		}

		/// <summary>
		/// Runs the specified executable file, passing the optional input to the standard input stream and stores all the standard
		/// output and errors in output
		/// </summary>
		/// <param name="filePath">Path to executable file to execute</param>
		/// <param name="args">Arguments to use when executing process</param>
		/// <param name="workingDirectory">Directory to execute the program in</param>
		/// <param name="timeout">Time to wait in seconds before aborting or 0 for no timeout</param>
		/// <param name="outputFunction">Function to send all program output to</param>
		/// <param name="input">List of input to send to process</param>
		/// <returns>Error code returned from program's main entrypoint or -2 when process times out. Success is 0, everything else is failure.</returns>
		public static int GetProgramOutput(string filePath, string args, Action<string> outputFunction = null, int timeout = 0, ICollection<string> input = null)
		{
			return GetProgramOutput(filePath, args, Path.GetDirectoryName(filePath), outputFunction, timeout, input);
		}

		/// <summary>
		/// Runs the specified executable file, passing the optional input to the standard input stream and stores all the standard
		/// output and errors in output
		/// </summary>
		/// <param name="filePath">Path to executable file to execute</param>
		/// <param name="args">Arguments to use when executing process</param>
		/// <param name="workingDirectory">Directory to execute the program in</param>
		/// <param name="timeout">Time to wait in seconds before aborting or 0 for no timeout</param>
		/// <param name="outputList">List where program's output will be stored</param>
		/// <param name="input">List of input to send to process</param>
		/// <returns>Error code returned from program's main entrypoint or -2 when process times out. Success is 0, everything else is failure.</returns>
		public static int GetProgramOutput(string filePath, string args, string workingDirectory, List<string> outputList = null, int timeout = 0, ICollection<string> input = null)
		{
			object outputLock = new object();

			// redirect all output from GetProgramOutput(...) to outputList
			Action<string> outputFunction = new Action<string>(msg =>
			{
				if (outputList != null)
				{
					lock (outputLock)
					{
						outputList.Add(msg);
					}
				}
			});

			return GetProgramOutput(filePath, args, workingDirectory, outputFunction, timeout, input);
		}

		/// <summary>
		/// Runs the specified executable file, passing the optional input to the standard input stream and stores all the standard
		/// output and errors in output
		/// </summary>
		/// <param name="filePath">Path to executable file to execute</param>
		/// <param name="args">Arguments to use when executing process</param>
		/// <param name="workingDirectory">Directory to execute the program in</param>
		/// <param name="timeout">Time to wait in seconds before aborting or 0 for no timeout</param>
		/// <param name="outputList">List where program's output will be stored</param>
		/// <param name="input">List of input to send to process</param>
		/// <returns>Error code returned from program's main entrypoint or -2 when process times out. Success is 0, everything else is failure.</returns>
		public static int GetProgramOutput(string filePath, string args, List<string> outputList = null, int timeout = 0, ICollection<string> input = null)
		{
			return GetProgramOutput(filePath, args, Path.GetDirectoryName(filePath), outputList, timeout, input);
		}

		/// <summary>
		/// Attempt to shutdown and delete a running executable based on file path
		/// </summary>
		/// <param name="filePath">Path to executable that is currently running</param>
		private static void CleanupOrphanedProcess(string filePath)
		{
			string processName = Path.GetFileNameWithoutExtension(filePath);

			try
			{
				foreach (var procInUse in Process.GetProcessesByName(processName))
				{
					procInUse.Kill();
					procInUse.WaitForExit();
				}
			}
			catch (Exception ex)
			{
				Console.Write("Deletion Failed: " + ex.Message);
			}

			try
			{
				File.Delete(filePath);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Deletion  Failed: " + ex.Message);
			}
		}

		/// <summary>
		/// Deletes all the temporary files created during extraction, building and testing of a lab submission. All files 
		/// specified by filesToRemove will be deleted.
		/// </summary>
		public static void CleanupFiles(List<string> filesToRemove)
		{
			foreach (var item in filesToRemove)
			{
				try
				{
					File.Delete(item);
				}
				catch (Exception ex)
				{
					if (item.EndsWith(".exe"))
					{
						CleanupOrphanedProcess(item);
					}
					else
					{
						Console.WriteLine("Deletion Failed: " + ex.Message);
					}
				}
			}
		}
	}
}
