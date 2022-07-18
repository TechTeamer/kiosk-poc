/* This example shows how to parametrize the image scanning process.
 */

#include "Pr22.hpp"
#include <iostream>
#ifdef WIN32
#include <conio.h>
#else
#include <unistd.h>
void Sleep(int ms) {
	sleep(ms/1000);
	usleep((ms%1000)*1000);
}
#endif

namespace tutorial
{
	using namespace Pr22;
	using namespace Pr22::Task;

	class MainClass : public Events::DocEventHandler
	{
		DocumentReaderDevice *pr;
		bool DocPresent;

	public:

		MainClass() : pr(0) { }

		~MainClass() { delete pr; }

		//----------------------------------------------------------------------
		// Opens the first document reader device.
		int Open()
		{
			std::wcout << L"Opening a device" << std::endl;
			std::wcout << std::endl;
			pr = new DocumentReaderDevice();

			pr->AddEventHandler(Events::Event::Connection, this);
			pr->AddEventHandler(Events::Event::DeviceUpdate, this);

			try { pr->UseDevice(0); }
			catch(const Exceptions::NoSuchDevice &)
			{
				std::wcout << L"No device found!" << std::endl;
				return 1;
			}

			std::wcout << L"The device " << pr->GetDeviceName() << L" is opened." << std::endl;
			std::wcout << std::endl;
			return 0;
		}
		//----------------------------------------------------------------------

		int Program()
		{
			std::wcout << L"This tutorial guides you through a complex image scanning process." << std::endl;
			std::wcout << L"This will demonstrate all possible options of page management." << std::endl;
			std::wcout << L"The stages of the scan process will be saved into separate zip files" << std::endl;
			std::wcout << L"in order to provide the possibility of comparing them to each other." << std::endl;
			std::wcout << std::endl;

			//Devices can be manipulated only after opening.
			if(Open() != 0) return 1;

			//Subscribing to scan events
			pr->AddEventHandler(Events::Event::ScanStarted, this);
			pr->AddEventHandler(Events::Event::ImageScanned, this);
			pr->AddEventHandler(Events::Event::ScanFinished, this);
			pr->AddEventHandler(Events::Event::DocFrameFound, this);
			pr->AddEventHandler(Events::Event::PresenceDetection, this);

			DocScanner &Scanner = pr->GetScanner();

			TaskControl LiveTask = Scanner.StartTask(FreerunTask::Detection());

			//first page
			{
				DocScannerTask FirstTask;

				std::wcout << L"At first the device scans only a white image..." << std::endl;
				FirstTask.Add(Imaging::Light::White);
				Processing::Page page1 = Scanner.Scan(FirstTask, Imaging::PagePosition::First);

				std::wcout << L"And then the program saves it as a PNG file." << std::endl;
				page1.Select(Imaging::Light::White).GetImage().Save(Imaging::RawImage::FileFormat::Png).Save("original.png");

				std::wcout << L"Saving stage 1." << std::endl;
				pr->GetEngine().GetRootDocument().Save(Processing::Document::FileFormat::Zipped).Save("1stScan.zip");
				std::wcout << std::endl;

				std::wcout << L"If scanning of an additional infra image of the same page is required..." << std::endl;
				std::wcout << L"We need to scan it into the current page." << std::endl;
				FirstTask.Add(Imaging::Light::Infra);
				Scanner.Scan(FirstTask, Imaging::PagePosition::Current);

				try
				{
					std::wcout << L"If a cropped image of the document is available" << std::endl;
					std::wcout << L" then the program saves it as a PNG file." << std::endl;
					Scanner.GetPage(0).Select(Imaging::Light::White).DocView().GetImage().Save(Imaging::RawImage::FileFormat::Png).Save("document.png");
				}
				catch(const Exceptions::ImageProcessingFailed &)
				{
					std::wcout << L"Cropped image is not available!" << std::endl;
				}

				std::wcout << L"Saving stage 2." << std::endl;
				pr->GetEngine().GetRootDocument().Save(Processing::Document::FileFormat::Zipped).Save("2ndScan.zip");
				std::wcout << std::endl;
			}

			//second page
			{
				std::wcout << L"At this point, if scanning of an additional page of the document is needed" << std::endl;
				std::wcout << L"with all of the available lights except the infra light." << std::endl;
				std::wcout << L"It is recommended to execute in one scan process" << std::endl;
				std::wcout << L" - as it is the fastest in such a way." << std::endl;
				DocScannerTask SecondTask;
				SecondTask.Add(Imaging::Light::All).Del(Imaging::Light::Infra);
				std::wcout << std::endl;

				DocPresent = false;
				std::wcout << L"At this point, the user has to change the document on the reader." << std::endl;
				while(DocPresent == false) Sleep(100);

				std::wcout << L"Scanning the images." << std::endl;
				Scanner.Scan(SecondTask, Imaging::PagePosition::Next);

				std::wcout << L"Saving stage 3." << std::endl;
				pr->GetEngine().GetRootDocument().Save(Processing::Document::FileFormat::Zipped).Save("3rdScan.zip");
				std::wcout << std::endl;

				std::wcout << L"Upon putting incorrect page on the scanner, the scanned page has to be removed." << std::endl;
				Scanner.CleanUpLastPage();

				DocPresent = false;
				std::wcout << L"And the user has to change the document on the reader again." << std::endl;
				while(DocPresent == false) Sleep(100);

				std::wcout << L"Scanning..." << std::endl;
				Scanner.Scan(SecondTask, Imaging::PagePosition::Next);

				std::wcout << L"Saving stage 4." << std::endl;
				pr->GetEngine().GetRootDocument().Save(Processing::Document::FileFormat::Zipped).Save("4thScan.zip");
				std::wcout << std::endl;
			}

			LiveTask.Stop();

			std::wcout << L"Scanning processes are finished." << std::endl;
			pr->Close();
			return 0;
		}
		//----------------------------------------------------------------------
		// Event handlers
		//----------------------------------------------------------------------

		void OnConnection(int devno)
		{
			std::wcout << L"Connection event. Device number: " << devno << std::endl;
		}
		//----------------------------------------------------------------------

		void OnDeviceUpdate(int part)
		{
			std::wcout << L"Update event." << std::endl;
			switch(part)
			{
			case 1:
				std::wcout << L"  Reading calibration file from device." << std::endl;
				break;
			case 2:
				std::wcout << L"  Scanner firmware update." << std::endl;
				break;
			case 4:
				std::wcout << L"  RFID reader firmware update." << std::endl;
				break;
			case 5:
				std::wcout << L"  License update." << std::endl;
				break;
			}
		}
		//----------------------------------------------------------------------

		void OnScanStarted(int page, const Exceptions::ErrorCodes & status)
		{
			std::wcout << L"Scan started. Page: " << page << std::endl;
		}
		//----------------------------------------------------------------------

		void OnImageScanned(int page, const Imaging::Light & light)
		{
			std::wcout << L"Image scanned. Page: " << page << L" Light: " << light.ToString() << std::endl;
			Imaging::RawImage img = pr->GetScanner().GetPage(page).Select(light).GetImage();
			std::wostringstream ws;
			ws << L"page_" << page << L"_light_" << light.ToString() << L".bmp";
			img.Save(Imaging::RawImage::FileFormat::Bmp).Save(ws.str());
		}
		//----------------------------------------------------------------------

		void OnScanFinished(int page, const Exceptions::ErrorCodes & status)
		{
			std::wcout << L"Page scanned. Page: " << page << L" Status: " << status.ToString() << std::endl;
		}
		//----------------------------------------------------------------------

		void OnDocFrameFound(int page, const Exceptions::ErrorCodes & status)
		{
			std::wcout << L"Document frame found. Page: " << page << std::endl;
		}
		//----------------------------------------------------------------------

		void OnPresenceStateChanged(const Util::PresenceState & state)
		{
			if(state == Pr22::Util::PresenceState::Present) DocPresent = true;
		}
		//----------------------------------------------------------------------
	};
}

int main()
{
	try
	{
		tutorial::MainClass prog;
		prog.Program();
	}
	catch(const Pr22::Exceptions::General &e)
	{
		std::wcerr << e.what() << std::endl;
	}
#ifdef WIN32
	std::wcout << L"Press any key to exit!" << std::endl;
	_getch();
#endif
	return 0;
}
