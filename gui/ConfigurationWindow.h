#pragma once

#include "globals.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace newswatcher {

	/// <summary>
	/// Summary for ConfigurationWindow
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ConfigurationWindow : public System::Windows::Forms::Form
	{
	public:
		ConfigurationWindow(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

    void load_configuration()
    {
      char *toto = "toto";
      textBoxFeedsFile->Text = System::Runtime::InteropServices::Marshal::PtrToStringAnsi((int) toto);
    }

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~ConfigurationWindow()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Button^  okButton;
  protected: 
  private: System::Windows::Forms::Button^  cancelButton;
  private: System::Windows::Forms::TabControl^  tabConfiguration;


  private: System::Windows::Forms::TabPage^  tabPageFiles;
  private: System::Windows::Forms::TextBox^  textBoxFeedsFile;

  private: System::Windows::Forms::Label^  labelFeedFile;
  private: System::Windows::Forms::TabPage^  tabPageParameters;

  private: System::Windows::Forms::OpenFileDialog^  openFileDialogConfiguration;

  private: System::Windows::Forms::Button^  buttonBrowseAnalyzedFile;

  private: System::Windows::Forms::TextBox^  textBoxAnalyzedDB;

  private: System::Windows::Forms::Label^  label2;
  private: System::Windows::Forms::Button^  buttonBrowseArticleFile;

  private: System::Windows::Forms::TextBox^  textBoxArticleDB;

  private: System::Windows::Forms::Label^  label1;
  private: System::Windows::Forms::Button^  buttonBrowseFeedsFile;
  private: System::Windows::Forms::TabPage^  tabPageFeeds;
  private: System::Windows::Forms::Label^  labelWatch;
  private: System::Windows::Forms::Button^  buttonBrowseCookies;
  private: System::Windows::Forms::TextBox^  textBoxCookie;
  private: System::Windows::Forms::Label^  labelCookieFile;
  private: System::Windows::Forms::TextBox^  textBox1;
  private: System::Windows::Forms::TreeView^  treeView1;
  private: System::Windows::Forms::Button^  button3;
  private: System::Windows::Forms::Button^  button2;
  private: System::Windows::Forms::Button^  buttonAdd;
  private: System::Windows::Forms::TextBox^  textBox2;


  private: System::ComponentModel::IContainer^  components;


  protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->okButton = (gcnew System::Windows::Forms::Button());
      this->cancelButton = (gcnew System::Windows::Forms::Button());
      this->tabConfiguration = (gcnew System::Windows::Forms::TabControl());
      this->tabPageFiles = (gcnew System::Windows::Forms::TabPage());
      this->tabPageParameters = (gcnew System::Windows::Forms::TabPage());
      this->labelFeedFile = (gcnew System::Windows::Forms::Label());
      this->textBoxFeedsFile = (gcnew System::Windows::Forms::TextBox());
      this->openFileDialogConfiguration = (gcnew System::Windows::Forms::OpenFileDialog());
      this->buttonBrowseFeedsFile = (gcnew System::Windows::Forms::Button());
      this->buttonBrowseArticleFile = (gcnew System::Windows::Forms::Button());
      this->textBoxArticleDB = (gcnew System::Windows::Forms::TextBox());
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->buttonBrowseAnalyzedFile = (gcnew System::Windows::Forms::Button());
      this->textBoxAnalyzedDB = (gcnew System::Windows::Forms::TextBox());
      this->label2 = (gcnew System::Windows::Forms::Label());
      this->tabPageFeeds = (gcnew System::Windows::Forms::TabPage());
      this->labelWatch = (gcnew System::Windows::Forms::Label());
      this->textBox1 = (gcnew System::Windows::Forms::TextBox());
      this->buttonBrowseCookies = (gcnew System::Windows::Forms::Button());
      this->textBoxCookie = (gcnew System::Windows::Forms::TextBox());
      this->labelCookieFile = (gcnew System::Windows::Forms::Label());
      this->treeView1 = (gcnew System::Windows::Forms::TreeView());
      this->textBox2 = (gcnew System::Windows::Forms::TextBox());
      this->buttonAdd = (gcnew System::Windows::Forms::Button());
      this->button2 = (gcnew System::Windows::Forms::Button());
      this->button3 = (gcnew System::Windows::Forms::Button());
      this->tabConfiguration->SuspendLayout();
      this->tabPageFiles->SuspendLayout();
      this->tabPageParameters->SuspendLayout();
      this->tabPageFeeds->SuspendLayout();
      this->SuspendLayout();
      // 
      // okButton
      // 
      this->okButton->Location = System::Drawing::Point(151, 351);
      this->okButton->Name = L"okButton";
      this->okButton->Size = System::Drawing::Size(80, 24);
      this->okButton->TabIndex = 1;
      this->okButton->Text = L"&Ok";
      this->okButton->UseVisualStyleBackColor = true;
      // 
      // cancelButton
      // 
      this->cancelButton->Location = System::Drawing::Point(237, 351);
      this->cancelButton->Name = L"cancelButton";
      this->cancelButton->Size = System::Drawing::Size(79, 24);
      this->cancelButton->TabIndex = 2;
      this->cancelButton->Text = L"&Cancel";
      this->cancelButton->UseVisualStyleBackColor = true;
      // 
      // tabConfiguration
      // 
      this->tabConfiguration->Controls->Add(this->tabPageFiles);
      this->tabConfiguration->Controls->Add(this->tabPageParameters);
      this->tabConfiguration->Controls->Add(this->tabPageFeeds);
      this->tabConfiguration->Location = System::Drawing::Point(6, 6);
      this->tabConfiguration->Name = L"tabConfiguration";
      this->tabConfiguration->SelectedIndex = 0;
      this->tabConfiguration->Size = System::Drawing::Size(310, 339);
      this->tabConfiguration->TabIndex = 3;
      // 
      // tabPageFiles
      // 
      this->tabPageFiles->Controls->Add(this->buttonBrowseCookies);
      this->tabPageFiles->Controls->Add(this->textBoxCookie);
      this->tabPageFiles->Controls->Add(this->labelCookieFile);
      this->tabPageFiles->Controls->Add(this->buttonBrowseAnalyzedFile);
      this->tabPageFiles->Controls->Add(this->textBoxAnalyzedDB);
      this->tabPageFiles->Controls->Add(this->label2);
      this->tabPageFiles->Controls->Add(this->buttonBrowseArticleFile);
      this->tabPageFiles->Controls->Add(this->textBoxArticleDB);
      this->tabPageFiles->Controls->Add(this->label1);
      this->tabPageFiles->Controls->Add(this->buttonBrowseFeedsFile);
      this->tabPageFiles->Controls->Add(this->textBoxFeedsFile);
      this->tabPageFiles->Controls->Add(this->labelFeedFile);
      this->tabPageFiles->Location = System::Drawing::Point(4, 22);
      this->tabPageFiles->Name = L"tabPageFiles";
      this->tabPageFiles->Padding = System::Windows::Forms::Padding(3);
      this->tabPageFiles->Size = System::Drawing::Size(302, 313);
      this->tabPageFiles->TabIndex = 0;
      this->tabPageFiles->Text = L"Files";
      this->tabPageFiles->UseVisualStyleBackColor = true;
      // 
      // tabPageParameters
      // 
      this->tabPageParameters->Controls->Add(this->textBox1);
      this->tabPageParameters->Controls->Add(this->labelWatch);
      this->tabPageParameters->Location = System::Drawing::Point(4, 22);
      this->tabPageParameters->Name = L"tabPageParameters";
      this->tabPageParameters->Padding = System::Windows::Forms::Padding(3);
      this->tabPageParameters->Size = System::Drawing::Size(302, 313);
      this->tabPageParameters->TabIndex = 1;
      this->tabPageParameters->Text = L"Parameters";
      this->tabPageParameters->UseVisualStyleBackColor = true;
      // 
      // labelFeedFile
      // 
      this->labelFeedFile->AutoSize = true;
      this->labelFeedFile->Location = System::Drawing::Point(6, 18);
      this->labelFeedFile->Name = L"labelFeedFile";
      this->labelFeedFile->Size = System::Drawing::Size(55, 13);
      this->labelFeedFile->TabIndex = 0;
      this->labelFeedFile->Text = L"Feeds file:";
      // 
      // textBoxFeedsFile
      // 
      this->textBoxFeedsFile->Location = System::Drawing::Point(9, 34);
      this->textBoxFeedsFile->Name = L"textBoxFeedsFile";
      this->textBoxFeedsFile->Size = System::Drawing::Size(211, 20);
      this->textBoxFeedsFile->TabIndex = 1;
      this->textBoxFeedsFile->WordWrap = false;
      // 
      // openFileDialogConfiguration
      // 
      this->openFileDialogConfiguration->FileName = L"*.*";
      this->openFileDialogConfiguration->InitialDirectory = L"C:\\";
      this->openFileDialogConfiguration->Title = L"Select a file...";
      // 
      // buttonBrowseFeedsFile
      // 
      this->buttonBrowseFeedsFile->Location = System::Drawing::Point(229, 35);
      this->buttonBrowseFeedsFile->Name = L"buttonBrowseFeedsFile";
      this->buttonBrowseFeedsFile->Size = System::Drawing::Size(67, 19);
      this->buttonBrowseFeedsFile->TabIndex = 2;
      this->buttonBrowseFeedsFile->Text = L"Browse...";
      this->buttonBrowseFeedsFile->UseVisualStyleBackColor = true;
      // 
      // buttonBrowseArticleFile
      // 
      this->buttonBrowseArticleFile->Location = System::Drawing::Point(229, 79);
      this->buttonBrowseArticleFile->Name = L"buttonBrowseArticleFile";
      this->buttonBrowseArticleFile->Size = System::Drawing::Size(67, 19);
      this->buttonBrowseArticleFile->TabIndex = 5;
      this->buttonBrowseArticleFile->Text = L"Browse...";
      this->buttonBrowseArticleFile->UseVisualStyleBackColor = true;
      // 
      // textBoxArticleDB
      // 
      this->textBoxArticleDB->Location = System::Drawing::Point(9, 78);
      this->textBoxArticleDB->Name = L"textBoxArticleDB";
      this->textBoxArticleDB->Size = System::Drawing::Size(211, 20);
      this->textBoxArticleDB->TabIndex = 4;
      this->textBoxArticleDB->WordWrap = false;
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(6, 62);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(107, 13);
      this->label1->TabIndex = 3;
      this->label1->Text = L"Articles database file:";
      // 
      // buttonBrowseAnalyzedFile
      // 
      this->buttonBrowseAnalyzedFile->Location = System::Drawing::Point(229, 122);
      this->buttonBrowseAnalyzedFile->Name = L"buttonBrowseAnalyzedFile";
      this->buttonBrowseAnalyzedFile->Size = System::Drawing::Size(67, 19);
      this->buttonBrowseAnalyzedFile->TabIndex = 8;
      this->buttonBrowseAnalyzedFile->Text = L"Browse...";
      this->buttonBrowseAnalyzedFile->UseVisualStyleBackColor = true;
      // 
      // textBoxAnalyzedDB
      // 
      this->textBoxAnalyzedDB->Location = System::Drawing::Point(9, 121);
      this->textBoxAnalyzedDB->Name = L"textBoxAnalyzedDB";
      this->textBoxAnalyzedDB->Size = System::Drawing::Size(211, 20);
      this->textBoxAnalyzedDB->TabIndex = 7;
      this->textBoxAnalyzedDB->WordWrap = false;
      // 
      // label2
      // 
      this->label2->AutoSize = true;
      this->label2->Location = System::Drawing::Point(6, 105);
      this->label2->Name = L"label2";
      this->label2->Size = System::Drawing::Size(152, 13);
      this->label2->TabIndex = 6;
      this->label2->Text = L"Analyzed articles database file:";
      // 
      // tabPageFeeds
      // 
      this->tabPageFeeds->Controls->Add(this->button3);
      this->tabPageFeeds->Controls->Add(this->button2);
      this->tabPageFeeds->Controls->Add(this->buttonAdd);
      this->tabPageFeeds->Controls->Add(this->textBox2);
      this->tabPageFeeds->Controls->Add(this->treeView1);
      this->tabPageFeeds->Location = System::Drawing::Point(4, 22);
      this->tabPageFeeds->Name = L"tabPageFeeds";
      this->tabPageFeeds->Padding = System::Windows::Forms::Padding(3);
      this->tabPageFeeds->Size = System::Drawing::Size(302, 313);
      this->tabPageFeeds->TabIndex = 2;
      this->tabPageFeeds->Text = L"Feeds";
      this->tabPageFeeds->UseVisualStyleBackColor = true;
      // 
      // labelWatch
      // 
      this->labelWatch->AutoSize = true;
      this->labelWatch->Location = System::Drawing::Point(6, 20);
      this->labelWatch->Name = L"labelWatch";
      this->labelWatch->Size = System::Drawing::Size(119, 13);
      this->labelWatch->TabIndex = 0;
      this->labelWatch->Text = L"Watch cycle (seconds):";
      this->labelWatch->Click += gcnew System::EventHandler(this, &ConfigurationWindow::labelWatch_Click);
      // 
      // textBox1
      // 
      this->textBox1->Location = System::Drawing::Point(128, 17);
      this->textBox1->Name = L"textBox1";
      this->textBox1->Size = System::Drawing::Size(93, 20);
      this->textBox1->TabIndex = 1;
      // 
      // buttonBrowseCookies
      // 
      this->buttonBrowseCookies->Location = System::Drawing::Point(229, 166);
      this->buttonBrowseCookies->Name = L"buttonBrowseCookies";
      this->buttonBrowseCookies->Size = System::Drawing::Size(67, 19);
      this->buttonBrowseCookies->TabIndex = 11;
      this->buttonBrowseCookies->Text = L"Browse...";
      this->buttonBrowseCookies->UseVisualStyleBackColor = true;
      // 
      // textBoxCookie
      // 
      this->textBoxCookie->Location = System::Drawing::Point(9, 165);
      this->textBoxCookie->Name = L"textBoxCookie";
      this->textBoxCookie->Size = System::Drawing::Size(211, 20);
      this->textBoxCookie->TabIndex = 10;
      this->textBoxCookie->WordWrap = false;
      // 
      // labelCookieFile
      // 
      this->labelCookieFile->AutoSize = true;
      this->labelCookieFile->Location = System::Drawing::Point(6, 149);
      this->labelCookieFile->Name = L"labelCookieFile";
      this->labelCookieFile->Size = System::Drawing::Size(64, 13);
      this->labelCookieFile->TabIndex = 9;
      this->labelCookieFile->Text = L"Cookies file:";
      // 
      // treeView1
      // 
      this->treeView1->Location = System::Drawing::Point(0, 0);
      this->treeView1->Name = L"treeView1";
      this->treeView1->Size = System::Drawing::Size(303, 239);
      this->treeView1->TabIndex = 0;
      // 
      // textBox2
      // 
      this->textBox2->Location = System::Drawing::Point(1, 247);
      this->textBox2->Name = L"textBox2";
      this->textBox2->Size = System::Drawing::Size(300, 20);
      this->textBox2->TabIndex = 1;
      // 
      // buttonAdd
      // 
      this->buttonAdd->Location = System::Drawing::Point(7, 274);
      this->buttonAdd->Name = L"buttonAdd";
      this->buttonAdd->Size = System::Drawing::Size(75, 23);
      this->buttonAdd->TabIndex = 2;
      this->buttonAdd->Text = L"button1";
      this->buttonAdd->UseVisualStyleBackColor = true;
      // 
      // button2
      // 
      this->button2->Location = System::Drawing::Point(89, 274);
      this->button2->Name = L"button2";
      this->button2->Size = System::Drawing::Size(75, 23);
      this->button2->TabIndex = 3;
      this->button2->Text = L"button2";
      this->button2->UseVisualStyleBackColor = true;
      // 
      // button3
      // 
      this->button3->Location = System::Drawing::Point(171, 274);
      this->button3->Name = L"button3";
      this->button3->Size = System::Drawing::Size(75, 23);
      this->button3->TabIndex = 4;
      this->button3->Text = L"button3";
      this->button3->UseVisualStyleBackColor = true;
      // 
      // ConfigurationWindow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(328, 381);
      this->Controls->Add(this->tabConfiguration);
      this->Controls->Add(this->cancelButton);
      this->Controls->Add(this->okButton);
      this->Name = L"ConfigurationWindow";
      this->Text = L"Configuration";
      this->Load += gcnew System::EventHandler(this, &ConfigurationWindow::ConfigurationWindow_Load);
      this->tabConfiguration->ResumeLayout(false);
      this->tabPageFiles->ResumeLayout(false);
      this->tabPageFiles->PerformLayout();
      this->tabPageParameters->ResumeLayout(false);
      this->tabPageParameters->PerformLayout();
      this->tabPageFeeds->ResumeLayout(false);
      this->tabPageFeeds->PerformLayout();
      this->ResumeLayout(false);

    }
#pragma endregion
private: System::Void labelWatch_Click(System::Object^  sender, System::EventArgs^  e) 
         {
         }
private: System::Void ConfigurationWindow_Load(System::Object^  sender, System::EventArgs^  e) 
         {
         }
};
}
