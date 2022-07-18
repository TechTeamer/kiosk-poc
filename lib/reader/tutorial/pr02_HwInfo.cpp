/* This example shows how to get general information about the device capabilities.
 */

#include "Pr22.hpp"
#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif
#ifdef QT_CORE_LIB
#include <QRect>
#else
struct QRect {
	int x, y, w, h;
	QRect() : x(0), y(0), w(0), h(0) { }
	QRect(int x, int y, int w, int h) : x(x), y(y), w(w), h(h) { }
	int width() { return w; }
	int height() { return h; }
};
#endif

namespace tutorial
{
	using namespace Pr22;

	class MainClass : public Events::DocEventHandler
	{
		DocumentReaderDevice *pr;

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
			//Devices can be manipulated only after opening.
			if(Open() != 0) return 1;

			std::wcout << L"SDK versions:" << std::endl;
			std::wcout << L"\tInterface: " << pr->GetVersion('A') << std::endl;
			std::wcout << L"\tSystem: " << pr->GetVersion('S') << std::endl;
			std::wcout << std::endl;

			DocScanner::Information scannerinfo = pr->GetScanner().GetInfo();

			//Devices provide proper image quality only if they are calibrated.
			//Devices are calibrated by default. If you receive the message "not calibrated"
			//then please contact your hardware supplier.
			std::wcout << L"Calibration state of the device:" << std::endl;
			if(scannerinfo.IsCalibrated())
				std::wcout << L"\tcalibrated" << std::endl;
			else
				std::wcout << L"\tnot calibrated" << std::endl;
			std::wcout << std::endl;

			std::wcout << L"Available lights for image scanning:" << std::endl;
			std::vector<Imaging::Light> lights = scannerinfo.GetLights();
			size_t ix;
			for(ix=0; ix<lights.size(); ++ix)
				std::wcout << L"\t" << lights[ix].ToString() << std::endl;
			std::wcout << std::endl;

			std::wcout << L"Available object windows for image scanning:" << std::endl;
			for(ix=0; ix<(size_t)scannerinfo.GetWindowCount(); ++ix)
			{
				QRect frame = scannerinfo.GetSize<QRect>((int)ix);
				std::wcout << L"\t" << ix << L": " << frame.width() / 1000.0f << L" x " << frame.height() / 1000.0f << L" mm" << std::endl;
			}
			std::wcout << std::endl;

			std::wcout << L"Scanner component versions:" << std::endl;
			std::wcout << L"\tFirmware: " << scannerinfo.GetVersion('F') << std::endl;
			std::wcout << L"\tHardware: " << scannerinfo.GetVersion('H') << std::endl;
			std::wcout << L"\tSoftware: " << scannerinfo.GetVersion('S') << std::endl;
			std::wcout << std::endl;

			std::wcout << L"Available card readers:" << std::endl;
			std::vector<ECardReader> &readers = pr->GetReaders();
			for(ix=0; ix<readers.size(); ++ix)
			{
				std::wcout << L"\t" << ix << L": " << readers[ix].GetInfo().GetHwType().ToString() << std::endl;
				std::wcout << L"\t\tFirmware: " << readers[ix].GetInfo().GetVersion('F') << std::endl;
				std::wcout << L"\t\tHardware: " << readers[ix].GetInfo().GetVersion('H') << std::endl;
				std::wcout << L"\t\tSoftware: " << readers[ix].GetInfo().GetVersion('S') << std::endl;
			}
			std::wcout << std::endl;

			std::wcout << L"Available status LEDs:" << std::endl;
			std::vector<Control::StatusLed> leds = pr->GetPeripherals().GetStatusLeds();
			for(ix=0; ix<leds.size(); ++ix)
				std::wcout << L"\t" << ix << L": color " << leds[ix].GetLight().ToString() << std::endl;
			std::wcout << std::endl;

			Engine::Information EngineInfo = pr->GetEngine().GetInfo();

			std::wcout << L"Engine version: " + EngineInfo.GetVersion('E') << std::endl;
			std::wstring licok[] = { L"no presence info", L"not available", L"present", L"expired" };
			std::wstring lictxt = EngineInfo.GetRequiredLicense().ToString();
			if(EngineInfo.GetRequiredLicense() == Processing::EngineLicense::MrzOcrBarcodeReading)
				lictxt = L"MrzOcrBarcodeReadingL or MrzOcrBarcodeReadingF";
			std::wcout << L"Required license: " << lictxt.c_str() << L" - " << licok[TestLicense(EngineInfo)] << std::endl;
			std::wcout << L"Engine release date: " << EngineInfo.GetRequiredLicenseDate() << std::endl;
			std::wcout << std::endl;

			std::wcout << L"Available licenses:" << std::endl;
			std::vector<Processing::EngineLicense> licenses = EngineInfo.GetAvailableLicenses();
			for(ix=0; ix<licenses.size(); ix++)
				std::wcout << L"\t" << licenses[ix].ToString() << L" (" << EngineInfo.GetLicenseDate(licenses[ix]) << L")" << std::endl;
			std::wcout << std::endl;

			std::wcout << L"Closing the device." << std::endl;
			pr->Close();
			return 0;
		}
		//----------------------------------------------------------------------
		// Tests if the required OCR license is present.
		int TestLicense(const Engine::Information &info)
		{
			if(info.GetRequiredLicense() == Processing::EngineLicense::Unknown) return 0;
			std::wstring availdate = info.GetLicenseDate(info.GetRequiredLicense());
			if(availdate == L"-") return 1;
			if(info.GetRequiredLicenseDate() == L"-") return 2;
			if(availdate[0] != 'X' && availdate > info.GetRequiredLicenseDate()) return 2;
			return 3;
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
