namespace _8551_asgn4
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
            this.startBlendButton = new System.Windows.Forms.Button();
            this.openOriginalButton = new System.Windows.Forms.Button();
            this.blendImageButton = new System.Windows.Forms.Button();
            this.originalPictureBox = new System.Windows.Forms.PictureBox();
            this.finalPictureBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.originalPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.finalPictureBox)).BeginInit();
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
            // originalPictureBox
            // 
            this.originalPictureBox.Location = new System.Drawing.Point(26, 25);
            this.originalPictureBox.Name = "originalPictureBox";
            this.originalPictureBox.Size = new System.Drawing.Size(135, 142);
            this.originalPictureBox.TabIndex = 3;
            this.originalPictureBox.TabStop = false;
            // 
            // finalPictureBox
            // 
            this.finalPictureBox.Location = new System.Drawing.Point(199, 25);
            this.finalPictureBox.Name = "finalPictureBox";
            this.finalPictureBox.Size = new System.Drawing.Size(157, 142);
            this.finalPictureBox.TabIndex = 4;
            this.finalPictureBox.TabStop = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(402, 334);
            this.Controls.Add(this.finalPictureBox);
            this.Controls.Add(this.originalPictureBox);
            this.Controls.Add(this.blendImageButton);
            this.Controls.Add(this.openOriginalButton);
            this.Controls.Add(this.startBlendButton);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.originalPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.finalPictureBox)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button startBlendButton;
        private System.Windows.Forms.Button openOriginalButton;
        private System.Windows.Forms.Button blendImageButton;
        private System.Windows.Forms.PictureBox originalPictureBox;
        private System.Windows.Forms.PictureBox finalPictureBox;
    }
}

