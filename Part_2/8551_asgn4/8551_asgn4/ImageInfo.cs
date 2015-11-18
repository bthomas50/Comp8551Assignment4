using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace _8551_asgn4
{
    [StructLayout (LayoutKind.Sequential)]
    public struct ImageInfo
    {
        public IntPtr linearData;
        public int height;
        public int stride; 
        public int width;
    }
}
