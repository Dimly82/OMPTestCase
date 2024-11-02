#include "systemTime.h"

#include <QDBusConnectionInterface>
#include <csignal>

quint64 Time::GetSystemTime() {
  // Get name of the executable
  QString exePath =
      QString("/proc/%1/exe")
          .arg(connection().interface()->servicePid(message().service()));
  char resolvedPath[PATH_MAX];
  ssize_t len = readlink(exePath.toStdString().c_str(), resolvedPath,
                         sizeof(resolvedPath) - 1);

  if (len == -1) {
    throw QDBusError(
        QDBusError::InternalError,
        QString("Failed to read executable path for PID %1")
            .arg(connection().interface()->servicePid(message().service())));
  }
  resolvedPath[len] = '\0';

  QDBusConnection bus = QDBusConnection::sessionBus();
  QDBusMessage message = QDBusMessage::createMethodCall(
      "com.system.permissions", "/", "com.system.permissions",
      "CheckApplicationHasPermission");
  message << resolvedPath << 0;

  QDBusReply<bool> reply = bus.call(message);
  bool hasPermission = reply.value();

  if (!hasPermission)
    throw QDBusError(QDBusError::AccessDenied,
                     "Application does not have permission to use this method");

  // Getting current system time
  auto now = std::chrono::system_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      now.time_since_epoch());
  return duration.count();
}
