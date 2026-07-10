typedef int Fix12;
typedef short s16;
typedef unsigned short u16;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* c, void* cc);
extern void _ZN8Platform21UpdateModelPosAndRotYEv(char* c);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(char* c, Fix12 a, Fix12 b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(char* c);
extern u16 data_ov091_02134504[];

int _ZN17SlidingPlatformWf8BehaviorEv(char* c) {
  if (DecIfAbove0_Byte((unsigned char*)(c+0x31e)) == 0) {
    if (DecIfAbove0_Short((unsigned short*)(c+0x320)) == 0) {
      s16* a = (s16*)(((int)c + 0x94) & 0xffffffffffffffff);
      s16 v = data_ov091_02134504[*(unsigned char*)(c+0x322)];
      *(s16*)(c+0x320) = v;
      *a += 0x8000;
      *(unsigned char*)(c+0x31e) = 0xf;
    } else {
      _ZN5Actor9UpdatePosEP12CylinderClsn(c, 0);
    }
  }
  _ZN8Platform21UpdateModelPosAndRotYEv(c);
  if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0, 0) != 0) {
    _ZN8Platform19UpdateClsnPosAndRotEv(c);
  }
  return 1;
}
