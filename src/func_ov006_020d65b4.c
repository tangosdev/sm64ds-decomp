typedef struct { char pad[0x68]; short f; } BrEnt65b4;
void func_ov006_020d65b4(int c, int i){
  BrEnt65b4 *p = (BrEnt65b4*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
