#include <QCoreApplication>
#include <QDBusConnection>

#include "systemTime.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QDBusConnection connection = QDBusConnection::sessionBus();
  if (!connection.registerService("com.system.time")) {
    qDebug() << "error: " << connection.lastError().message();
    exit(-1);
  }
  Time object;
  connection.registerObject("/", "com.system.time", &object,
                            QDBusConnection::ExportAllSlots);
  return app.exec();
}
