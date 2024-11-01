#include <sdbus-c++/sdbus-c++.h>

#include <iostream>

#include "adaptor.h"

class Permissions
    : public sdbus::AdaptorInterfaces<com::system::permissions_adaptor> {
 public:
  Permissions(sdbus::IConnection& connection,
              const sdbus::ObjectPath& objectPath)
      : AdaptorInterfaces(connection, objectPath) {
    registerAdaptor();
  }

  ~Permissions() { unregisterAdaptor(); }

 protected:
  void RequestPermission(const int32_t& permissionEnumCode) override {
    std::cout << permissionEnumCode;
  }

  bool CheckApplicationHasPermission(
      const std::string& applicationExecPath,
      const int32_t& permissionEnumCode) override {
    if (permissionEnumCode == 0) return true;
    return false;
  }
};
