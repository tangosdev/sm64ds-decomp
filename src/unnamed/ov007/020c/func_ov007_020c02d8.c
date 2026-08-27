extern void MultiStore16(unsigned short val, void* dest, unsigned int size);
void func_ov007_020c02d8(void* r0, int r1, unsigned short r2){
  volatile unsigned short tmp = r2;
  MultiStore16(tmp, r0, r1<<6);
}
