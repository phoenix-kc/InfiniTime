#pragma once
//#include <FreeRTOS.h>
#include <task.h>
//#include <bits/unique_ptr.h>
//#include <queue.h>
#include "BootErrors.h"
#include "displayapp/Messages.h"

namespace Pinetime {

  namespace System {
    class SystemTask;
  };

  namespace Applications {
    class DisplayAppInterface {
    public:
      virtual ~DisplayAppInterface() { }
      virtual void Start(Pinetime::System::BootErrors) = 0;
      virtual void PushMessage(Pinetime::Applications::Display::Messages msg) = 0;
      virtual void Register(Pinetime::System::SystemTask* systemTask) = 0;
    };
  }
}
