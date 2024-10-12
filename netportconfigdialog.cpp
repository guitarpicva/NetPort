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
#include "netportconfigdialog.h"
#include "ui_netportconfigdialog.h"

#include <QSettings>
//#include <QDebug>
#include <QSerialPortInfo>

QString myIniFile;
QSettings * settings = 0;

NetPortConfigDialog::NetPortConfigDialog(const QString iniFile, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetPortConfigDialog)
{
    ui->setupUi(this);
    myIniFile = iniFile;
    NetPortConfigDialog::setAttribute(Qt::WA_DeleteOnClose);
    settings = new QSettings(myIniFile, QSettings::IniFormat, this);
    loadSettings();
}

NetPortConfigDialog::~NetPortConfigDialog()
{
    delete ui;
}

void NetPortConfigDialog::on_saveButton_clicked()
{
    saveSettings();
    emit(settingsSaved());
    this->close();
}

void NetPortConfigDialog::on_cancelButton_clicked()
{
    this->close();
}

void NetPortConfigDialog::loadSettings()
{
    ui->displayNameLineEdit->setText(settings->value(tr("deviceName"),"").toString());
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portlist;
    foreach(const QSerialPortInfo i, ports)
        portlist << i.portName();
    ui->serialPortNameComboBox->addItems(portlist);
    ui->serialPortNameComboBox->setCurrentText(settings->value(tr("serialPortName"), "COM1").toString());
    ui->ipAddressLineEdit->setText(settings->value(tr("ipAddress"), "127.0.0.1").toString());
    ui->tcpPortLineEdit->setText(settings->value(tr("tcpPort"), "9876").toString());
    ui->tcpPortTimeoutSpinBox->setValue(settings->value(tr("tcpPortTimeout"), 20).toInt());
    ui->serialBitParamsComboBox->setCurrentText(settings->value(tr("serialBitParams"), "8N1").toString());
    ui->serialBaudRateComboBox->setCurrentText(settings->value(tr("serialBaudRate"), "38400").toString());
    ui->serialPortTimeoutSpinBox->setValue(settings->value(tr("serialPortTimeout"), 40).toInt());
}

void NetPortConfigDialog::saveSettings()
{
    //qDebug()<<"saveSettings"<<settings;
    settings->setValue(tr("deviceName"), ui->displayNameLineEdit->text().trimmed());
    settings->setValue("serialPortName", ui->serialPortNameComboBox->currentText());
    settings->setValue("serialBaudRate", ui->serialBaudRateComboBox->currentText());
    settings->setValue("serialBitParams", ui->serialBitParamsComboBox->currentText());
    settings->setValue("serialPortTimeout", ui->serialPortTimeoutSpinBox->value());
    settings->setValue("ipAddress", ui->ipAddressLineEdit->text().trimmed());
    settings->setValue("tcpPort", ui->tcpPortLineEdit->text().trimmed());
    settings->setValue("tcpPortTimeout", ui->tcpPortTimeoutSpinBox->value());
}
