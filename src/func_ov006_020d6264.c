typedef struct { char pad[0x88]; short f; } BrEnt6264;
void func_ov006_020d6264(int c, int i){
  BrEnt6264 *p = (BrEnt6264*)(c + (i<<4) + 0x6200);
  p->f = 0;
}
