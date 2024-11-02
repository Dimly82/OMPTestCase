#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDateTime>
#include <QDebug>

int requestPermission(QDBusConnection bus);

int main() {
  QDBusConnection bus = QDBusConnection::sessionBus();
  QDBusMessage timeMsg = QDBusMessage::createMethodCall(
      "com.system.time", "/", "com.system.time", "GetSystemTime");

  qDebug() << "Attempting to get system time...";
  QDBusReply<quint64> timeReply = bus.call(timeMsg);
  while (!timeReply.isValid()) {
    qDebug() << timeReply.error().message();
    if (requestPermission(bus) == -1) return -1;
    timeReply = bus.call(timeMsg);
  }
  qDebug() << QDateTime::fromMSecsSinceEpoch(timeReply.value())
                  .toString("dd/MM/yyyy HH:mm:ss");
}

int requestPermission(QDBusConnection bus) {
  QDBusMessage permMsg = QDBusMessage::createMethodCall(
      "com.system.permissions", "/", "com.system.permissions",
      "RequestPermission");
  permMsg << 0;
  qDebug() << "Requesting permission...";
  QDBusReply<void> permReply = bus.call(permMsg);
  if (!permReply.isValid()) {
    qDebug() << permReply.error().message();
    return -1;
  }
  qDebug() << "Permission granted";
  return 0;
}