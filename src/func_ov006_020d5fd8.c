typedef struct { char pad[0xa8]; short f; } BrEnt5fd8;
void func_ov006_020d5fd8(int c, int i){
  BrEnt5fd8 *p = (BrEnt5fd8*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
