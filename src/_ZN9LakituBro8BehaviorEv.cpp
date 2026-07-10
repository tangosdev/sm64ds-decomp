//cpp
struct C;
typedef void (C::*PMF)();
struct State { char pad[8]; PMF fn; };
extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void*, void*);
extern void _ZN9Animation7AdvanceEv(void*);
extern int func_ov085_0212e858(void*);
extern void func_ov085_0212e778(void*);
}
extern State data_ov085_021307d0;
extern State data_ov085_021307e0;
struct C { char pad[0x10000]; };
extern "C" int _ZN9LakituBro8BehaviorEv(C* c){
  char* p=(char*)c;
  DecIfAbove0_Short((unsigned short*)(p+0x100));
  State* st=*(State**)(p+0x1ec);
  if(st->fn) (c->*st->fn)();
  _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
  _ZN9Animation7AdvanceEv(p+0x160);
  _ZN9Animation7AdvanceEv(p+0x1d8);
  if(*(State**)(p+0x1ec)==&data_ov085_021307d0){
    *(short*)(p+0x8c)=*(short*)(p+0x92);
    *(short*)(p+0x8e)=*(short*)(p+0x94);
    *(short*)(p+0x90)=*(short*)(p+0x96);
  }
  if(*(State**)(p+0x1ec)==&data_ov085_021307e0) return 1;
  if(*(int*)(p+0x2d0)==0) func_ov085_0212e858(c);
  else func_ov085_0212e778(c);
  return 1;
}
