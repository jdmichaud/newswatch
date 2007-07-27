#pragma once
//#pragma managed(push,off)
#include <iostream>
#include <string>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include "ConfigurationWindow.h"
#include "logging.hpp"
#include "globals.h"
#include "runcom_parser.hpp"
#include "feed_watcher.hpp"
#include "db_access.hpp"
#include "text_extractor.hpp"

#define VERSION "0.0.2"

#ifdef _DEBUG
# define LOG_LEVEL 2 // Max log level for debug
#else
# define LOG_LEVEL 1 // Max log level for release
#endif // !_DEBUG

//#pragma managed(pop)

namespace newswatcher {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MainWindow
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainWindow : public System::Windows::Forms::Form
	{
	public:
		MainWindow(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//

			runcom_parser rc_parser("../newswatch.rc");
			rc_parser.parse();
			rc_parser.parse_feeds(rc_parser.get_config()->m_feed_filename);
			rc_parser.parse_cookies(rc_parser.get_config()->m_cookies_filename);
			configuration::get_instance()->print(std::cout);

			// Initialiaze database
			db_access::get_instance()->init();

			//Launch feed watcher
			//feed_watcher fw;
			//boost::thread feed_watcher_thread(boost::bind(&(feed_watcher::watch), &fw));
			//feed_watcher_thread.join();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MainWindow()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::MenuStrip^  menuStrip1;
  protected: 
  private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  configurationToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
  private: System::Windows::Forms::StatusStrip^  mainStatusStrip;
  private: System::Windows::Forms::ToolStripStatusLabel^  mainStatusStripLabel;
  private: System::Windows::Forms::TreeView^  mainTreeView;
  private: System::Windows::Forms::WebBrowser^  mainWebBrowser;
  private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;
  private: System::Windows::Forms::ToolStripProgressBar^  mainStripProgressBar;



	private:
		/// <summary>
		/// Required designer variable.
    ConfigurationWindow m_configuration_window;
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
      this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
      this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->configurationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->mainStatusStrip = (gcnew System::Windows::Forms::StatusStrip());
      this->mainStatusStripLabel = (gcnew System::Windows::Forms::ToolStripStatusLabel());
      this->mainStripProgressBar = (gcnew System::Windows::Forms::ToolStripProgressBar());
      this->mainTreeView = (gcnew System::Windows::Forms::TreeView());
      this->mainWebBrowser = (gcnew System::Windows::Forms::WebBrowser());
      this->menuStrip1->SuspendLayout();
      this->mainStatusStrip->SuspendLayout();
      this->SuspendLayout();
      // 
      // menuStrip1
      // 
      this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->fileToolStripMenuItem, 
        this->helpToolStripMenuItem});
      this->menuStrip1->Location = System::Drawing::Point(0, 0);
      this->menuStrip1->Name = L"menuStrip1";
      this->menuStrip1->Size = System::Drawing::Size(806, 24);
      this->menuStrip1->TabIndex = 0;
      this->menuStrip1->Text = L"menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->configurationToolStripMenuItem, 
        this->exitToolStripMenuItem});
      this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
      this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
      this->fileToolStripMenuItem->Text = L"File";
      // 
      // configurationToolStripMenuItem
      // 
      this->configurationToolStripMenuItem->Name = L"configurationToolStripMenuItem";
      this->configurationToolStripMenuItem->Size = System::Drawing::Size(152, 22);
      this->configurationToolStripMenuItem->Text = L"&Configuration...";
      this->configurationToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::configurationToolStripMenuItem_Click);
      // 
      // exitToolStripMenuItem
      // 
      this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
      this->exitToolStripMenuItem->Size = System::Drawing::Size(152, 22);
      this->exitToolStripMenuItem->Text = L"&Exit";
      this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainWindow::exitToolStripMenuItem_Click);
      // 
      // helpToolStripMenuItem
      // 
      this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
      this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
      this->helpToolStripMenuItem->Size = System::Drawing::Size(40, 20);
      this->helpToolStripMenuItem->Text = L"Help";
      // 
      // aboutToolStripMenuItem
      // 
      this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
      this->aboutToolStripMenuItem->Size = System::Drawing::Size(103, 22);
      this->aboutToolStripMenuItem->Text = L"&About";
      // 
      // mainStatusStrip
      // 
      this->mainStatusStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->mainStatusStripLabel, 
        this->mainStripProgressBar});
      this->mainStatusStrip->Location = System::Drawing::Point(0, 512);
      this->mainStatusStrip->Name = L"mainStatusStrip";
      this->mainStatusStrip->Size = System::Drawing::Size(806, 22);
      this->mainStatusStrip->TabIndex = 1;
      this->mainStatusStrip->Text = L"statusStrip1";
      // 
      // mainStatusStripLabel
      // 
      this->mainStatusStripLabel->Name = L"mainStatusStripLabel";
      this->mainStatusStripLabel->Size = System::Drawing::Size(42, 17);
      this->mainStatusStripLabel->Text = L"Ready.";
      // 
      // mainStripProgressBar
      // 
      this->mainStripProgressBar->Alignment = System::Windows::Forms::ToolStripItemAlignment::Right;
      this->mainStripProgressBar->Name = L"mainStripProgressBar";
      this->mainStripProgressBar->Size = System::Drawing::Size(100, 16);
      this->mainStripProgressBar->Visible = false;
      // 
      // mainTreeView
      // 
      this->mainTreeView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
        | System::Windows::Forms::AnchorStyles::Left));
      this->mainTreeView->Location = System::Drawing::Point(0, 27);
      this->mainTreeView->Name = L"mainTreeView";
      this->mainTreeView->Size = System::Drawing::Size(184, 482);
      this->mainTreeView->TabIndex = 2;
      // 
      // mainWebBrowser
      // 
      this->mainWebBrowser->AllowNavigation = false;
      this->mainWebBrowser->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
        | System::Windows::Forms::AnchorStyles::Left) 
        | System::Windows::Forms::AnchorStyles::Right));
      this->mainWebBrowser->Location = System::Drawing::Point(190, 27);
      this->mainWebBrowser->MinimumSize = System::Drawing::Size(20, 20);
      this->mainWebBrowser->Name = L"mainWebBrowser";
      this->mainWebBrowser->Size = System::Drawing::Size(616, 482);
      this->mainWebBrowser->TabIndex = 3;
      // 
      // MainWindow
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(806, 534);
      this->Controls->Add(this->mainWebBrowser);
      this->Controls->Add(this->mainTreeView);
      this->Controls->Add(this->mainStatusStrip);
      this->Controls->Add(this->menuStrip1);
      this->MainMenuStrip = this->menuStrip1;
      this->Name = L"MainWindow";
      this->Text = L"News Watcher";
      this->menuStrip1->ResumeLayout(false);
      this->menuStrip1->PerformLayout();
      this->mainStatusStrip->ResumeLayout(false);
      this->mainStatusStrip->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void configurationToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
           {
             m_configuration_window.load_configuration();
             m_configuration_window.Show();
           }
  private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
           {
             exit(0);
           }
};
}

