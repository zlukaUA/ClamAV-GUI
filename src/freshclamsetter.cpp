#include "freshclamsetter.h"
#include "ui_freshclamsetter.h"

freshclamsetter::freshclamsetter(QWidget *parent) : QWidget(parent), ui(new Ui::freshclamsetter)
{
QDir tempDir;

    ui->setupUi(this);
    updateLogHighLighter = new highlighter(ui->logPlainText->document());
    freshclamLogHighLighter = new highlighter(ui->deamonLogText->document());
    setupFile = new setupFileHandler(QDir::homePath() + "/.clamav-gui/settings.ini");
    freshclamConf = new setupFileHandler(QDir::homePath() + "/.clamav-gui/freshclam.conf");
    updater = new QProcess(this);
    connect(updater,SIGNAL(readyReadStandardError()),this,SLOT(slot_updaterHasOutput()));
    connect(updater,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_updaterHasOutput()));
    connect(updater,SIGNAL(finished(int)),this,SLOT(slot_updaterFinished(int)));

    startDeamonProcess = new QProcess(this);
    connect(startDeamonProcess,SIGNAL(finished(int)),this, SLOT(slot_startDeamonProcessFinished(int)));

    pidFileWatcher = new QFileSystemWatcher(this);
    connect(pidFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_fileSystemWatcherTriggered()));

    logFileWatcher = new QFileSystemWatcher(this);
    connect(logFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_logFileWatcherTriggered()));

    updateLogFileWatcher = new QFileSystemWatcher(this);
    connect(updateLogFileWatcher,SIGNAL(fileChanged(QString)),this,SLOT(slot_updateFileWatcherTriggered()));

    ps_process = new QProcess;
    connect(ps_process,SIGNAL(finished(int)),this,SLOT(slot_ps_processFinished()));
    connect(ps_process,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_ps_processHasOutput()));
    connect(ps_process,SIGNAL(readyReadStandardError()),this,SLOT(slot_ps_processHasOutput()));

    startDelayTimer = new QTimer(this);
    startDelayTimer->setSingleShot(true);
    connect(startDelayTimer,SIGNAL(timeout()),this,SLOT(slot_startDelayTimerExpired()));

    QFile file(QDir::homePath() + "/.clamav-gui/update.log");
    if (tempDir.exists(QDir::homePath() + "/.clamav-gui/update.log") == true) {
        slot_updateFileWatcherTriggered();
        file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
    } else {
        if (file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
            QTextStream stream(&file);
            stream << "";
            file.close();
            slot_updateFileWatcherTriggered();
            updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
            file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        }
    }

    QFile fileFreshclamLog(QDir::homePath() + "/.clamav-gui/freshclam.log");
    if (tempDir.exists(QDir::homePath() + "/.clamav-gui/freshclam.log") == true) {
        fileFreshclamLog.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        logFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");
    } else {
        if (fileFreshclamLog.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
            QTextStream stream(&file);
            stream << "";
            fileFreshclamLog.close();
            logFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");
            fileFreshclamLog.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
        }
    }
    startup = true;
    checkDaemonRunning();
    setUpdaterInfo();
}

freshclamsetter::~freshclamsetter()
{
    delete ui;
}

void freshclamsetter::slot_updateNowButtonClicked(){
QStringList parameters;

    busyLabel = new progressDialog(this);
    busyLabel->setStyleSheet("background-color: rgba(192,192,192, 200);");
    busyLabel->setGeometry((this->width() - 300) / 2,(this->height() - 160) / 2,300,160);
    busyLabel->show();
    busyLabel->setText(tr("Update process startet ....."));
    setForm(false);
    if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true){
        QStringList databaseToUpdate;
        databaseToUpdate << "all" << "main" << "daily" << "bytecode";
        QString whatDB = "";
        QString para = "echo \"### ###  ### ##   ### ###   ## ##   ###  ##   ## ##   ####       ##     ##   ##\n";
        para = para +  " ##  ##   ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##         ##     ## ##\n";
        para = para +  " ##       ##  ##   ##      ####      ##  ##  ##        ##       ## ##   # ### #\n";
        para = para +  " ## ##    ## ##    ## ##    #####    ## ###  ##        ##       ##  ##  ## # ##\n";
        para = para +  " ##       ## ##    ##          ###   ##  ##  ##        ##       ## ###  ##   ##\n";
        para = para +  " ##       ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##  ##   ##  ##  ##   ##\n";
        para = para +  "####     #### ##  ### ###   ## ##   ###  ##   ## ##   ### ###  ###  ##  ##   ##\n\";";
        if (setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate") > 0) whatDB = " --update-db=" + databaseToUpdate[setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate")];
        if ((setupFile->getSectionValue("Directories","LoadSupportedDBFiles") != "") && (setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("not checked") == -1)){
            para = para + "echo \"******************************************\nStarting freshclam as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo " + setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " --datadir=" + setupFile->getSectionValue("Directories","LoadSupportedDBFiles").mid(setupFile->getSectionValue("Directories","LoadSupportedDBFiles").indexOf("|")+1) + " 2>&1 > " + QDir::homePath() + "/.clamav-gui/update.log" + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf" + whatDB;
        } else {
            para = para + "echo \"******************************************\nStarting freshclam as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo " + setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " 2>&1 > " + QDir::homePath() + "/.clamav-gui/update.log" + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf" + whatDB;
        }
        parameters << "-e" << para;
        updater->start("xterm",parameters);
    } else {
        QStringList databaseToUpdate;
        databaseToUpdate << "all" << "main" << "daily" << "bytecode";
        QString databaseDir = setupFile->getSectionValue("Directories","LoadSupportedDBFiles");
        QString checked = databaseDir.mid(0,databaseDir.indexOf("|"));
        databaseDir = databaseDir.mid(databaseDir.indexOf("|") + 1);
        if ((databaseDir != "") && (checked == "checked")) parameters << "--datadir=" + databaseDir;
        if (setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate") > 0) parameters << "--update-db=" + databaseToUpdate[setupFile->getSectionIntValue("FreshClam","DataBaseToUpdate")];
        parameters << "-l" << QDir::homePath() + "/.clamav-gui/update.log";
        parameters << "--show-progress";
        parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/freshclam.conf";

        updater->start(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") ,parameters);
    }
    updateLogFileWatcher->removePath(QDir::homePath() + "/.clamav-gui/update.log");
    updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/update.log");
}


void freshclamsetter::slot_startStopDeamonButtonClicked(){
QStringList parameters;
    if (pidFile == ""){
        if (logFile != "") logFileWatcher->removePath(logFile);
        pidFile = freshclamConf->getSingleLineValue("PidFile");
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
        logFileWatcher->addPath(logFile);
        if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true){
            if (startup == false) {
                QString para = "echo \"### ###  ### ##   ### ###   ## ##   ###  ##   ## ##   ####       ##     ##   ##\n";
                para = para +  " ##  ##   ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##         ##     ## ##\n";
                para = para +  " ##       ##  ##   ##      ####      ##  ##  ##        ##       ## ##   # ### #\n";
                para = para +  " ## ##    ## ##    ## ##    #####    ## ###  ##        ##       ##  ##  ## # ##\n";
                para = para +  " ##       ## ##    ##          ###   ##  ##  ##        ##       ## ###  ##   ##\n";
                para = para +  " ##       ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##  ##   ##  ##  ##   ##\n";
                para = para +  "####     #### ##  ### ###   ## ##   ###  ##   ## ##   ### ###  ###  ##  ##   ##\n\";";
                para = para + "echo \"******************************************\nStarting freshclam daemon as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo " + setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " -d -l " + logFile + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf";
                parameters << "-e" << para;
                startDeamonProcess->start("xterm",parameters);
            } else {
                startDelayTimer->start(5000);
            }
        } else {
            parameters << "-d";
            parameters << "-l" << logFile;
            parameters << "--config-file" << QDir::homePath() + "/.clamav-gui/freshclam.conf";
            startDeamonProcess->start(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation"),parameters);
        }
    } else {
        QFile tempFile(pidFile);
        tempFile.open(QIODevice::ReadOnly);
        QTextStream stream(&tempFile);
        QString pidString = stream.readLine();
        if (setupFile->getSectionBoolValue("FreshClam","runasroot") == true) {
          parameters << "-e";
          QString para = "echo \"### ###  ### ##   ### ###   ## ##   ###  ##   ## ##   ####       ##     ##   ##\n";
          para = para +  " ##  ##   ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##         ##     ## ##\n";
          para = para +  " ##       ##  ##   ##      ####      ##  ##  ##        ##       ## ##   # ### #\n";
          para = para +  " ## ##    ## ##    ## ##    #####    ## ###  ##        ##       ##  ##  ## # ##\n";
          para = para +  " ##       ## ##    ##          ###   ##  ##  ##        ##       ## ###  ##   ##\n";
          para = para +  " ##       ##  ##   ##  ##  ##   ##   ##  ##  ##   ##   ##  ##   ##  ##  ##   ##\n";
          para = para +  "####     #### ##  ### ###   ## ##   ###  ##   ## ##   ### ###  ###  ##  ##   ##\n\";";
          para = para + "echo \"******************************************\nTerminating freshclam running as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo kill -sigterm " + pidString + " && /usr/bin/sudo rm " + pidFile;
          parameters << para;
          QProcess::execute("xterm",parameters);
          checkDaemonRunning();
        } else {
          parameters << "-sigterm" << pidString;
          QProcess::execute("kill",parameters);
          checkDaemonRunning();
        }
    }

}

void freshclamsetter::checkDaemonRunning(){
    QStringList ps_parameters;
    ps_parameters << "ax";
    ps_process->start("ps",ps_parameters);
}

void freshclamsetter::slot_ps_processFinished()
{
    QFile tempFile;
    ps_processOutput = ps_processOutput + ps_process->readAllStandardOutput();
    ps_processOutput = ps_processOutput + ps_process->readAllStandardError();
    QStringList processes = ps_processOutput.split("\n");
    QString freshclamProcess;


    for (int x = 0; x < processes.size(); x++) {
        if ((processes[x].indexOf(setupFile->getSectionValue("FreshclamSettings","FreshclamLocation")) != -1) || (processes[x].indexOf(" freshclam ") != -1)) freshclamProcess = processes[x];
    }

    QStringList values;
    QString part;

    for (int x = 1; x < freshclamProcess.length(); x++) {
        if ((freshclamProcess.mid(x,1) != " ") && (freshclamProcess.mid(x,1) != "\t")) {
          part = part + freshclamProcess.mid(x,1);
        } else {
          if (part.length() > 0) values.append(part);
          part = "";
        }
    }
    if (part.length() > 0) values.append(part);

    if (freshclamProcess != "") {
        pidFile = freshclamConf->getSingleLineValue("PidFile");
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
    } else {
        pidFile = "";
        logFile = "";
    }
    if ((pidFile != "") && (tempFile.exists(pidFile) == true)){
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:green");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        setupFile->setSectionValue("Freshclam","Started",true);
        pidFileWatcher->addPath(pidFile);
        emit setSetupFrameState(false);
    } else {
        pidFile = "";
        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:red");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        setupFile->setSectionValue("Freshclam","Started",false);
        emit setSetupFrameState(true);
        if ((startup == true) && (setupFile->getSectionBoolValue("Freshclam","StartDaemon") == true)) {
            slot_startStopDeamonButtonClicked();
        }
    }

    if ((logFile != "") && (tempFile.exists(logFile) == true)){
        logFileWatcher->addPath(logFile);
        slot_logFileWatcherTriggered();
    } else {
        logFile = QDir::homePath() + "/.clamav-gui/freshclam.log";
        if (tempFile.exists(logFile) == true){
          logFileWatcher->addPath(logFile);
          slot_logFileWatcherTriggered();
        } else {
          QFile file(QDir::homePath() + "/.clamav-gui/freshclam.log");
          if (file.open(QIODevice::ReadWrite|QIODevice::Append|QIODevice::Text)){
              QTextStream stream(&file);
              stream << "";
              file.close();
              logFileWatcher->addPath(logFile);
              slot_logFileWatcherTriggered();
          }
        }
    }
    startup = false;
}

void freshclamsetter::slot_disableUpdateButtons()
{
    setForm(false);
}

void freshclamsetter::slot_startDelayTimerExpired()
{
QStringList parameters;

    QString para = "echo \"******************************************\nStarting freshclam daemon as root\nPlease enter root password\n******************************************\n\"; /usr/bin/sudo " + setupFile->getSectionValue("FreshclamSettings","FreshclamLocation") + " -d -l " + logFile + " --config-file=" + QDir::homePath() + "/.clamav-gui/freshclam.conf";
    parameters << "-e" << para;
    startDeamonProcess->start("xterm",parameters);
}


void freshclamsetter::slot_ps_processHasOutput()
{
    ps_processOutput = ps_processOutput + ps_process->readAllStandardOutput();
    ps_processOutput = ps_processOutput + ps_process->readAllStandardError();
}


void freshclamsetter::slot_updaterFinished(int rc){
    delete busyLabel;

    if (rc == 0) {
        emit setBallonMessage(0,tr("INFO"),tr("Update-Process finished"));
        setUpdaterInfo();
    } else {
        emit setBallonMessage(1,tr("WARNING"),tr("Update-Process failed!\nRead log-messages for possible reason."));
    }
    setForm(true);
}

void freshclamsetter::slot_fileSystemWatcherTriggered(){
QDir tempDir;

    if ((pidFile != "") && (tempDir.exists(pidFile) == true)){
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:green");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        pidFileWatcher->addPath(pidFile);
    } else {
        pidFile = "";
        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:red");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/freshclam.png"));
        //if (setupFile->getSectionBoolValue("Freshclam","Started") == true) slot_startStopDeamonButtonClicked();
    }
}

void freshclamsetter::slot_clearLogButtonClicked(){
QFile file(QDir::homePath() + "/.clamav-gui/update.log");

    file.remove();
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    stream << "";
    file.close();
    ui->logPlainText->setPlainText("");
}

void freshclamsetter::slot_logFileWatcherTriggered(){
QFile file(logFile);
QString content;
QString value;
int pos;

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();

    pos = content.lastIndexOf("ClamAV update process started at");
    if (pos != -1){
        value = content.mid(pos + 33,content.indexOf("\n",pos + 33) - (pos + 33));
        setupFile->setSectionValue("Updater","LastUpdate",value);
    }
    pos = content.lastIndexOf("main.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 17,content.indexOf("\n",pos + 17) - (pos + 17));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",value);
    }
    pos = content.lastIndexOf("main.cvd is up to date");
    if (pos != -1){
        value = content.mid(pos + 23,content.indexOf("\n",pos + 23) - (pos + 23));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",value);
    }
    pos = content.lastIndexOf("daily.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 18,content.indexOf("\n",pos + 18) - (pos + 18));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }
    pos = content.lastIndexOf("daily.cld updated");
    if (pos != -1){
        value = content.mid(pos + 18,content.indexOf("\n",pos + 18) - (pos + 18));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }
    pos = content.lastIndexOf("daily.cld is up to date");
    if (pos != -1){
        value = content.mid(pos + 24,content.indexOf("\n",pos + 24) - (pos + 24));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }
    pos = content.lastIndexOf("bytecode.cvd is up to date");
    if (pos != -1){
        value = content.mid(pos + 27,content.indexOf("\n",pos + 27) - (pos + 27));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",value);
    }
    pos = content.lastIndexOf("bytecode.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 21,content.indexOf("\n",pos + 21) - (pos + 21));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",value);
    }
    pos = content.lastIndexOf("Database updated");
    if (pos != -1){
        value = content.mid(pos,content.indexOf("\n",pos) - (pos));
        setupFile->setSectionValue("Updater","DatabaseFrom",value);
    }

    setUpdaterInfo();

    ui->deamonLogText->clear();
    QStringList lines = content.split("\n");
    foreach(QString line,lines){
        ui->deamonLogText->insertPlainText(line + "\n");
        ui->deamonLogText->ensureCursorVisible();
    }
}

void freshclamsetter::slot_updateFileWatcherTriggered(){
QFile file(QDir::homePath() + "/.clamav-gui/update.log");
QString content;
QString value;
int pos;

    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    content = stream.readAll();
    file.close();

    pos = content.indexOf("ClamAV update process started at");
    if (pos != -1){
        value = content.mid(pos + 33,content.indexOf("\n",pos + 33) - (pos + 33));
        setupFile->setSectionValue("Updater","LastUpdate",value);
    }
    pos = content.indexOf("main.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 17,content.indexOf("\n",pos + 17) - (pos + 17));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","MainVersion",value);
    }
    pos = content.indexOf("daily.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 18,content.indexOf("\n",pos + 18) - (pos + 18));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","DailyVersion",value);
    }
    pos = content.indexOf("bytecode.cvd updated");
    if (pos != -1){
        value = content.mid(pos + 21,content.indexOf("\n",pos + 21) - (pos + 21));
        value.replace("(","");
        value.replace(")","");
        setupFile->setSectionValue("Updater","BytecodeVersion",value);
    }
    pos = content.indexOf("Database updated");
    if (pos != -1){
        value = content.mid(pos,content.indexOf("\n",pos) - (pos));
        setupFile->setSectionValue("Updater","DatabaseFrom",value);
    }

    ui->logPlainText->setPlainText("");
    QStringList lines = content.split("\n");
    foreach(QString line,lines){
        ui->logPlainText->insertPlainText(line + "\n");
        ui->logPlainText->ensureCursorVisible();
    }
}

void freshclamsetter::setForm(bool mode){
    ui->clearLogButton->setEnabled(mode);
    ui->deamonClearLogButton->setEnabled(mode);
    ui->deamonClearLogButton->setEnabled(mode);
    ui->startStopDeamonButton->setEnabled(mode);
    ui->updateNowButton->setEnabled(mode);
    ui->updateSettingsGroupBox->setEnabled(mode);
}

void freshclamsetter::slot_clearDeamonLogButtonClicked(){
QFile file(QDir::homePath() + "/.clamav-gui/freshclam.log");

    ui->deamonLogText->setPlainText("");
    updateLogFileWatcher->removePath(QDir::homePath() + "/.clamav-gui/freshclam.log");
    file.remove();
    file.open(QIODevice::ReadWrite);
    QTextStream stream(&file);
    stream << "";
    file.close();
    file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner|QFileDevice::ReadUser|QFileDevice::WriteUser|QFileDevice::ReadGroup|QFileDevice::WriteGroup|QFileDevice::ReadOther|QFileDevice::WriteOther);
    updateLogFileWatcher->addPath(QDir::homePath() + "/.clamav-gui/freshclam.log");

}

void freshclamsetter::setUpdaterInfo(){
    QString htmlCode = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>";
    htmlCode = htmlCode + "<table><tr><td width='200'>";
    if (setupFile->getSectionValue("Updater","DatabaseFrom") != "") {
        htmlCode = htmlCode + tr("Database origin : </td><td>") + setupFile->getSectionValue("Updater","DatabaseFrom") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","LastUpdate") != "") {
        htmlCode = htmlCode + tr("Last Update : </td><td>") + setupFile->getSectionValue("Updater","LastUpdate") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","MainVersion") != "") {
        htmlCode = htmlCode + tr("Main File : </td><td>") + setupFile->getSectionValue("Updater","MainVersion") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","DailyVersion") != "") {
        htmlCode = htmlCode + tr("Daily File : </td><td>") + setupFile->getSectionValue("Updater","DailyVersion") + "</td></tr><tr><td>";
    }
    if (setupFile->getSectionValue("Updater","BytecodeVersion") != "") {
        htmlCode = htmlCode + tr("ByteCode File : </td><td>") + setupFile->getSectionValue("Updater","BytecodeVersion") + "</td></tr></table>";
    }
    htmlCode = htmlCode + "</body></html>";
    ui->updateInfoText->setHtml(htmlCode);
}

void freshclamsetter::slot_updaterHasOutput(){
    static QString oldLine;
    QString output = updater->readAll();
    int start = output.lastIndexOf("]") + 1;
    int end = output.lastIndexOf("[");
    QString line = output.mid(start,end-start-1);

    if ((line != oldLine) && (line.indexOf("MiB") != -1)){
        busyLabel->setText(line);
    }
    oldLine = line;
}

void freshclamsetter::slot_startDeamonProcessFinished(int rc)
{
    if (rc == 0){
        ui->startStopDeamonButton->setText(tr("Deamon running - stop deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:green");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
        pidFileWatcher->addPath(pidFile);
        logFileWatcher->addPath(logFile);
        slot_logFileWatcherTriggered();
        pidFileWatcher->addPath(pidFile);
        checkDaemonRunning();
    } else {
        pidFile = "";
        ui->startStopDeamonButton->setText(tr("Deamon not running - start deamon"));
        ui->startStopDeamonButton->setStyleSheet("background:red");
        ui->startStopDeamonButton->setIcon(QIcon(":/icons/icons/Clam.png"));
    }
}
