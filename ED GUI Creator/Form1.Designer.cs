namespace ED_GUI_Creator
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.newToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.saveToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.saveAsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.optionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.setDirectoryToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.formToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.materialToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.modelToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.radiansToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.writeLightingValuesToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.TexturePanel = new System.Windows.Forms.PictureBox();
            this.ParentNum = new System.Windows.Forms.NumericUpDown();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label9 = new System.Windows.Forms.Label();
            this.ScaleXNum = new System.Windows.Forms.NumericUpDown();
            this.ScaleYNum = new System.Windows.Forms.NumericUpDown();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.RotZNum = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.RotXNum = new System.Windows.Forms.NumericUpDown();
            this.RotYNum = new System.Windows.Forms.NumericUpDown();
            this.label8 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.PosXNum = new System.Windows.Forms.NumericUpDown();
            this.PosYNum = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.MeshType = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.TexturePanel)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ParentNum)).BeginInit();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ScaleXNum)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ScaleYNum)).BeginInit();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RotZNum)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RotXNum)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RotYNum)).BeginInit();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PosXNum)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PosYNum)).BeginInit();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.optionsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(641, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.newToolStripMenuItem,
            this.toolStripSeparator1,
            this.openToolStripMenuItem,
            this.toolStripSeparator2,
            this.saveToolStripMenuItem,
            this.saveAsToolStripMenuItem,
            this.toolStripSeparator3,
            this.exitToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // newToolStripMenuItem
            // 
            this.newToolStripMenuItem.Name = "newToolStripMenuItem";
            this.newToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.newToolStripMenuItem.Text = "New";
            this.newToolStripMenuItem.Click += new System.EventHandler(this.newToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(111, 6);
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.openToolStripMenuItem.Tag = "";
            this.openToolStripMenuItem.Text = "Open";
            this.openToolStripMenuItem.ToolTipText = "Folder directories must be pointing at applicable files for GUI to load";
            this.openToolStripMenuItem.Click += new System.EventHandler(this.openToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(111, 6);
            // 
            // saveToolStripMenuItem
            // 
            this.saveToolStripMenuItem.Name = "saveToolStripMenuItem";
            this.saveToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.saveToolStripMenuItem.Text = "Save";
            this.saveToolStripMenuItem.Click += new System.EventHandler(this.saveToolStripMenuItem_Click);
            // 
            // saveAsToolStripMenuItem
            // 
            this.saveAsToolStripMenuItem.Name = "saveAsToolStripMenuItem";
            this.saveAsToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.saveAsToolStripMenuItem.Text = "Save As";
            this.saveAsToolStripMenuItem.Click += new System.EventHandler(this.saveAsToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(111, 6);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(114, 22);
            this.exitToolStripMenuItem.Text = "Exit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // optionsToolStripMenuItem
            // 
            this.optionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.setDirectoryToolStripMenuItem,
            this.radiansToolStripMenuItem,
            this.writeLightingValuesToolStripMenuItem});
            this.optionsToolStripMenuItem.Name = "optionsToolStripMenuItem";
            this.optionsToolStripMenuItem.Size = new System.Drawing.Size(61, 20);
            this.optionsToolStripMenuItem.Text = "Options";
            // 
            // setDirectoryToolStripMenuItem
            // 
            this.setDirectoryToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.formToolStripMenuItem,
            this.materialToolStripMenuItem,
            this.modelToolStripMenuItem});
            this.setDirectoryToolStripMenuItem.Name = "setDirectoryToolStripMenuItem";
            this.setDirectoryToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
            this.setDirectoryToolStripMenuItem.Text = "Set Directory";
            // 
            // formToolStripMenuItem
            // 
            this.formToolStripMenuItem.Name = "formToolStripMenuItem";
            this.formToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.formToolStripMenuItem.Text = "Form";
            this.formToolStripMenuItem.Click += new System.EventHandler(this.formToolStripMenuItem_Click);
            // 
            // materialToolStripMenuItem
            // 
            this.materialToolStripMenuItem.Name = "materialToolStripMenuItem";
            this.materialToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.materialToolStripMenuItem.Text = "Material";
            this.materialToolStripMenuItem.Click += new System.EventHandler(this.materialToolStripMenuItem_Click);
            // 
            // modelToolStripMenuItem
            // 
            this.modelToolStripMenuItem.Name = "modelToolStripMenuItem";
            this.modelToolStripMenuItem.Size = new System.Drawing.Size(117, 22);
            this.modelToolStripMenuItem.Text = "Model";
            this.modelToolStripMenuItem.Click += new System.EventHandler(this.modelToolStripMenuItem_Click);
            // 
            // radiansToolStripMenuItem
            // 
            this.radiansToolStripMenuItem.CheckOnClick = true;
            this.radiansToolStripMenuItem.Name = "radiansToolStripMenuItem";
            this.radiansToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
            this.radiansToolStripMenuItem.Text = "Radians";
            this.radiansToolStripMenuItem.ToolTipText = "Switches between degrees and radians";
            this.radiansToolStripMenuItem.Click += new System.EventHandler(this.radiansToolStripMenuItem_Click);
            // 
            // writeLightingValuesToolStripMenuItem
            // 
            this.writeLightingValuesToolStripMenuItem.CheckOnClick = true;
            this.writeLightingValuesToolStripMenuItem.Name = "writeLightingValuesToolStripMenuItem";
            this.writeLightingValuesToolStripMenuItem.Size = new System.Drawing.Size(186, 22);
            this.writeLightingValuesToolStripMenuItem.Text = "Write Lighting Values";
            this.writeLightingValuesToolStripMenuItem.ToolTipText = "Determines whether the ambient, diffuse, specular values will be written";
            this.writeLightingValuesToolStripMenuItem.Click += new System.EventHandler(this.writeLightingValuesToolStripMenuItem_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer1.Location = new System.Drawing.Point(0, 24);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.AutoScroll = true;
            this.splitContainer1.Panel1.Controls.Add(this.TexturePanel);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.ParentNum);
            this.splitContainer1.Panel2.Controls.Add(this.groupBox3);
            this.splitContainer1.Panel2.Controls.Add(this.groupBox2);
            this.splitContainer1.Panel2.Controls.Add(this.groupBox1);
            this.splitContainer1.Panel2.Controls.Add(this.MeshType);
            this.splitContainer1.Panel2.Controls.Add(this.label2);
            this.splitContainer1.Panel2.Controls.Add(this.label1);
            this.splitContainer1.Size = new System.Drawing.Size(641, 376);
            this.splitContainer1.SplitterDistance = 455;
            this.splitContainer1.TabIndex = 1;
            // 
            // TexturePanel
            // 
            this.TexturePanel.AccessibleDescription = "";
            this.TexturePanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TexturePanel.Location = new System.Drawing.Point(0, 0);
            this.TexturePanel.Name = "TexturePanel";
            this.TexturePanel.Size = new System.Drawing.Size(455, 376);
            this.TexturePanel.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.TexturePanel.TabIndex = 0;
            this.TexturePanel.TabStop = false;
            this.TexturePanel.Tag = "";
            this.TexturePanel.DoubleClick += new System.EventHandler(this.pictureBox1_DoubleClick);
            this.TexturePanel.MouseHover += new System.EventHandler(this.TexturePanel_MouseHover);
            // 
            // ParentNum
            // 
            this.ParentNum.Location = new System.Drawing.Point(13, 27);
            this.ParentNum.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.ParentNum.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            this.ParentNum.Name = "ParentNum";
            this.ParentNum.Size = new System.Drawing.Size(156, 20);
            this.ParentNum.TabIndex = 6;
            this.ParentNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ParentNum.Value = new decimal(new int[] {
            1,
            0,
            0,
            -2147483648});
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.ScaleXNum);
            this.groupBox3.Controls.Add(this.ScaleYNum);
            this.groupBox3.Controls.Add(this.label11);
            this.groupBox3.Location = new System.Drawing.Point(13, 286);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(156, 78);
            this.groupBox3.TabIndex = 5;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Scale";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(6, 21);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(14, 13);
            this.label9.TabIndex = 4;
            this.label9.Text = "X";
            // 
            // ScaleXNum
            // 
            this.ScaleXNum.DecimalPlaces = 2;
            this.ScaleXNum.Location = new System.Drawing.Point(26, 19);
            this.ScaleXNum.Name = "ScaleXNum";
            this.ScaleXNum.Size = new System.Drawing.Size(120, 20);
            this.ScaleXNum.TabIndex = 5;
            this.ScaleXNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ScaleXNum.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // ScaleYNum
            // 
            this.ScaleYNum.DecimalPlaces = 2;
            this.ScaleYNum.Location = new System.Drawing.Point(26, 45);
            this.ScaleYNum.Name = "ScaleYNum";
            this.ScaleYNum.Size = new System.Drawing.Size(120, 20);
            this.ScaleYNum.TabIndex = 7;
            this.ScaleYNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            this.ScaleYNum.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(6, 47);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(14, 13);
            this.label11.TabIndex = 6;
            this.label11.Text = "Y";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.RotZNum);
            this.groupBox2.Controls.Add(this.label6);
            this.groupBox2.Controls.Add(this.label7);
            this.groupBox2.Controls.Add(this.RotXNum);
            this.groupBox2.Controls.Add(this.RotYNum);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Location = new System.Drawing.Point(13, 176);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(156, 104);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Rotation";
            // 
            // RotZNum
            // 
            this.RotZNum.DecimalPlaces = 2;
            this.RotZNum.Location = new System.Drawing.Point(26, 71);
            this.RotZNum.Maximum = new decimal(new int[] {
            360,
            0,
            0,
            0});
            this.RotZNum.Minimum = new decimal(new int[] {
            360,
            0,
            0,
            -2147483648});
            this.RotZNum.Name = "RotZNum";
            this.RotZNum.Size = new System.Drawing.Size(120, 20);
            this.RotZNum.TabIndex = 9;
            this.RotZNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 21);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(14, 13);
            this.label6.TabIndex = 4;
            this.label6.Text = "X";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 73);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(14, 13);
            this.label7.TabIndex = 8;
            this.label7.Text = "Z";
            // 
            // RotXNum
            // 
            this.RotXNum.DecimalPlaces = 2;
            this.RotXNum.Location = new System.Drawing.Point(26, 19);
            this.RotXNum.Maximum = new decimal(new int[] {
            360,
            0,
            0,
            0});
            this.RotXNum.Minimum = new decimal(new int[] {
            360,
            0,
            0,
            -2147483648});
            this.RotXNum.Name = "RotXNum";
            this.RotXNum.Size = new System.Drawing.Size(120, 20);
            this.RotXNum.TabIndex = 5;
            this.RotXNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // RotYNum
            // 
            this.RotYNum.DecimalPlaces = 2;
            this.RotYNum.Location = new System.Drawing.Point(26, 45);
            this.RotYNum.Maximum = new decimal(new int[] {
            360,
            0,
            0,
            0});
            this.RotYNum.Minimum = new decimal(new int[] {
            360,
            0,
            0,
            -2147483648});
            this.RotYNum.Name = "RotYNum";
            this.RotYNum.Size = new System.Drawing.Size(120, 20);
            this.RotYNum.TabIndex = 7;
            this.RotYNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(6, 47);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(14, 13);
            this.label8.TabIndex = 6;
            this.label8.Text = "Y";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.PosXNum);
            this.groupBox1.Controls.Add(this.PosYNum);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Location = new System.Drawing.Point(13, 92);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(156, 78);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Position Offset";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 21);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(14, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "X";
            // 
            // PosXNum
            // 
            this.PosXNum.DecimalPlaces = 2;
            this.PosXNum.Location = new System.Drawing.Point(26, 19);
            this.PosXNum.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.PosXNum.Name = "PosXNum";
            this.PosXNum.Size = new System.Drawing.Size(120, 20);
            this.PosXNum.TabIndex = 5;
            this.PosXNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // PosYNum
            // 
            this.PosYNum.DecimalPlaces = 2;
            this.PosYNum.Location = new System.Drawing.Point(26, 45);
            this.PosYNum.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.PosYNum.Name = "PosYNum";
            this.PosYNum.Size = new System.Drawing.Size(120, 20);
            this.PosYNum.TabIndex = 7;
            this.PosYNum.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(6, 47);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(14, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Y";
            // 
            // MeshType
            // 
            this.MeshType.FormattingEnabled = true;
            this.MeshType.Location = new System.Drawing.Point(12, 65);
            this.MeshType.Name = "MeshType";
            this.MeshType.Size = new System.Drawing.Size(157, 21);
            this.MeshType.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 49);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "Mesh Type";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Parent Index";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(641, 400);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.menuStrip1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "ED GUI Creator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.TexturePanel)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ParentNum)).EndInit();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ScaleXNum)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ScaleYNum)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.RotZNum)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RotXNum)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RotYNum)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PosXNum)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PosYNum)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem newToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem saveToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem saveAsToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem optionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem setDirectoryToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem formToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem materialToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem modelToolStripMenuItem;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.PictureBox TexturePanel;
        private System.Windows.Forms.ComboBox MeshType;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolStripMenuItem radiansToolStripMenuItem;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown PosXNum;
        private System.Windows.Forms.NumericUpDown PosYNum;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.NumericUpDown ScaleXNum;
        private System.Windows.Forms.NumericUpDown ScaleYNum;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.NumericUpDown RotZNum;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown RotXNum;
        private System.Windows.Forms.NumericUpDown RotYNum;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.NumericUpDown ParentNum;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem writeLightingValuesToolStripMenuItem;
    }
}

