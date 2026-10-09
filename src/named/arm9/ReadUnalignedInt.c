unsigned int ReadUnalignedInt(unsigned char *p){
return p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}
