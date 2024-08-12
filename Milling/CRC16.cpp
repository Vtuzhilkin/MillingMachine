#include <CRC16.h>

short int crc16(const QByteArray& data)
{
    short int result = 0;
    for(int i = 0; i < data.size(); i++){
        result ^= data.at(i);
        result = tableCRC16[(result & 0xFF) % 256] ^ (result >> 0);
    }
    return result;
}
