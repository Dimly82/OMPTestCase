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
    // Get name of the executable
    std::string exePath =
            "/proc/" +
            std::to_string(
                    getObject().getCurrentlyProcessedMessage()->getCredsPid()) +
            "/exe";
    char resolvedPath[PATH_MAX];
    ssize_t len =
            readlink(exePath.c_str(), resolvedPath, sizeof(resolvedPath) - 1);

    if (len == -1) {
      throw sdbus::Error(
              "com.system.time.Error",
              "Failed to read executable path for PID " +
              std::to_string(
                      getObject().getCurrentlyProcessedMessage()->getCredsPid()));
    }
    resolvedPath[len] = '\0';

//     Connecting to com.system.permissions service and calling CheckApplicationHasPermission method
    auto proxy = sdbus::createProxy("com.system.permissions", "/");
    proxy->finishRegistration();
    bool hasPermission = false;
    proxy->callMethod("CheckApplicationHasPermission").onInterface("com.system.permissions").
            withArguments(resolvedPath, 0).storeResultsTo(hasPermission);

    if (!hasPermission)
      throw sdbus::Error("com.system.time.UnauthorizedAccess",
                         "Application does not have permission to use this method");

    // Getting current system time
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch());
    return duration.count();
  }
};