// changepassworddialog.cpp
#include "changepassworddialog.h"
#include "ui_changepassworddialog.h"
#include "databasemanager.h"
#include <QMessageBox>
#include "virtualkeyboard.h"
#include <QEvent>       // For event filter
#include <QFocusEvent>  // For event filter
#include <QTimer>       // For delayed hide
#include <QScreen>

ChangePasswordDialog::ChangePasswordDialog(const QString &username, QWidget *parent) : QDialog(parent),
    ui(new Ui::ChangePasswordDialog),
    m_username(username),
    m_virtualKeyboard(nullptr),
    m_currentLineEditWithKeyboard(nullptr)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->saveButton, &QPushButton::clicked, this, &ChangePasswordDialog::on_saveButton_clicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &ChangePasswordDialog::on_cancelButton_clicked);
    setupVirtualKeyboard();
}

ChangePasswordDialog::~ChangePasswordDialog() {
    delete ui;
}

void ChangePasswordDialog::setupVirtualKeyboard()
{
    m_virtualKeyboard = new VirtualKeyboard(this); // Parent to this dialog
    m_virtualKeyboard->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    m_virtualKeyboard->hide();

    ui->oldPasswordLineEdit->installEventFilter(this);
    ui->newPasswordLineEdit->installEventFilter(this);
    ui->confirmPasswordLineEdit->installEventFilter(this);

    connect(m_virtualKeyboard, &VirtualKeyboard::enterPressed, this, &ChangePasswordDialog::on_saveButton_clicked);
    connect(m_virtualKeyboard, &VirtualKeyboard::escapePressed, this, [this](){
        hideVirtualKeyboard();
        if (m_currentLineEditWithKeyboard) m_currentLineEditWithKeyboard->clearFocus();
    });
}

void ChangePasswordDialog::showVirtualKeyboardFor(QLineEdit *lineEdit)
{
    if (!m_virtualKeyboard || !lineEdit) return;

    m_currentLineEditWithKeyboard = lineEdit;
    m_virtualKeyboard->setTargetLineEdit(lineEdit);
    QPoint globalPos = lineEdit->mapToGlobal(QPoint(0, lineEdit->height() + 40)); // +2 for a small gap
    m_virtualKeyboard->move(globalPos);

    // Ensure it's not shown off-screen
    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    if (globalPos.x() + m_virtualKeyboard->width() > screenGeometry.right()) {
        globalPos.setX(screenGeometry.right() - m_virtualKeyboard->width());
    }
    if (globalPos.y() + m_virtualKeyboard->height() > screenGeometry.bottom()) {
        globalPos.setY(lineEdit->mapToGlobal(QPoint(0,0)).y() - m_virtualKeyboard->height() - 2); // Show above
    }
    if (globalPos.x() < screenGeometry.left()) globalPos.setX(screenGeometry.left());
    if (globalPos.y() < screenGeometry.top()) globalPos.setY(screenGeometry.top());

    m_virtualKeyboard->move(globalPos);
    m_virtualKeyboard->show();
}

void ChangePasswordDialog::hideVirtualKeyboard()
{
    if (m_virtualKeyboard) {
        m_virtualKeyboard->hide();
    }
    this->setFocus();
    m_currentLineEditWithKeyboard = nullptr;
}

bool ChangePasswordDialog::eventFilter(QObject *obj, QEvent *event)
{
    QLineEdit *targetLineEdit = qobject_cast<QLineEdit*>(obj);
    if (targetLineEdit && (obj == ui->oldPasswordLineEdit || obj == ui->newPasswordLineEdit || obj == ui->confirmPasswordLineEdit)) {
        if (event->type() == QEvent::FocusIn) {
            showVirtualKeyboardFor(targetLineEdit);
        } else if (event->type() == QEvent::FocusOut) {
            // More robust FocusOut handling: Hide if the new focus isn't the keyboard
            // This requires the keyboard to not accept focus itself OR careful checking
            QFocusEvent* fe = static_cast<QFocusEvent*>(event);
            if (m_virtualKeyboard && m_virtualKeyboard->isVisible()) {
                // Check if the reason for focus out is that a popup (like the keyboard itself, if it could take focus) appeared
                if (fe->reason() != Qt::PopupFocusReason) {
                     // Check if the mouse is over the keyboard. If not, schedule a hide.
                     // This allows clicking on the keyboard buttons without it immediately disappearing.
                    if (!m_virtualKeyboard->geometry().contains(m_virtualKeyboard->mapFromGlobal(QCursor::pos()))) {
                        QTimer::singleShot(150, this, [this, targetLineEdit]() {
                            // Check again: if current focus is still this line edit or keyboard hasn't changed target, then hide
                            if (m_currentLineEditWithKeyboard == targetLineEdit &&
                                (!QApplication::focusWidget() || !m_virtualKeyboard->isAncestorOf(QApplication::focusWidget())) &&
                                !m_virtualKeyboard->geometry().contains(m_virtualKeyboard->mapFromGlobal(QCursor::pos()))
                                ) {
                                // hideVirtualKeyboard(); // This might be too aggressive
                            }
                        });
                    }
                }
            }
        } else if (event->type() == QEvent::MouseButtonPress && m_virtualKeyboard) {
            if (!m_virtualKeyboard->isVisible() || m_currentLineEditWithKeyboard != targetLineEdit) {
                 showVirtualKeyboardFor(targetLineEdit);
            } else {
                 m_virtualKeyboard->setTargetLineEdit(targetLineEdit); // Re-affirm target
            }
        }
    }
    // Hide keyboard if user clicks outside the current line edit and the keyboard itself
    if (event->type() == QEvent::MouseButtonPress && m_virtualKeyboard && m_virtualKeyboard->isVisible()) {
        if (m_currentLineEditWithKeyboard && obj != m_currentLineEditWithKeyboard && !m_virtualKeyboard->isAncestorOf(qobject_cast<QWidget*>(obj))) {
            if (!m_virtualKeyboard->geometry().contains(static_cast<QMouseEvent*>(event)->globalPos()))
            {
                 hideVirtualKeyboard();
            }
        }
    }


    return QDialog::eventFilter(obj, event);
}

void ChangePasswordDialog::on_saveButton_clicked() {
    QString oldPassword = ui->oldPasswordLineEdit->text();
    QString newPassword = ui->newPasswordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();

    // Old password check is now part of DatabaseManager::changePassword
    // but we can provide immediate feedback if new passwords don't match or are empty

    if (newPassword.isEmpty()) {
        QMessageBox::warning(this, "Error", "New password cannot be empty.");
        ui->newPasswordLineEdit->setFocus();
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "Error", "New passwords do not match.");
        ui->newPasswordLineEdit->clear();
        ui->confirmPasswordLineEdit->clear();
        ui->newPasswordLineEdit->setFocus();
        return;
    }

    if (DatabaseManager::instance().changePassword(m_username, oldPassword, newPassword)) {
        QMessageBox::information(this, "Success", "Password changed successfully for '" + m_username + "'.");
        accept();
    } else {
        QMessageBox::warning(this, "Error",
                             "Failed to change password. The old password might be incorrect or a database error occurred.");
        ui->oldPasswordLineEdit->setFocus(); // Focus on old password as it's the likely culprit
        // ui->oldPasswordLineEdit->selectAll(); // Optional: select text for easy re-entry
    }
}

void ChangePasswordDialog::on_cancelButton_clicked() {
    reject();
}
