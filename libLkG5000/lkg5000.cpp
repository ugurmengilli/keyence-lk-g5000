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
    QByteArray head_no = QByteArray::number(0).append(QByteArray::number(headNO));
    responses_.empty();
}

LkG5000::~LkG5000()
{
}

void LkG5000::clearResponses()
{
    responses_.clear();
}

void LkG5000::close()
{
    serial_->close();
}

QSerialPort::SerialPortError LkG5000::getErrorCode() const
{
    return serial_->error();
}

const QList<QString>& LkG5000::getResponses() const
{
    return responses_;
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
        current_response = QString("");	// Clean current_response. If there are multiple items, the
        // next complete response will be appended to current_response. This loop skips the last
        // item since it is incomplete or empty due to split(). Therefore, it won't be appended to
        // the responses_.
    }
    // - If for-loop is skipped, the only item in split_data is an incomplete response. The count
    // is 1 and index is 0, so append (count - 1)th = 0th item to current_response for next call to
    // readData().
    // - If for-loop is processed, then the last element is an incomplete response and current_response
    // is empty. Then, append (count - 1)th element to empty current_response.
    current_response.append(split_data[split_data.count() - 1]);
}
