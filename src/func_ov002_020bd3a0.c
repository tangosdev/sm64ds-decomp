// NONMATCHING: different op / idiom (div=16). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
extern unsigned short ReadUnalignedUshort(unsigned char* p);
extern unsigned short KS_FRAME_COUNTER;
extern unsigned char data_020a0e40;
extern unsigned short data_0209f49e;
extern unsigned short data_0209f49c;
int func_ov002_020bd3a0(char* p0, unsigned char* p1, unsigned int p2){
  unsigned short val = ReadUnalignedUshort(p1);
  if (p2 == KS_FRAME_COUNTER) {
    *(unsigned short*)((char*)&data_0209f49e + *(unsigned char*)(p0+0x6d8)*0x18) =
        val | *(unsigned short*)((char*)&data_0209f49e + data_020a0e40*0x18);
    *(unsigned short*)((char*)&data_0209f49c + *(unsigned char*)(p0+0x6d8)*0x18) =
        val | *(unsigned short*)((char*)&data_0209f49c + data_020a0e40*0x18);
  }
  return 1;
}
