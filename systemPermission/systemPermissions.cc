#include "systemPermissions.h"

#include <QDBusConnectionInterface>
#include <QDBusContext>
#include <QDBusMessage>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <csignal>

Permissions::Permissions() { InitDB(); }

void Permissions::RequestPermission(int permissionEnumCode) {
  try {
    // Get name of the executable
    QString exePath =
        QString("/proc/%1/exe")
            .arg(connection().interface()->servicePid(message().service()));
    char resolvedPath[PATH_MAX];
    ssize_t len = readlink(exePath.toStdString().c_str(), resolvedPath,
                           sizeof(resolvedPath) - 1);

    if (len == -1) {
      QDBusMessage errorReply = message().createErrorReply(
          QDBusError::InternalError,
          QString("Failed to read executable path for PID %1")
              .arg(connection().interface()->servicePid(message().service())));
      connection().send(errorReply);
      return;
    }
    resolvedPath[len] = '\0';

    // Connect to the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(_dbPath);

    if (!db.open()) {
      QDBusMessage errorReply = message().createErrorReply(
          QDBusError::InternalError, "Error connecting to the database");
      connection().send(errorReply);
      return;
    }

    // Prepare the SQL query
    QSqlQuery query;
    query.prepare(
        "INSERT INTO Permissions (executable, permission_code) VALUES "
        "(:executable, :permission_code);");
    query.bindValue(":executable",
                    QString(const_cast<const char *>(resolvedPath)));
    query.bindValue(":permission_code", permissionEnumCode);

    // Execute the query
    if (!query.exec()) {
      db.close();
      QDBusMessage errorReply = message().createErrorReply(
          QDBusError::InternalError,
          QString("Error executing SQL query: ").arg(query.lastError().text()));
      connection().send(errorReply);
      return;
    }

    db.close();
  } catch (const QDBusError &e) {
    throw;
  } catch (const std::exception &e) {
    QDBusMessage errorReply =
        message().createErrorReply(QDBusError::InternalError, e.what());
    connection().send(errorReply);
    return;
  }
}

bool Permissions::CheckApplicationHasPermission(QString applicationExecPath,
                                                int permissionEnumCode) {
  // Connect to the database
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(_dbPath);

  if (!db.open()) {
    QDBusMessage errorReply = message().createErrorReply(
        QDBusError::InternalError, "Error connecting to the database");
    connection().send(errorReply);
    return false;
  }

  // Prepare the SQL query to check permissions
  QSqlQuery query;
  query.prepare(
      "SELECT 1 FROM Permissions WHERE executable = :executable AND "
      "permission_code = :permission_code;");
  query.bindValue(":executable", applicationExecPath);
  query.bindValue(":permission_code", permissionEnumCode);

  // Execute the query
  bool hasPermission = false;
  if (query.exec()) {
    if (query.next()) {  // If a row is returned, permission exists
      hasPermission = true;
    }
  } else {
    QDBusMessage errorReply = message().createErrorReply(
        QDBusError::InternalError,
        QString("Error executing SQL query: ").arg(query.lastError().text()));
    connection().send(errorReply);
    return false;
  }

  db.close();
  return hasPermission;
}

void Permissions::InitDB() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(_dbPath);
  if (!db.open()) {
    QDBusMessage errorReply = message().createErrorReply(
        QDBusError::InternalError, "Error connecting to the database");
    connection().send(errorReply);
    return;
  }

  QSqlQuery query;
  query.prepare(
      "CREATE TABLE IF NOT EXISTS Permissions ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "executable TEXT NOT NULL,"
      "permission_code INTEGER NOT NULL);");
  if (!query.exec()) {
    db.close();
    QDBusMessage errorReply = message().createErrorReply(
        QDBusError::InternalError,
        QString("Error executing SQL query: ").arg(query.lastError().text()));
    connection().send(errorReply);
    return;
  }
  db.close();
}
