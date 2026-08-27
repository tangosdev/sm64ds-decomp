unsigned short ReadUnalignedUshort(unsigned char *p){
return (unsigned short)(p[0] | (p[1]<<8));
}
