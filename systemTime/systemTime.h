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

class Time : public QObject, protected QDBusContext {
  Q_OBJECT

 public:
  Time() = default;

 public slots:
  quint64 GetSystemTime();
};

#endif  // DBUS_SERVICE_H
