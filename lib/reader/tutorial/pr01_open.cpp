/* This example shows the possibilities of opening a device.
 * The following tutorials show the easy way of opening in their Open methods.
 */

#include "Pr22.hpp"
#include <iostream>
#ifdef WIN32
#include <conio.h>
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

		int Program()
		{
			/* To open more than one device simultaneously, create more DocumentReaderDevice objects */
			std::wcout << L"Opening system" << std::endl;
			std::wcout << std::endl;
			pr = new DocumentReaderDevice();

			pr->AddEventHandler(Events::Event::Connection, this);
			pr->AddEventHandler(Events::Event::DeviceUpdate, this);

			std::vector<std::wstring> deviceList = DocumentReaderDevice::GetDeviceList();

			if(deviceList.empty())
			{
				std::wcout << L"No device found!" << std::endl;
				return 0;
			}

			std::wcout << deviceList.size() << L" device" << (deviceList.size() > 1 ? L"s" : L"") << L" found." << std::endl;
			for(size_t ix=0; ix<deviceList.size(); ++ix)
			{
				std::wcout << L"  Device: " << deviceList[ix] << std::endl;
			}
			std::wcout << std::endl;

			std::wcout << L"Connecting to the first device by its name: " << deviceList[0] << std::endl;
			std::wcout << std::endl;
			std::wcout << L"If this is the first usage of this device on this PC," << std::endl;
			std::wcout << L"the \"calibration file\" will be downloaded from the device." << std::endl;
			std::wcout << L"This can take a while." << std::endl;
			std::wcout << std::endl;

			pr->UseDevice(deviceList[0]);

			std::wcout << L"The device is opened." << std::endl;

			std::wcout << L"Closing the device." << std::endl;
			pr->Close();
			std::wcout << std::endl;

			/* Opening the first device without using any device lists. */

			std::wcout << L"Connecting to the first device by its ordinal number: 0" << std::endl;

			pr->UseDevice(0);

			std::wcout << L"The device is opened." << std::endl;

			std::wcout << L"Closing the device." << std::endl;
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
