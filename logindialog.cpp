// logindialog.cpp
#include "logindialog.h"
#include "ui_logindialog.h"
#include "changepassworddialog.h"
#include "databasemanager.h" // Include DatabaseManager
#include <QMessageBox>
#include <QInputDialog> // For initial password setup
#include <QTimer>      // For deferred initial setup
#include "virtualkeyboard.h" // <<< INCLUDE THIS
#include <QEvent>       // For event filter
#include <QFocusEvent>  // For event filter
#include <QScreen>

LoginDialog::LoginDialog(QWidget *parent) : QDialog(parent),
                                            ui(new Ui::LoginDialog),
                                            m_virtualKeyboard(nullptr),
                                            m_currentLineEditWithKeyboard(nullptr),
                                            m_changePasswordDialog(nullptr) {
    ui->setupUi(this);

    QTimer::singleShot(0, this, &LoginDialog::handleInitialSetup);
    setupVirtualKeyboard();
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::setupVirtualKeyboard() {
    if (m_virtualKeyboard == nullptr) {
        m_virtualKeyboard = new VirtualKeyboard(this); // Parent to this dialog
    }
    m_virtualKeyboard->setWindowFlags(Qt::ToolTip |  Qt::WindowStaysOnTopHint); // Popup style
    m_virtualKeyboard->hide();

    // Install event filter to detect focus on password field
    ui->passwordLineEdit->installEventFilter(this);

    // Connect Enter/Esc from virtual keyboard
    connect(m_virtualKeyboard, &VirtualKeyboard::enterPressed, this, &LoginDialog::on_loginButton_clicked);
    connect(m_virtualKeyboard, &VirtualKeyboard::escapePressed, this, [this]() {
        hideVirtualKeyboard();
        ui->passwordLineEdit->clearFocus(); // Or focus another widget
    });
}

void LoginDialog::showVirtualKeyboardFor(QLineEdit *lineEdit) {
    if (!m_virtualKeyboard || !lineEdit) return;

    m_currentLineEditWithKeyboard = lineEdit;
    m_virtualKeyboard->setTargetLineEdit(lineEdit);

    // Position the keyboard below the line edit
    QPoint globalPos = lineEdit->mapToGlobal(QPoint(-50, lineEdit->height()+70));
    m_virtualKeyboard->move(globalPos);
    m_virtualKeyboard->show();
    m_virtualKeyboard->activateWindow(); // May not be needed with WindowDoesNotAcceptFocus
}

void LoginDialog::hideVirtualKeyboard() {
    if (m_virtualKeyboard) {
        m_virtualKeyboard->hide();
    }
    m_currentLineEditWithKeyboard = nullptr;
}

bool LoginDialog::eventFilter(QObject *obj, QEvent *event) {
    if (obj == ui->passwordLineEdit) {
        if (event->type() == QEvent::FocusIn) {
            showVirtualKeyboardFor(ui->passwordLineEdit);
        } else if (event->type() == QEvent::FocusOut) {
            // Hide only if focus doesn't go to the keyboard itself (tricky)
            // A simpler approach: hide if focus goes to something other than the current line edit
            // For more robustness, you might need to check if QGuiApplication::focusWindow() is the keyboard
            QFocusEvent *fe = static_cast<QFocusEvent *>(event);
            if (m_virtualKeyboard && !m_virtualKeyboard->geometry().contains(QCursor::pos()) && fe->reason() !=
                Qt::PopupFocusReason) {
                // Delay hiding to allow clicks on keyboard
                // QTimer::singleShot(100, this, &LoginDialog::hideVirtualKeyboard);
                // Or hide if the new focus widget is not the keyboard or its child
                QWidget *newFocus = QApplication::focusWidget();
                if (!m_virtualKeyboard || (newFocus && !m_virtualKeyboard->isAncestorOf(newFocus))) {
                    // QTimer::singleShot(100, this, &LoginDialog::hideVirtualKeyboard);
                }
            }
        } else if (event->type() == QEvent::MouseButtonPress && m_virtualKeyboard) {
            // If user clicks on the line edit again while keyboard is visible,
            // ensure keyboard is shown and targeted correctly.
            if (!m_virtualKeyboard->isVisible()) {
                showVirtualKeyboardFor(ui->passwordLineEdit);
            } else {
                m_virtualKeyboard->setTargetLineEdit(ui->passwordLineEdit); // Re-affirm target
            }
        }
    }
    return QDialog::eventFilter(obj, event);
}

void LoginDialog::handleInitialSetup() {
    if (!DatabaseManager::instance().userExists(developerUsername)) {
        QMessageBox msgBoxInfo(this);
        msgBoxInfo.setIcon(QMessageBox::Information);
        msgBoxInfo.setWindowTitle("Initial Setup Required");
        msgBoxInfo.setText(
            "No developer account found.\nA virtual keyboard will be shown to help you set an initial password.");
        msgBoxInfo.setStandardButtons(QMessageBox::Ok);
        msgBoxInfo.exec();

        // Ensure virtual keyboard is created if it wasn't already (e.g., if setupVirtualKeyboard is called later)
        // setupVirtualKeyboard() should have been called in LoginDialog constructor.
        if (!m_virtualKeyboard) {
            qWarning("Virtual keyboard not initialized in LoginDialog constructor before handleInitialSetup!");
            // Attempt to set it up now, though it's better if done in constructor
            setupVirtualKeyboard();
            if (!m_virtualKeyboard) {
                // If still not set up, can't proceed with VK
                QMessageBox::critical(this, "Error", "Virtual keyboard could not be initialized.");
            }
        }

        bool passwordSet = false;
        while (!passwordSet) {
            QInputDialog inputDialog(this);
            inputDialog.setWindowTitle("Set Initial Password");
            inputDialog.setLabelText(QString("Enter a password for the '%1' account:").arg(developerUsername));
            inputDialog.setTextEchoMode(QLineEdit::Password);
            inputDialog.setCancelButtonText("Cancel Setup");

            if (inputDialog.exec() == QDialog::Accepted) {
                QString password = inputDialog.textValue();
                if (!password.isEmpty()) {
                    if (DatabaseManager::instance().createDeveloperUser(password)) {
                        QMessageBox::information(this, "Setup Complete",
                                                 "Developer account created. You can now log in.");
                        passwordSet = true;
                    } else {
                        QMessageBox::critical(this, "Setup Failed",
                                              "Could not create developer account. Please try again.");
                    }
                } else {
                    QMessageBox::warning(this, "Invalid Input", "Password cannot be empty. Using 123 as password.");
                    if (DatabaseManager::instance().createDeveloperUser("123")) {
                        QMessageBox::information(this, "Setup Complete",
                                                 "Developer account created. You can now log in.");
                        passwordSet = true;
                    } else {
                        QMessageBox::critical(this, "Setup Failed",
                                              "Could not create developer account. Please try again.");
                    }
                }
            } else {
                // User cancelled QInputDialog
                QMessageBox::StandardButton reply;
                reply = QMessageBox::warning(this, "Setup Aborted",
                                             "Initial password setup was cancelled. The application requires this setup to continue.\n"
                                             "Do you want to try setting the password again?",
                                             QMessageBox::Retry | QMessageBox::Abort);
                if (reply == QMessageBox::Abort) {
                    if (m_virtualKeyboard) m_virtualKeyboard->hide();
                    QApplication::processEvents();
                    reject();
                    return;
                }
            }
        } // end while

        // Hide virtual keyboard once done with setup
        if (m_virtualKeyboard) {
            m_virtualKeyboard->hide();
            QApplication::processEvents();
        }

        if (!passwordSet) {
            // Should only be if loop logic has an issue or Abort path wasn't fully exited
            reject();
        }
    }
}

void LoginDialog::on_loginButton_clicked() {
    m_virtualKeyboard->hide();
    QString enteredPassword = ui->passwordLineEdit->text();

    if (DatabaseManager::instance().verifyPassword(developerUsername, enteredPassword)) {
        QMessageBox::information(this, "登陆成功", "Welcome...");
        accept();
        delete m_virtualKeyboard;
    } else {
        QMessageBox::warning(this, "Login Failed", "Incorrect password.");
        ui->passwordLineEdit->clear();
        ui->passwordLineEdit->setFocus();
    }
}

void LoginDialog::on_changePasswordButton_clicked() {
    // The ChangePasswordDialog will now directly use DatabaseManager
    m_virtualKeyboard->hide();
    // It needs to know the username, which is fixed here.
    m_changePasswordDialog = new ChangePasswordDialog(developerUsername, this);
    m_changePasswordDialog->exec();
}
