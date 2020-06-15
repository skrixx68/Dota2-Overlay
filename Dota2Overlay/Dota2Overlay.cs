using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Dota2Overlay
{
    public partial class Dota2Overlay : Form
    {
        string pathDir = "", txtPath = "";
        int x = Screen.PrimaryScreen.Bounds.Width, y = Screen.PrimaryScreen.Bounds.Width;
        //percentage of x location
        int xPsize = 43;

        //initialize form design
        public Dota2Overlay()
        {
            InitializeComponent();
            if (x > 1600) // widescreen
            {
                int szW = lbl_vBe.Size.Width, szH = lbl_vBe.Size.Height;
                lbl_vBe.Size = new System.Drawing.Size(szW + 2, szH + 2);
                xPsize += 2;
            }
            this.Left = getPercent(xPsize, x); // visible
            this.Top = getPercent(2, y);
            this.BackColor = Color.Black;
            this.TransparencyKey = Color.Black;
            this.TopMost = true;

        }

        //initialize
        private void Form1_Load(object sender, EventArgs e)
        {
            // Save exe & txt filepath
            pathDir = Application.ExecutablePath;
            pathDir = pathDir.Substring(0, pathDir.Length - 16);
            txtPath = pathDir + "vBe.txt";

            if (File.Exists(pathDir + "Dota2MemReader.exe"))
            {
                Process.Start(pathDir + "Dota2MemReader.exe");
                timer1.Start();

            }
            else
            {
                MessageBox.Show("Missing Files, Please make sure to extract all files in the same folder and run program as admin.");
                Application.Exit();

            }
        }

        //Draw Function
        bool vBe = false;
        private void timer1_Tick(object sender, EventArgs e)
        {
            vBe = readTxt();
            if (vBe == true)
            {
                this.Left = getPercent(xPsize, x); // visible
                lbl_vBe.ForeColor = Color.Red;
                lbl_vBe.Text = "Visible";

            }
            else
            {
                this.Left = getPercent(xPsize - 3, x); // not visible
                lbl_vBe.ForeColor = Color.LimeGreen;
                lbl_vBe.Text = "Not Visible";

            }
        }

        //Read value of vbe from text.
        private bool readTxt()
        {
            bool bVisible = false;
            try
            {
                string text = File.ReadAllText(txtPath);
                if (text.Equals("1"))
                {
                    bVisible = true;


                }
                else if (text.Equals("0"))
                {
                    bVisible = false;

                }
            }
            catch (System.IO.IOException) { Debug.WriteLine("ERROR"); }

            return bVisible;
        }

        //Exit
        private void Form1_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.End)
            {
                Application.Exit();
            }
        }

        private int getPercent(float percent , float value)
        {
            float temp = (percent / 100) * value;
            int res = Convert.ToInt32(temp);
            return res;
        }
    }
}
