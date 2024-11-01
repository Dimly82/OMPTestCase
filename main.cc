#include "permissions.h"

int main(int argc, char *argv[]) {
  auto connection = sdbus::createSessionBusConnection("com.system.permissions");

  sdbus::ObjectPath objectPath("/com/system/permissions");
  Permissions permissions(*connection, objectPath);

  connection->enterEventLoop();
}