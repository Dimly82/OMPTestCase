#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>

#include <climits>
#include <iostream>

#include "permissions_adaptor.h"

class Permissions
    : public sdbus::AdaptorInterfaces<com::system::permissions_adaptor> {
 public:
  Permissions(sdbus::IConnection &connection,
              const sdbus::ObjectPath &objectPath)
      : AdaptorInterfaces(connection, objectPath) {
    registerAdaptor();
    InitDB();
  }

  ~Permissions() { unregisterAdaptor(); }

 protected:
  void RequestPermission(const int32_t &permissionEnumCode) override {
    try {
      // Get name of the executable
      std::string exePath =
          "/proc/" +
          std::to_string(
              getObject().getCurrentlyProcessedMessage()->getCredsPid()) +
          "/exe";
      char resolvedPath[PATH_MAX];
      ssize_t len =
          readlink(exePath.c_str(), resolvedPath, sizeof(resolvedPath) - 1);

      if (len == -1) {
        throw sdbus::Error(
            "com.system.permissions.Error",
            "Failed to read executable path for PID " +
                std::to_string(
                    getObject().getCurrentlyProcessedMessage()->getCredsPid()));
      }
      resolvedPath[len] = '\0';

      // Connecting to db
      sqlite3 *db;
      if (sqlite3_open(_db_path.c_str(), &db))
        throw sdbus::Error("com.system.permissions.Error",
                           "Error connecting to database");

      // Query to insert requested permission
      std::string sql =
          "INSERT INTO Permissions (executable, permission_code) VALUES (?, "
          "?);";
      sqlite3_stmt *stmt;

      if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) !=
          SQLITE_OK) {
        throw sdbus::Error(
            "com.system.permissions.Error",
            "Error preparing SQL query: " + std::string(sqlite3_errmsg(db)));
      }

      // Insert arguments into the query
      sqlite3_bind_text(stmt, 1, resolvedPath, -1, SQLITE_TRANSIENT);
      sqlite3_bind_int(stmt, 2, permissionEnumCode);

      // Execute the query
      if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw sdbus::Error(
            "com.system.permissions.Error",
            "Error executing SQL query: " + std::string(sqlite3_errmsg(db)));
      }

      sqlite3_finalize(stmt);
      sqlite3_close(db);
    } catch (const sdbus::Error &e) {
      throw;
    } catch (const std::exception &e) {
      throw sdbus::Error("com.system.permissions.Error", e.what());
    }
  }

  bool CheckApplicationHasPermission(
      const std::string &applicationExecPath,
      const int32_t &permissionEnumCode) override {
    // Connecting to db
    sqlite3 *db;
    if (sqlite3_open(_db_path.c_str(), &db))
      throw sdbus::Error("com.system.permissions.Error",
                         "Error connecting to database");

    // Query to get rows with matching executable and permission
    std::string sql =
        "SELECT 1 FROM Permissions WHERE executable = ? AND permission_code = "
        "?;";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
      throw sdbus::Error(
          "com.system.permissions.Error",
          "Error preparing SQL query: " + std::string(sqlite3_errmsg(db)));
    }

    // Insert arguments into the query
    sqlite3_bind_text(stmt, 1, applicationExecPath.c_str(), -1,
                      SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, permissionEnumCode);

    // Execute the query. If it returns a row, permission is given
    bool hasPermission = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) hasPermission = true;

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return hasPermission;
  }

 private:
  std::string _db_path = "./db.sqlite";

  void InitDB() {
    sqlite3 *db;
    if (sqlite3_open(_db_path.c_str(), &db) != SQLITE_OK)
      throw sdbus::Error("com.system.permissions.Error",
                         "Error connecting to database");

    const char *createTableSQL =
        "CREATE TABLE IF NOT EXISTS Permissions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "executable TEXT NOT NULL,"
        "permission_code INTEGER NOT NULL);";
    char *errMsg;
    if (sqlite3_exec(db, createTableSQL, nullptr, nullptr, &errMsg) !=
        SQLITE_OK) {
      std::string error = "Error creating a table: ";
      error += errMsg;
      sqlite3_free(errMsg);
      sqlite3_close(db);
      throw sdbus::Error("com.system.permissions.Error", error);
    }
    sqlite3_close(db);
  }
};
