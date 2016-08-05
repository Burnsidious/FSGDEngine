using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace GameObjectEditor
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        internal static class NativeMethods
        {
            const string dllPath = "../../FSGDEngineToolsInterface.dll";

            [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
            public static extern void InitializeRenderer(Int32 windowHandle);

            [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
            public static extern void ShutdownRenderer();

            [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
            public static extern void RenderMainView();

            [DllImport(dllPath, CallingConvention = CallingConvention.Cdecl)]
            public static extern void InitializeScene();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            Int32 theHWND = RenderPanel.Handle.ToInt32();

            System.IO.Directory.SetCurrentDirectory("../../");

            try
            {
                NativeMethods.InitializeRenderer(theHWND);
                NativeMethods.InitializeScene();
            }
            catch (Exception E)
            {
                //MessageBox.Show(E.StackTrace, E.Message);
                MessageBox.Show("Failed to call DLLImport method NativeMethods.InitializeRenderer", E.Message);
            }

        }

        private void RenderPanel_Paint(object sender, PaintEventArgs e)
        {
            NativeMethods.RenderMainView();
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            NativeMethods.ShutdownRenderer();
        }
    }
}
