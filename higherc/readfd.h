#ifndef tid0310e98cb332k1vz5c8cnc4411diib1i62vgatsf9ic /* higherc-readfd-h */
#define tid0310e98cb332k1vz5c8cnc4411diib1i62vgatsf9ic /* higherc-readfd-h */

/* readfd has a buffer of 0x10000 (65536 bytes)
 */
int hcns(readfd)(int fd, int (*doit)(const char *buf, int len, hcns(bool) eof));

#endif
