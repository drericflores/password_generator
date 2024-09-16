#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QClipboard>
#include <QRandomGenerator>
#include <QTranslator>
#include <QLocale>
#include <QSlider>
#include <QProgressBar>
#include <QDebug>
#include <QComboBox>
#include <QLibraryInfo>

// Main class for the Password Generator GUI
class PasswordGenerator : public QWidget {
    Q_OBJECT

public:
    PasswordGenerator(QWidget *parent = nullptr);

private slots:
    void generatePassword();
    void clearOutput();
    void copyToClipboard();
    void showAboutDialog();
    void quitApp();
    void updateStrengthIndicator();
    void toggleDarkMode();
    void setLanguage(const QString &languageCode);

private:
    QLineEdit *passwordOutput;
    QLineEdit *lengthInput;
    QCheckBox *upperCaseOnly;
    QCheckBox *lowerCaseOnly;
    QCheckBox *comboCase;
    QCheckBox *useSpecialChars;
    QCheckBox *useNumbers;
    QCheckBox *requireUpper;
    QCheckBox *requireLower;
    QCheckBox *requireNumber;
    QCheckBox *requireSpecial;
    QCheckBox *excludeSimilarChars;
    QLineEdit *customCharSetInput;
    QSlider *complexitySlider;
    QProgressBar *strengthBar;
    QTranslator translator;
    QLabel *strengthLabel;

    QString characterSet;
    QString currentLanguage;
    void setupUI();
    bool validateInput(unsigned int &length);
    int calculatePasswordStrength(const QString &password);
};

// Constructor: Initializes the UI and sets up the layout
PasswordGenerator::PasswordGenerator(QWidget *parent) : QWidget(parent), currentLanguage("en") {
    setupUI();
    setLanguage("en"); // Set default language to English
}

// Function to set up the UI components, including menus
void PasswordGenerator::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Menu bar
    QMenuBar *menuBar = new QMenuBar(this);
    
    QMenu *fileMenu = new QMenu(tr("File"), this);
    fileMenu->addAction(tr("Quit"), this, SLOT(quitApp()));
    menuBar->addMenu(fileMenu);

    QMenu *settingsMenu = new QMenu(tr("Settings"), this);
    settingsMenu->addAction(tr("Toggle Dark Mode"), this, SLOT(toggleDarkMode()));
    QMenu *languageMenu = settingsMenu->addMenu(tr("Language"));

    QAction *englishAction = languageMenu->addAction("English");
    QAction *chineseAction = languageMenu->addAction("中文");
    QAction *spanishAction = languageMenu->addAction("Español");
    QAction *italianAction = languageMenu->addAction("Italiano");
    QAction *japaneseAction = languageMenu->addAction("日本語");
    QAction *portugueseAction = languageMenu->addAction("Português");

    connect(englishAction, &QAction::triggered, this, [this]() { setLanguage("en"); });
    connect(chineseAction, &QAction::triggered, this, [this]() { setLanguage("zh"); });
    connect(spanishAction, &QAction::triggered, this, [this]() { setLanguage("es"); });
    connect(italianAction, &QAction::triggered, this, [this]() { setLanguage("it"); });
    connect(japaneseAction, &QAction::triggered, this, [this]() { setLanguage("ja"); });
    connect(portugueseAction, &QAction::triggered, this, [this]() { setLanguage("pt"); });

    menuBar->addMenu(settingsMenu);

    QMenu *helpMenu = new QMenu(tr("Help"), this);
    helpMenu->addAction(tr("About"), this, SLOT(showAboutDialog()));
    menuBar->addMenu(helpMenu);

    mainLayout->setMenuBar(menuBar);

    // Length input
    QLabel *lengthLabel = new QLabel(tr("Enter your password length (4 to 256):"), this);
    mainLayout->addWidget(lengthLabel);

    lengthInput = new QLineEdit(this);
    lengthInput->setText("12");
    mainLayout->addWidget(lengthInput);

    // User-defined character set input
    QLabel *customCharSetLabel = new QLabel(tr("Custom character set:"), this);
    customCharSetInput = new QLineEdit(this);
    mainLayout->addWidget(customCharSetLabel);
    mainLayout->addWidget(customCharSetInput);

    // Options checkboxes
    upperCaseOnly = new QCheckBox(tr("Upper Case Only"), this);
    lowerCaseOnly = new QCheckBox(tr("Lower Case Only"), this);
    comboCase = new QCheckBox(tr("Combination of Upper and Lower Case"), this);
    useSpecialChars = new QCheckBox(tr("Use Special Characters"), this);
    useNumbers = new QCheckBox(tr("Use Numbers"), this);

    requireUpper = new QCheckBox(tr("Require at least one uppercase letter"), this);
    requireLower = new QCheckBox(tr("Require at least one lowercase letter"), this);
    requireNumber = new QCheckBox(tr("Require at least one number"), this);
    requireSpecial = new QCheckBox(tr("Require at least one special character"), this);
    excludeSimilarChars = new QCheckBox(tr("Exclude similar characters (0, O, 1, l)"), this);

    mainLayout->addWidget(upperCaseOnly);
    mainLayout->addWidget(lowerCaseOnly);
    mainLayout->addWidget(comboCase);
    mainLayout->addWidget(useSpecialChars);
    mainLayout->addWidget(useNumbers);
    mainLayout->addWidget(requireUpper);
    mainLayout->addWidget(requireLower);
    mainLayout->addWidget(requireNumber);
    mainLayout->addWidget(requireSpecial);
    mainLayout->addWidget(excludeSimilarChars);

    // Complexity slider
    QLabel *complexityLabel = new QLabel(tr("Password Complexity:"), this);
    complexitySlider = new QSlider(Qt::Horizontal, this);
    complexitySlider->setRange(1, 5); // 1 = weak, 5 = very strong
    complexitySlider->setTickPosition(QSlider::TicksBelow);
    complexitySlider->setTickInterval(1);
    mainLayout->addWidget(complexityLabel);
    mainLayout->addWidget(complexitySlider);

    // Password strength indicator
    QLabel *strengthTextLabel = new QLabel(tr("Password Strength:"), this);
    mainLayout->addWidget(strengthTextLabel);
    strengthBar = new QProgressBar(this);
    strengthBar->setRange(0, 100);
    mainLayout->addWidget(strengthBar);
    strengthLabel = new QLabel(this);
    mainLayout->addWidget(strengthLabel);

    // Buttons
    QPushButton *generateButton = new QPushButton(tr("Generate Password"), this);
    QPushButton *clearButton = new QPushButton(tr("Clear"), this);
    QPushButton *copyButton = new QPushButton(tr("Copy to Clipboard"), this);

    // Horizontal layout for buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(generateButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(copyButton);
    mainLayout->addLayout(buttonLayout);

    // Output field for generated password
    passwordOutput = new QLineEdit(this);
    passwordOutput->setReadOnly(true);
    mainLayout->addWidget(passwordOutput);

    // Connect buttons to their respective slots
    connect(generateButton, &QPushButton::clicked, this, &PasswordGenerator::generatePassword);
    connect(clearButton, &QPushButton::clicked, this, &PasswordGenerator::clearOutput);
    connect(copyButton, &QPushButton::clicked, this, &PasswordGenerator::copyToClipboard);

    setLayout(mainLayout);
    setWindowTitle(tr("Password Generator"));
    resize(400, 300);
}

// Slot for generating a password with balanced distribution of character types
void PasswordGenerator::generatePassword() {
    unsigned int length;
    if (!validateInput(length)) {
        return;
    }

    QString upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    QString lower = "abcdefghijklmnopqrstuvwxyz";
    QString numbers = "0123456789";
    QString special = "!@#$%^&*()_+-=[]{}|;':,./<>?";
    QString chars = customCharSetInput->text(); // User-defined character set

    if (chars.isEmpty()) {
        if (upperCaseOnly->isChecked()) chars = upper;
        if (lowerCaseOnly->isChecked()) chars = lower;
        if (comboCase->isChecked()) chars = upper + lower;
        if (useSpecialChars->isChecked()) chars += special;
        if (useNumbers->isChecked()) chars += numbers;
    }

    // Remove similar-looking characters if the option is selected
    if (excludeSimilarChars->isChecked()) {
        chars.remove(QRegExp("[O0l1]"));
    }

    if (chars.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select at least one character set!"));
        return;
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    QString password;

    for (unsigned int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        password.append(chars[index]);
    }

    passwordOutput->setText(password);

    // Update the password strength indicator
    updateStrengthIndicator();
}

// Function to update the password strength indicator
void PasswordGenerator::updateStrengthIndicator() {
    QString password = passwordOutput->text();
    int strength = calculatePasswordStrength(password);

    strengthBar->setValue(strength);

    if (strength < 40) {
        strengthLabel->setText(tr("Weak"));
    } else if (strength < 70) {
        strengthLabel->setText(tr("Moderate"));
    } else {
        strengthLabel->setText(tr("Strong"));
    }
}

// Function to calculate password strength
int PasswordGenerator::calculatePasswordStrength(const QString &password) {
    int score = 0;
    if (password.length() >= 8) score += 20;
    if (password.contains(QRegExp("[A-Z]"))) score += 20;
    if (password.contains(QRegExp("[a-z]"))) score += 20;
    if (password.contains(QRegExp("[0-9]"))) score += 20;
    if (password.contains(QRegExp("[!@#$%^&*(),.?\":{}|<>]"))) score += 20;

    return score;
}

// Function to validate the user input for password length
bool PasswordGenerator::validateInput(unsigned int &length) {
    bool ok;
    int tempLength = lengthInput->text().toInt(&ok);
    if (!ok || tempLength < 4 || tempLength > 256) {
        QMessageBox::warning(this, tr("Input Error"), tr("Please enter a valid length between 4 and 256."));
        return false;
    }
    length = static_cast<unsigned int>(tempLength);
    return true;
}

// Slot for clearing the password output
void PasswordGenerator::clearOutput() {
    passwordOutput->clear();
}

// Function to copy the password to the clipboard
void PasswordGenerator::copyToClipboard() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(passwordOutput->text());
    QMessageBox::information(this, tr("Copied"), tr("Password copied to clipboard!"));
}

// Slot for showing the About dialog
void PasswordGenerator::showAboutDialog() {
    QMessageBox::about(this, tr("About Password Generator"),
        tr("Password Generator - Version 1.5\n"
           "Author: Dr. Eric O. Flores\n"
           "(c) Copyrights 2024 Dr. Eric O. Flores\n"
           "\n"
           "This software uses C++ with Qt for the GUI.\n"
           "Language Packages Enhancements coded by ChatGPT.\n"
           "Licensed under GPL 3.\n"
           "\n"
           "This software is free to use and distribute."));
}

// Slot for quitting the application
void PasswordGenerator::quitApp() {
    QApplication::quit();
}

// Function to toggle between dark and light mode
void PasswordGenerator::toggleDarkMode() {
    static bool darkMode = false;
    darkMode = !darkMode;

    if (darkMode) {
        qApp->setStyleSheet(
            "QWidget { background-color: #2d2d2d; color: white; }"
            "QLineEdit, QPushButton, QComboBox, QSlider, QProgressBar { "
            "border: 1px solid lightgreen; }"
            "QCheckBox { color: lightgreen; }"
            "QCheckBox::indicator { background-color: #90EE90; border: 1px solid lightgreen; }"
            "QCheckBox::indicator:checked { background-color: #32CD32; }"
            
            // QSlider handle styling for better visibility
            "QSlider::handle { "
            "background-color: lightgreen; "   // Light green slider handle
            "border: 1px solid lightgreen; "
            "width: 15px; "                    // Adjust the width of the handle
            "height: 15px; "                   // Adjust the height of the handle
            "border-radius: 7px; "             // Rounded handle for smooth look
            "} "
        );
    } else {
        qApp->setStyleSheet("");
    }
}

// Function to change the application's language
void PasswordGenerator::setLanguage(const QString &languageCode) {
    if (currentLanguage != languageCode) {
        currentLanguage = languageCode;

        if (translator.load("password_generator_" + languageCode, ".")) {
            qApp->installTranslator(&translator);
            this->setWindowTitle(tr("Password Generator"));
            update();  // Update the UI text to the new language
        } else {
            qDebug() << "Translation file not loaded for language:" << languageCode;
        }
    }
}

// Main entry point for the application
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    PasswordGenerator window;
    window.show();
    return app.exec();
}

#include "password_generator.moc"

