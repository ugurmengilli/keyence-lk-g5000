#ifndef LKG5000_H
#define LKG5000_H

#include <QMap>
#include <QObject>
#include <QSerialPort>
#include <QTime>

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
    /// <summary> Gets the list of time stamps and the measured values as a reference. </summary>
    /// <return> List of time stamps and the measured values. </return>
    QMap<int, double>& getMeasuredValues();
    /// <summary> Gets a list of raw responses sent by the LK-G5000. Raw responses to the measure()
    /// function are not recorded in this list. See getMeasuredValues for the records. </summary>
    const QList<QString>& getResponses() const;
    /// <summary> Sends the measurement command and records the time.
    /// </summary>
    void measure();
    bool open();
    /// <summary> Writes any command given by the protocol and adds CR at the end of it. </summary>
    /// <param name="command"> Any command without CR at the end of it </param>
    void writeCommand(QByteArray command);
    void setPortName(QString port);
    void setBaudRate(qint32 baudRate = QSerialPort::Baud115200);

private slots:
    void recordMeasuredValueOutput();
    void readData();

signals:
    void errorOccurred();
	/// <summary> Emitted when serial port reads a response until CR. Then the response can be
    /// read safely by connecting a slot. </summary>
    void dataReady();
    void measurementReady();

private:
    QByteArray head_no_;    // The position of the scanner socket on the controller.
    QMap<int, double> measured_values_;     // Log of the measurement times and values.
    QList<QString> responses_;      // List of raw responses to the last commands.
    QSerialPort* serial_;   // Serial communication with the LK-G5000 controller
};

// Make the type known to all template-based functions and in direct signal-slot connections
Q_DECLARE_METATYPE(LkG5000*)
// Add the following line in main() function to register the class for using in signal-slots:
//      qRegisterMetaType<LkG5000*>("LkG5000*");
#endif // LKG5000_H
