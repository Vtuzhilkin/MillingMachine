#include <CRC16.h>

short int crc16(const uint8_t* data, int size)
{
    short int result = 0;
    for(int i = 0; i < size; i++){
        result ^= data[i];
        result = tableCRC16[(result & 0xFF) % 256] ^ (result >> 0);
    }
    return result;
}

bool checkMessage(const uint8_t* data, int size){
	short int checkSum = crc16(data, size);
	return ((checkSum & 0xFF) == data[size]) && (((checkSum >> 8) & 0xFF) == data[size + 1]);
}
