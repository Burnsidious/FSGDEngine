using System;
using System.Windows.Forms;
using EngineInterface;

namespace Editor
{
    public partial class Editor : Form
    {
        public Editor()
        {
            InitializeComponent();
        }

        private void MainDisplay_Paint(object sender, PaintEventArgs e)
        {
            Engine.Paint();
        }

        private void Editor_Load(object sender, EventArgs e)
        {
            Engine.Initialize(MainDisplay.Handle);
        }

        private void Editor_FormClosed(object sender, FormClosedEventArgs e)
        {
            Engine.ShutDown();
        }

    }
}
