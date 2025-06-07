// virtualkeyboard.cpp
#include "virtualkeyboard.h"
#include "ui_virtualkeyboard.h" // Generated from the QGridLayout virtualkeyboard.ui
#include <QDebug>
#include <QVariant> // For setProperty / property

VirtualKeyboard::VirtualKeyboard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::VirtualKeyboard),
    m_targetLineEdit(nullptr),
    m_capsLockActive(false),
    m_shiftActive(false)
{
    ui->setupUi(this);

    // Populate letter buttons (for case changing)
    m_letterButtons = {
        ui->pushButton_q, ui->pushButton_w, ui->pushButton_e, ui->pushButton_r, ui->pushButton_t,
        ui->pushButton_y, ui->pushButton_u, ui->pushButton_i, ui->pushButton_o, ui->pushButton_p,
        ui->pushButton_a, ui->pushButton_s, ui->pushButton_d, ui->pushButton_f, ui->pushButton_g,
        ui->pushButton_h, ui->pushButton_j, ui->pushButton_k, ui->pushButton_l,
        ui->pushButton_z, ui->pushButton_x, ui->pushButton_c, ui->pushButton_v, ui->pushButton_b,
        ui->pushButton_n, ui->pushButton_m
    };

    // All buttons that produce a character directly
    m_characterProducingButtons = {
        // Row 0 Symbols
        ui->pushButton_tilde, ui->pushButton_exclamation, ui->pushButton_at, ui->pushButton_hash,
        ui->pushButton_dollar, ui->pushButton_percent, ui->pushButton_caret, ui->pushButton_ampersand,
        ui->pushButton_asterisk, ui->pushButton_parenthesisLeft, ui->pushButton_parenthesisRight,
        ui->pushButton_minus, ui->pushButton_plus,
        // Row 1 Numbers & Letters
        ui->pushButton_1, ui->pushButton_2, ui->pushButton_3, ui->pushButton_4, ui->pushButton_5,
        ui->pushButton_6, ui->pushButton_7, ui->pushButton_8, ui->pushButton_9, ui->pushButton_0,
        ui->pushButton_q, ui->pushButton_w, ui->pushButton_e, ui->pushButton_r, ui->pushButton_t,
        // Row 2 Letters
        ui->pushButton_y, ui->pushButton_u, ui->pushButton_i, ui->pushButton_o, ui->pushButton_p,
        ui->pushButton_a, ui->pushButton_s, ui->pushButton_d, ui->pushButton_f, ui->pushButton_g,
        ui->pushButton_h, ui->pushButton_j, ui->pushButton_k, ui->pushButton_l,
        // Row 3 Letters & Symbols
        ui->pushButton_z, ui->pushButton_x, ui->pushButton_c, ui->pushButton_v, ui->pushButton_b,
        ui->pushButton_n, ui->pushButton_m,
        ui->pushButton_comma, ui->pushButton_dot, ui->pushButton_slash, ui->pushButton_question,
        // Row 4 Symbols
        ui->pushButton_underscore
    };

    storeOriginalCharacters();
    updateButtonCases();

    ui->displayLineEdit->setVisible(false); // Typically hidden
}

// Added override specifier for clarity and to catch potential signature mismatches
// with base class QWidget destructor (though QWidget's destructor isn't virtual by default,
// QObject's is, and QWidget inherits QObject). It's good practice for QObject subclasses.
VirtualKeyboard::~VirtualKeyboard()
{
    delete ui;
}

void VirtualKeyboard::setTargetLineEdit(QLineEdit *lineEdit)
{
    m_targetLineEdit = lineEdit;
    if (m_targetLineEdit && ui->displayLineEdit->isVisible()) {
        ui->displayLineEdit->setText(m_targetLineEdit->text());
        ui->displayLineEdit->setCursorPosition(m_targetLineEdit->cursorPosition());
    }
}

QLineEdit* VirtualKeyboard::targetLineEdit() const
{
    return m_targetLineEdit;
}

void VirtualKeyboard::storeOriginalCharacters()
{
    for (QPushButton* button : m_characterProducingButtons) {
        if(button) {
            button->setProperty("originalChar", button->text());
        } else {
            qWarning() << "A button pointer is null in m_characterProducingButtons during storeOriginalCharacters().";
        }
    }
}

void VirtualKeyboard::handleCharacterPress(const QString &character)
{
    if (!m_targetLineEdit) return;
    m_targetLineEdit->insert(character);
    m_targetLineEdit->setFocus();
    emit characterPressed(character);

    if (ui->displayLineEdit->isVisible()) {
        ui->displayLineEdit->setText(m_targetLineEdit->text());
        ui->displayLineEdit->setCursorPosition(m_targetLineEdit->cursorPosition());
    }

    if (m_shiftActive) {
        m_shiftActive = false;
        updateButtonCases();
    }
}

// --- Implementations for all declared slots ---

// Row 0 Symbols
void VirtualKeyboard::on_pushButton_tilde_clicked() { handleCharacterPress(ui->pushButton_tilde->text()); }
void VirtualKeyboard::on_pushButton_exclamation_clicked() { handleCharacterPress(ui->pushButton_exclamation->text()); }
void VirtualKeyboard::on_pushButton_at_clicked() { handleCharacterPress(ui->pushButton_at->text()); }
void VirtualKeyboard::on_pushButton_hash_clicked() { handleCharacterPress(ui->pushButton_hash->text()); }
void VirtualKeyboard::on_pushButton_dollar_clicked() { handleCharacterPress(ui->pushButton_dollar->text()); }
void VirtualKeyboard::on_pushButton_percent_clicked() { handleCharacterPress(ui->pushButton_percent->text()); }
void VirtualKeyboard::on_pushButton_caret_clicked() { handleCharacterPress(ui->pushButton_caret->text()); }
void VirtualKeyboard::on_pushButton_ampersand_clicked() { handleCharacterPress(ui->pushButton_ampersand->text()); }
void VirtualKeyboard::on_pushButton_asterisk_clicked() { handleCharacterPress(ui->pushButton_asterisk->text()); }
void VirtualKeyboard::on_pushButton_parenthesisLeft_clicked() { handleCharacterPress(ui->pushButton_parenthesisLeft->text()); }
void VirtualKeyboard::on_pushButton_parenthesisRight_clicked() { handleCharacterPress(ui->pushButton_parenthesisRight->text()); }
void VirtualKeyboard::on_pushButton_minus_clicked() { handleCharacterPress(ui->pushButton_minus->text()); }
void VirtualKeyboard::on_pushButton_plus_clicked() { handleCharacterPress(ui->pushButton_plus->text()); }

// Row 1 Numbers & Letters
void VirtualKeyboard::on_pushButton_1_clicked() { handleCharacterPress(ui->pushButton_1->text()); }
void VirtualKeyboard::on_pushButton_2_clicked() { handleCharacterPress(ui->pushButton_2->text()); }
void VirtualKeyboard::on_pushButton_3_clicked() { handleCharacterPress(ui->pushButton_3->text()); }
void VirtualKeyboard::on_pushButton_4_clicked() { handleCharacterPress(ui->pushButton_4->text()); }
void VirtualKeyboard::on_pushButton_5_clicked() { handleCharacterPress(ui->pushButton_5->text()); }
void VirtualKeyboard::on_pushButton_6_clicked() { handleCharacterPress(ui->pushButton_6->text()); }
void VirtualKeyboard::on_pushButton_7_clicked() { handleCharacterPress(ui->pushButton_7->text()); }
void VirtualKeyboard::on_pushButton_8_clicked() { handleCharacterPress(ui->pushButton_8->text()); }
void VirtualKeyboard::on_pushButton_9_clicked() { handleCharacterPress(ui->pushButton_9->text()); }
void VirtualKeyboard::on_pushButton_0_clicked() { handleCharacterPress(ui->pushButton_0->text()); }
void VirtualKeyboard::on_pushButton_q_clicked() { handleCharacterPress(ui->pushButton_q->text()); }
void VirtualKeyboard::on_pushButton_w_clicked() { handleCharacterPress(ui->pushButton_w->text()); }
void VirtualKeyboard::on_pushButton_e_clicked() { handleCharacterPress(ui->pushButton_e->text()); }
void VirtualKeyboard::on_pushButton_r_clicked() { handleCharacterPress(ui->pushButton_r->text()); }
void VirtualKeyboard::on_pushButton_t_clicked() { handleCharacterPress(ui->pushButton_t->text()); }

// Row 2 Letters
void VirtualKeyboard::on_pushButton_y_clicked() { handleCharacterPress(ui->pushButton_y->text()); }
void VirtualKeyboard::on_pushButton_u_clicked() { handleCharacterPress(ui->pushButton_u->text()); }
void VirtualKeyboard::on_pushButton_i_clicked() { handleCharacterPress(ui->pushButton_i->text()); }
void VirtualKeyboard::on_pushButton_o_clicked() { handleCharacterPress(ui->pushButton_o->text()); }
void VirtualKeyboard::on_pushButton_p_clicked() { handleCharacterPress(ui->pushButton_p->text()); }
void VirtualKeyboard::on_pushButton_a_clicked() { handleCharacterPress(ui->pushButton_a->text()); }
void VirtualKeyboard::on_pushButton_s_clicked() { handleCharacterPress(ui->pushButton_s->text()); }
void VirtualKeyboard::on_pushButton_d_clicked() { handleCharacterPress(ui->pushButton_d->text()); }
void VirtualKeyboard::on_pushButton_f_clicked() { handleCharacterPress(ui->pushButton_f->text()); }
void VirtualKeyboard::on_pushButton_g_clicked() { handleCharacterPress(ui->pushButton_g->text()); }
void VirtualKeyboard::on_pushButton_h_clicked() { handleCharacterPress(ui->pushButton_h->text()); }
void VirtualKeyboard::on_pushButton_j_clicked() { handleCharacterPress(ui->pushButton_j->text()); }
void VirtualKeyboard::on_pushButton_k_clicked() { handleCharacterPress(ui->pushButton_k->text()); }
void VirtualKeyboard::on_pushButton_l_clicked() { handleCharacterPress(ui->pushButton_l->text()); }

// Row 3 Letters & Symbols
void VirtualKeyboard::on_pushButton_z_clicked() { handleCharacterPress(ui->pushButton_z->text()); }
void VirtualKeyboard::on_pushButton_x_clicked() { handleCharacterPress(ui->pushButton_x->text()); }
void VirtualKeyboard::on_pushButton_c_clicked() { handleCharacterPress(ui->pushButton_c->text()); }
void VirtualKeyboard::on_pushButton_v_clicked() { handleCharacterPress(ui->pushButton_v->text()); }
void VirtualKeyboard::on_pushButton_b_clicked() { handleCharacterPress(ui->pushButton_b->text()); }
void VirtualKeyboard::on_pushButton_n_clicked() { handleCharacterPress(ui->pushButton_n->text()); }
void VirtualKeyboard::on_pushButton_m_clicked() { handleCharacterPress(ui->pushButton_m->text()); }
void VirtualKeyboard::on_pushButton_comma_clicked() { handleCharacterPress(ui->pushButton_comma->text()); }
void VirtualKeyboard::on_pushButton_dot_clicked() { handleCharacterPress(ui->pushButton_dot->text()); }
void VirtualKeyboard::on_pushButton_slash_clicked() { handleCharacterPress(ui->pushButton_slash->text()); }
void VirtualKeyboard::on_pushButton_question_clicked() { handleCharacterPress(ui->pushButton_question->text()); }

// Row 4 Symbols
void VirtualKeyboard::on_pushButton_underscore_clicked() { handleCharacterPress(ui->pushButton_underscore->text()); }

// Control buttons
void VirtualKeyboard::on_pushButtonBackspace_clicked()
{
    if (!m_targetLineEdit) return;
    m_targetLineEdit->backspace();
    m_targetLineEdit->setFocus();
    if (ui->displayLineEdit->isVisible()) {
        ui->displayLineEdit->setText(m_targetLineEdit->text());
        ui->displayLineEdit->setCursorPosition(m_targetLineEdit->cursorPosition());
    }
}

void VirtualKeyboard::on_pushButtonClear_clicked()
{
    if (!m_targetLineEdit) return;
    m_targetLineEdit->clear();
    m_targetLineEdit->setFocus();
    if (ui->displayLineEdit->isVisible()) {
        ui->displayLineEdit->clear();
    }
}

void VirtualKeyboard::on_pushButtonEnter_clicked()
{
    if (!m_targetLineEdit) return;
    emit enterPressed();
}

void VirtualKeyboard::on_pushButtonSpace_clicked()
{
    handleCharacterPress(" ");
}

void VirtualKeyboard::on_pushButtonCaps_toggled(bool checked)
{
    m_capsLockActive = checked;
    if (m_shiftActive) {
        m_shiftActive = false;
    }
    updateButtonCases();
}

void VirtualKeyboard::on_pushButtonShiftLeft_clicked()
{
    m_shiftActive = !m_shiftActive;
    updateButtonCases();
}

void VirtualKeyboard::on_pushButtonShiftRight_clicked()
{
    m_shiftActive = !m_shiftActive;
    updateButtonCases();
}

void VirtualKeyboard::updateButtonCases()
{
    bool makeUpper = m_capsLockActive ^ m_shiftActive;

    for (QPushButton* button : m_letterButtons) {
        if (!button) continue;
        QString originalChar = button->property("originalChar").toString();
        if (originalChar.isEmpty()) {
            qWarning() << "Button" << button->objectName() << "missing originalChar property.";
            continue;
        }
        if (makeUpper) {
            button->setText(originalChar.toUpper());
        } else {
            button->setText(originalChar.toLower());
        }
    }
}