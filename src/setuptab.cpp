#include "setuptab.h"
#include "ui_setuptab.h"

setupTab::setupTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setupTab)
{
    ui->setupUi(this);

    pidWatcher = new QFileSystemWatcher(this);
    connect(pidWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_pidWatcherTriggered()));

    if (checkClamdRunning() == true) {
        ui->startStopClamdPushButton->setStyleSheet("background-color:green");
        ui->startStopClamdPushButton->setText("Clamd running - Stop clamd");
        pidWatcher->addPath("/tmp/clamd.pid");
    } else {
        ui->startStopClamdPushButton->setStyleSheet("background-color:red");
    }
    int index = -1;
    QString langhelper;
    supressMessage = true;

    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    getDBUserProcess = new QProcess(this);
    connect(getDBUserProcess,SIGNAL(finished(int)),this,SLOT(slot_getDBUserProcessFinished()));

    freshclamLocationProcess = new QProcess(this);
    connect(freshclamLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_freshclamLocationProcessFinished()));
    connect(freshclamLocationProcess,SIGNAL(readyRead()),this,SLOT(slot_freshclamLocationProcessHasOutput()));

    clamdLocationProcess = new QProcess(this);
    connect(clamdLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_clamdLocationProcessFinished()));

    clamonaccLocationProcess = new QProcess(this);
    connect(clamonaccLocationProcess,SIGNAL(finished(int)),this,SLOT(slot_clamonaccLocationProcessFinished()));

    startClamdProcess = new QProcess(this);
    connect(startClamdProcess,SIGNAL(finished(int)),this,SLOT(slot_startClamdProcessFinished()));

    findclamonaccProcess = new QProcess(this);
    connect(findclamonaccProcess,SIGNAL(finished(int)),this,SLOT(slot_findclamonaccProcessFinished()));

    restartClamonaccProcess = new QProcess(this);
    connect(restartClamonaccProcess,SIGNAL(finished(int)),this,SLOT(slot_restartClamonaccProcessFinished()));

    initFreshclamSettings();

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

    logHighlighter = new highlighter(ui->clamdLogPlainTextEdit->document());

    supressMessage = false;

    QStringList monitorings = setupFile->getKeywords("Clamonacc");
    ui->monitoringComboBox->addItems(monitorings);
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

    slot_updateClamdConf();
    freshclamlocationProcessOutput = "";
    parameters.clear();
    parameters << "freshclam";
    freshclamLocationProcess->start("whereis",parameters);

    parameters.clear();
    parameters << "clamd";
    clamdLocationProcess->start("whereis",parameters);

    parameters.clear();
    parameters << "clamonacc";
    clamonaccLocationProcess->start("whereis",parameters);

}

void setupTab::slot_updateClamdConf()
{
    QStringList path;
    QString logPath = QDir::homePath() + "/.clamav-gui/clamd.log";
    QFile checkFile(logPath);
    if (checkFile.exists() == false) {
        if (checkFile.open(QIODevice::WriteOnly|QIODevice::Text)){
            checkFile.close();
        }
    } else {
        slot_logFileContentChanged();
    }

    path << logPath;
    logWatcher = new QFileSystemWatcher(path,this);
    connect(logWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_logFileContentChanged()));
    setupFileHandler * helperHandler = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");

    clamdConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/clamd.conf");
    clamdConf->clearSetupFile();
    clamdConf->setSingleLineValue("DatabaseDirectory", freshclamConf->getSingleLineValue("DatabaseDirectory"));
    clamdConf->setSingleLineValue("LogSyslog", freshclamConf->getSingleLineValue("LogSyslog"));
    clamdConf->setSingleLineValue("LogFacility", freshclamConf->getSingleLineValue("LogFacility"));
    clamdConf->setSingleLineValue("PidFile", "/tmp/clamd.pid");
    clamdConf->setSingleLineValue("ExtendedDetectionInfo","yes");
    clamdConf->setSingleLineValue("LocalSocket", QDir::homePath() + "/.clamav-gui/clamd-socket");
    clamdConf->addSingleLineValue("LogFile",QDir::homePath() + "/.clamav-gui/clamd.log");
    clamdConf->setSingleLineValue("LocalSocketGroup", "users");
    clamdConf->setSingleLineValue("TCPAddr", "127.0.0.1");
    clamdConf->addSingleLineValue("TCPAddr", "::1");
    clamdConf->addSingleLineValue("LogFileMaxSize","2M");
    clamdConf->addSingleLineValue("LogTime","yes");
    clamdConf->addSingleLineValue("LogRotate","yes");
    clamdConf->addSingleLineValue("OnAccessMaxFileSize","10M");
    clamdConf->addSingleLineValue("OnAccessMaxThreads","10");
    clamdConf->addSingleLineValue("OnAccessPrevention","yes");
    clamdConf->addSingleLineValue("OnAccessDenyOnError","yes");
    clamdConf->addSingleLineValue("OnAccessExtraScanning","yes");
    clamdConf->addSingleLineValue("OnAccessExcludeUname","root");
    clamdConf->addSingleLineValue("OnAccessExcludeUID","0");
    QStringList watchList = setupFile->getKeywords("Clamonacc");
    foreach (QString entry, watchList) {
        clamdConf->addSingleLineValue("OnAccessIncludePath",entry);
    }
    if (helperHandler->keywordExists("SelectedOptions","") == true) clamdConf->setSingleLineValue("", "yes");
    if (helperHandler->keywordExists("SelectedOptions","") == true) clamdConf->setSingleLineValue("", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-broken-media<equal>yes") == true) clamdConf->setSingleLineValue("AlertBrokenMedia", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-broken-media<equal>no") == true) clamdConf->setSingleLineValue("AlertBrokenMedia", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-broken<equal>yes") == true) clamdConf->setSingleLineValue("AlertBrokenExecutables", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-broken<equal>no") == true) clamdConf->setSingleLineValue("AlertBrokenExecutables", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted-archive<equal>yes") == true) clamdConf->setSingleLineValue("AlertEncryptedArchive", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted-archive<equal>no") == true) clamdConf->setSingleLineValue("AlertEncryptedArchive", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted-doc<equal>yes") == true) clamdConf->setSingleLineValue("AlertEncryptedDoc", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted-doc<equal>no") == true) clamdConf->setSingleLineValue("AlertEncryptedDoc", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted<equal>yes") == true) clamdConf->setSingleLineValue("AlertEncrypted", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-encrypted<equal>no") == true) clamdConf->setSingleLineValue("AlertEncrypted", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-exceeds-max<equal>yes") == true) clamdConf->setSingleLineValue("AlertExceedsMax", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-exceeds-max<equal>no") == true) clamdConf->setSingleLineValue("AlertExceedsMax", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-macros<equal>yes") == true) clamdConf->setSingleLineValue("AlertOLE2Macros", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-macros<equal>no") == true) clamdConf->setSingleLineValue("AlertOLE2Macros", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-partition-intersection<equal>yes") == true) clamdConf->setSingleLineValue("AlertPartitionIntersection", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-partition-intersection<equal>no") == true) clamdConf->setSingleLineValue("AlertPartitionIntersection", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-phishing-cloak<equal>yes") == true) clamdConf->setSingleLineValue("AlertPhishingCloak", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-phishing-cloak<equal>no") == true) clamdConf->setSingleLineValue("AlertPhishingCloak", "no");
    if (helperHandler->keywordExists("SelectedOptions","--alert-phishing-ssl<equal>yes") == true) clamdConf->setSingleLineValue("AlertPhishingSSLMismatch", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--alert-phishing-ssl<equal>no") == true) clamdConf->setSingleLineValue("AlertPhishingSSLMismatch", "no");
    if (helperHandler->keywordExists("SelectedOptions","--allmatch<equal>yes") == true) clamdConf->setSingleLineValue("AllowAllMatchScan", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--allmatch<equal>no") == true) clamdConf->setSingleLineValue("AllowAllMatchScan", "no");
    if (helperHandler->keywordExists("SelectedOptions","--bytecode-unsigned<equal>yes") == true) clamdConf->setSingleLineValue("BytecodeUnsigned", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--bytecode-unsigned<equal>no") == true) clamdConf->setSingleLineValue("BytecodeUnsigned", "no");
    if (helperHandler->keywordExists("SelectedOptions","--cross-fs<equal>yes") == true) clamdConf->setSingleLineValue("CrossFilesystems", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--cross-fs<equal>no") == true) clamdConf->setSingleLineValue("CrossFilesystems", "no");
    if (helperHandler->keywordExists("SelectedOptions","--debug") == true) clamdConf->setSingleLineValue("DEBUG", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--detect-pua<equal>yes") == true) clamdConf->setSingleLineValue("DetectPUA","yes");
    if (helperHandler->keywordExists("SelectedOptions","--detect-pua<equal>no") == true) clamdConf->setSingleLineValue("DetectPUA", "no");
    if (helperHandler->keywordExists("SelectedOptions","--detect-structured<equal>yes") == true) clamdConf->setSingleLineValue("StructuredDataProtection", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--detect-structured<equal>no") == true) clamdConf->setSingleLineValue("StructuredDataProtection", "no");
    if (helperHandler->keywordExists("SelectedOptions","--heuristic-alerts<equal>yes") == true) clamdConf->setSingleLineValue("HeuristicAlerts", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--heuristic-alerts<equal>no") == true) clamdConf->setSingleLineValue("HeuristicAlerts", "no");
    if (helperHandler->keywordExists("SelectedOptions","--heuristic-scan-precedence<equal>yes") == true) clamdConf->setSingleLineValue("HeuristicScanPrecedence", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--heuristic-scan-precedence<equal>no") == true) clamdConf->setSingleLineValue("HeuristicScanPrecedence", "no");
    if (helperHandler->keywordExists("SelectedOptions","--leave-temps<equal>yes") == true) clamdConf->setSingleLineValue("LeaveTemporaryFiles", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--leave-temps<equal>no") == true) clamdConf->setSingleLineValue("LeaveTemporaryFiles", "no");
    if (helperHandler->keywordExists("SelectedOptions","--nocerts") == true) clamdConf->setSingleLineValue("DisableCertCheck", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--official-db-only<equal>yes") == true) clamdConf->setSingleLineValue("OfficialDatabaseOnly", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--official-db-only<equal>no") == true) clamdConf->setSingleLineValue("OfficialDatabaseOnly", "no");
    if (helperHandler->keywordExists("SelectedOptions","--phishing-scan-urls<equal>yes") == true) clamdConf->setSingleLineValue("PhishingScanURLs", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--phishing-scan-urls<equal>no") == true) clamdConf->setSingleLineValue("PhishingScanURLs", "no");
    if (helperHandler->keywordExists("SelectedOptions","--phishing-sigs<equal>yes") == true) clamdConf->setSingleLineValue("PhishingSignatures", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--phishing-sigs<equal>no") == true) clamdConf->setSingleLineValue("PhishingSignatures", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-archive<equal>yes") == true) clamdConf->setSingleLineValue("ScanArchive", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-archive<equal>no") == true) clamdConf->setSingleLineValue("ScanArchive", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-elf<equal>yes") == true) clamdConf->setSingleLineValue("ScanELF", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-elf<equal>no") == true) clamdConf->setSingleLineValue("ScanELF", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-html<equal>no") == true) clamdConf->setSingleLineValue("ScanHTML", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-html<equal>no") == true) clamdConf->setSingleLineValue("ScanHTML", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-hwp3<equal>yes") == true) clamdConf->setSingleLineValue("ScanHWP3", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-hwp3<equal>no") == true) clamdConf->setSingleLineValue("ScanHWP3", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-mail<equal>yes") == true) clamdConf->setSingleLineValue("ScanMail", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-mail<equal>no") == true) clamdConf->setSingleLineValue("ScanMail", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-ole2<equal>yes") == true) clamdConf->setSingleLineValue("ScanOLE2", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-ole2<equal>no") == true) clamdConf->setSingleLineValue("ScanOLE2", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-pdf<equal>yes") == true) clamdConf->setSingleLineValue("--scan-ole2<equal>no", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-pdf<equal>no") == true) clamdConf->setSingleLineValue("--scan-ole2<equal>no", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-pe<equal>yes") == true) clamdConf->setSingleLineValue("--scan-pdf<equal>no", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-pe<equal>no") == true) clamdConf->setSingleLineValue("--scan-pdf<equal>no", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-swf<equal>yes") == true) clamdConf->setSingleLineValue("ScanSWF", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-swf<equal>no") == true) clamdConf->setSingleLineValue("ScanSWF", "no");
    if (helperHandler->keywordExists("SelectedOptions","--scan-xmldocs<equal>yes") == true) clamdConf->setSingleLineValue("ScanXMLDOCS", "yes");
    if (helperHandler->keywordExists("SelectedOptions","--scan-xmldocs<equal>no") == true) clamdConf->setSingleLineValue("ScanXMLDOCS", "no");
    if (helperHandler->keywordExists("SelectedOptions","--verbose") == true) clamdConf->setSingleLineValue("LogVerbose", "yes");

    if ((helperHandler->sectionExists("REGEXP_and_IncludeExclude")) && (helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").indexOf("not checked|") != 0 )) clamdConf->addSingleLineValue("ExcludePath",helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if ((helperHandler->sectionExists("REGEXP_and_IncludeExclude")) && (helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanFileNamesMatchingRegExp").indexOf("not checked|") != 0 )) clamdConf->addSingleLineValue("ExcludePath",helperHandler->getSectionValue("REGEXP_and_IncludeExclude","DontScanDiretoriesMatchingRegExp").mid(8));
    if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","EnablePUAOptions") == true) {
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAPacked") == true) clamdConf->addSingleLineValue("IncludePUA","Packed");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAPWTool") == true) clamdConf->addSingleLineValue("IncludePUA","PWTool");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUANetTool") == true) clamdConf->addSingleLineValue("IncludePUA","NetTool");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAP2P") == true) clamdConf->addSingleLineValue("IncludePUA","P2P");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAIRC") == true) clamdConf->addSingleLineValue("IncludePUA","IRC");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUARAT") == true) clamdConf->addSingleLineValue("IncludePUA","RAT");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUANetToolSpy") == true) clamdConf->addSingleLineValue("IncludePUA","NetToolSpy");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAServer") == true) clamdConf->addSingleLineValue("IncludePUA","Server");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAScript") == true) clamdConf->addSingleLineValue("IncludePUA","Script");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAAndr") == true) clamdConf->addSingleLineValue("IncludePUA","Andr");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAJava") == true) clamdConf->addSingleLineValue("IncludePUA","Java");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAOsx") == true) clamdConf->addSingleLineValue("IncludePUA","Osx");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUATool") == true) clamdConf->addSingleLineValue("IncludePUA","Tool");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAUnix") == true) clamdConf->addSingleLineValue("IncludePUA","Unix");
        if (helperHandler->getSectionBoolValue("REGEXP_and_IncludeExclude","LoadPUAWin") == true) clamdConf->addSingleLineValue("IncludePUA","Win");
    }
    if (helperHandler->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxFileSize",helperHandler->getSectionValue("ScanLimitations","Files larger than this will be skipped and assumed clean").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxScanSize",helperHandler->getSectionValue("ScanLimitations","The maximum amount of data to scan for each container file").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxFiles",helperHandler->getSectionValue("ScanLimitations","The maximum number of files to scan for each container file").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max Scan-Time").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxScanTime",helperHandler->getSectionValue("ScanLimitations","Max Scan-Time").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum directory recursion level").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxRecursion",helperHandler->getSectionValue("ScanLimitations","Maximum directory recursion level").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxEmbeddedPE",helperHandler->getSectionValue("ScanLimitations","Maximum size file to check for embedded PE").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxHTMLNormalize",helperHandler->getSectionValue("ScanLimitations","Maximum size of HTML file to normalize").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxHTMLNoTags",helperHandler->getSectionValue("ScanLimitations","Maximum size of normalized HTML file to scan").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size of script file to normalize").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxScriptNormalize",helperHandler->getSectionValue("ScanLimitations","Maximum size of script file to normalize").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxZipTypeRcg",helperHandler->getSectionValue("ScanLimitations","Maximum size zip to type reanalyze").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxPartitions",helperHandler->getSectionValue("ScanLimitations","Maximum number of partitions in disk image to be scanned").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxIconsPE",helperHandler->getSectionValue("ScanLimitations","Maximum number of icons in PE file to be scanned").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("MaxRecHWP3",helperHandler->getSectionValue("ScanLimitations","Max recursion to HWP3 parsing function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max calls to PCRE match function").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("PCREMatchLimit",helperHandler->getSectionValue("ScanLimitations","Max calls to PCRE match function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("PCRERecMatchLimit",helperHandler->getSectionValue("ScanLimitations","Max recursion calls to the PCRE match function").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Max PCRE file size").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("PCREMaxFileSize",helperHandler->getSectionValue("ScanLimitations","Max PCRE file size").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Structured CC Count").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("StructuredMinCreditCardCount",helperHandler->getSectionValue("ScanLimitations","Structured CC Count").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Structured CC Mode").indexOf("checked|1") == 0 ) clamdConf->addSingleLineValue("StructuredCCOnly","yes");
    if (helperHandler->getSectionValue("ScanLimitations","Structured SSN Count").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("StructuredMinSSNCount",helperHandler->getSectionValue("ScanLimitations","Structured SSN Count").mid(8));
    if (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|0") == 0 ) clamdConf->addSingleLineValue("StructuredSSNFormatNormal","yes");
    if (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|1") == 0 ) clamdConf->addSingleLineValue("StructuredSSNFormatStripped","yes");
    if (helperHandler->getSectionValue("ScanLimitations","Structured SSN Format").indexOf("checked|2") == 0 ) {
        clamdConf->addSingleLineValue("StructuredSSNFormatNormal","yes");
        clamdConf->addSingleLineValue("StructuredSSNFormatStripped","yes");
    }
    if (helperHandler->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").indexOf("not checked|") != 0 ) clamdConf->addSingleLineValue("BytecodeTimeout",helperHandler->getSectionValue("ScanLimitations","Bytecode timeout in milliseconds").mid(8));
}

void setupTab::slot_logFileContentChanged()
{
    QFile file(QDir::homePath() + "/.clamav-gui/clamd.log");
    QString content;
    QString checkString;

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();

    QStringList lines =  content.split("\n");
    foreach (QString line, lines) {
        if (line.indexOf("FOUND") > -1) checkString = line;
    }

    if (checkString != lastFound) {
        lastFound = checkString;
        emit setBallonMessage(2,tr("WARNING"),lastFound);
    }

    ui->clamdLogPlainTextEdit->clear();
    ui->clamdLogPlainTextEdit->insertPlainText(content);
    ui->clamdLogPlainTextEdit->ensureCursorVisible();
}

void setupTab::slot_clamdStartStopButtonClicked()
{
    QStringList parameters;
    QString para;
    QFile pidFile("/tmp/clamd.pid");

    if (checkClamdRunning() == false) {
        ui->clamdIconLabel->setMovie(new QMovie(":/icons/icons/gifs/spinning_segments.GIF"));
        ui->clamdIconLabel->movie()->start();
        logWatcher->removePath(QDir::homePath() + "/.clamav-gui/clamd.log");
        QFile logFile(QDir::homePath() + "/.clamav-gui/clamd.log");
        logFile.remove();
        logFile.open(QIODevice::ReadWrite);
        QTextStream stream(&logFile);
        stream << "";
        logFile.close();
        ui->clamdLogPlainTextEdit->clear();
        para = " echo \"   ####   ####       ##     ##   ##  #####\n";
        para +=        "  ##  ##   ##       ####    ### ###   ## ##\n";
        para +=        " ##        ##      ##  ##   #######   ##  ##\n";
        para +=        " ##        ##      ##  ##   #######   ##  ##\n";
        para +=        " ##        ##   #  ######   ## # ##   ##  ##\n";
        para +=        "  ##  ##   ##  ##  ##  ##   ##   ##   ## ##\n";
        para +=        "   ####   #######  ##  ##   ##   ##  #####\n\";";
        para += "echo \"******************************************\nStarting clamd as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo " + clamdLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf && sudo " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log";
        parameters << "-e" << para;
        startClamdProcess->start("xterm",parameters);
    } else {
        QProcess * killProcess = new QProcess(this);
        pidFile.open(QIODevice::ReadOnly);
        QTextStream stream(&pidFile);
        QString pid = stream.readLine();
        pidFile.close();
        para = " echo \"   ####   ####       ##     ##   ##  #####\n";
        para +=        "  ##  ##   ##       ####    ### ###   ## ##\n";
        para +=        " ##        ##      ##  ##   #######   ##  ##\n";
        para +=        " ##        ##      ##  ##   #######   ##  ##\n";
        para +=        " ##        ##   #  ######   ## # ##   ##  ##\n";
        para +=        "  ##  ##   ##  ##  ##  ##   ##   ##   ## ##\n";
        para +=        "   ####   #######  ##  ##   ##   ##  #####\n\";";
        para += "echo \"******************************************\nStopping clamd as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo kill -sigterm " + pid + " && /usr/bin/sudo kill -9 " + clamonaccPid;
        parameters << "-e" << para;
        killProcess->start("xterm",parameters);
    }
    ui->startStopClamdPushButton->setEnabled(false);
}

void setupTab::slot_startClamdProcessFinished() {
    ui->clamdIconLabel->clear();
    ui->clamdIconLabel->setPixmap(QPixmap(":/icons/icons/onaccess.png"));
    if (checkClamdRunning() == false) {
        pidWatcher->removePath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet("background-color:red;color:yellow");
        ui->startStopClamdPushButton->setText("Clamd not running - Start Clamd");
    } else {
        pidWatcher->addPath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet("background-color:green;color:yellow");
        ui->startStopClamdPushButton->setText("Clamd running - Stop Clamd");
        logWatcher->addPath(QDir::homePath() + "/.clamav-gui/clamd.log");
        slot_logFileContentChanged();
        QStringList parameters;
        parameters << "ax";
        findclamonaccProcess->start("ps",parameters);
    }
    ui->startStopClamdPushButton->setEnabled(true);
}

void setupTab::slot_findclamonaccProcessFinished()
{
    QString output = findclamonaccProcess->readAll();
    int start;
    int stop;
    start = output.indexOf(clamonaccLocation + " -c");
    if (start != -1) {
        start = output.lastIndexOf("\n",start);
        start++;
        while (output.mid(start,1) == " ") start++;
        stop = output.indexOf(" ",start);
        clamonaccPid = output.mid(start,stop-start);
    }

}

void setupTab::slot_pidWatcherTriggered()
{
    QFile pidFile("/tmp/clamd.pid");
    if (pidFile.exists() == false) {
        pidWatcher->removePath("/tmp/clamd.pid");
        ui->startStopClamdPushButton->setStyleSheet("background-color:red;color:yellow");
        ui->startStopClamdPushButton->setText("Clamd not running - Start Clamd");
        ui->startStopClamdPushButton->setEnabled(true);
    }
}

void setupTab::slot_selectedLanguageChanged()
{
    setupFile->setSectionValue("Setup","language",ui->languageSelectComboBox->currentText().mid(0,7));
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

void setupTab::slot_clamdLocationProcessFinished()
{
    QString searchstring = "clamd:";
    QString output = clamdLocationProcess->readAll();
    if (output == searchstring + "\n") {
        ui->startStopClamdPushButton->setEnabled(false);
    } else {
        int start = output.indexOf(" ") + 1;
        int end = output.indexOf(" ",start) - start;
        clamdLocation = output.mid(start,end);
    }
}

void setupTab::slot_clamonaccLocationProcessFinished()
{
    QString searchstring = "clamonacc:";
    QString output = clamonaccLocationProcess->readAll();
    if (output == searchstring + "\n") {
        ui->startStopClamdPushButton->setEnabled(false);
    } else {
        int start = output.indexOf(" ") + 1;
        int end = output.indexOf(" ",start) - start;
        clamonaccLocation = output.mid(start,end);

        QStringList parameters;
        parameters << "ax";
        findclamonaccProcess->start("ps",parameters);
    }
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


void setupTab::slot_monitoringAddButtonClicked()
{
    QString path = QFileDialog::getExistingDirectory(this,tr("Directory to monitor"),QDir::homePath());
    if (path != "") {
        if (ui->monitoringComboBox->findText(path) == -1) {
            ui->monitoringComboBox->addItem(path);
            setupFile->setSectionValue("Clamonacc",path,"under monitoring");
            clamdConf->addSingleLineValue("OnAccessIncludePath",path);
            if (checkClamdRunning() == true) restartClamonacc();
        } else {
            QMessageBox::warning(this,tr("WARNING"),tr("Path already under monitoring"));
        }
    }
}

void setupTab::slot_monitoringDelButtonClicked()
{
    QString entry = ui->monitoringComboBox->currentText();
    if (entry != "") {
        if (QMessageBox::information(this,tr("Remove Folder from monitoring"),tr("Path: ") + entry + "\n" + tr("Do you want to remove the folder from the monitoring list?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes) {
            setupFile->removeKeyword("Clamonacc",entry);
            ui->monitoringComboBox->removeItem(ui->monitoringComboBox->currentIndex());
            clamdConf->removeSingleLine("OnAccessIncludePath",entry);
            if (checkClamdRunning() == true) restartClamonacc();
        }
    }
}

void setupTab::slot_restartClamonaccProcessFinished()
{
    QStringList parameters;
    parameters << "ax";
    findclamonaccProcess->start("ps",parameters);
}


void setupTab::restartClamonacc()
{
    QStringList parameters;
    QString command = "echo \"## ##   ####       ##     ##   ##   ## ##   ###  ##    ##      ## ##    ## ##   ";
    command += "##   ##   ##         ##     ## ##   ##   ##    ## ##     ##    ##   ##  ##   ## ";
    command += "##        ##       ## ##   # ### #  ##   ##   # ## #   ## ##   ##       ##      ";
    command += "##        ##       ##  ##  ## # ##  ##   ##   ## ##    ##  ##  ##       ##      ";
    command += "##        ##       ## ###  ##   ##  ##   ##   ##  ##   ## ###  ##       ##      ";
    command += "##   ##   ##  ##   ##  ##  ##   ##  ##   ##   ##  ##   ##  ##  ##   ##  ##   ## ";
    command += "## ##   ### ###  ###  ##  ##   ##   ## ##   ###  ##  ###  ##   ## ##    ## ##   \";";
    command += "echo \"******************************************\nStarting clamd as root\nPlease enter root password\n******************************************\n\";";
    command += "sudo kill -9 " + clamonaccPid + " && sudo " + clamonaccLocation + " -c " + QDir::homePath() + "/.clamav-gui/clamd.conf -l " + QDir::homePath() + "/.clamav-gui/clamd.log";
    parameters << "-e" << command;
    restartClamonaccProcess->start("xterm",parameters);
}

bool setupTab::checkClamdRunning()
{
    bool rc = false;

    QFile clamdPidFile("/tmp/clamd.pid");
    if (clamdPidFile.exists() == true) {
        if (clamdPidFile.open(QIODevice::ReadOnly) == true){
            QTextStream stream(&clamdPidFile);
            QString pid = stream.readLine();
            clamdPidFile.close();
            QDir processDir("/proc/" + pid);
            rc = processDir.exists();
        }
    }

    return rc;
}



