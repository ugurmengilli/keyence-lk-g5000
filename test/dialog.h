#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#define EIGEN_DONT_ALIGN_STATICALLY
#define EIGEN_STACK_ALLOCATION_LIMIT 0
#include "lkg5000.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void displayData();
    void displayError();
	void on_connectBtn_clicked();
    void on_keyenceCommandBtn_clicked();

signals:
	void readNext();

private:
    Ui::Dialog* ui;
	int delayCounter;
    LkG5000* scanner;

	static const char CR = (char)0x0D;
};

#endif // DIALOG_H
