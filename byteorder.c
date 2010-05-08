
void hcns(be4put)(char s[4], int i)
{
	
}

int hcns(be4get)(char s[4])
{
	return 0;
}


#define get_be32(p)	( \
	(*((unsigned char *)(p) + 0) << 24) | \
	(*((unsigned char *)(p) + 1) << 16) | \
	(*((unsigned char *)(p) + 2) <<  8) | \
	(*((unsigned char *)(p) + 3) <<  0) )

#define put_be32(p, v)	do { \
	unsigned int __v = (v); \
	*((unsigned char *)(p) + 0) = __v >> 24; \
	*((unsigned char *)(p) + 1) = __v >> 16; \
	*((unsigned char *)(p) + 2) = __v >>  8; \
	*((unsigned char *)(p) + 3) = __v >>  0; } while (0)
