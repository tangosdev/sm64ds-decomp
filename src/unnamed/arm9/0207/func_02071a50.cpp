/* EH runtime: read an unsigned variable-length integer from an exception
   table and return the byte after it. The low bits of the first byte give
   the length: bit 0 clear is one byte (value >> 1), bit 1 clear two bytes
   (14 bits), bit 2 clear three bytes, otherwise four. ReadSignedVarInt is
   the signed sibling. */
unsigned char *func_02071a50(unsigned char *p, int *out)
{
    unsigned int b0 = p[0];
    if ((b0 & 1) == 0) {
        *out = b0 >> 1;
        return p + 1;
    }
    {
        unsigned int b1 = p[1];
        if ((b0 & 2) == 0) {
            *out = b1 | ((b0 >> 2) << 8);
            return p + 2;
        }
        {
            unsigned int b2 = p[2];
            if ((b0 & 4) == 0) {
                *out = ((b0 >> 3) << 16) | (b1 << 8) | b2;
                return p + 3;
            }
            {
                unsigned int b3 = p[3];
                *out = ((b0 >> 3) << 24) | (b1 << 16) | (b2 << 8) | b3;
                return p + 4;
            }
        }
    }
}
