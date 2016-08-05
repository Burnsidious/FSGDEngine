using System;
using System.Runtime.InteropServices;

namespace EngineInterface
{
    public class Engine
    {
        [DllImport("EditorInterface.dll")]
        public static extern void Initialize(IntPtr hWnd);

        [DllImport("EditorInterface.dll")]
        public static extern void Paint();

        [DllImport("EditorInterface.dll")]
        public static extern void ShutDown();
    }
}
