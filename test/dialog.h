#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include "lkg5000.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

    enum DisplayMode
    {
        Data,
        Measurement,
        AutoMeasurement,
    };
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void displayData();
    void displayError();
	void on_connectBtn_clicked();
    void on_measureBtn_clicked();
    void on_sendCommandBtn_clicked();

signals:
	void readNext();

private:
    Ui::Dialog* ui;
    DisplayMode display_mode_;
    LkG5000* scanner_;

	static const char CR = (char)0x0D;
};

#endif // DIALOG_H
