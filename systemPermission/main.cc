#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

#include "systemPermissions.h"

int main(int argc, char *argv[]) {
  QCoreApplication app(argc, argv);
  QDBusConnection connection = QDBusConnection::sessionBus();
  if (!connection.registerService("com.system.permissions")) {
    qDebug() << "error: " << connection.lastError().message();
    exit(-1);
  }
  Permissions object;
  connection.registerObject("/", "com.system.permissions", &object,
                            QDBusConnection::ExportAllSlots);
  return app.exec();
}
