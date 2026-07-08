extern int func_ov002_020caf68(void *c);
extern int _ZN6Player8ST_CLIMBE[];
extern int _ZN6Player12ST_HEADSTANDE[];
int _ZN6Player16St_Climb_CleanupEv(void *c){
  int v = *(int*)((char*)c+0x378);
  if(v != (int)_ZN6Player8ST_CLIMBE && v != (int)_ZN6Player12ST_HEADSTANDE){
    func_ov002_020caf68(c);
    *(unsigned short*)((char*)c+0x600+0xb6)=8;
  }
  *(unsigned short*)((char*)c+0x600+0xa8)=0;
  return 1;
}
