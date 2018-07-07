#include "dialog.h"
#include "ui_dialog.h"
#include <iostream>
#include <QList>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    // KEYENCE
	scanner = new LkG5000();
	connect(scanner, &LkG5000::errorOccurred, this, &Dialog::displayError);

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	foreach (QSerialPortInfo port, ports)
        ui->portList->addItem(port.portName());     // List the available ports.
    // Automatically set COM1 if available.
    int com1index = ui->portList->findText("COM1");
    if (com1index > -1)
        ui->portList->setCurrentIndex(com1index);
}

Dialog::~Dialog()
{
    ui->connectBtn->setText("Disconnect");  // Ensure that the correct if-block runs.
	on_connectBtn_clicked();    // Close the port etc.
}

void Dialog::displayData()
{
    QList<QString> data = scanner->getResponses();
    scanner->clearResponses();
    for (int i = 0; i < data.count(); i++)
        ui->outBrowser->append(data[i]);
    disconnect(scanner, &LkG5000::dataReady, this, &Dialog::displayData);
}

void Dialog::displayError()
{
    QSerialPort::SerialPortError errorCode = scanner->getErrorCode();
	if (errorCode == QSerialPort::TimeoutError)
		delayCounter++;
	else
	    ui->outBrowser->append(QString("Error: %1").arg(errorCode));
}

void Dialog::on_connectBtn_clicked()
{
    ui->outBrowser->clear();
    if (ui->connectBtn->text() == "Connect") {
        // Setup LkG5000
        scanner->setPortName(ui->portList->currentText());
        scanner->setBaudRate();
        scanner->open();
        
        ui->keyenceCommandBtn->setEnabled(true);
        ui->connectBtn->setText("Disconnect");
    }
    else {
        scanner->close();
        ui->keyenceCommandBtn->setEnabled(false);
        ui->connectBtn->setText("Connect");
    }
}

void Dialog::on_keyenceCommandBtn_clicked()
{
    QByteArray cmd = ui->keyenceCommand->text().toLatin1();
	connect(scanner, &LkG5000::dataReady, this, &Dialog::displayData);
	scanner->writeCommand(cmd);
}
