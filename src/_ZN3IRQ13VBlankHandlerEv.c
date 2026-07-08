// NONMATCHING: base materialization / addressing (div=5). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.

typedef unsigned int u32;
typedef unsigned char u8;
extern int data_0209d514;
extern int GAME_SPEED_RELATED;
extern u8 data_0209d4f0;
extern int data_0209d500;
extern int data_0209d4fc;
extern u32 data_023c0000[];
extern volatile u32 reg[];
extern char base[];
extern void func_0205807c(int *p);
extern void func_02019144(void);
extern void func_02019100(void);
void _ZN3IRQ13VBlankHandlerEv(void)
{
  int *new_var;
  data_0209d514 = data_0209d514 + 1;
  if ((data_0209d514 >= GAME_SPEED_RELATED) && (data_0209d4f0 != 0))
  {
    func_0205807c(&data_0209d500);
    data_0209d514 = 0;
  }
  new_var = &data_0209d4fc;
  func_02019144();
  func_0205807c(new_var);
  reg[((0x3ff8 & 0xFF) & 0xFF) / 4] = reg[0x3ff8 / 4] | 1;
  func_02019100();
}
