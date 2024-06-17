#include "setuptab.h"
#include "ui_setuptab.h"

setupTab::setupTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setupTab)
{
    ui->setupUi(this);

    int index = -1;
    QString langhelper;
    supressMessage = true; // verhindert, dass bei der Initialisierung der Sprachauswahl die Warnmeldung kommt.
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    if (setupFile->keywordExists("Setup","language") == true) {
        langhelper = setupFile->getSectionValue("Setup","language");
        index = ui->languageSelectComboBox->findText(langhelper,Qt::MatchStartsWith);
        if (index == -1) index = ui->languageSelectComboBox->findText("[en_GB]",Qt::MatchStartsWith);
        ui->languageSelectComboBox->setCurrentIndex(index);
    } else {
        QString lang = QLocale::system().name();
        index = ui->languageSelectComboBox->findText("["+lang+"]",Qt::MatchStartsWith);
        if (index == -1) index = ui->languageSelectComboBox->findText("[en_GB]",Qt::MatchStartsWith);
        ui->languageSelectComboBox->setCurrentIndex(index);
    }

    if (setupFile->keywordExists("Setup","WindowState") == true) {
        if (setupFile->getSectionValue("Setup","WindowState") == "minimized") ui->windowStateComboBox->setCurrentIndex(1); else ui->windowStateComboBox->setCurrentIndex(0);
    }

    supressMessage = false;
}

setupTab::~setupTab()
{
    delete ui;
}

void setupTab::slot_selectedLanguageChanged()
{
    setupFile->setSectionValue("Setup","language",ui->languageSelectComboBox->currentText().mid(0,7));
    if (supressMessage == false) QMessageBox::information(this,tr("Warning"),tr("You have to restart the application for changes to take effect!"));
}

void setupTab::slot_basicSettingsChanged()
{
    if (ui->windowStateComboBox->currentIndex() == 0) setupFile->setSectionValue("Setup","WindowState","maximized");
    if (ui->windowStateComboBox->currentIndex() == 1) setupFile->setSectionValue("Setup","WindowState","minimized");
}

