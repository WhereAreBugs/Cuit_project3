// changepassworddialog.h
#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QString> // For username

QT_BEGIN_NAMESPACE

namespace Ui {
    class ChangePasswordDialog;
}

QT_END_NAMESPACE

// No forward declaration for LoginDialog needed if we don't call its methods directly
class VirtualKeyboard; // Forward declaration

class ChangePasswordDialog : public QDialog {
    Q_OBJECT

public:
    // Pass the username whose password is to be changed
    explicit ChangePasswordDialog(const QString &username, QWidget *parent = nullptr);

    ~ChangePasswordDialog();

private slots:
    void on_saveButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::ChangePasswordDialog *ui;
    QString m_username; // Store the username
    VirtualKeyboard *m_virtualKeyboard; // <<< ADD THIS
    QLineEdit *m_currentLineEditWithKeyboard; // <<< ADD THIS

    void setupVirtualKeyboard(); // <<< ADD THIS
    void showVirtualKeyboardFor(QLineEdit *lineEdit); // <<< ADD THIS
    void hideVirtualKeyboard(); // <<< ADD THIS

protected:
    bool eventFilter(QObject *obj, QEvent *event) override; // <<< ADD THIS
};

#endif // CHANGEPASSWORDDIALOG_H
