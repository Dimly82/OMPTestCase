#include <sdbus-c++/sdbus-c++.h>

#include <climits>
#include <iostream>

#include "time_adaptor.h"

class Time : public sdbus::AdaptorInterfaces<com::system::time_adaptor> {
 public:
  Time(sdbus::IConnection &connection, const sdbus::ObjectPath &objectPath)
      : AdaptorInterfaces(connection, objectPath) {
    registerAdaptor();
  }

  ~Time() { unregisterAdaptor(); }

 protected:
  uint64_t GetSystemTime() override {
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
    return duration.count();
  }
};