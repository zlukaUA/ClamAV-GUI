#include "infodialog.h"
#include "ui_infodialog.h"

infoDialog::infoDialog(QWidget *parent) : QWidget(parent), ui(new Ui::infoDialog)
{
 QString version = "0.9.0";

    QString infoText = "<html><head/><body><p align='center'><span style=' font-size:22px; color:#000080;'>ClamAV-GUI</span></p><p align='center'><span style=' font-size:15px; color:#000000;'><br/>Version " + version + " (2015 - 2024), published unter GPL 3.0</span></p><hr/><table border='0' style=' margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;' align='center' cellspacing='2' cellpadding='0'><tr><td colspan='7'><p><span style=' font-size:14px; font-weight:700; text-decoration: underline;'>Author:</span><span style=' font-size:14px;'>  Joerg Macedo da Costa Zopes &lt;joerg.zopes@gmx.de&gt;<br/></span></p></td></tr><tr><td><p><span style=' font-size:14px; font-weight:700; text-decoration: underline;'>Translators</span></p></td><td/><td/><td/><td/><td/><td/></tr><tr><td><p><img src='qrc:///icons/icons/dansk.png'/><span style=' font-size:14px;'> Danish:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Martin Eilssøe &lt;martin.eilsoe@gmail.com&gt;</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><img src='qrc:///icons/icons/spain.png'/><span style=' font-size:14px;'> Spanish:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Pedro Álamo &lt;palamodz@gmail.com&gt;</span></p></td></tr><tr><td><p><img src='qrc:///icons/icons/germany.png'/><span style=' font-size:14px;'> German:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>translated by the author.</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><img src='qrc:///icons/icons/france.png'/><span style=' font-size:14px;'> French:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Not be named </span></p></td></tr><tr><td><p><img src='qrc:///icons/icons/Portugal.png'/><span style=' font-size:14px;'> Portuguese:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Translated by the author.</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><img src='qrc:///icons/icons/Brasil.png'/><span style=' font-size:14px;'> Brasilian:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Translated by the author.</span></p></td></tr><tr><td><p><img src='qrc:///icons/icons/GB.png'/><span style=' font-size:14px;'> English:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>Translated by the author.</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><img src='qrc:///icons/icons/italy.png'/><span style=' font-size:14px;'> Italian:</span></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>translated by DeepL</span></p></td></tr></table><hr/><table border='0' style=' margin-top:20px; margin-bottom:0px; margin-left:20px; margin-right:0px;' cellspacing='2' cellpadding='0'><tr><td><p><img src='qrc:///icons/icons/ukraine.png'/></p></td><td><p><span style=' font-size:14px;'/></p></td><td><p><span style=' font-size:14px;'>A special thanks goes to UALinux &lt;main@ualinux.com&gt;.<br/>We were in close contact for more than a week and<br/>with their help I was able to fix some bugs<br/>and implement useful new features.<br/><br/>Many thanks for that.</span></p></td></tr></table></body></html>";
    ui->setupUi(this);
    ui->creditTextEdit->setHtml(infoText);
}

infoDialog::~infoDialog()
{
    delete ui;
}
