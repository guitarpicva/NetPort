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

NetPortConfigDialog::NetPortConfigDialog(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NetPortConfigDialog)
{
    ui->setupUi(this);
    myIniFile = "NetPort.ini";
    npname = name;
    qDebug()<<"npname:"<<npname;
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
    // emit(settingsSaved());
    //close();
}

void NetPortConfigDialog::on_cancelButton_clicked()
{
    this->close();
}

void NetPortConfigDialog::loadSettings()
{
    settings->beginGroup(npname);
    const QString dev = settings->value(tr("deviceName"), "DEVICE").toString();
    ui->displayNameLineEdit->setText(dev);
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    QStringList portlist;
    foreach(const QSerialPortInfo i, ports) { portlist << i.portName(); }
    ui->serialPortNameComboBox->addItems(portlist);
    QString tmp = settings->value("serialPortName", "").toString();
    if(!tmp.isEmpty()) {
            ui->serialPortNameComboBox->setCurrentText(tmp);
    }
    ui->ipAddressLineEdit->setText(settings->value(tr("ipAddress"), "127.0.0.1").toString());
    ui->tcpPortSpinBox->setValue(settings->value(tr("tcpPort"), 9876).toInt());
    ui->tcpPortTimeoutSpinBox->setValue(settings->value(tr("tcpPortTimeout"), 20).toInt());
    ui->serialBitParamsComboBox->setCurrentText(settings->value(tr("serialBitParams"), "8N1").toString());
    ui->serialBaudRateComboBox->setCurrentText(settings->value(tr("serialBaudRate"), "38400").toString());
    ui->serialPortTimeoutSpinBox->setValue(settings->value(tr("serialPortTimeout"), 40).toInt());
    settings->endGroup();
}

void NetPortConfigDialog::saveSettings()
{
    const QString newdev = ui->displayNameLineEdit->text().trimmed().toUpper();
    qDebug()<<"saveSettings"<<npname<<newdev;
    if(newdev == "DEVICE") {
        return;
    }
    if(npname != newdev) {
        qDebug()<<"rename device"<<newdev;
        settings->beginGroup(newdev);
        settings->setValue("deviceName", newdev);
        settings->setValue("serialPortName", ui->serialPortNameComboBox->currentText());
        settings->setValue("serialBaudRate", ui->serialBaudRateComboBox->currentText());
        settings->setValue("serialBitParams", ui->serialBitParamsComboBox->currentText());
        settings->setValue("serialPortTimeout", ui->serialPortTimeoutSpinBox->value());
        settings->setValue("ipAddress", ui->ipAddressLineEdit->text().trimmed());
        settings->setValue("tcpPort", ui->tcpPortSpinBox->value());
        settings->setValue("tcpPortTimeout", ui->tcpPortTimeoutSpinBox->value());
        settings->endGroup();
        settings->beginGroup(npname);
        settings->remove("");
    }
    else {
        settings->beginGroup(npname);
        //settings->setValue("deviceName", newdev);
        settings->setValue("serialPortName", ui->serialPortNameComboBox->currentText());
        settings->setValue("serialBaudRate", ui->serialBaudRateComboBox->currentText());
        settings->setValue("serialBitParams", ui->serialBitParamsComboBox->currentText());
        settings->setValue("serialPortTimeout", ui->serialPortTimeoutSpinBox->value());
        settings->setValue("ipAddress", ui->ipAddressLineEdit->text().trimmed());
        settings->setValue("tcpPort", ui->tcpPortSpinBox->value());
        settings->setValue("tcpPortTimeout", ui->tcpPortTimeoutSpinBox->value());
        settings->endGroup();
    }
    emit settingsSaved();
}
