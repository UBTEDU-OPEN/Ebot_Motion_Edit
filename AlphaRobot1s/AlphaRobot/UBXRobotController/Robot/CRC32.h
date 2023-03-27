#ifndef CRC32_H
#define CRC32_H

#define CRC32_INIT_VALUE 0xFFFFFFFF

unsigned int crc32_standard(unsigned char * buffer, unsigned int size);
unsigned int crc32_custom(unsigned int crc, unsigned char *buffer, unsigned int size);

#endif