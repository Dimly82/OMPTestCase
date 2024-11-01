#include <sdbus-c++/sdbus-c++.h>

#include <climits>
#include <iostream>

#include "../permissions/permissions_client.h"
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

    // Connecting to com.system.permissions service and calling
    // CheckApplicationHasPermission method
    //    auto proxy = sdbus::createProxy("com.system.permissions", "/");
    //    proxy->finishRegistration();
    //    bool hasPermission = false;
    //    proxy->callMethod("CheckApplicationHasPermission").onInterface("com.system.permissions").
    //            withArguments(resolvedPath, 0).storeResultsTo(hasPermission);

    // I know this is some bs, but it won't work otherwise
    std::string cmd =
        "gdbus call -e -d com.system.permissions -o / -m "
        "com.system.permissions.CheckApplicationHasPermission " +
        std::string(resolvedPath) + " " + std::to_string(0);
    char buffer[128];
    std::string result;
    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw sdbus::Error("com.system.time.Error", "popen() failed!");
    try {
      while (fgets(buffer, sizeof buffer, pipe) != nullptr) result += buffer;
    } catch (...) {
      pclose(pipe);
      throw;
    }
    pclose(pipe);

    if (result.empty())
      throw sdbus::Error("com.system.time.Error",
                         "Got no response from com.system.permissions");
    if (result != "(true,)\n")
      throw sdbus::Error(
          "com.system.time.UnauthorizedAccess",
          "Application does not have permission to use this method");

    //    PermissionsProxy pProxy("com.system.permissions", "/");
    //    auto hasPermission =
    //    pProxy.CheckApplicationHasPermission(resolvedPath, 0);

    // Getting current system time
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch());
    return duration.count();
  }
};