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
    setupFileHandler    * setupFile; // clamd && freshclam
    bool                  supressMessage;

private slots:
    void slot_selectedLanguageChanged();
    void slot_basicSettingsChanged();
    void slot_updateSystemInfo();
};

#endif // SETUPTAB_H
