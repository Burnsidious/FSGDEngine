using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ED_GUI_Creator
{
    public class CSettings
    {
        string m_szModelDir;
        public string ModelDirectory
        {
            get { return m_szModelDir; }
            set { m_szModelDir = value; }
        }

        string m_szMatDir;
        public string MaterialDirectory
        {
            get { return m_szMatDir; }
            set { m_szMatDir = value; }
        }
        
        string m_szFormDir;
        public string FormDirectory
        {
            get { return m_szFormDir; }
            set { m_szFormDir = value; }
        }

        string m_szTextureDir;
        public string TextureDirectory
        {
            get { return m_szTextureDir; }
            set { m_szTextureDir = value; }
        }

        string m_szXMLFormName;
        public string XMLFormName
        {
            get { return m_szXMLFormName; }
            set { m_szXMLFormName = value; }
        }

        string m_szXMLMaterialName;
        public string XMLMaterialName
        {
            get { return m_szXMLMaterialName; }
            set { m_szXMLMaterialName = value; }
        }

        bool m_bRadians;
        public bool Radians
        {
            get { return m_bRadians; }
            set { m_bRadians = value; }
        }

        bool m_bWriteLighting;
        public bool WriteLighting
        {
            get { return m_bWriteLighting; }
            set { m_bWriteLighting = value; }
        }

        public CSettings()
        {
            m_szModelDir = "Model";
            m_szMatDir = "Material";
            m_szFormDir = "Form";
            m_szTextureDir = "Textures";
            m_szXMLFormName = "mat_GUITest";
            m_szXMLMaterialName = "GUITest";
        }
    }
}
