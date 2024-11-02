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

    if (len == -1)
      throw QDBusError(
          QDBusError::InternalError,
          QString("Failed to read executable path for PID %1")
              .arg(connection().interface()->servicePid(message().service())));
    resolvedPath[len] = '\0';

    // Connect to the database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(_dbPath);

    if (!db.open())
      throw QDBusError(QDBusError::InternalError,
                       "Error connecting to database");

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
      throw QDBusError(QDBusError::InternalError,
                       QString("Error executing SQL query: %1")
                           .arg(query.lastError().text()));
    }

    db.close();
  } catch (const QDBusError &e) {
    throw;
  } catch (const std::exception &e) {
    throw QDBusError(QDBusError::InternalError, e.what());
  }
}

bool Permissions::CheckApplicationHasPermission(QString applicationExecPath,
                                                int permissionEnumCode) {
  // Connect to the database
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(_dbPath);

  if (!db.open()) {
    qCritical() << "Error connecting to database:" << db.lastError().text();
    throw std::runtime_error("Error connecting to database");
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
    qCritical() << "Error executing SQL query:" << query.lastError().text();
    throw std::runtime_error("Error executing SQL query: " +
                             query.lastError().text().toStdString());
  }

  db.close();
  return hasPermission;
}

void Permissions::InitDB() {
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.setDatabaseName(_dbPath);
  if (!db.open())
    throw QDBusError(QDBusError::InternalError, "Error connecting to database");

  QSqlQuery query;
  query.prepare(
      "CREATE TABLE IF NOT EXISTS Permissions ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "executable TEXT NOT NULL,"
      "permission_code INTEGER NOT NULL);");
  if (!query.exec()) {
    db.close();
    throw QDBusError(
        QDBusError::InternalError,
        QString("Error executing SQL query: %1").arg(query.lastError().text()));
  }
  db.close();
}
