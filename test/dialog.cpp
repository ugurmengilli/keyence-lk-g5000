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
	scanner_ = new LkG5000();

    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	foreach (QSerialPortInfo port, ports)           // List the available ports.
        ui->portList->addItem(port.portName());
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
    // To prevent unintended calls, disconnect this function from the signals
    disconnect(scanner_, 0, this, 0);

    QList<QString> data;

    switch (display_mode_)
    {
    case Dialog::Data:
        data = scanner_->getResponses();
        scanner_->clearResponses();
        for (int i = 0; i < data.count(); i++)
            ui->outBrowser->append(data[i]);
        break;
    case Dialog::AutoMeasurement:
        connect(scanner_, &LkG5000::measurementReady,
                this, &Dialog::displayData, Qt::UniqueConnection);
    case Dialog::Measurement:
        ui->outBrowser->append(
            QTime::fromMSecsSinceStartOfDay(
                scanner_->getMeasuredValues().lastKey()).toString("hh:mm:ss.zzz"));
        ui->outBrowser->append(
            QString::number(scanner_->getMeasuredValues().last(), 'f', 6));
        break;
    default:
        break;
    }
}

void Dialog::displayError()
{
    QSerialPort::SerialPortError errorCode = scanner_->getErrorCode();
    ui->outBrowser->append(QString("Error: %1").arg(errorCode));
}

void Dialog::on_connectBtn_clicked()
{
    ui->outBrowser->clear();
    if (ui->connectBtn->text() == "Connect") {
        // Setup LkG5000
        connect(scanner_, &LkG5000::errorOccurred, this, &Dialog::displayError);
        scanner_->setPortName(ui->portList->currentText());
        scanner_->setBaudRate();
        scanner_->open();
        // Enable scanner controls
        ui->controlGroup->setEnabled(true);
        ui->connectBtn->setText("Disconnect");
    }
    else {
        scanner_->close();
        // Disable scanner controls
        ui->controlGroup->setEnabled(false);
        ui->connectBtn->setText("Connect");
    }
}

void Dialog::on_measureBtn_clicked()
{
    connect(scanner_, &LkG5000::measurementReady,
            this, &Dialog::displayData, Qt::UniqueConnection);

    if (ui->measureBtn->text() == "Stop") {
        scanner_->autoMeasure(0);
        ui->measureBtn->setText("Measure");
    }
    else if (ui->autoCheckBox->isChecked()) {
        display_mode_ = AutoMeasurement;
        scanner_->autoMeasure(ui->measurementCycle->value());

        ui->measureBtn->setText("Stop");
    }
    else {
        display_mode_ = Measurement;    // For the correct decoding of the display data
        scanner_->measure();
    }
}

void Dialog::on_sendCommandBtn_clicked()
{
    QByteArray cmd = ui->keyenceCommand->text().toLatin1();
	connect(scanner_, &LkG5000::dataReady, this, &Dialog::displayData, Qt::UniqueConnection);
    display_mode_ = Data;   // For the correct decoding of the display data

	scanner_->writeCommand(cmd);
}
