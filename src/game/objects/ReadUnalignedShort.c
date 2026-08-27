short ReadUnalignedShort(unsigned char *p){
return (short)(p[0] | (p[1]<<8));
}
