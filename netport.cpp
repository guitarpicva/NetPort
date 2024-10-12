// ----------------------------------------------------------------------------
// Copyright (C) 2016-2024 Mitch Winkle
// ALL RIGHTS RESERVED
//
// This file is part of the NetPort software project.  NetPort is
// NOT free software.  It has been released to the members of the Military
// Auxiliary Radio System organization under no-cost terms.  That release
// grant may be rescinded at any time by the author.
//
// NetPort is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Parts of NetPort may contain source code from Open Source projects.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
#include "netport.h"
#include "ui_netport.h"
#include "netportconfigdialog.h"
//#include "ui_netportconfigdialog.h"

#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>
#include <QThread>

NetPort::NetPort(const QString iniFile, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetPort)
{
    ui->setupUi(this);
    cfgIniFile = iniFile;
    //qDebug()<<"cfgIniFile"<<cfgIniFile;
    loadSettings();
    connectToSerial();

    QWidget::setWindowTitle("NetPort - " + versionNumber);
    this->restoreGeometry(settings->value("geometry").toByteArray());
    //this->restoreState(settings->value("windowState").toByteArray());
}

NetPort::~NetPort()
{
    delete ui;
}

void NetPort::closeEvent(QCloseEvent * event)
{
    // save GUI state
    settings->setValue("geometry", saveGeometry());
    if(socket)
    {
        socket->blockSignals(true);
        socket->disconnectFromHost();
    }
    //settings->setValue("windowState", saveState());
    if(event) // pass the close event up the line
        event->accept();
}

void NetPort::loadSettings()
{
    settings = new QSettings(cfgIniFile, QSettings::IniFormat, this);
    i_tcpPort = settings->value(tr("tcpPort"), 9876).toInt();
    s_ipAddress = settings->value(tr("ipAddress"), "127.0.0.1").toString();
    s_serialPortName = settings->value(tr("serialPortName"), "COM4").toString();
    s_serialBitParams = settings->value(tr("serialBitParams"), "8N1").toString();
    i_serialBaudRate = settings->value(tr("serialBaudRate"), 38400).toInt();
    i_serialTimeoutMs = settings->value(tr("serialTimeoutMs"), 40).toInt();
    i_tcpTimeoutMs = settings->value(tr("tcpTimeoutMs"), 20).toInt();
    ui->nameLabel->setText(settings->value(tr("deviceName"), "DEVICE").toString());
}

void NetPort::connectToSerial()
{
    if(s_port)
    {
        disconnect(s_port, &QSerialPort::readyRead, this, &NetPort::serialReadyRead);
        s_port->close();
    }
    // setup and open the serial port
    s_port = new QSerialPort(this);
    s_port->setPortName(s_serialPortName);
    s_port->setBaudRate(i_serialBaudRate);
    s_port->setFlowControl(QSerialPort::NoFlowControl);
    if (s_serialBitParams.mid(0, 1) == "8")
        s_port->setDataBits(QSerialPort::Data8);
    else
        s_port->setDataBits(QSerialPort::Data7);

    if (s_serialBitParams.mid(1, 1) == "N")
        s_port->setParity(QSerialPort::NoParity);
    else if (s_serialBitParams.mid(1, 1) == "O")
        s_port->setParity(QSerialPort::OddParity);
    else
        s_port->setParity(QSerialPort::EvenParity);

    if (s_serialBitParams.mid(2, 1) == "1")
        s_port->setStopBits(QSerialPort::OneStop);
    else
        s_port->setStopBits(QSerialPort::TwoStop);

    if (s_port->open(QSerialPort::ReadWrite))
    {
        connect(s_port, &QSerialPort::readyRead, this, &NetPort::serialReadyRead);
        ui->serialConnectedLabel->setStyleSheet("background-color:green;border:1px solid green;border-radius:10px;color:white;");
        ui->serialConnectedLabel->setToolTip(s_port->portName());
        startServer();
        serialTimeout = new QTimer(this);
        serialTimeout->setInterval(i_serialTimeoutMs);
        connect(serialTimeout, &QTimer::timeout, this, &NetPort::serialDone);
        tcpTimeout = new QTimer(this);
        tcpTimeout->setInterval(i_tcpTimeoutMs);
        connect(tcpTimeout, &QTimer::timeout, this, &NetPort::tcpDone);
    } else
    {
        QMessageBox::information(this, "NO SERIAL CONNECTION", "NetPort was unable to make a connection to the serial port.\n\nPlease check your configuration and try again.");
    }
}

void NetPort::startServer()
{
    if (server)
    {
        server->close();
        disconnect(server, &QTcpServer::newConnection, this, &NetPort::tcpConnectionOpened);
    }
    // start the socket listener if the serial port has been connected
    else
    {
        server = new QTcpServer(this);
    }
    server->listen(QHostAddress(s_ipAddress), i_tcpPort);
    if (server->isListening())
    {
        ui->netConnectedLabel->setStyleSheet("background-color:blue;color:white;border:1px solid blue;border-radius:10px;");
        ui->netConnectedLabel->setToolTip("Listening on " + s_ipAddress + ":" + QString::number(i_tcpPort));
        connect(server, &QTcpServer::newConnection, this, &NetPort::tcpConnectionOpened);
    }
}

void NetPort::serialReadyRead()
{
    //qDebug()<<"serialReadyRead";
    serialTimeout->stop();
    serialBytes.append(s_port->readAll());
    //qDebug()<<serialBytes;
    serialTimeout->start();
}

void NetPort::serialDone()
{
    //qDebug()<<"serialDone"<<serialBytes;
    serialTimeout->stop();
    // shuttle serial to tcp
    int bytesToWrite = serialBytes.length();
    if(socket)
    {
        // if anything didn't get written
        // keep trying...up to one second
        sretries = 0;
        int written = socket->write(serialBytes);
        socket->flush();
        while(written < bytesToWrite)
        {
            if(sretries > 20)
                break;
            written += socket->write(serialBytes.mid(written));
            sretries++;
            QThread::msleep(i_serialTimeoutMs);
        }
    }
    else
    {
        qDebug()<<"no socket";
        ui->netConnectedLabel->setStyleSheet("color:darkred");
    }
    serialBytes.clear(); // if no socket, send to bit bucket
}

void NetPort::tcpReadyRead()
{
    qDebug()<<"tcpReadyRead";
    tcpTimeout->stop();
    tcpBytes.append(socket->readAll());
    tcpTimeout->start();
}

void NetPort::tcpDone()
{
    qDebug()<<"tcpDone"<<tcpBytes;
    tcpTimeout->stop();
    int bytesToWrite = tcpBytes.length();
    int written = s_port->write(tcpBytes.replace("\n",""));
    s_port->flush();
    // if anything didn't get written
    // keep trying...up to 20 tries
    tretries = 0;
    while (written < bytesToWrite)
    {
        if (tretries > 20)
            break;
        written += s_port->write(tcpBytes.mid(written));
        tretries++;
        QThread::msleep(i_tcpTimeoutMs);
    }
    tcpBytes.clear();
}

void NetPort::tcpConnectionOpened()
{
//    if (socket)
//    {
//        disconnect(socket, 0, 0, 0);
//        socket->abort();
//    }
    //tcpDisconnected();
    if(socket) {
        socket->disconnectFromHost();
        disconnect(socket, 0, 0, 0);
    }
    qApp->processEvents();
    socket = server->nextPendingConnection();
    if (socket->isOpen())
    {
        ui->netConnectedLabel->setStyleSheet("background-color:green;color:white;border:1px solid green;border-radius:10px;");
        ui->netConnectedLabel->setToolTip("CONNECTED " + socket->peerAddress().toString() + ":" + QString::number(socket->localPort()));
        connect(socket, &QTcpSocket::disconnected, this, &NetPort::tcpDisconnected);
        connect(socket, &QTcpSocket::readyRead, this, &NetPort::tcpReadyRead);
    }
    //qDebug()<<"socket connection"<<socket->peerAddress().toString();
    qDebug()<<"Send IF;";
    socket->write("IF;");
    socket->flush();
}

void NetPort::tcpDisconnected()
{
    qDebug()<<"tcp dicsonnected";
    if(socket)
    {
        disconnect(socket, 0, 0, 0);
        socket->abort(); // close and flush pending data immediately
    }
    if(socket)
    {
        if(ui->netConnectedLabel && ui->netConnectedLabel->isVisible())
        {
            ui->netConnectedLabel->setStyleSheet("background-color:blue;color:white;border:1px solid blue;border-radius:10px;");
            ui->netConnectedLabel->setToolTip("Listening on " + s_ipAddress + ":" + QString::number(i_tcpPort));
        }
    }
}

void NetPort::on_configButton_clicked()
{
    NetPortConfigDialog * conf = new NetPortConfigDialog(cfgIniFile, this);
    connect(conf, &NetPortConfigDialog::settingsSaved, this, &NetPort::on_settingsSaved);
    conf->setVisible(true);
}

void NetPort::on_settingsSaved()
{
    // settings have changed so update all ports
    if(serialTimeout)
        serialTimeout->stop();
    if(tcpTimeout)
        tcpTimeout->stop();
    //qDebug()<<"load settings";
    loadSettings();
    //qDebug()<<"connect to serial";
    connectToSerial();
    //qDebug()<<"start server";
    startServer();
}
