#include "Message.h"

Message::Message(unsigned char opCode)
    : operationCode(opCode), dataLength(0), data(), crc1(0), crc2(0) {
    calculateCRC();
}

Message::Message(unsigned char opCode, unsigned char length, const QVector<unsigned char> &inputData)
    : operationCode(opCode), dataLength(length), data(inputData), crc1(0), crc2(0) {
    calculateCRC();
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

void Message::calculateCRC() {
    uint16_t crc = 0xFFFF;

    crc = updateCRC(crc, operationCode);
    crc = updateCRC(crc, dataLength);

    for (unsigned char byte : data) {
        crc = updateCRC(crc, byte);
    }

    crc1 = crc & 0xFF;
    crc2 = (crc >> 8) & 0xFF;
}

uint16_t Message::updateCRC(uint16_t crc, unsigned char byte) {
    uint16_t tmp = crc ^ byte;
    crc = (crc >> 8) ^ tableCRC16[tmp & 0xFF];
    return crc;
}
