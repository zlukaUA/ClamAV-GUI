#ifndef CLAMDMANAGER_H
#define CLAMDMANAGER_H

#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QWidget>
#include <QProcess>
#include <QMovie>
#include "setupfilehandler.h"
#include "highlighter.h"

namespace Ui {
class clamdManager;
}

class clamdManager : public QWidget
{
    Q_OBJECT

public:
    explicit clamdManager(QWidget *parent = nullptr);
    ~clamdManager();
    setupFileHandler    * setupFile; // clamd && freshclam
    setupFileHandler    * clamdConf; // clamd
    setupFileHandler    * freshclamConf;
    QProcess            * clamdLocationProcess; // clamd
    QProcess            * clamonaccLocationProcess; // clamd
    QProcess            * startClamdProcess; // clamd
    QProcess            * findclamonaccProcess; // clamd
    QProcess            * restartClamonaccProcess; // clamd
    QProcess            * killProcess;
    QString               clamdLocation; // clamd
    QString               clamonaccLocation; // clamd
    QString               clamonaccPid; // clamd
    QString               lastFound; // clamd
    highlighter         * logHighlighter; // clamd
    QFileSystemWatcher  * clamdLogWatcher; // clamd
    QFileSystemWatcher  * clamdPidWatcher; // clamd
    bool                  clamdRestartInProgress; // clamd
    bool                  startup;
    void initClamdSettings(); // clamd
    void restartClamonacc(); // clamd
    bool checkClamdRunning(); // clamd

private slots:
    void slot_updateClamdConf(); // ab hier --> clamd
    void slot_logFileContentChanged();
    void slot_clamdStartStopButtonClicked();
    void slot_pidWatcherTriggered();
    void slot_startClamdProcessFinished();
    void slot_killClamdProcessFinished();
    void slot_findclamonaccProcessFinished();
    void slot_monitoringAddButtonClicked();
    void slot_monitoringDelButtonClicked();
    void slot_restartClamonaccProcessFinished();
    void slot_restartClamdButtonClicked();
    void slot_clamdSettingsChanged();
    void slot_clamdLocationProcessFinished();
    void slot_clamonaccLocationProcessFinished();
    void slot_startClamdOnStartupCheckBoxClicked();


signals:
    void setBallonMessage(int, QString,QString); // clamd
    void setActiveTab();

private:
    Ui::clamdManager *ui;
};

#endif // CLAMDMANAGER_H
