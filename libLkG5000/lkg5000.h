#ifndef LKG5000_H
#define LKG5000_H

#include <QMap>
#include <QObject>
#include <QSerialPort>
#include <QTime>
#include <QVector>

class LkG5000 : public QObject
{
    Q_OBJECT

public:
    LkG5000(int headNO = 1, QObject* parent = 0);
    ~LkG5000();
    
    static const char CR = (char)0x0D;

public slots:
    /// <summary> Calls the measure() function with given interval.</summary>
    /// <param name="msec"> Interval for measure to be called in milliseconds. </param>
    void autoMeasure(uint msec);
    void close();
    /// <summary> Gets error code returned by the serial port. See Qt SerialPortError enumeration
    /// for details about the error codes. </summary>
    QSerialPort::SerialPortError getErrorCode() const;
    /// <summary> Gets the list of time stamps and the measured values as a reference. </summary>
    /// <return> List of time stamps and the measured values. </return>
    QMap<int, double>& getMeasuredValues();
    /// <summary> Gets the raw response of the last valid command. Raw responses to the implemented
    /// functions are not recorded in this list. See the summaries of related commands.
    /// e.g. measure(); ---> getMeasuredValue();</summary>
    /// <return> Raw response from the LK-G5000 controller. </return>
    QString getResponse();
    /// <summary> Sends the measurement command and records the time.
    /// </summary>
    void measure();
    bool open();
    /// <summary> Writes a command described in communication protocol and adds CR at the end of
    /// it. This command should NOT BE CALLED DURING AUTO-MEASUREMENT!. </summary>
    /// <param name="command"> Any command without CR at the end of it </param>
    void writeCommand(QByteArray command);
    void setPortName(QString port);
    void setBaudRate(qint32 baudRate = QSerialPort::Baud115200);

protected:
    /// <summary> Periodically called when the Timer is started by autoMeasure
    /// </summary>
    void timerEvent(QTimerEvent *event);

private slots:
    void recordMeasuredValueOutput();
    void readData();

signals:
    void errorOccurred();
	/// <summary> Emitted when serial port reads a response until CR. Then the response can be
    /// read safely by connecting a slot. </summary>
    void dataReady();
    /// <summary> Emitted when the measurement response is recorded to the log with the time when
    /// the measurement command is sent. </summary>
    void measurementReady();

private:
    QByteArray head_no_;    // The position of the scanner socket on the controller.
    QMap<int, double> measured_values_;     // Log of the measurement times and values.
    QVector<int> measurement_keys_to_record_;   // Order of the measurements to be recorded.
	QVector<QString> responses_;    // List of raw responses to the last commands.
	QSerialPort* serial_;   // Serial communication with the LK-G5000 controller
};

// Make the type known to all template-based functions and in direct signal-slot connections
Q_DECLARE_METATYPE(LkG5000*)
// Add the following line in main() function to register the class for using in signal-slots:
//      qRegisterMetaType<LkG5000*>("LkG5000*");
#endif // LKG5000_H
