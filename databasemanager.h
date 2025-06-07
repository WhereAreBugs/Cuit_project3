// databasemanager.h
#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariantMap>

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    static DatabaseManager& instance(); // Singleton instance

    bool initDatabase(); // Call this once at application startup
    bool userExists(const QString& username);
    bool createDeveloperUser(const QString& password); // For initial setup or reset
    bool verifyPassword(const QString& username, const QString& password);
    bool changePassword(const QString& username, const QString& oldPassword, const QString& newPassword);
    void close();
    // Utility for hashing
    static QByteArray hashPassword(const QString& password, const QByteArray& salt);
    static QByteArray generateSalt();

private:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase m_db;
    bool openDatabase();
    bool createUsersTable();
    QVariantMap getUserCredentials(const QString& username); // Fetches hash and salt
};

#endif // DATABASEMANAGER_H