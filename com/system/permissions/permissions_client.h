#include "permissions_proxy.h"

class PermissionsProxy
    : public sdbus::ProxyInterfaces<com::system::permissions_proxy> {
 public:
  PermissionsProxy(std::string destination, sdbus::ObjectPath objectPath)
      : ProxyInterfaces(std::move(destination), std::move(objectPath)) {
    registerProxy();
  }

  ~PermissionsProxy() { unregisterProxy(); }
};