/* This example shows how to generate document type string.
 */

#include "Pr22.hpp"
#include "Pr22.Extension/DocumentType.hpp"
#include "Pr22.Extension/CountryCode.hpp"
#include <iostream>
#ifdef WIN32
#include <conio.h>
#endif

namespace tutorial
{
	using namespace Pr22;
	using namespace Pr22::Processing;

	// This class makes string concatenation with spaces and prefixes.
	class StrCon
	{
		const wchar_t *fstr, *cstr;
		StrCon(const wchar_t *str, const StrCon &csv) : fstr(str), cstr(csv.cstr) { }

		static std::wstring con(const wchar_t *fstr, const std::wstring &lstr) {
			return *fstr ? std::wstring(fstr).append(lstr[0]!=','?1:0, ' ').append(lstr) : lstr;
		}

	public:
		StrCon(const wchar_t *bounder = L"") : fstr(L""), cstr(bounder?bounder:L"") { }

		friend std::wstring operator +(const StrCon &csv, const std::wstring &str) {
			if(str.empty()) return csv.fstr;
			return con(csv.fstr, con(csv.cstr, str));
		}

		friend StrCon operator +(const std::wstring &str, const StrCon &csv) {
			return StrCon(str.c_str(), csv);
		}
	};
	//--------------------------------------------------------------------------

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

			std::wcout << L"Reading all the field data." << std::endl;
			Task::EngineTask ReadingTask;
			//Specify the fields we would like to receive.
			ReadingTask.Add(FieldSource::All, FieldId::All);

			Document OcrDoc = OcrEngine.Analyze(DocPage, ReadingTask);

			std::wcout << std::endl;
			std::wcout << L"Document code: " << OcrDoc.ToVariant().ToInt() << std::endl;
			std::wcout << L"Document type: " << GetDocType(OcrDoc) << std::endl;
			std::wcout << L"Status: " << OcrDoc.GetStatus().ToString() << std::endl;

			pr->Close();
			return 0;
		}
		//----------------------------------------------------------------------

		static std::wstring GetFieldValue(const Document &Doc, const Processing::FieldId &Id)
		{
			FieldReference filter(FieldSource::All, Id);
			std::vector<FieldReference> Fields = Doc.GetFields(filter);
			for(size_t ix=0; ix<Fields.size(); ++ix)
			{
				std::wstring value = Doc.GetField(Fields[ix]).GetBestStringValue();
				if(value.length()) return value;
			}
			return L"";
		}
		//----------------------------------------------------------------------

		static std::wstring GetDocType(const Document &OcrDoc)
		{
			std::wstring documentTypeName;

			int documentCode = OcrDoc.ToVariant().ToInt();
			documentTypeName = Extension::DocumentType::GetDocumentName(documentCode);

			if(documentTypeName.empty())
			{
				std::wstring issue_country = GetFieldValue(OcrDoc, FieldId::IssueCountry);
				std::wstring issue_state = GetFieldValue(OcrDoc, FieldId::IssueState);
				std::wstring doc_type = GetFieldValue(OcrDoc, FieldId::DocType);
				std::wstring doc_page = GetFieldValue(OcrDoc, FieldId::DocPage);
				std::wstring doc_subtype = GetFieldValue(OcrDoc, FieldId::DocTypeDisc);

				std::wstring tmpval = Extension::CountryCode::GetName(issue_country);
				if(tmpval.length()) issue_country = tmpval;

				documentTypeName = issue_country + StrCon() + issue_state
					+ StrCon() + Extension::DocumentType::GetDocTypeName(doc_type)
					+ StrCon(L"-") + Extension::DocumentType::GetPageName(doc_page)
					+ StrCon(L",") + doc_subtype;

			}
			return documentTypeName;
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
