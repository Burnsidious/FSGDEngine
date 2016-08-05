using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace EDLabMaker
{
	public class PropertyData
	{
		/// <summary>
		/// Name of solution to modify. 
		/// Eg: EDMemoryManager\EDMemoryManager.vcxproj
		/// </summary>
		public string SolutionName { get; set; }
		/// <summary>
		/// Build config to modify. 
		/// Eg: debug|win32 or * for all configs
		/// </summary>
		public string Config { get; set; }
		/// <summary>
		/// Rule containing property to modify. 
		/// Eg: Link
		/// </summary>
		public string Rule { get; set; }
		/// <summary>
		/// Property to be modified. 
		/// Eg: AdditionalDependencies
		/// </summary>
		public string Property { get; set; }
		/// <summary>
		/// Data to be added. 
		/// Eg: EDmemoryManager.lib;
		/// </summary>
		public string DataToAdd { get; set; }
		/// <summary>
		/// Determines if property data should either be prepended to current data or if property
		/// data should replace all existing data.
		/// </summary>
		public bool IsPrependingData { get; set; }


		public override string ToString()
		{
			return String.Format("{0} {1}->{2} on project {3}:{4}",
				IsPrependingData ? "Adding" : "Setting",
				Rule,
				Property,
				Path.GetFileName(SolutionName),
				Config);
		}
	}
}
