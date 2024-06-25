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

    slot_updateSystemInfo();
    supressMessage = false;
}

setupTab::~setupTab()
{
    delete ui;
}

void setupTab::slot_updateSystemInfo()
{
    if (setupFile->keywordExists("Clamd","ClamdLocation") == true) ui->clamdPath->setText(setupFile->getSectionValue("Clamd","ClamdLocation"));
    if (setupFile->keywordExists("Clamd","ClamonaccLocation") == true) ui->clamonaccPath->setText(setupFile->getSectionValue("Clamd","ClamonaccLocation"));
    if (setupFile->keywordExists("FreshclamSettings","FreshclamLocation") == true) ui->freshclamPath->setText(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"));
    if (setupFile->sectionExists("Updater") == true) {
        ui->databasePath->setText(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").mid(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("|")+1));
        ui->databaseLastUpdate->setText(setupFile->getSectionValue("Updater","LastUpdate"));
        ui->databaseMainFile->setText(setupFile->getSectionValue("Updater","MainVersion"));
        ui->databaseDailyFile->setText(setupFile->getSectionValue("Updater","DailyVersion"));
        ui->databaseBytecodeFile->setText(setupFile->getSectionValue("Updater","BytecodeVersion"));
    }
    if (setupFile->keywordExists("Clamd","ClamonaccPid") == true) {
        ui->clamonaccPID->setText(setupFile->getSectionValue("Clamd","ClamonaccPid"));
        if (setupFile->getSectionValue("Clamd","ClamonaccPid") == "n/a") {
            ui->clamonaccStatus->setText("Clamonacc is down");
            ui->clamonaccStatus->setStyleSheet("background-color:red;color:white");
        } else {
            ui->clamonaccStatus->setText("Clamonacc is up");
            ui->clamonaccStatus->setStyleSheet("background-color:green;color:yellow");
        }
    }
    if (setupFile->keywordExists("Clamd","ClamdPid") == true) {
        ui->clamdPID->setText(setupFile->getSectionValue("Clamd","ClamdPid"));
        if (setupFile->getSectionValue("Clamd","ClamdPid") == "n/a") {
            ui->clamdStatus->setText("Clamd is down");
            ui->clamdStatus->setStyleSheet("background-color:red;color:white");
        } else {
            ui->clamdStatus->setText("Clamd is up");
            ui->clamdStatus->setStyleSheet("background-color:green;color:yellow");
        }
    }
    if (setupFile->keywordExists("Freshclam","Pid") == true) {
        ui->freshclamPID->setText(setupFile->getSectionValue("Freshclam","Pid"));
        if (setupFile->getSectionValue("Freshclam","Pid") == "n/a") {
            ui->freshclamStatus->setText("Freshclam is down");
            ui->freshclamStatus->setStyleSheet("background-color:red;color:white");
        } else {
            ui->freshclamStatus->setText("Freshclam is up");
            ui->freshclamStatus->setStyleSheet("background-color:green;color:yellow");
        }
    }
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

