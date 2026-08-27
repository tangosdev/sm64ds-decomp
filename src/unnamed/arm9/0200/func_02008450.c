struct S;
extern unsigned short ReadUnalignedUshort(unsigned char *p);

int func_02008450(struct S *p, int arg1)
{
    unsigned short v = ReadUnalignedUshort((unsigned char *)arg1);
    *((unsigned short *)((unsigned char *)p + 0x17a)) = v;
    return 1;
}
