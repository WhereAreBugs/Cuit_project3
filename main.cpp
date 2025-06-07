#include "mypro.h"
#include <QPushButton>
#include <QApplication>
#include <QAction>

#include "logindialog.h" // The new login dialog
#include <QMessageBox> // For showing messages if needed

#include "databasemanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyPro w;
    LoginDialog loginDialog;
    QCoreApplication::setOrganizationName("CUIT");
    QCoreApplication::setApplicationName("Project_CUIT_2");

    if (!DatabaseManager::instance().initDatabase()) {
        QMessageBox::critical(nullptr, "Database Error",
                              "Failed to initialize the application database. The application will now exit.");
        return 1; // Exit with an error code
    }
    loginDialog.move(0, 0);
    if (loginDialog.exec() == QDialog::Accepted) {
    // if (1) {
        // DatabaseManager::instance().close();
    } else {
        return 0;
    }
    w.show();
    return QApplication::exec();
}
