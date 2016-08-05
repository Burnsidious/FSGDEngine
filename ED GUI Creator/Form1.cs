using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Xml.Serialization;
using System.Xml.Linq;

namespace ED_GUI_Creator
{
    public partial class Form1 : Form
    {
        // Settings for tool
        CSettings m_Settings;

        // Current file for save
        string m_CurrFile;

        // Current texture name
        string m_Texture;
        string m_TexturePath;
        
        public Form1()
        {
            InitializeComponent();

            LoadSettings();

            MeshType.Items.Add("Quad");
            MeshType.SelectedIndex = 0;

            try
            {
                TexturePanel.Image = Image.FromFile("Textures\\default.png");
                m_Texture = "default.png";
                m_TexturePath = "Textures\\default.png";
            }
            catch (System.IO.FileNotFoundException)
            {
                OpenFileDialog dlg = new OpenFileDialog();
                dlg.Filter = "Bitmap Image Files(*.bmp)|*.bmp|PNG Image Files(*.png)|*.png";
                dlg.FilterIndex = 2;
                dlg.InitialDirectory = m_Settings.TextureDirectory;

                if (DialogResult.OK == dlg.ShowDialog(this))
                {
                    TexturePanel.Image = Image.FromFile(dlg.FileName.ToString());

                    m_Texture = dlg.SafeFileName;
                    m_TexturePath = dlg.FileName;
                    int dirLength = m_TexturePath.Length - m_Texture.Length - 2;
                    m_Settings.TextureDirectory = m_TexturePath.Remove(dirLength);
                }
            }
        }

        // Change form directory
        private void ChangeFormDirectory()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            //dlg.RootFolder = System.Environment.SpecialFolder.Desktop;
            dlg.Description = "Set the folder to put the form files";

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                m_Settings.FormDirectory = dlg.SelectedPath;
            }
        }

        // Change material directory
        private void ChangeMaterialDirectory()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            //dlg.RootFolder = System.Environment.SpecialFolder.Desktop;
            dlg.Description = "Set the folder to put the material files";

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                m_Settings.MaterialDirectory = dlg.SelectedPath;
            }
        }
        
        // Change model directory
        private void ChangeModelDirectory()
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            //dlg.RootFolder = System.Environment.SpecialFolder.Desktop;
            dlg.Description = "Set the folder to put the model files";

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                m_Settings.ModelDirectory = dlg.SelectedPath;
            }
        }
        
        // Loads in directories
        private void LoadSettings()
        {
            m_Settings = new CSettings();
            XmlSerializer deserializer = new XmlSerializer(typeof(CSettings));
            TextReader reader;

            try
            {
                reader = new StreamReader(@"settings.xml");
            }
            catch (System.IO.FileNotFoundException)
            {
                ChangeFormDirectory();
                ChangeMaterialDirectory();
                ChangeModelDirectory();
                return;
            }

            m_Settings = (CSettings)deserializer.Deserialize(reader);
            reader.Close();
        }

        // Save out directories
        private void SaveSettings()
        {
            XmlSerializer serializer = new XmlSerializer(typeof(CSettings));
            TextWriter writer = new StreamWriter(@"settings.xml");
            serializer.Serialize(writer, m_Settings);
            writer.Close();
        }

        // Save out form file
        private void SaveFormXML(string _Filename, string _Path)
        {
            // Create the root
            XElement root = new XElement("GDForm");
            XAttribute name = new XAttribute("name", m_Settings.XMLFormName);
            root.Add(name);

            // Write out the mesh file
            XElement meshfile = new XElement("GDMeshFile");
            switch(MeshType.SelectedIndex)
            {
                case 0:
                {
                    meshfile.Value = "GDMesh/ScreenQuad/ScreenQuadShape.xml";
                    break;
                }
            }
            root.Add(meshfile);

            // Write out the material file
            XElement materialfile = new XElement("GDMaterialFile");
            materialfile.Value = "GDMaterial/mat_" + _Path;
            root.Add(materialfile);

            // Write out the render context file
            XElement contextfile = new XElement("GDRenderContextFile");
            contextfile.Value = "GUI";
            root.Add(contextfile);

            // Write out the render shape type
            XElement shapetype = new XElement("RenderShapeType");
            shapetype.Value = "RenderShapeGUI";
            root.Add(shapetype);

            // Save it out
            root.Save(_Filename);
        }

        // Save out material file
        private void SaveMaterialXML(string _Filename, string _Path)
        {
            XElement root = new XElement("GDMaterial");
            XAttribute name = new XAttribute("name", m_Settings.XMLMaterialName);
            root.Add(name);

            // Write out the diffuse map
            XElement diffusemap = new XElement("Diffuse_Map");
            diffusemap.Value = m_Texture;
            root.Add(diffusemap);

            XElement diffusepath = new XElement("Diffuse_Map_Path");
            diffusepath.Value = m_TexturePath;
            root.Add(diffusepath);

            if (m_Settings.WriteLighting)
            {
                // Write out the ambient
                XElement ambient = new XElement("Ambient");
                root.Add(ambient);

                XElement red = new XElement("r");
                red.Value = Convert.ToString(0);
                ambient.Add(red);

                XElement green = new XElement("g");
                green.Value = Convert.ToString(0);
                ambient.Add(green);

                XElement blue = new XElement("b");
                blue.Value = Convert.ToString(0);
                ambient.Add(blue);

                XElement alpha = new XElement("a");
                alpha.Value = Convert.ToString(1);
                ambient.Add(alpha);

                // Write out the diffuse
                XElement diffuse = new XElement("Diffuse");
                root.Add(diffuse);

                diffuse.Add(red);
                diffuse.Add(green);
                diffuse.Add(blue);
                diffuse.Add(alpha);

                // Write out the emmissive
                XElement emmissive = new XElement("Emmisive");
                root.Add(emmissive);

                emmissive.Add(red);
                emmissive.Add(green);
                emmissive.Add(blue);
                emmissive.Add(alpha);

                // Write out the reflected
                XElement reflected = new XElement("Reflected");
                root.Add(reflected);

                reflected.Add(red);
                reflected.Add(green);
                reflected.Add(blue);
                reflected.Add(alpha);

                // Write out the specular
                XElement specular = new XElement("Specular");
                root.Add(specular);

                red.Value = Convert.ToString(0.5);
                specular.Add(red);

                green.Value = Convert.ToString(0.5);
                specular.Add(green);

                blue.Value = Convert.ToString(0.5);
                specular.Add(blue);
            }

            // Save it out
            root.Save(_Filename);
        }

        private void OpenMaterialXML(string _Path)
        {
            XElement root = XElement.Load(_Path);

            // Pull the diffuse map
            m_Texture = root.Element("Diffuse_Map").Value;
            // Pull the map's path
            m_TexturePath = root.Element("Diffuse_Map_Path").Value;

            // Load in the texture
            TexturePanel.Image = Image.FromFile(m_TexturePath);
        }

        // Save out model file
        private void SaveModelXML(string _Filename, string _Path)
        {
            XElement root = new XElement("GDModel");
            XElement modelpart = new XElement("GDModelPart");
            root.Add(modelpart);

            // Write out the parent's index
            XElement parentindex = new XElement("ParentIndex");
            parentindex.Value = Convert.ToString(ParentNum.Value);
            modelpart.Add(parentindex);

            // Create the matrix
            CMatrix4x4 matrix = new CMatrix4x4();

            // Rotate
            if (radiansToolStripMenuItem.Checked)
            {
                matrix.RotateLocalX((float)RotXNum.Value);
                matrix.RotateLocalY((float)RotYNum.Value);
                matrix.RotateLocalZ((float)RotZNum.Value);
            }
            else
            {
                matrix.RotateLocalX((float)Math.PI * (float)RotXNum.Value / 180.0f);
                matrix.RotateLocalY((float)Math.PI * (float)RotYNum.Value / 180.0f);
                matrix.RotateLocalZ((float)Math.PI * (float)RotZNum.Value / 180.0f);
            }

            // Translate
            matrix.Translate((float)PosXNum.Value, (float)PosYNum.Value);

            // Scale
            matrix.Scale((float)ScaleXNum.Value, (float)ScaleYNum.Value);

            // Write out the local transform
            XElement localtrans = new XElement("LocalTransform");
            modelpart.Add(localtrans);
          
            // Write out the X Axis
            XElement xaxis = new XElement("XAxis");
            localtrans.Add(xaxis);

            XElement xx = new XElement("x");
            xx.Value = Convert.ToString(matrix.GetIndex(0, 0));
            xaxis.Add(xx);

            XElement xy = new XElement("y");
            xy.Value = Convert.ToString(matrix.GetIndex(0, 1));
            xaxis.Add(xy);

            XElement xz = new XElement("z");
            xz.Value = Convert.ToString(matrix.GetIndex(0, 2));
            xaxis.Add(xz);

            XElement xw = new XElement("w");
            xw.Value = Convert.ToString(matrix.GetIndex(0, 3));
            xaxis.Add(xw);

            // Write out the Y Axis
            XElement yaxis = new XElement("YAxis");
            localtrans.Add(yaxis);

            XElement yx = new XElement("x");
            yx.Value = Convert.ToString(matrix.GetIndex(1, 0));
            yaxis.Add(yx);

            XElement yy = new XElement("y");
            yy.Value = Convert.ToString(matrix.GetIndex(1, 1));
            yaxis.Add(yy);

            XElement yz = new XElement("z");
            yz.Value = Convert.ToString(matrix.GetIndex(1, 2));
            yaxis.Add(yz);

            XElement yw = new XElement("w");
            yw.Value = Convert.ToString(matrix.GetIndex(1, 3));
            yaxis.Add(yw);

            // Write out the Z Axis
            XElement zaxis = new XElement("ZAxis");
            localtrans.Add(zaxis);

            XElement zx = new XElement("x");
            zx.Value = Convert.ToString(matrix.GetIndex(2, 0));
            zaxis.Add(zx);

            XElement zy = new XElement("y");
            zy.Value = Convert.ToString(matrix.GetIndex(2, 1));
            zaxis.Add(zy);

            XElement zz = new XElement("z");
            zz.Value = Convert.ToString(matrix.GetIndex(2, 2));
            zaxis.Add(zz);

            XElement zw = new XElement("w");
            zw.Value = Convert.ToString(matrix.GetIndex(2, 3));
            zaxis.Add(zw);

            // Write out the W Axis
            XElement waxis = new XElement("WAxis");
            localtrans.Add(waxis);

            XElement wx = new XElement("x");
            wx.Value = Convert.ToString(matrix.GetIndex(3, 0));
            waxis.Add(wx);

            XElement wy = new XElement("y");
            wy.Value = Convert.ToString(matrix.GetIndex(3, 1));
            waxis.Add(wy);

            XElement wz = new XElement("z");
            wz.Value = Convert.ToString(0.9);
            waxis.Add(wz);

            XElement ww = new XElement("w");
            ww.Value = Convert.ToString(matrix.GetIndex(3, 3));
            waxis.Add(ww);

            // Write out the form file
            XElement formfile = new XElement("GDFormFile");
            formfile.Value = "GDForm/frm_" + _Path;
            modelpart.Add(formfile);

            // Save it out
            root.Save(_Filename);
        }

        private void OpenModelXML(string _Path)
        {
            XElement root = XElement.Load(_Path);

            // Pull the model part
            XElement modelpart = root.Element("GDModelPart");

            // Pull the parent index
            ParentNum.Value = Convert.ToInt32(modelpart.Element("ParentIndex").Value);

            // Pull the matrix
            CMatrix4x4 matrix = new CMatrix4x4();
            XElement transform = modelpart.Element("LocalTransform");

            // Pull the X Axis
            XElement xaxis = transform.Element("XAxis");

            matrix.SetIndex(0, 0, (float)Convert.ToDouble(xaxis.Element("x").Value));
            matrix.SetIndex(0, 1, (float)Convert.ToDouble(xaxis.Element("y").Value));
            matrix.SetIndex(0, 2, (float)Convert.ToDouble(xaxis.Element("z").Value));
            matrix.SetIndex(0, 3, (float)Convert.ToDouble(xaxis.Element("w").Value));

            // Pull the Y Axis
            XElement yaxis = transform.Element("YAxis");

            matrix.SetIndex(1, 0, (float)Convert.ToDouble(yaxis.Element("x").Value));
            matrix.SetIndex(1, 1, (float)Convert.ToDouble(yaxis.Element("y").Value));
            matrix.SetIndex(1, 2, (float)Convert.ToDouble(yaxis.Element("z").Value));
            matrix.SetIndex(1, 3, (float)Convert.ToDouble(yaxis.Element("w").Value));

            // Pull the Z Axis
            XElement zaxis = transform.Element("ZAxis");

            matrix.SetIndex(2, 0, (float)Convert.ToDouble(zaxis.Element("x").Value));
            matrix.SetIndex(2, 1, (float)Convert.ToDouble(zaxis.Element("y").Value));
            matrix.SetIndex(2, 2, (float)Convert.ToDouble(zaxis.Element("z").Value));
            matrix.SetIndex(2, 3, (float)Convert.ToDouble(zaxis.Element("w").Value));

            // Pull the W Axis
            XElement waxis = transform.Element("WAxis");

            matrix.SetIndex(3, 0, (float)Convert.ToDouble(waxis.Element("x").Value));
            matrix.SetIndex(3, 1, (float)Convert.ToDouble(waxis.Element("y").Value));
            matrix.SetIndex(3, 2, (float)Convert.ToDouble(waxis.Element("z").Value));
            matrix.SetIndex(3, 3, (float)Convert.ToDouble(waxis.Element("w").Value));

            float y1, y2, x1, x2, z1, z2;

            // Convert back to radians
            if (matrix.GetIndex(3, 1) != 1 && matrix.GetIndex(3, 1) != -1)
            {
                y1 = (float)-Math.Asin(matrix.GetIndex(3, 1));
                y2 = (float)Math.PI - y1;

                float cosy1 = (float)Math.Cos(y1);
                float cosy2 = (float)Math.Cos(y2);

                x1 = (float)Math.Atan2(matrix.GetIndex(3, 2) / cosy1, matrix.GetIndex(3, 3) / cosy1);
                x2 = (float)Math.Atan2(matrix.GetIndex(3, 2) / cosy2, matrix.GetIndex(3, 3) / cosy2);

                z1 = (float)Math.Atan2(matrix.GetIndex(2, 1) / cosy1, matrix.GetIndex(1, 1) / cosy1);
                z2 = (float)Math.Atan2(matrix.GetIndex(2, 1) / cosy2, matrix.GetIndex(1, 1) / cosy2);
            }
            else
            {
                z2 = 0.0f;

                if (matrix.GetIndex(3, 1) == -1)
                {
                    y2 = (float)Math.PI / 2.0f;
                    x2 = (float)Math.Atan2(matrix.GetIndex(1, 2), matrix.GetIndex(1, 3));
                }
                else
                {
                    y2 = (float)-Math.PI / 2.0f;
                    x2 = (float)Math.Atan2(-matrix.GetIndex(1, 2), -matrix.GetIndex(1, 3));
                }
            }

            if (radiansToolStripMenuItem.Checked)
            {
                RotXNum.Value = (decimal)x2;
                RotYNum.Value = (decimal)y2;
                RotZNum.Value = (decimal)z2;
            }
            else
            {
                RotXNum.Value = (decimal)(x2 * 180.0f / (float)Math.PI);
                RotYNum.Value = (decimal)(y2 * 180.0f / (float)Math.PI);
                RotZNum.Value = (decimal)(z2 * 180.0f / (float)Math.PI);
            }
        }

        // Loads in a new texture
        private void pictureBox1_DoubleClick(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Bitmap Image Files(*.bmp)|*.bmp|PNG Image Files(*.png)|*.png";
            dlg.FilterIndex = 2;
            dlg.InitialDirectory = m_Settings.TextureDirectory;

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                TexturePanel.Image = Image.FromFile(dlg.FileName.ToString());

                m_Texture = dlg.SafeFileName;
                m_TexturePath = dlg.FileName;
                int dirLength = m_TexturePath.Length - m_Texture.Length - 2;
                m_Settings.TextureDirectory = m_TexturePath.Remove(dirLength);
            }
        }

        // Zeros out tool
        private void newToolStripMenuItem_Click(object sender, EventArgs e)
        {
            TexturePanel.Image = null;

            ParentNum.Value = -1;
            MeshType.SelectedIndex = 0;

            PosXNum.Value = 0;
            PosYNum.Value = 0;

            RotXNum.Value = 0;
            RotYNum.Value = 0;
            RotZNum.Value = 0;

            ScaleXNum.Value = 1;
            ScaleYNum.Value = 1;
        }

        // Opens XML file
        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "XML Files (*xml)|*.xml";
            dlg.InitialDirectory = m_Settings.FormDirectory;

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                // Determine which file has been opened
                XElement root = XElement.Load(dlg.FileName);

                // Make sure we opened up a good file
                if (root.Name != "GDForm" && root.Name != "GDMaterial" &&
                    root.Name != "GDModel")
                {
                    // Trying to open up a bad file
                }
                else
                {
                    // Pull the true filename
                    string filename = dlg.SafeFileName;
                    m_CurrFile = filename.Remove(0, 4);

                    OpenMaterialXML(m_Settings.MaterialDirectory + "\\mat_" + m_CurrFile);
                    OpenModelXML(m_Settings.ModelDirectory + "\\mdl_" + m_CurrFile);
                }
            }
        }

        // Saves to current XML file
        private void saveToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (m_CurrFile != "" && m_CurrFile != null)
            {                
                SaveFormXML(m_Settings.FormDirectory + "\\frm_" + m_CurrFile, m_CurrFile);
                SaveMaterialXML(m_Settings.MaterialDirectory + "\\mat_" + m_CurrFile, m_CurrFile);
                SaveModelXML(m_Settings.ModelDirectory + "\\mdl_" + m_CurrFile, m_CurrFile);
            }
            else
            {
                saveAsToolStripMenuItem_Click(sender, e);
            }
        }

        // Saves to specified XML file
        private void saveAsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "XML Files(*.xml)|*.xml";
            dlg.InitialDirectory = m_Settings.ModelDirectory;
            dlg.AddExtension = true;

            if (DialogResult.OK == dlg.ShowDialog(this))
            {
                FileInfo info = new FileInfo(dlg.FileName);
                
                // See if the user has already added the prefix
                string prefix = info.Name.Remove(4);

                if (prefix == "mdl_")
                {
                    SaveFormXML(m_Settings.FormDirectory + "\\" + info.Name, info.Name);
                    SaveMaterialXML(m_Settings.MaterialDirectory + "\\" + info.Name, info.Name);
                    SaveModelXML(m_Settings.ModelDirectory + "\\" + info.Name, info.Name);
                }
                else
                {
                    SaveFormXML(m_Settings.FormDirectory + "\\frm_" + info.Name, info.Name);
                    SaveMaterialXML(m_Settings.MaterialDirectory + "\\mat_" + info.Name, info.Name);
                    SaveModelXML(m_Settings.ModelDirectory + "\\mdl_" + info.Name, info.Name);
                }
            }
        }

        // Exits the tool
        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        // Changes the form directory
        private void formToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeFormDirectory();
        }

        // Changes the material directory
        private void materialToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeMaterialDirectory();
        }

        // Changes the model directory
        private void modelToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ChangeModelDirectory();
        }

        // Final things to do before closing
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            SaveSettings();
        }

        // Sets radians setting
        private void radiansToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_Settings.Radians = radiansToolStripMenuItem.Checked;
        }

        // Sets write lighting setting
        private void writeLightingValuesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            m_Settings.WriteLighting = writeLightingValuesToolStripMenuItem.Checked;
        }

        private void TexturePanel_MouseHover(object sender, EventArgs e)
        {
            ToolTip text = new ToolTip();
            text.SetToolTip(TexturePanel, "Double-click to open texture");
        }
    }
}