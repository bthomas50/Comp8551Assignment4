namespace _8551_asgn4
{
    partial class Blender
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
            this.startBlendButton = new System.Windows.Forms.Button();
            this.openOriginalButton = new System.Windows.Forms.Button();
            this.blendImageButton = new System.Windows.Forms.Button();
            this.originalImageLabel = new System.Windows.Forms.Label();
            this.blendImageLabel = new System.Windows.Forms.Label();
            this.alphaLabel = new System.Windows.Forms.Label();
            this.trackBar1 = new System.Windows.Forms.TrackBar();
            this.timeLabel = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.radioButton3 = new System.Windows.Forms.RadioButton();
            this.radioButton2 = new System.Windows.Forms.RadioButton();
            this.radioButton1 = new System.Windows.Forms.RadioButton();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // startBlendButton
            // 
            this.startBlendButton.Location = new System.Drawing.Point(26, 272);
            this.startBlendButton.Name = "startBlendButton";
            this.startBlendButton.Size = new System.Drawing.Size(75, 23);
            this.startBlendButton.TabIndex = 0;
            this.startBlendButton.Text = "Blend";
            this.startBlendButton.UseVisualStyleBackColor = true;
            this.startBlendButton.Click += new System.EventHandler(this.startBlendButton_Click);
            // 
            // openOriginalButton
            // 
            this.openOriginalButton.Location = new System.Drawing.Point(124, 271);
            this.openOriginalButton.Name = "openOriginalButton";
            this.openOriginalButton.Size = new System.Drawing.Size(102, 23);
            this.openOriginalButton.TabIndex = 1;
            this.openOriginalButton.Text = "Open Original";
            this.openOriginalButton.UseVisualStyleBackColor = true;
            this.openOriginalButton.Click += new System.EventHandler(this.openOriginalButton_Click);
            // 
            // blendImageButton
            // 
            this.blendImageButton.Location = new System.Drawing.Point(232, 271);
            this.blendImageButton.Name = "blendImageButton";
            this.blendImageButton.Size = new System.Drawing.Size(124, 23);
            this.blendImageButton.TabIndex = 2;
            this.blendImageButton.Text = "Open Blending Image";
            this.blendImageButton.UseVisualStyleBackColor = true;
            this.blendImageButton.Click += new System.EventHandler(this.blendImageButton_Click);
            // 
            // originalImageLabel
            // 
            this.originalImageLabel.AutoSize = true;
            this.originalImageLabel.Location = new System.Drawing.Point(23, 9);
            this.originalImageLabel.Name = "originalImageLabel";
            this.originalImageLabel.Size = new System.Drawing.Size(64, 13);
            this.originalImageLabel.TabIndex = 3;
            this.originalImageLabel.Text = "path original";
            this.originalImageLabel.Click += new System.EventHandler(this.label1_Click);
            // 
            // blendImageLabel
            // 
            this.blendImageLabel.AutoSize = true;
            this.blendImageLabel.Location = new System.Drawing.Point(23, 35);
            this.blendImageLabel.Name = "blendImageLabel";
            this.blendImageLabel.Size = new System.Drawing.Size(57, 13);
            this.blendImageLabel.TabIndex = 4;
            this.blendImageLabel.Text = "path blend";
            // 
            // alphaLabel
            // 
            this.alphaLabel.AutoSize = true;
            this.alphaLabel.Location = new System.Drawing.Point(23, 204);
            this.alphaLabel.Name = "alphaLabel";
            this.alphaLabel.Size = new System.Drawing.Size(33, 13);
            this.alphaLabel.TabIndex = 5;
            this.alphaLabel.Text = "alpha";
            // 
            // trackBar1
            // 
            this.trackBar1.Location = new System.Drawing.Point(26, 220);
            this.trackBar1.Name = "trackBar1";
            this.trackBar1.Size = new System.Drawing.Size(104, 45);
            this.trackBar1.TabIndex = 6;
            this.trackBar1.Scroll += new System.EventHandler(this.trackBar1_Scroll);
            // 
            // timeLabel
            // 
            this.timeLabel.AutoSize = true;
            this.timeLabel.Location = new System.Drawing.Point(23, 59);
            this.timeLabel.Name = "timeLabel";
            this.timeLabel.Size = new System.Drawing.Size(0, 13);
            this.timeLabel.TabIndex = 7;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.radioButton3);
            this.groupBox1.Controls.Add(this.radioButton2);
            this.groupBox1.Controls.Add(this.radioButton1);
            this.groupBox1.Location = new System.Drawing.Point(190, 59);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(200, 100);
            this.groupBox1.TabIndex = 8;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "groupBox1";
            this.groupBox1.Enter += new System.EventHandler(this.groupBox1_Enter);
            // 
            // radioButton3
            // 
            this.radioButton3.AutoSize = true;
            this.radioButton3.Location = new System.Drawing.Point(7, 66);
            this.radioButton3.Name = "radioButton3";
            this.radioButton3.Size = new System.Drawing.Size(46, 17);
            this.radioButton3.TabIndex = 2;
            this.radioButton3.TabStop = true;
            this.radioButton3.Text = "SSE";
            this.radioButton3.UseVisualStyleBackColor = true;
            this.radioButton3.CheckedChanged += new System.EventHandler(this.radioButton3_CheckedChanged);
            // 
            // radioButton2
            // 
            this.radioButton2.AutoSize = true;
            this.radioButton2.Location = new System.Drawing.Point(7, 42);
            this.radioButton2.Name = "radioButton2";
            this.radioButton2.Size = new System.Drawing.Size(50, 17);
            this.radioButton2.TabIndex = 1;
            this.radioButton2.TabStop = true;
            this.radioButton2.Text = "MMX";
            this.radioButton2.UseVisualStyleBackColor = true;
            this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton2_CheckedChanged);
            // 
            // radioButton1
            // 
            this.radioButton1.AutoSize = true;
            this.radioButton1.Location = new System.Drawing.Point(7, 18);
            this.radioButton1.Name = "radioButton1";
            this.radioButton1.RightToLeft = System.Windows.Forms.RightToLeft.No;
            this.radioButton1.Size = new System.Drawing.Size(44, 17);
            this.radioButton1.TabIndex = 0;
            this.radioButton1.TabStop = true;
            this.radioButton1.Text = "C++";
            this.radioButton1.UseVisualStyleBackColor = true;
            this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton1_CheckedChanged);
            // 
            // Blender
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(402, 334);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.timeLabel);
            this.Controls.Add(this.trackBar1);
            this.Controls.Add(this.alphaLabel);
            this.Controls.Add(this.blendImageLabel);
            this.Controls.Add(this.originalImageLabel);
            this.Controls.Add(this.blendImageButton);
            this.Controls.Add(this.openOriginalButton);
            this.Controls.Add(this.startBlendButton);
            this.Name = "Blender";
            this.Text = "Blender";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button startBlendButton;
        private System.Windows.Forms.Button openOriginalButton;
        private System.Windows.Forms.Button blendImageButton;
        private System.Windows.Forms.Label originalImageLabel;
        private System.Windows.Forms.Label blendImageLabel;
        private System.Windows.Forms.Label alphaLabel;
        private System.Windows.Forms.TrackBar trackBar1;
        private System.Windows.Forms.Label timeLabel;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton radioButton3;
        private System.Windows.Forms.RadioButton radioButton2;
        private System.Windows.Forms.RadioButton radioButton1;
    }
}

