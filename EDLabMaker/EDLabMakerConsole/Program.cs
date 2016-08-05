using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace EDLabMaker
{
	class Program
	{
		static void Output(string message)
		{
			Console.Write(message);
		}

		[STAThread]
		static void Main(string[] args)
		{
			if (args.Length != 1)
			{
				Console.WriteLine("Missing argument: Path to config");
				return;
			}

			try
			{
				Config.Instance.ReadConfig(args[0]);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Failed to read config: " + ex.Message);
				return;
			}

			Driver driver = new Driver();
			driver.outputFunction = Output;
			driver.DriverThreadEntrypoint(null);
		}
	}
}
