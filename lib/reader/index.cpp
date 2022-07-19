#define NAPI_VERSION 3
#include "napi.h"

#include "Pr22.hpp"

#include <iostream>
#include <unistd.h>

using namespace Pr22;
using namespace Pr22::Processing;
using namespace Pr22::Task;

class EventHandler :  public Pr22::Events::DocEventHandler
{

  public:
      explicit EventHandler() {}
      ~EventHandler() { }

      void RegisterEvents(Pr22::DocumentReaderDevice* PR);

  private:
      void OnConnection(int device);
      void OnDocFrameFound(int page, const Exceptions::ErrorCodes &);
      void OnScanFinished(int, const Exceptions::ErrorCodes &);
      void OnImageScanned(int page, const Imaging::Light & light);
      void OnPresenceStateChanged(const Util::PresenceState & state);
};

class Reader : public Napi::ObjectWrap<Reader>
{
  DocumentReaderDevice *pr;
  bool DocPresent;
  EventHandler* eventHandler;

  public:
    static void Initialize(Napi::Env &env, Napi::Object &exports);
    bool DeviceIsConnected;

    // void closeHandle();

    Reader(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Reader>(info) {
      pr = new DocumentReaderDevice();
    };
    ~Reader() { delete pr; }

    int Open()
    {
      // pr = new DocumentReaderDevice();
      eventHandler = new EventHandler();
      eventHandler->RegisterEvents(pr);
      // pr->AddEventHandler(Events::Event::Connection, this);
      // pr->AddEventHandler(Events::Event::DeviceUpdate, this);

      try { pr->UseDevice(0); }
      catch(const Exceptions::NoSuchDevice &)
      {
        // Napi::TypeError::New(env, "No active device").ThrowAsJavaScriptException();
        return 1;
      }

      // std::wcout << L"The device " << pr->GetDeviceName() << L" is opened." << std::endl;
      // std::wcout << std::endl;
      return 0;
    }

    //------------------------------------------------------------------------------

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
      Imaging::RawImage img = pr->GetScanner().GetPage(page).Select(light).GetImage();
      std::wostringstream ws;
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

  private:

    Napi::Value test(const Napi::CallbackInfo &info);
};

void EventHandler::RegisterEvents(Pr22::DocumentReaderDevice* PR)
{
    PR->AddEventHandler(Events::Event::Connection, this);
    PR->AddEventHandler(Events::Event::PresenceDetection, this);
    PR->AddEventHandler(Events::Event::ImageScanned, this);
    PR->AddEventHandler(Events::Event::ScanFinished, this);
    PR->AddEventHandler(Events::Event::DocFrameFound, this);
}


/*
Reader::Reader(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Reader>(info)
{
  Napi::Env env = info.Env();
  pr = NULL;


  if (!info.IsConstructCall())
  {
    Napi::TypeError::New(env, "Reader function can only be used as a constructor").ThrowAsJavaScriptException();
    return;
  }
}*/





//-----------

Napi::Value Reader::test(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  return Napi::String::New(env, "OK");
}

void Reader::Initialize(Napi::Env &env, Napi::Object &exports)
{
  // napi_add_env_cleanup_hook(env, deinitialize, nullptr);

  Napi::Function ctor = DefineClass(env, "Reader", {
                                                    InstanceMethod("test", &Reader::test)
                                                });

  exports.Set("Reader", ctor);
}


Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  Reader::Initialize(env, exports);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
