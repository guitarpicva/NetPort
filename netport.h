// ----------------------------------------------------------------------------
// Copyright (C) 2016 Mitch Winkle
// ALL RIGHTS RESERVED
//
// This file is part of Station Manager software project.  Station Manager is
// NOT free software.  It has been released to the members of the Military
// Auxiliary Radio System organization under no-cost terms.  That release
// grant may be rescinded at any time by the author.
//
// Station Manager is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// Parts of Station Manager may contain source code from Open Source projects.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// ----------------------------------------------------------------------------
#ifndef NETPORT_H
#define NETPORT_H

#include <QWidget>
#include <QCloseEvent>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSerialPort>
#include <QTimer>

namespace Ui {
class NetPort;
}

class NetPort : public QWidget
{
    Q_OBJECT

public:
    explicit NetPort(const QString iniFile, QWidget *parent = 0);
    ~NetPort();

public slots:
    void on_settingsSaved();
private slots:
    void closeEvent(QCloseEvent *event);
    void on_configButton_clicked();
    void serialDone();
    void tcpDone();
    void tcpConnectionOpened();
    void tcpReadyRead();
    void serialReadyRead();
    void tcpDisconnected();
private:
    Ui::NetPort *ui;
    const QString versionNumber = QString(__DATE__);
    void loadSettings();
    void connectToSerial();
    void startServer();
    QSettings *settings = nullptr;
    int i_tcpPort = 9876;
    int tretries = 0;
    int sretries = 0;
    QString s_ipAddress = "127.0.0.1";
    QString s_serialPortName = "COM1";
    QString s_serialBitParams = "8N1";
    QString cfgIniFile = "";
    int i_serialBaudRate = 38400;
    int i_serialTimeoutMs = 50;
    int i_tcpTimeoutMs = 10;
    QTcpServer *server = nullptr;
    QTcpSocket *socket = nullptr;
    QSerialPort *s_port = nullptr;
    QTimer *tcpTimeout = nullptr;
    QTimer *serialTimeout = nullptr;
    QByteArray serialBytes;
    QByteArray tcpBytes;
};

#endif // NETPORT_H
