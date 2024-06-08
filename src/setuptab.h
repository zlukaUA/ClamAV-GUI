#ifndef SETUPTAB_H
#define SETUPTAB_H

#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QProcess>
#include <QWidget>
#include <QMovie>
#include <QDir>
#include "setupfilehandler.h"
#include "highlighter.h"

namespace Ui {
class setupTab;
}

class setupTab : public QWidget
{
    Q_OBJECT

public:
    explicit setupTab(QWidget *parent = nullptr);
    ~setupTab();

private:
    Ui::setupTab *ui;
    setupFileHandler    * setupFile;
    setupFileHandler    * freshclamConf;
    setupFileHandler    * clamdConf;
    QProcess            * getDBUserProcess;
    QProcess            * freshclamLocationProcess;
    QProcess            * clamdLocationProcess;
    QProcess            * clamonaccLocationProcess;
    QProcess            * startClamdProcess;
    QProcess            * findclamonaccProcess;
    QProcess            * restartClamonaccProcess;
    QString               freshclamlocationProcessOutput;
    QString               clamdLocation;
    QString               clamonaccLocation;
    QString               clamonaccPid;
    QString               lastFound;
    highlighter         * logHighlighter;
    QFileSystemWatcher  * logWatcher;
    QFileSystemWatcher  * pidWatcher;
    bool                  supressMessage;
    void initFreshclamSettings();
    void restartClamonacc();
    bool checkClamdRunning();

private slots:
    void slot_selectedLanguageChanged();
    void slot_runasrootCheckBoxChanged();
    void slot_basicSettingsChanged();
    void slot_writeFreshclamSettings();
    void slot_dbPathChanged(QString dbPath);
    void slot_getDBUserProcessFinished();
    void slot_pidFileSelectButtonClicked();
    void slot_freshclamLocationProcessFinished();
    void slot_freshclamLocationProcessHasOutput();
    void slot_clamdLocationProcessFinished();
    void slot_clamonaccLocationProcessFinished();
    void slot_setFreshclamsettingsFrameState(bool state);
    void slot_autoStartDaemon();
    void slot_updateClamdConf();
    void slot_logFileContentChanged();
    void slot_clamdStartStopButtonClicked();
    void slot_pidWatcherTriggered();
    void slot_startClamdProcessFinished();
    void slot_findclamonaccProcessFinished();
    void slot_monitoringAddButtonClicked();
    void slot_monitoringDelButtonClicked();
    void slot_restartClamonaccProcessFinished();

signals:
    void disableUpdateButtons();
    void reportError();
    void updateDatabase();
    void setBallonMessage(int, QString,QString);
};

#endif // SETUPTAB_H
