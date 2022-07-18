/* This example shows how to read and process data from ECards.
 * After ECard selection before reading some authentication process have to
 * be called for accessing the data files.
 */

#include "Pr22.hpp"
#include <iostream>
#include <iomanip>
#include <sys/stat.h>
#ifdef WIN32
#include <conio.h>
#include <io.h>
#else
#include <sys/types.h>
#include <dirent.h>
#endif

namespace tutorial
{
	using namespace Pr22;
	using namespace Pr22::Processing;

	class SampleFilesys
	{
		static bool Match(const char *s, const char *m);
		static void AddFiles(std::vector<std::string> &Files, const std::string &Directory,
			const std::string &Pattern);

	public:
		static bool IsDir(const std::string &Entry);
		static bool IsFile(const std::string &Entry);
		static std::vector<std::string> GetFiles(const std::string &Directory,
			const std::string &Pattern);
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
		// Loads certificates from a directory.
		void LoadCertificates(const std::string &dir)
		{
			std::string exts[] = { "*.cer", "*.crt", "*.der", "*.pem", "*.crl", "*.cvcert", "*.ldif", "*.ml" };
			int cnt = 0;

			for(unsigned int exti=0; exti<gx_arraycnt(exts); ++exti)
			{
				std::vector<std::string> list = SampleFilesys::GetFiles(dir, exts[exti]);
				for(unsigned int fix=0; fix<list.size(); ++fix)
				{
					std::string &file = list[fix];
					try
					{
						BinData priv;
						if(exts[exti] == "*.cvcert") {
							//Searching for private key
							std::string pk = file.substr(0, file.find_last_of('.') + 1).append("pkcs8");
							if(SampleFilesys::IsFile(pk)) priv.Load(pk);
						}
						pr->GetGlobalCertificateManager().Load(BinData().Load(file), priv);
						std::cout << "Certificate " << file << " is loaded" <<
							(priv.RawSize() ? " with private key." : ".") << std::endl;
						++cnt;
					}
					catch(const Pr22::Exceptions::General &) {
						std::cout << "Loading certificate " << file << " is failed!" << std::endl;
					}
				}
			}
			if(cnt == 0) std::cout << "No certificates loaded from " << dir << std::endl;
			std::cout << std::endl;
		}
		//----------------------------------------------------------------------
		// Does an authentication after collecting the necessary information.
		bool Authenticate(ECard &SelectedCard, const ECardHandling::AuthProcess &CurrentAuth)
		{
			BinData AdditionalAuthData;
			int selector = 0;
			switch(CurrentAuth)
			{
			case ECardHandling::AuthProcess::BAC:
			case ECardHandling::AuthProcess::PACE:
			case ECardHandling::AuthProcess::BAP:
			{
				//Read MRZ (necessary for BAC, PACE and BAP)
				Task::DocScannerTask ScanTask;
				ScanTask.Add(Imaging::Light::Infra);
				Page FirstPage = pr->GetScanner().Scan(ScanTask, Imaging::PagePosition::First);

				Task::EngineTask MrzReadingTask;
				MrzReadingTask.Add(FieldSource::Mrz, FieldId::All);
				Document MrzDoc = pr->GetEngine().Analyze(FirstPage, MrzReadingTask);

				AdditionalAuthData.SetString(MrzDoc.GetField(FieldSource::Mrz, FieldId::All).GetRawStringValue());
				selector = 1;
				break;
			}
			case ECardHandling::AuthProcess::Passive:
			case ECardHandling::AuthProcess::Terminal:

				//Load the certificates if not done yet
				break;

			case ECardHandling::AuthProcess::SelectApp:
				if(SelectedCard.GetApplications().size()>0) selector = SelectedCard.GetApplications()[0];
				break;
			}
			try
			{
				SelectedCard.Authenticate(CurrentAuth, AdditionalAuthData, selector);
				std::wcout << L"- " << CurrentAuth.ToString() << L" authentication succeeded" << std::endl;
				return true;
			}
			catch(const Exceptions::General &e)
			{
				std::wcout << L"- " << CurrentAuth.ToString() << L" authentication failed: " << e.what() << std::endl;
				return false;
			}
		}
		//----------------------------------------------------------------------

		int Program()
		{
			//Devices can be manipulated only after opening.
			if(Open() != 0) return 1;

			//Please set the appropriate path
			std::wstring certdir = pr->GetProperty(L"rwdata_dir") + L"\\certs";
			LoadCertificates(std::string(certdir.begin(), certdir.end()));

			std::vector<ECardReader> &CardReaders = pr->GetReaders();

			//Connecting to the 1st card of any reader
			ECard SelectedCard;
			int CardCount = 0;
			std::wcout << L"Detected readers and cards:" << std::endl;
			size_t ix;
			for(ix=0; ix<CardReaders.size(); ++ix)
			{
				std::wcout << L"\tReader: " << CardReaders[ix].GetInfo().GetHwType().ToString() << std::endl;
				std::vector<std::wstring> cards = CardReaders[ix].GetCards();
				if(SelectedCard.GetSerial().empty() && cards.size()) SelectedCard = CardReaders[ix].ConnectCard(0);
				for(size_t iy=0; iy<cards.size(); ++iy)
				{
					std::wcout << L"\t\t(" << CardCount++ << L")card: " << cards[iy] << std::endl;
				}
				std::wcout << std::endl;
			}
			if(SelectedCard.GetSerial().empty())
			{
				std::wcout << L"No card selected!" << std::endl;
				return 1;
			}

			std::wcout << L"Executing authentications:" << std::endl;
			ECardHandling::AuthProcess CurrentAuth = SelectedCard.GetNextAuthentication(false);
			bool PassiveAuthImplemented = false;

			while(CurrentAuth != ECardHandling::AuthProcess::None)
			{
				if(CurrentAuth == ECardHandling::AuthProcess::Passive) PassiveAuthImplemented = true;
				bool authOk = Authenticate(SelectedCard, CurrentAuth);
				CurrentAuth = SelectedCard.GetNextAuthentication(!authOk);
			}
			std::wcout << std::endl;

			std::wcout << L"Reading data:" << std::endl;
			std::vector<ECardHandling::File> FilesOnSelectedCard = SelectedCard.GetFiles();
			if(PassiveAuthImplemented)
			{
				FilesOnSelectedCard.push_back(ECardHandling::FileId::CertDS);
				FilesOnSelectedCard.push_back(ECardHandling::FileId::CertCSCA);
			}
			for(ix=0; ix<FilesOnSelectedCard.size(); ++ix)
			{
				try
				{
					ECardHandling::File &File = FilesOnSelectedCard[ix];
					std::wcout << L"File: " << File.ToString() << L".";
					BinData RawFileData = SelectedCard.GetFile(File);
					RawFileData.Save(File.ToString() + L".dat");
					Document FileData = pr->GetEngine().Analyze(RawFileData);
					FileData.Save(Document::FileFormat::Xml).Save(File.ToString() + L".xml");

					//Executing mandatory data integrity check for Passive Authentication
					if(PassiveAuthImplemented)
					{
						ECardHandling::File f = File;
						if(f.Id >= ECardHandling::FileId::GeneralData) f = SelectedCard.ConvertFileId(f);
						if(f.Id >= 1 && f.Id <= 16)
						{
							std::wcout << L" hash check...";
							std::wcout << (SelectedCard.CheckHash(f) ? L"OK" : L"failed");
						}
					}
					std::wcout << std::endl;
					PrintDocFields(FileData);
				}
				catch(const Exceptions::General &e)
				{
					std::wcout << L" Reading failed: " << e.what();
				}
				std::wcout << std::endl;
			}

			std::wcout << L"Authentications:" << std::endl;
			Document AuthData = SelectedCard.GetAuthResult();
			AuthData.Save(Document::FileFormat::Xml).Save("AuthResult.xml");
			PrintDocFields(AuthData);
			std::wcout << std::endl;

			SelectedCard.Disconnect();

			pr->Close();
			return 0;
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
						//for(size_t cnt=0; cnt<binValue.size(); cnt+=16)
						//	std::wcout << PrintBinary(binValue, cnt, 16) << std::endl;
					}
					else
					{
						std::wcout << L"  " << std::setw(20) << Fieldname << std::setw(17) << Status.ToString() << L"[" << Value << L"]" << std::endl;
						std::wcout << L"\t" << std::setw(31) << L"   - Formatted" << L"[" << FormattedValue << L"]" << std::endl;
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
	};

	//--------------------------------------------------------------------------

	bool SampleFilesys::IsDir(const std::string &Entry)
	{
		struct stat s;
		if(stat(Entry.c_str(), &s)) return false;
		if(!(s.st_mode & S_IFDIR)) return false;
		return true;
	}
	//--------------------------------------------------------------------------

	bool SampleFilesys::IsFile(const std::string &Entry)
	{
		struct stat s;
		if(stat(Entry.c_str(), &s)) return false;
		if(!(s.st_mode & S_IFREG)) return false;
		return true;
	}
	//--------------------------------------------------------------------------

	bool SampleFilesys::Match(const char *s, const char *m)
	{
		while(*s && *m) {
			if(*s == *m || *m == '?') {
				// Continue on character match or on ? wildcard
				s++; m++;
			}
			else if(*m == '*') {
				// Check for * wildcard, recursively for each possible matching
				while(*m=='*') m++;
				if(!*m) return true;

				while(*s && *m != *s) s++;
				if(!*s) return false;

				if(Match(s, m)) return true;

				s++; m--;
			}
			else {
				return false;
			}
		}
		while(*m=='*') m++;
		return (!*s && !*m);
	}
	//--------------------------------------------------------------------------

	std::vector<std::string> SampleFilesys::GetFiles(const std::string &Directory,
		const std::string &Pattern)
	{
		std::vector<std::string> Files;
		if(IsDir(Directory)) AddFiles(Files, Directory, Pattern);
		return Files;
	}
	//--------------------------------------------------------------------------

#ifdef WIN32
	void SampleFilesys::AddFiles(std::vector<std::string> &Files,
		const std::string &Directory, const std::string &Pattern)
	{
		#if _MSC_VER >= 1400
			intptr_t d;
		#else
			int d;
		#endif
		struct _finddata_t de;

		std::string dn = Directory + "\\*.*";
		d = _findfirst((char*)dn.c_str(), &de);
		do {
			if(!strcmp(de.name, ".") || !strcmp(de.name, "..")) continue;

			dn = Directory + "\\" + de.name;
			if(IsDir(dn)) AddFiles(Files, dn, Pattern);
			else if(Match(de.name, Pattern.c_str())) Files.push_back(dn);

		} while (_findnext(d, &de) != -1);
		_findclose(d);
	}
#endif

#ifdef LINUX
	void SampleFilesys::AddFiles(std::vector<std::string> &Files,
		const std::string &Directory, const std::string &Pattern)
	{
		DIR *d = opendir(Directory.c_str());
		if(!d) return;

		struct dirent *de;
		while((de = readdir(d))) {
			if(!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) continue;

			std::string dn = Directory + "/" + de->d_name;
			if(IsDir(dn)) AddFiles(Files, dn, Pattern);
			else if(Match(de->d_name, Pattern.c_str())) Files.push_back(dn);
		}
		closedir(d);
	}
#endif
	//--------------------------------------------------------------------------
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
