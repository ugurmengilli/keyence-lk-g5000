#include "lkg5000.h"

LkG5000::LkG5000(int headNO, QObject* parent) :
    QObject(parent)
{
    // Set serial port
    serial_ = new QSerialPort(this);
    serial_->setDataBits(QSerialPort::Data8);
    serial_->setParity(QSerialPort::NoParity);
    serial_->setStopBits(QSerialPort::OneStop);
    connect(serial_, &QSerialPort::errorOccurred, this, &LkG5000::errorOccurred);
    connect(serial_, &QSerialPort::readyRead, this, &LkG5000::readData);
    // Assign empty values:
    head_no_ = QByteArray::number(0).append(QByteArray::number(headNO));
    measurement_keys_to_record_.reserve(10);
    responses_.reserve(150);    // some space for faster allocation.
}

LkG5000::~LkG5000()
{
}

void LkG5000::autoMeasure(uint msec)
{
    static int timer_id = -1;
    responses_.clear();     // Before starting a measurement series since it would corrupt the
    // measurement log by reading the first item in this list.
    if (msec)
        timer_id = startTimer(msec, Qt::TimerType::PreciseTimer);
    else
        killTimer(timer_id);
}

void LkG5000::close()
{
    serial_->close();
}

QSerialPort::SerialPortError LkG5000::getErrorCode() const
{
    return serial_->error();
}

QMap<int, double>& LkG5000::getMeasuredValues()
{
    return measured_values_;
}

QString LkG5000::getResponse()
{
    if (responses_.isEmpty())
        return QString();
    return responses_.takeFirst();
}

void LkG5000::measure()
{
    // After sending the measurement command, record the measured value using the event loop. When
    // the signal dataReady is emitted, it is guarateed that the controller responded to the command.
    connect(this, &LkG5000::dataReady,
            this, &LkG5000::recordMeasuredValueOutput, Qt::UniqueConnection);

    QByteArray cmd_measured_value_output =  // Command structure
        QByteArray("MS,").append(head_no_).append(CR);
    serial_->write(cmd_measured_value_output);

    // After writing the command, record the current time as milliseconds. The measurement
    // value is invalid by default. If measurement is succesful, the correct value is written using
    // the corresponding key.
    measured_values_.insert(QTime::currentTime().msecsSinceStartOfDay(), 100);
    // Add the key of the measurement to the queue for recording in the correct order even if
    // another measurement command is sent before the response of the previous one is recorded.
    measurement_keys_to_record_.append(measured_values_.lastKey());

}

bool LkG5000::open()
{
    return serial_->open(QSerialPort::ReadWrite);
}

void LkG5000::setBaudRate(qint32 baudRate)
{
    serial_->setBaudRate(baudRate);
}

void LkG5000::setPortName(QString port)
{
    serial_->setPortName(port);
}

void LkG5000::writeCommand(QByteArray command)
{
    disconnect(this, &LkG5000::dataReady, this, &LkG5000::recordMeasuredValueOutput);
    serial_->write(command.append(CR));
}

void LkG5000::readData()
{
    // For long responses or due to other lags, it may not be possible to read the full response.
    // Local static variable will store any partial response from previous calls to this function.
    static QString current_response = QString("");
    QByteArray data = serial_->readAll();
    // Try to split the data by data delimiter (CR) defined in communication protochol. If CR is not
    // found, split_data has 1 item. If CR is found, split_data has 2/multiple items where the 
    // 2nd/last one is empty string or partial response of a queued command.
    QList<QByteArray> split_data = data.split(CR);
    
    // If there is 1 element, the response is not complete. Skip for-loop.
    for (int i = 0; i < split_data.count() - 1; i++)
    {
        // - If current_response is not empty, it is an incomplete response from the previous call 
        // to readData(). Then the first item in split_data completes it. This is assured since 
        // 1-item split_data skips the loop. Append the first item to current_response.
        // - If current_response is empty, the first item is a complete response and we can safely
        // append the item to it.
        current_response.append(split_data[i]);
        responses_.append(current_response);
        emit dataReady();
        // Clean current_response. If there are multiple items, the next complete response will be
        // appended to current_response. This loop skips the last item since it is incomplete or 
        // empty due to split(). Therefore, it won't be appended to the responses_.
        current_response = QString("");
    }
    // - If for-loop is skipped, the only item in split_data is an incomplete response. The count
    // is 1 and index is 0, so append (count - 1)th = 0th item to current_response for next call to
    // readData().
    // - If for-loop is processed, then the last element is an incomplete response and current_response
    // is empty. Then, append (count - 1)th element to empty current_response.
    current_response.append(split_data[split_data.count() - 1]);
}

void LkG5000::recordMeasuredValueOutput()
{
    bool measurement_OK = false;
    // Take the first unhandled response from the list and convert to double. Measurement mode
    // does not allow configuration commands, which ensures that the unhandled response is a 
    // measurement response. Therefore, header check for the response can be skipped.
    double measured_value = responses_.takeFirst().right(8).toDouble(&measurement_OK);
    if (measurement_OK)     // replace the measured value for the next measurement call.
        measured_values_.insert(measurement_keys_to_record_.takeFirst(), measured_value);

    emit measurementReady();
}

void LkG5000::timerEvent(QTimerEvent *event)
{
    return measure();
}