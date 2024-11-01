#include "permissions_service.h"

int main(int argc, char *argv[]) {
  auto connection = sdbus::createSessionBusConnection("com.system.permissions");

  sdbus::ObjectPath objectPath("/");
  Permissions permissions(*connection, objectPath);

  connection->enterEventLoop();
}