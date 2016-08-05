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
using EDLabMaker;
using System.Threading;
using LabChecker;

namespace EDLabMaker
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		object outputLock = new object();
		string configFile = "./EDLabMaker.xml";
		Driver driver = null;
		Thread driverThread = null;

		public MainWindow()
		{
			InitializeComponent();

			try
			{
				Config.Instance.ReadConfig(configFile);
			}
			catch (Exception ex)
			{
				OutputLine("Failed to read config: " + ex.Message);
				Config.Instance.ResetConfigToDefaults();
			}

			textBoxOutput.Text = "Press 'Start' to begin\n\nNote: GUI will become unresponsive when applying fixes to student solution (Single threaded COM stuff).";
		}

		/// <summary>
		/// Relays output to user interface
		/// </summary>
		/// <param name="message">Message to output</param>
		public void Output(string message)
		{
			textBoxOutput.Dispatcher.BeginInvoke(new Action(() =>
			{
				this.textBoxOutput.AppendText(message);
				this.textBoxOutput.CaretIndex = this.textBoxOutput.Text.Length;
				this.textBoxOutput.ScrollToEnd();
			}), null);
		}

		/// <summary>
		/// Relays output to user interface
		/// </summary>
		/// <param name="message">Message to output</param>
		public void OutputLine(string message)
		{
			Output(message + '\n');
		}

		/// <summary>
		/// Method to execute some logic on the primary thread
		/// </summary>
		/// <param name="method"></param>
		public void Invoke(Delegate method)
		{
			this.Dispatcher.Invoke(method, null);
		}
		
		private void buttonExit_Click(object sender, RoutedEventArgs e)
		{
			Close();
		}

		private void Window_Closed(object sender, EventArgs e)
		{
			Config.Instance.WriteConfig(configFile);
		}

		private void buttonStart_Click(object sender, RoutedEventArgs e)
		{
			if (driverThread != null && driverThread.IsAlive)
				return;

			textBoxOutput.Clear();
			tabControlMain.SelectedIndex = 1;
			driver = new Driver();
			driver.outputFunction = Output;
			driverThread = new Thread(new ParameterizedThreadStart(driver.DriverThreadEntrypoint));
			driverThread.Start(new Action<Delegate>(Invoke));
		}
	}
}
