/* The +0x496 store goes through byte-pointer arithmetic: with `(int)c + 0x496`
   mwcc materializes the offset from the literal pool and stores with a register
   index (strh r3,[r0,r1]); the cartridge splits it as add r0,r0,#0x400 /
   strh r1,[r0,#0x96]. The old u64 masks on the other three accesses were not
   load-bearing and are gone. */
extern unsigned int data_0209b454;
void func_ov002_020e7104(char* c, int r1){
  if(r1==0){
    *(unsigned short*)(c + 0x4A2) &= ~0x100;
    if(data_0209b454 & 0x4000000) return;
    *(unsigned int*)(c + 0xB0) |= 0x4000000;
    data_0209b454 |= 0x4000000;
    *(unsigned short*)(c + 0x496) = 0x64;
    return;
  }
  *(unsigned short*)(c + 0x4A2) |= 0x100;
}
