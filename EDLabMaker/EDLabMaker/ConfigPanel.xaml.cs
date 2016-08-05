using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace EDLabMaker
{
	/// <summary>
	/// Interaction logic for ConfigPanel.xaml
	/// </summary>
	public partial class ConfigPanel : UserControl
	{
		/// <summary>
		/// Only to give the view access to the config
		/// </summary>
		public Config MyConfig 
		{
			get { return Config.Instance; }
		}

		public ConfigPanel()
		{
			InitializeComponent();

			this.DataContext = this;
		}

		private void RemoveSelectedItemsFromCollection(ListBox source, ICollection<string> collection)
		{
			foreach (var item in source.SelectedItems)
			{
				collection.Remove(item.ToString());
			}

			source.Items.Refresh();
		}

		private void AddItemToCollection(ListBox correspondingListbox, string item, ICollection<string> collection)
		{
			if (String.IsNullOrWhiteSpace(item))
			{
				return;
			}

			collection.Add(item.ToLower().Trim());
			correspondingListbox.Items.Refresh();
		}

		private void ButtonAddExcludedProject_Click(object sender, RoutedEventArgs e)
		{
			AddItemToCollection(listBoxProjectsToRemove, textBoxAddExcludedProject.Text, Config.Instance.NamesOfProjectsToRemove);
		}

		private void ButtonRemoveExcludedProject_Click(object sender, RoutedEventArgs e)
		{
			RemoveSelectedItemsFromCollection(listBoxProjectsToRemove, Config.Instance.NamesOfProjectsToRemove);
		}

		private void ButtonAddBuildConfig_Click(object sender, RoutedEventArgs e)
		{
			AddItemToCollection(listBoxConfigs, textBoxAddBuildConfig.Text, Config.Instance.SolutionConfigNames);
		}

		private void ButtonRemoveBuildConfig_Click(object sender, RoutedEventArgs e)
		{
			RemoveSelectedItemsFromCollection(listBoxConfigs, Config.Instance.SolutionConfigNames);
		}

		private void ButtonAddSolutionExtensionBuildConfig_Click(object sender, RoutedEventArgs e)
		{
			AddItemToCollection(listBoxSolutionExtensions, textBoxAddSolutionExtension.Text, Config.Instance.ExtensionsToRemoveSolutionsFrom);
		}

		private void ButtonRemoveSolutionExtensionBuildConfig_Click(object sender, RoutedEventArgs e)
		{
			RemoveSelectedItemsFromCollection(listBoxSolutionExtensions, Config.Instance.ExtensionsToRemoveSolutionsFrom);
		}

		private void textBoxAddBuildConfig_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
			{
				AddItemToCollection(listBoxConfigs, textBoxAddBuildConfig.Text, Config.Instance.SolutionConfigNames);
				textBoxAddBuildConfig.Clear();
			}
		}

		private void textBoxAddExcludedProject_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Enter)
			{
				AddItemToCollection(listBoxProjectsToRemove, textBoxAddExcludedProject.Text, Config.Instance.NamesOfProjectsToRemove);
				textBoxAddExcludedProject.Clear();
			}
		}

		private void listBoxConfigs_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Delete)
			{
				RemoveSelectedItemsFromCollection(listBoxConfigs, Config.Instance.SolutionConfigNames);
			}
		}

		private void listBoxProjectsToRemove_KeyDown(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Delete)
			{
				RemoveSelectedItemsFromCollection(listBoxProjectsToRemove, Config.Instance.NamesOfProjectsToRemove);
			}
		}

		private void buttonResetConfig_Click(object sender, RoutedEventArgs e)
		{
			Config.Instance.ResetConfigToDefaults();
			this.DataContext = null;
			this.DataContext = this;

		}
	}
}
