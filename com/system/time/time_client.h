#include "time_proxy.h"

class TimeProxy : public sdbus::ProxyInterfaces<com::system::time_proxy> {
 public:
  TimeProxy(std::string destination, sdbus::ObjectPath objectPath)
      : ProxyInterfaces(std::move(destination), std::move(objectPath)) {
    registerProxy();
  }

  ~TimeProxy() { unregisterProxy(); }
};