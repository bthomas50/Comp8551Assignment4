using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.InteropServices;

namespace _8551_asgn4
{
    public partial class Blender : Form
    {
        private enum BlendFunc { CPP, MMX, SSE };
        private string originalImageName, blendImageName;
        Bitmap originalImage, blendImage, finalImage;
        private float alphaTick;
        Form2 blendWindow = new Form2();
        Form2 originalWindow = new Form2();
        Form2 finalWindow = new Form2();
        private BlendFunc func;

        public Blender()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            radioButton1.Checked = true;
            alphaTick = 0;
            alphaLabel.Text = "Alpha: " + alphaTick;
            func = BlendFunc.CPP;
            blendWindow.Text = "Blend Image";
            originalWindow.Text = "Original Image";
            finalWindow.Text = "Final Image";
        }

        private void openOriginalButton_Click(object sender, EventArgs e)
        {
            originalImageName = getFilename();
            if (originalImageName == null)
                return;
            try
            {
                originalImage = (Bitmap)Image.FromFile(originalImageName, true);
                finalImage = new Bitmap(originalImage);
                originalImageLabel.Text = "Original: " + originalImageName;
                originalWindow.Size = originalImage.Size;
                originalWindow.Image = (Image)originalImage;
                originalWindow.Show();
            }
            catch (System.IO.FileNotFoundException)
            {
                MessageBox.Show("There was an error opening the bitmap." +
                    "Please check the path.");
            }
            

        }

        private void startBlendButton_Click(object sender, EventArgs e)
        {
            if (originalImage == null || blendImage == null)
                return;
            ImageInfo originalInfo, blendToInfo, finalInfo;
            BitmapData originalBitmap, blendToBitmap, finalBitmap;
            Rectangle blendRect = new Rectangle(0, 0, blendImage.Width, blendImage.Height);
            originalBitmap = originalImage.LockBits(blendRect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            blendToBitmap = blendImage.LockBits(blendRect, ImageLockMode.ReadOnly, PixelFormat.Format32bppArgb);
            finalBitmap = finalImage.LockBits(blendRect, ImageLockMode.ReadWrite, PixelFormat.Format32bppArgb);
            //
            originalInfo.height = originalBitmap.Height;
            originalInfo.width = originalBitmap.Width;
            originalInfo.stride = originalBitmap.Stride;
            originalInfo.linearData = originalBitmap.Scan0;
            //
            blendToInfo.height = blendToBitmap.Height;
            blendToInfo.width = blendToBitmap.Width;
            blendToInfo.stride = blendToBitmap.Stride;
            blendToInfo.linearData = blendToBitmap.Scan0;
            //
            finalInfo.height = finalBitmap.Height;
            finalInfo.width = finalBitmap.Width;
            finalInfo.stride = finalBitmap.Stride;
            finalInfo.linearData = finalBitmap.Scan0;


            int microseconds = -1;
            switch(func)
            {
                case BlendFunc.CPP:
                    microseconds = Blend(ref originalInfo, ref blendToInfo, ref finalInfo, alphaTick);
                    break;
                case BlendFunc.MMX:
                    microseconds = Blend_MMX(ref originalInfo, ref blendToInfo, ref finalInfo, alphaTick);
                    break;
                case BlendFunc.SSE:
                    microseconds = Blend_SSE(ref originalInfo, ref blendToInfo, ref finalInfo, alphaTick);
                    break;
            }

            timeLabel.Text = "Time elapsed: " + microseconds + " microseconds";

            originalImage.UnlockBits(originalBitmap);
            blendImage.UnlockBits(blendToBitmap);
            finalImage.UnlockBits(finalBitmap);

            finalWindow.Size = finalImage.Size;
            finalWindow.Image = finalImage;
            finalWindow.Show();
        }

        private void blendImageButton_Click(object sender, EventArgs e)
        {
            blendImageName = getFilename();
            if (blendImageName == null)
                return;
            try
            {
                blendImage = (Bitmap)Image.FromFile(blendImageName, true);
                blendImageLabel.Text = "Blended: " + blendImageName;
                blendWindow.Size = blendImage.Size;
                blendWindow.Image = blendImage;
                blendWindow.Show();
            }
            catch (System.IO.FileNotFoundException)
            {
                MessageBox.Show("There was an error opening the bitmap." +
                    "Please check the path.");
            }
        }

        private string getFilename()
        {
            OpenFileDialog openFileDialog1 = new OpenFileDialog();

            openFileDialog1.InitialDirectory = "c:\\";
            openFileDialog1.Filter = "All files (*.*)|*.*";
            openFileDialog1.FilterIndex = 1;
            openFileDialog1.RestoreDirectory = true;

            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                return openFileDialog1.FileName;
            }
            return null;
        }

        [DllImport("RealBlender.dll", CharSet = CharSet.Ansi)]
        private extern static int Blend(
            ref ImageInfo original
            , ref ImageInfo blendTo
            , ref ImageInfo finalImage
            , float blend_factor
            );
        [DllImport("RealBlender.dll", CharSet = CharSet.Ansi)]
        private extern static int Blend_MMX(
            ref ImageInfo original
            , ref ImageInfo blendTo
            , ref ImageInfo finalImage
            , float blend_factor
            );
        [DllImport("RealBlender.dll", CharSet = CharSet.Ansi)]
        private extern static int Blend_SSE(
            ref ImageInfo original
            , ref ImageInfo blendTo
            , ref ImageInfo finalImage
            , float blend_factor
            );

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            alphaTick = (float)trackBar1.Value/(float)trackBar1.Maximum;
            alphaLabel.Text = "Alpha: " + alphaTick;
        }

        private void groupBox1_Enter(object sender, EventArgs e)
        {
        
        }

        private void radioButton1_CheckedChanged(object sender, EventArgs e)
        {
            func = BlendFunc.CPP;
        }

        private void radioButton2_CheckedChanged(object sender, EventArgs e)
        {
            func = BlendFunc.MMX;
        }

        private void radioButton3_CheckedChanged(object sender, EventArgs e)
        {
            func = BlendFunc.SSE;
        }
    }
}
