#ifndef SETUPTAB_H
#define SETUPTAB_H

#include <QWidget>
#include <QProcess>
#include <QFileDialog>
#include <QDir>
#include "setupfilehandler.h"

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
    QProcess            * getDBUserProcess;
    QProcess            * freshclamLocationProcess;
    QString               freshclamlocationProcessOutput;
    bool                  supressMessage;
    void initFreshclamSettings();

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
    void slot_setFreshclamsettingsFrameState(bool state);
    void slot_autoStartDaemon();

signals:
    void disableUpdateButtons();
    void reportError();
    void updateDatabase();
};

#endif // SETUPTAB_H
