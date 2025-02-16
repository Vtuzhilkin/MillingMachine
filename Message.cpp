#include "Message.h"
#include <QDebug>

Message::Message(unsigned char opCode)
    : operationCode(opCode), dataLength(0) {
    uint16_t crc = calculateCRC();
    crc1 = crc & 0xFF;
    crc2 = (crc >> 8) & 0xFF;
}

Message::Message(unsigned char opCode, unsigned char length, const QVector<unsigned char> &inputData)
    : operationCode(opCode), dataLength(length), data(inputData) {
    uint16_t crc = calculateCRC();
    crc1 = crc & 0xFF;
    crc2 = (crc >> 8) & 0xFF;
}

Message::Message(const QByteArray &byteArray): operationCode(0), dataLength(0), crc1(0), crc2(0)
{
    if (byteArray.size() < 4) {
        return;
    }

    operationCode = byteArray[0];
    dataLength = byteArray[1];

    if (byteArray.size() >= 2 + dataLength) {
        data.resize(dataLength);
        for (int i = 0; i < dataLength; ++i) {
            data[i] = byteArray[2 + i];
        }
    }

    if (byteArray.size() >= 2 + dataLength + 2) {
        crc1 = byteArray[2 + dataLength];
        crc2 = byteArray[3 + dataLength];
    }
}

QByteArray Message::toQByteArray() const {
    QByteArray result;
    result.append(operationCode);
    result.append(dataLength);
    for (unsigned char byte : data) {
        result.append(byte);
    }
    result.append(crc1);
    result.append(crc2);
    return result;
}

unsigned char Message::getCode() const
{
    return operationCode;
}

QVector<unsigned char> Message::getData() const
{
    return data;
}

uint16_t Message::calculateCRC() const {
    uint16_t crc = 0;

    crc = updateCRC(crc, operationCode);
    crc = updateCRC(crc, dataLength);

    for (unsigned char byte : data) {
        crc = updateCRC(crc, byte);
    }
    return crc;
}

uint16_t Message::updateCRC(uint16_t crc, uint8_t byte) const{
    crc ^= byte;
    crc = tableCRC16[(crc & 0xFF) % 256] ^ (crc >> 0);
    return crc;
}

bool Message::checkCRC() const
{
    if(operationCode == 0){
        return false;
    }

    uint16_t crc = calculateCRC();

    unsigned char calculatedCRC1 = crc & 0xFF;
    unsigned char calculatedCRC2 = (crc >> 8) & 0xFF;

    return calculatedCRC1 == crc1 && calculatedCRC2 == crc2;
}
