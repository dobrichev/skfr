using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace skfrDllTest

{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void buttonRate_Click(object sender, EventArgs e)
        {
            int er, ep, ed, aig, status;
            try
            {
                status = skfrDll.ratePuzzle(sourcePuzzle.Text, out er, out ep, out ed, out aig);
            }
            catch (Exception ex)
            {
                ratingText.Text = "Exception: " + ex.Message;
                return;
            }
            ratingText.Text = "SKFR=" + er + "/" + ep + "/" + ed + " aig=" + aig + " ret=" + status;
        }
    }
    //extern "C" DLLEXPORT int __stdcall ratePuzzle(char *ze, int * er, int * ep, int * ed, int * aig);

    public class skfrDll
    {
        [DllImport("skfrdll.dll", CharSet = CharSet.Ansi)]
        public static extern int ratePuzzle(String text, out int er, out int ep, out int ed, out int aig);
    }
}