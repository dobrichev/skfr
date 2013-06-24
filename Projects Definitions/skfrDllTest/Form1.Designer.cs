namespace skfrDllTest
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
            this.buttonRate = new System.Windows.Forms.Button();
            this.sourcePuzzle = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.ratingText = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // buttonRate
            // 
            this.buttonRate.Location = new System.Drawing.Point(426, 61);
            this.buttonRate.Name = "buttonRate";
            this.buttonRate.Size = new System.Drawing.Size(75, 23);
            this.buttonRate.TabIndex = 0;
            this.buttonRate.Text = "Rate It";
            this.buttonRate.UseVisualStyleBackColor = true;
            this.buttonRate.Click += new System.EventHandler(this.buttonRate_Click);
            // 
            // sourcePuzzle
            // 
            this.sourcePuzzle.Location = new System.Drawing.Point(12, 61);
            this.sourcePuzzle.MaxLength = 81;
            this.sourcePuzzle.Name = "sourcePuzzle";
            this.sourcePuzzle.Size = new System.Drawing.Size(395, 20);
            this.sourcePuzzle.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(309, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(112, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "skfr DLL example form";
            // 
            // ratingText
            // 
            this.ratingText.Location = new System.Drawing.Point(522, 61);
            this.ratingText.MaxLength = 20;
            this.ratingText.Name = "ratingText";
            this.ratingText.ReadOnly = true;
            this.ratingText.Size = new System.Drawing.Size(270, 20);
            this.ratingText.TabIndex = 3;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(804, 116);
            this.Controls.Add(this.ratingText);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.sourcePuzzle);
            this.Controls.Add(this.buttonRate);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonRate;
        private System.Windows.Forms.TextBox sourcePuzzle;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox ratingText;
    }
}

