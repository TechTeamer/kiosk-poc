/* This example shows the main capabilities of the image processing analyzer function.
 */

#include "Pr22.hpp"
#include <iostream>
#include <iomanip>
#include <clocale>
#ifdef WIN32
#include <conio.h>
#endif

namespace tutorial
{
	using namespace Pr22;
	using namespace Pr22::Processing;

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

			//Subscribing to scan events
			pr->AddEventHandler(Events::Event::ScanStarted, this);
			pr->AddEventHandler(Events::Event::ImageScanned, this);
			pr->AddEventHandler(Events::Event::ScanFinished, this);
			pr->AddEventHandler(Events::Event::DocFrameFound, this);

			DocScanner &Scanner = pr->GetScanner();
			Engine &OcrEngine = pr->GetEngine();

			std::wcout << L"Scanning some images to read from." << std::endl;
			Task::DocScannerTask ScanTask;
			//For OCR (MRZ) reading purposes, IR (infrared) image is recommended.
			ScanTask.Add(Imaging::Light::White).Add(Imaging::Light::Infra);
			Page DocPage = Scanner.Scan(ScanTask, Imaging::PagePosition::First);
			std::wcout << std::endl;

			std::wcout << L"Reading all the field data of the Machine Readable Zone." << std::endl;
			Task::EngineTask MrzReadingTask;
			//Specify the fields we would like to receive.
			MrzReadingTask.Add(FieldSource::Mrz, FieldId::All);
			Document MrzDoc = OcrEngine.Analyze(DocPage, MrzReadingTask);

			std::wcout << std::endl;
			PrintDocFields(MrzDoc);
			//Returned fields by the Analyze function can be saved to an XML file:
			MrzDoc.Save(Document::FileFormat::Xml).Save("MRZ.xml");

			std::wcout << L"Scanning more images for VIZ reading and image authentication." << std::endl;
			//Reading from VIZ -except face photo- is available in special OCR engines only.
			ScanTask.Add(Imaging::Light::All);
			DocPage = Scanner.Scan(ScanTask, Imaging::PagePosition::Current);
			std::wcout << std::endl;

			std::wcout << L"Reading all the textual and graphical field data as well as " <<
				L"authentication result from the Visual Inspection Zone." << std::endl;
			Task::EngineTask VIZReadingTask;
			VIZReadingTask.Add(FieldSource::Viz, FieldId::All);
			Document VizDoc = OcrEngine.Analyze(DocPage, VIZReadingTask);

			std::wcout << std::endl;
			PrintDocFields(VizDoc);
			VizDoc.Save(Document::FileFormat::Xml).Save("VIZ.xml");

			std::wcout << L"Reading barcodes." << std::endl;
			Task::EngineTask BCReadingTask;
			BCReadingTask.Add(FieldSource::Barcode, FieldId::All);
			Document BcrDoc = OcrEngine.Analyze(DocPage, BCReadingTask);

			std::wcout << std::endl;
			PrintDocFields(BcrDoc);
			BcrDoc.Save(Document::FileFormat::Xml).Save("BCR.xml");

			pr->Close();
			return 0;
		}
		//----------------------------------------------------------------------
		/** Prints a hexa dump line from a part of an array into a string.
		 *
		 * @param arr		The whole array.
		 * @param pos		Position of the first item to print.
		 * @param sz		Number of items to print.
		 */
		static std::wstring PrintBinary(const std::vector<gxu8> &arr, int pos, int sz)
		{
			int p0;
			std::wostringstream str;
			std::wstring str2;
			for(p0=pos; p0<(int)arr.size() && p0<pos+sz; p0++)
			{
				str << std::hex << std::setfill(L'0') << std::setw(2) << (int)arr[p0] << L" ";
				str2 += arr[p0] < 0x21 || arr[p0] > 0x7e ? '.' : arr[p0];
			}
			for(; p0<pos+sz; p0++) { str << L"   "; str2 += L" "; }
			return str.str() + str2;
		}
		//----------------------------------------------------------------------
		/** Prints out all fields of a document structure to console.
		 *
		 * Values are printed in three different forms: raw, formatted and standardized.
		 * Status (checksum result) is printed together with fieldname and raw value.
		 * At the end, images of all fields are saved into png format.
		 */
		static void PrintDocFields(const Document &doc)
		{
			std::vector<FieldReference> Fields = doc.GetFields();

			std::wcout << L"  " << std::setw(20) << std::left << L"FieldId" << std::setw(17) << L"Status" << L"Value" << std::endl;
			std::wcout << L"  " << std::setw(20) << L"-------" << std::setw(17) << L"------" << L"-----" << std::endl;
			std::wcout << std::endl;

			size_t ix;
			for(ix=0; ix<Fields.size(); ++ix)
			{
				try
				{
					Field CurrentField = doc.GetField(Fields[ix]);
					std::wstring Value, FormattedValue, StandardizedValue;
					std::vector<gxu8> binValue;
					try { Value = CurrentField.GetRawStringValue(); }
					catch(const Exceptions::EntryNotFound &) { }
					catch(const Exceptions::InvalidParameter &) { binValue = CurrentField.GetBinaryValue(); }
					try { FormattedValue = CurrentField.GetFormattedStringValue(); }
					catch(const Exceptions::EntryNotFound &) { }
					try { StandardizedValue = CurrentField.GetStandardizedStringValue(); }
					catch(const Exceptions::EntryNotFound &) { }
					Status Status = CurrentField.GetStatus();
					std::wstring Fieldname = Fields[ix].ToString();
					if(binValue.size())
					{
						std::wcout << L"  " << std::setw(20) << Fieldname << std::setw(17) << Status.ToString() << L"Binary" << std::endl;
						for(size_t cnt=0; cnt<binValue.size(); cnt+=16)
							std::wcout << PrintBinary(binValue, (int)cnt, 16) << std::endl;
					}
					else
					{
						std::wcout << L"  " << std::setw(20) << Fieldname << std::setw(17) << Status.ToString() << L"[" << Value << L"]" << std::endl;
						std::wcout << L"\t" << std::setw(31) << L"   - Formatted" <<  L"[" << FormattedValue << L"]" << std::endl;
						std::wcout << L"\t" << std::setw(31) << L"   - Standardized" << L"[" << StandardizedValue << L"]" << std::endl;
					}

					std::vector<Checking> lst = CurrentField.GetDetailedStatus();
					for(size_t iy=0; iy<lst.size(); ++iy)
					{
						std::wcout << lst[iy].ToString() << std::endl;
					}

					try { CurrentField.GetImage().Save(Imaging::RawImage::FileFormat::Png).Save(Fieldname + L".png"); }
					catch(const Exceptions::General &) { }
				}
				catch(const Exceptions::General &)
				{
				}
			}
			std::wcout << std::endl;

			std::vector<FieldCompare> comp = doc.GetFieldCompareList();
			for(ix=0; ix<comp.size(); ++ix)
			{
				std::wcout << L"Comparing " << comp[ix].Field1.ToString() << L" vs. "
					<< comp[ix].Field2.ToString() << L" results " << comp[ix].Confidence << std::endl;
			}
			std::wcout << std::endl;
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
	};
}

int main()
{
#if defined(WIN32) && !defined(__BORLANDC__)
	setlocale(LC_CTYPE, ".OCP");
#else
	setlocale(LC_CTYPE, "");
#endif

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
