// logindialog.h
#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
// #include <QString> // No longer needed for password storage here

QT_BEGIN_NAMESPACE

namespace Ui {
    class LoginDialog;
}

QT_END_NAMESPACE

class ChangePasswordDialog; // Forward declaration
class VirtualKeyboard; // Forward declaration

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    ~LoginDialog();

    const QString developerUsername = "developer";

private slots:
    void on_loginButton_clicked();

    void on_changePasswordButton_clicked();

    void handleInitialSetup(); // New slot for first-time setup

private:
    Ui::LoginDialog *ui;
    ChangePasswordDialog *m_changePasswordDialog; // Keep this
    VirtualKeyboard *m_virtualKeyboard; // <<< ADD THIS
    QLineEdit *m_currentLineEditWithKeyboard; // <<< ADD THIS

    void setupVirtualKeyboard(); // <<< ADD THIS
    void showVirtualKeyboardFor(QLineEdit *lineEdit); // <<< ADD THIS
    void hideVirtualKeyboard(); // <<< ADD THIS

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // <<< ADD THIS
};

#endif // LOGINDIALOG_H
