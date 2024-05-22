#include "setuptab.h"
#include "ui_setuptab.h"

setupTab::setupTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setupTab)
{
    ui->setupUi(this);

    int index = -1;
    supressMessage = true;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    getDBUserProcess = new QProcess(this);
    connect(getDBUserProcess,SIGNAL(finished(int)),this,SLOT(slot_getDBUserProcessFinished()));

    freshclamLocationProcess = new QProcess(this);
    connect(freshclamLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_freshclamLocationProcessFinished()));
    connect(freshclamLocationProcess,SIGNAL(readyRead()),this,SLOT(slot_freshclamLocationProcessHasOutput()));

    initFreshclamSettings();

    if (setupFile->keywordExists("Setup","language") == true) {
        index = setupFile->getSectionIntValue("Setup","language");
        ui->languageSelectComboBox->setCurrentIndex(index);
    } else {
        QString lang = QLocale::system().name();
        if (lang == "pt_BR") index = 0;
        if (lang == "da_DK") index = 1;
        if (lang == "en_GB") index = 2;
        if (lang == "fr_FR") index = 3;
        if (lang == "de_DE") index = 4;
        if (lang == "es_ES") index = 5;
        if (lang == "pt_PT") index = 6;
        if (lang == "it_IT") index = 7;
        if (lang == "uk_UA") index = 8;
        ui->languageSelectComboBox->setCurrentIndex(index);
    }

    if (setupFile->keywordExists("FreshClam","runasroot") == true) ui->runasrootCheckBox->setChecked(setupFile->getSectionBoolValue("FreshClam","runasroot")); else {
        setupFile->setSectionValue("FreshClam","runasroot","true");
        ui->runasrootCheckBox->setChecked(true);
    }
    if (setupFile->keywordExists("Setup","WindowState") == true) {
        if (setupFile->getSectionValue("Setup","WindowState") == "minimized") ui->windowStateComboBox->setCurrentIndex(1); else ui->windowStateComboBox->setCurrentIndex(0);
    }
    if (setupFile->keywordExists("Freshclam","StartDaemon") == true) ui->autoStartDaemonCheckBox->setChecked(setupFile->getSectionBoolValue("Freshclam","StartDaemon")); else {
        setupFile->setSectionValue("Freshclam","StartDaemon",false);
        ui->autoStartDaemonCheckBox->setChecked(false);
    }
    supressMessage = false;
}

setupTab::~setupTab()
{
    delete ui;
}

void setupTab::initFreshclamSettings() {

    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf");
    if (freshclamConf->singleLineExists("DatabaseDirectory") == true) {
        ui->databaseDirectoryPathLabel->setText(freshclamConf->getSingleLineValue("DatabaseDirectory"));
    } else {
        QDir tempdir;
        if (tempdir.exists("/var/lib/clamav") == true) {
            freshclamConf->setSingleLineValue("DatabaseDirectory","/var/lib/clamav");
        } else {
            freshclamConf->setSingleLineValue("DatabaseDirectory",QDir::homePath() + "/.clamav-gui/signatures");
        }
        ui->databaseDirectoryPathLabel->setText(freshclamConf->getSingleLineValue("DatabaseDirectory"));
    }

    if (freshclamConf->singleLineExists("LogSyslog") == true) ui->logSysLogComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogSyslog")); else {
        freshclamConf->setSingleLineValue("LogSyslog","no");
        ui->logSysLogComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogSyslog"));
    }
    if (freshclamConf->singleLineExists("LogFacility") == true) ui->logFacilityComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogFacility")); else {
        freshclamConf->setSingleLineValue("LogFacility","LOG_LOCAL6");
        ui->logFacilityComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogFacility"));
    }
    if (freshclamConf->singleLineExists("LogTime") == true) ui->logTimeComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogTime")); else {
        freshclamConf->setSingleLineValue("LogTime","no");
        ui->logTimeComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogTime"));
    }
    if (freshclamConf->singleLineExists("LogRotate") == true) ui->logRotateComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogRotate")); else {
        freshclamConf->setSingleLineValue("LogRotate","no");
        ui->logRotateComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogRotate"));
    }
    if (freshclamConf->singleLineExists("PidFile") == true) ui->pidFilePathLabel->setText(freshclamConf->getSingleLineValue("PidFile")); else {
        freshclamConf->setSingleLineValue("PidFile","/tmp/freshclam.pid");
        ui->pidFilePathLabel->setText(freshclamConf->getSingleLineValue("PidFile"));
    }
    if (freshclamConf->singleLineExists("DatabaseOwner") == true) ui->databaseOwnerLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseOwner")); else {
        freshclamConf->setSingleLineValue("DatabaseOwner","clamav");
        ui->databaseOwnerLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseOwner"));
    }
    if (freshclamConf->singleLineExists("DatabaseMirror") == true) ui->databaseMirrorLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseMirror")); else {
        freshclamConf->setSingleLineValue("DatabaseMirror","database.clamav.net");
        ui->databaseMirrorLineEdit->setText(freshclamConf->getSingleLineValue("DatabaseMirror"));
    }
    if (freshclamConf->singleLineExists("LogVerbose") == true) ui->logVerboseComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogVerbose")); else {
        freshclamConf->setSingleLineValue("LogVerbose","no");
        ui->logVerboseComboBox->setCurrentText(freshclamConf->getSingleLineValue("LogVerbose"));
    }
    if (freshclamConf->singleLineExists("Checks") == true) ui->checkPerDaySpinBox->setValue(freshclamConf->getSingleLineValue("Checks").toInt()); else {
        freshclamConf->setSingleLineValue("Checks","12");
        ui->checkPerDaySpinBox->setValue(freshclamConf->getSingleLineValue("Checks").toInt());
    }
    ui->freshclamLocationLineEdit->setText(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"));
    ui->databaseTypeComboBox->setCurrentIndex(setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate"));

    QStringList parameters;
    parameters << "-ld" << ui->databaseDirectoryPathLabel->text();
    QDir dbDir;
    if (dbDir.exists(ui->databaseDirectoryPathLabel->text()) == true) {
        getDBUserProcess->start("ls",parameters);
    } else {
        freshclamConf->setSingleLineValue("DatabaseOwner","clamav");
    }

    freshclamlocationProcessOutput = "";
    parameters.clear();
    parameters << "freshclam";
    freshclamLocationProcess->start("whereis",parameters);
}

void setupTab::slot_selectedLanguageChanged()
{
    setupFile->setSectionValue("Setup","language",ui->languageSelectComboBox->currentIndex());
    if (supressMessage == false) QMessageBox::information(this,tr("Warning"),tr("You have to restart the application for changes to take effect!"));
}

void setupTab::slot_runasrootCheckBoxChanged()
{
    setupFile->setSectionValue("FreshClam","runasroot",ui->runasrootCheckBox->isChecked());
}

void setupTab::slot_basicSettingsChanged()
{
    if (ui->windowStateComboBox->currentIndex() == 0) setupFile->setSectionValue("Setup","WindowState","maximized");
    if (ui->windowStateComboBox->currentIndex() == 1) setupFile->setSectionValue("Setup","WindowState","minimized");
}

void setupTab::slot_writeFreshclamSettings()
{
    freshclamConf->setSingleLineValue("DatabaseDirectory",ui->databaseDirectoryPathLabel->text());
    freshclamConf->setSingleLineValue("LogSyslog",ui->logSysLogComboBox->currentText());
    freshclamConf->setSingleLineValue("LogFacility",ui->logFacilityComboBox->currentText());
    freshclamConf->setSingleLineValue("LogRotate",ui->logRotateComboBox->currentText());
    freshclamConf->setSingleLineValue("PidFile",ui->pidFilePathLabel->text());
    freshclamConf->setSingleLineValue("DatabaseOwner",ui->databaseOwnerLineEdit->text());
    freshclamConf->setSingleLineValue("DatabaseMirror",ui->databaseMirrorLineEdit->text());
    freshclamConf->setSingleLineValue("LogTime",ui->logTimeComboBox->currentText());
    freshclamConf->setSingleLineValue("LogVerbose",ui->logVerboseComboBox->currentText());
    setupFile->setSectionValue("FreshClam","UpdatesPerDay",ui->checkPerDaySpinBox->value());
    setupFile->setSectionValue("FreshClam","DataBaseToUpdate",ui->databaseTypeComboBox->currentIndex());
}

void setupTab::slot_dbPathChanged(QString dbPath)
{
    if (dbPath != "") {
        freshclamConf->setSingleLineValue("DatabaseDirectory",dbPath);
        ui->databaseDirectoryPathLabel->setText(dbPath);

        QStringList parameters;
        parameters << "-ld" << ui->databaseDirectoryPathLabel->text();
        QDir dbDir;
        if (dbDir.exists(ui->databaseDirectoryPathLabel->text()) == true) {
            getDBUserProcess->start("ls",parameters);
        } else {
            freshclamConf->setSingleLineValue("DatabaseOwner","clamav");
        }

    }
}

void setupTab::slot_getDBUserProcessFinished()
{
    QString output = getDBUserProcess->readAll();
    QStringList values = output.split(" ");
    if (values.size() > 1) {
        freshclamConf->setSingleLineValue("DatabaseOwner",values[2]);
        ui->databaseOwnerLineEdit->setText(values[2]);
    }
}

void setupTab::slot_pidFileSelectButtonClicked()
{
    QString rc = QFileDialog::getExistingDirectory(this,tr("Select Folder for the PID-File"),"/tmp");
    if (rc != "") {
        freshclamConf->setSingleLineValue("PidFile",rc + "/freshclam.pid");
        setupFile->setSectionValue("Freshclam","PidFile",rc + "/freshclam.pid");
        ui->pidFilePathLabel->setText(rc + "/freshclam.pid");
    }
}

void setupTab::slot_freshclamLocationProcessFinished()
{
    if (freshclamlocationProcessOutput != "freshclam:\n") {
        QStringList values = freshclamlocationProcessOutput.split(" ");
        if (values.size() > 1) {
            if (values.length() > 0) setupFile->setSectionValue("FreshclamSettings","FreshclamLocation",values[1]); else setupFile->setSectionValue("FreshclamSettings","FreshclamLocation","not found");
            ui->freshclamLocationLineEdit->setText(values[1]);
            QFile file(ui->databaseDirectoryPathLabel->text() + "/freshclam.dat");
            if (file.exists() == false) {
               if (QMessageBox::warning(this,tr("Virus definitions missing!"),tr("No virus definitions found in the database folder. Should the virus definitions be downloaded?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
                   emit updateDatabase();
               }
            }
        }
    } else {
        setupFile->setSectionValue("FreshclamSettings","FreshclamLocation","not found");
        ui->freshclamLocationLineEdit->setText("not found");
        QMessageBox::warning(this,"WARNING","Freshclam is missing. Please install!",QMessageBox::Ok);
        emit disableUpdateButtons();
        emit reportError();
    }
}

void setupTab::slot_freshclamLocationProcessHasOutput()
{
    freshclamlocationProcessOutput = freshclamlocationProcessOutput + freshclamLocationProcess->readAll();
}

void setupTab::slot_setFreshclamsettingsFrameState(bool state)
{
    ui->freshclamLocationLineEdit->setEnabled(state);
    ui->pidFileSelectPushButton->setEnabled(state);
    ui->pidFilePathLabel->setEnabled(state);
    ui->databaseOwnerLineEdit->setEnabled(state);
    ui->databaseMirrorLineEdit->setEnabled(state);
    ui->logSysLogComboBox->setEnabled(state);
    ui->logFacilityComboBox->setEnabled(state);
    ui->logRotateComboBox->setEnabled(state);
    ui->logTimeComboBox->setEnabled(state);
    ui->logVerboseComboBox->setEnabled(state);
    ui->databaseDirectoryPathLabel->setEnabled(state);
    ui->databaseTypeComboBox->setEnabled(state);
    ui->checkPerDaySpinBox->setEnabled(state);
    ui->runasrootCheckBox->setEnabled(state);
    ui->autoStartDaemonCheckBox->setEnabled(state);
}

void setupTab::slot_autoStartDaemon()
{
    setupFile->setSectionValue("Freshclam","StartDaemon",ui->autoStartDaemonCheckBox->isChecked());
}




