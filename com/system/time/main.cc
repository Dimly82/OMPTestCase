#include "time_service.h"

int main(int argc, char *argv[]) {
  auto connection = sdbus::createSessionBusConnection("com.system.time");

  sdbus::ObjectPath objectPath("/");
  Time permissions(*connection, objectPath);

  connection->enterEventLoop();
}