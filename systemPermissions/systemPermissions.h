#ifndef DBUS_SERVICE_H
#define DBUS_SERVICE_H

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QObject>
#include <QProcess>
#include <qt5/QtDBus/QDBusContext>

class Permissions : public QObject, protected QDBusContext {
  Q_OBJECT

 public:
  Permissions();

 private:
  QString _dbPath = "./db.sqlite";

  void InitDB();

 public slots:
  void RequestPermission(int permissionEnumCode);
  bool CheckApplicationHasPermission(QString applicationExecPath,
                                     int permissionEnumCode);
};

#endif  // DBUS_SERVICE_H
