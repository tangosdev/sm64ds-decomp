extern void _Z14ApproachLinearRsss(short *a, short b, short c);
extern void _ZN9Animation7AdvanceEv(void *);
extern void func_ov077_02124eb0(void *c);
extern void _ZN8dActor_c8PoofDustEv(void *);
extern void func_02012694(int a, void *b);
extern void _ZN7fBase_c18MarkForDestructionEv(void *);
extern void _ZN8dActor_c9UpdatePosEP5dCc_c(void *c, void *clsn);
extern void func_ov077_02124d08(void *c, void *p);
extern void _ZN5dCc_c5ClearEv(void *c);
extern void _ZN5dCc_c6UpdateEv(void *c);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern void func_ov077_02125e94(void *c, int a);

int func_ov077_02125a54(char *c){
  int d;
  _Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x3e6), 0x64);
  *(short*)(c + 0x94) = *(short*)(c + 0x8e);
  _ZN9Animation7AdvanceEv(c + 0x174);
  func_ov077_02124eb0(c);
  if(*(int*)(c + 0x3dc))
    d = *(int*)(c + 0x60) - *(int*)(c + 0x3dc);
  else
    d = 0;
  if(d < -0xc8000){
    _ZN8dActor_c8PoofDustEv(c);
    func_02012694(0x166, c + 0x74);
    _ZN7fBase_c18MarkForDestructionEv(c);
  }
  _ZN8dActor_c9UpdatePosEP5dCc_c(c, c + 0x1b0);
  func_ov077_02124d08(c, c + 0x1e4);
  _ZN5dCc_c5ClearEv(c + 0x1b0);
  _ZN5dCc_c6UpdateEv(c + 0x1b0);
  if(DecIfAbove0_Byte((unsigned char*)(c + 0x3e8)) == 0)
    func_ov077_02125e94(c, 1);
  return 1;
}
