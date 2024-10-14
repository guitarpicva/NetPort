#include "npwindow.h"
#include "ui_npwindow.h"

#include <QInputDialog>

#include <QDebug>

NPWindow::NPWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::NPWindow)
{
    ui->setupUi(this);
    settings = new QSettings("NetPort.ini", QSettings::IniFormat);
    loadSettings();
}

NPWindow::~NPWindow()
{
    delete ui;
}

void NPWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
}

void NPWindow::loadSettings()
{
    // build or rebuild the list of netports
    // set object name to np<i> where <i> is
    // the index of the NetPort object for
    // later reference
    const QStringList groups = settings->childGroups();
    qDebug()<<"NPW: Devices:"<<groups<<groups.length();
    numWidgets = groups.length();
    deleteWidgets();
    foreach (const QString g, groups) {
        createNetPort(g, false);
    }
    qApp->processEvents();
    qDebug()<<"NPW: Widgets End:"<<centralWidget()->findChildren<NetPort *>();
    adjustSize();
}

void NPWindow::deleteWidgets() // not currently used
{
    // clear all existing on reload
    const int itemCount = centralWidget()->findChildren<NetPort *>().length();
    qDebug()<<"NPW: Widgets:"<<itemCount<<centralWidget()->findChildren<NetPort *>();
    if(itemCount > 0) {
        foreach(NetPort *it, centralWidget()->findChildren<NetPort *>()) {
            qDebug()<<"NPW: delete:"<<it->objectName();
            delete it;
            // it->deleteLater();
            qApp->processEvents();
            // adjustSize();
        }
    }
}

void NPWindow::saveSettings()
{

}

void NPWindow::createNetPort(const QString name, bool newport)
{
    NetPort* np = new NetPort(name, centralWidget());
    np->setObjectName(name);
    connect(np, &NetPort::settingsSaved, this, &NPWindow::loadSettings);
    // qDebug()<<"object name:"<<np->objectName();
    if(newport) {
        addNPToSettings(np, name); // increment for next new
    }
    else {
        np->loadSettings();
    }
    centralWidget()->layout()->addWidget(np);
}

void NPWindow::on_action_New_NetPort_triggered()
{
    createNetPort("DEVICE", true);
}

void NPWindow::addNPToSettings(NetPort* np, const QString name) {
    // add the settings into the INI file
    qDebug()<<"addNPToList:"<<name;
    /*
deviceName=TEST
serialPortName=COM7
serialBaudRate=115200
serialBitParams=8N1
serialPortTimeout=40
ipAddress=127.0.0.1
tcpPort=9876
tcpPortTimeout=20
    */
    settings->beginGroup(name);
    settings->setValue("deviceName", name);
    settings->setValue("serialPortName", "COM7");
    settings->setValue("serialBaudRate", 115200);
    settings->setValue("serialBitParams", "8N1");
    settings->setValue("serialPortTimeout", 40);
    settings->setValue("ipAddress", "127.0.0.1");
    settings->setValue("tcpPort", 9876);
    settings->setValue("tcpPortTimeout", 20);
    settings->endGroup();
    np->on_configButton_clicked();
}

void NPWindow::on_action_NetPort_triggered()
{
    QStringList devices;
    QStringList groups = settings->childGroups();
    foreach(const QString g, groups) {
        QString device = g + "-" + settings->value(g + "/deviceName").toString();
        devices<<device;
    }
    bool ok = false;
    const QString item = QInputDialog::getItem(this, "Choose Device", "Select the device to remove.", devices, 0, false, &ok);
    qDebug()<<"Selected:"<<item<<ok;
    if(!ok || item.isEmpty()) { return; }
    settings->beginGroup(item.mid(0, item.indexOf("-")));
    settings->remove("");
    settings->endGroup();
    // --numWidgets;
    // settings->setValue("numWidgets", numWidgets);
    QLayoutItem *it = centralWidget()->layout()->itemAt(item.mid(2, item.indexOf("-") - 2).toInt());
    if(it) {
        centralWidget()->layout()->removeItem(it);
        it->widget()->deleteLater();
    }
    numWidgets = settings->childGroups().length();
    adjustSize();
}

