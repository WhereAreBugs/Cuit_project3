// virtualkeyboard.h
#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton> // For QList<QPushButton*>

QT_BEGIN_NAMESPACE
namespace Ui { class VirtualKeyboard; }
QT_END_NAMESPACE

class VirtualKeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit VirtualKeyboard(QWidget *parent = nullptr);
    ~VirtualKeyboard();

    void setTargetLineEdit(QLineEdit *lineEdit);
    QLineEdit* targetLineEdit() const;

signals:
    void enterPressed();
    void escapePressed(); // Good to have, though not explicitly used by a button in current UI
    void characterPressed(const QString &character); // Emits the actual character pressed

private slots:
    // --- Slots for character-producing buttons (auto-connected by name) ---
    // Row 0 Symbols
    void on_pushButton_tilde_clicked();
    void on_pushButton_exclamation_clicked();
    void on_pushButton_at_clicked();
    void on_pushButton_hash_clicked();
    void on_pushButton_dollar_clicked();
    void on_pushButton_percent_clicked();
    void on_pushButton_caret_clicked();
    void on_pushButton_ampersand_clicked();
    void on_pushButton_asterisk_clicked();
    void on_pushButton_parenthesisLeft_clicked();
    void on_pushButton_parenthesisRight_clicked();
    void on_pushButton_minus_clicked();
    void on_pushButton_plus_clicked();
    // Backspace handled separately

    // Row 1 Numbers & Letters
    void on_pushButton_1_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_0_clicked();
    void on_pushButton_q_clicked();
    void on_pushButton_w_clicked();
    void on_pushButton_e_clicked();
    void on_pushButton_r_clicked();
    void on_pushButton_t_clicked();

    // Row 2 Letters
    void on_pushButton_y_clicked();
    void on_pushButton_u_clicked();
    void on_pushButton_i_clicked();
    void on_pushButton_o_clicked();
    void on_pushButton_p_clicked();
    void on_pushButton_a_clicked();
    void on_pushButton_s_clicked();
    void on_pushButton_d_clicked();
    void on_pushButton_f_clicked();
    void on_pushButton_g_clicked();
    void on_pushButton_h_clicked();
    void on_pushButton_j_clicked();
    void on_pushButton_k_clicked();
    void on_pushButton_l_clicked();
    // Enter handled separately

    // Row 3 Letters & Symbols
    void on_pushButton_z_clicked();
    void on_pushButton_x_clicked();
    void on_pushButton_c_clicked();
    void on_pushButton_v_clicked();
    void on_pushButton_b_clicked();
    void on_pushButton_n_clicked();
    void on_pushButton_m_clicked();
    void on_pushButton_comma_clicked();
    void on_pushButton_dot_clicked();
    void on_pushButton_slash_clicked();
    void on_pushButton_question_clicked();
    // Shift buttons handled separately

    // Row 4 Symbols
    void on_pushButton_underscore_clicked();
    // Space, Clear handled separately

    // --- Slots for control buttons ---
    void on_pushButtonBackspace_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonEnter_clicked();
    void on_pushButtonSpace_clicked();
    void on_pushButtonCaps_toggled(bool checked);
    void on_pushButtonShiftLeft_clicked();
    void on_pushButtonShiftRight_clicked();

private:
    Ui::VirtualKeyboard *ui;
    QLineEdit *m_targetLineEdit;
    bool m_capsLockActive;
    bool m_shiftActive;

    QList<QPushButton*> m_letterButtons;
    QList<QPushButton*> m_characterProducingButtons;

    void handleCharacterPress(const QString &character);
    void updateButtonCases();
    void storeOriginalCharacters();
};

#endif // VIRTUALKEYBOARD_H