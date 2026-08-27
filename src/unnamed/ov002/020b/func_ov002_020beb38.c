extern unsigned char data_020a0e40;
extern unsigned short data_0209f49e;
extern unsigned short data_020a0e5a;
extern int AngleDiff(int a, int b);
int func_ov002_020beb38(char* c){
  unsigned char idx = data_020a0e40;
  unsigned char count = 0;
  unsigned short f = *(unsigned short*)((char*)&data_0209f49e + idx*0x18);
  if (f & 1) count++;
  if (f & 2) count++;
  if (f & 0x400) count++;
  if (f & 0x800) count++;
  if (*(unsigned short*)((char*)&data_020a0e5a + idx*4) & 0xf0) count++;
  if (AngleDiff(*(short*)(c+0x6d4), *(short*)(c+0x6d2)) >= 0x4000) count += 8;
  return count;
}
