using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace testImportdll
{
    class Program
    {
        [DllImport("ScreenSH.dll")]
        public static extern void Message();
        static void Main(string[] args)
        {
            
            Message();
            Console.ReadKey();
        }
    }
}
