#ifndef LKG5000_H
#define LKG5000_H

#include <QObject>
#include <QSerialPort>

class LkG5000 : public QObject
{
    Q_OBJECT

public:
    LkG5000(int headNO = 1, QObject* parent = 0);
    ~LkG5000();
    
    static const char CR = (char)0x0D;

public slots:
    void clearResponses();
    void close();
    /// <summary> Gets error code returned by the serial port. See Qt SerialPortError enumeration
    /// for details about the error codes. </summary>
    QSerialPort::SerialPortError getErrorCode() const;
    /// <summary> Gets a list of raw responses sent by the LK-G5000.
    /// </summary>
    const QList<QString>& getResponses() const;
    bool open();
    /// <summary> Writes any command given by the protocol and adds CR at the end of it. </summary>
    /// <param name="command"> Any command without CR at the end of it </param>
    void writeCommand(QByteArray command);
    void setPortName(QString port);
    void setBaudRate(qint32 baudRate = QSerialPort::Baud115200);

private slots:
    void readData();

signals:
    void errorOccurred();
	/// <summary> Emitted when serial port reads a response until CR. Then the response can be
    /// read safely by connecting a slot. </summary>
    void dataReady();

private:
    QList<QString> responses_;  // list of raw responses to the last commands
    QSerialPort* serial_;
};

// Make the type known to all template-based functions and in direct signal-slot connections
Q_DECLARE_METATYPE(LkG5000*)
// Add the following line in main() function to register the class for using in signal-slots:
//      qRegisterMetaType<LkG5000*>("LkG5000*");
#endif // LKG5000_H
