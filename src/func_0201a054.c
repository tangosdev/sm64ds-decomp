typedef unsigned short u16;
typedef unsigned int u32;

extern void func_020427f8(void);
extern u32 func_02042784(void);
extern void func_02053a8c(void);
extern void func_02053c40(void);
extern void func_0205b858(void);
extern void func_0203d740(void);
extern void func_02013e64(void);
extern void func_0201a4e4(void);
extern void _ZN3IRQ10EnableIRQsEj(u32 m);
extern void _ZN3IRQ6EnableEv(void);
extern int func_02053be0(int enable);
extern void func_0203bbc0(void);
extern void func_0203bb5c(void);
extern void func_0201fec8(void);
extern void func_02042f68(int a0, void* src);
extern void func_02018aa4(void);
extern void _ZN3IRQ11DisableIRQsEj(u32 m);
extern void func_0203ad84(void);
extern void func_0201a9fc(void* c);
extern void _ZN4Heap18InitializeGameHeapEjPS_(u32 sz, void* p);
extern void func_02019440(void);
extern void func_020134c8(void);
extern void func_0203b684(void);
extern void func_020233f0(void);
extern void _ZN5Scene18PrepareToSpawnBootEv(void);
extern void func_020196cc(void);
extern void func_0201a5cc(void);

extern char data_0208ee50[];
extern char data_0209d574[];
extern char data_02090864[];
extern void* data_020a4bb8;
struct S0209ee90 { char pad[0x44]; u32 f44; };
extern struct S0209ee90 data_0209ee90;

void func_0201a054(void){
  int r4 = (*(u16*)0x27ffc40 == 2);
  func_020427f8();
  func_02042784();
  func_02053a8c();
  func_02053c40();
  func_0205b858();
  func_0203d740();
  func_02013e64();
  func_0201a4e4();
  _ZN3IRQ10EnableIRQsEj(1);
  {
    volatile u16* p = (volatile u16*)0x4000208;
    u16 old = *p;
    *p = 1;
  }
  _ZN3IRQ6EnableEv();
  func_02053be0(1);
  func_0203bbc0();
  func_0203bb5c();
  if (!r4)
    func_0201fec8();
  func_02042f68(0xd01, data_0208ee50);
  if (!r4)
    func_02018aa4();
  if (r4)
    _ZN3IRQ11DisableIRQsEj(0x100000);
  if (!r4)
    func_0203ad84();
  func_0201a9fc(data_0209d574);
  _ZN4Heap18InitializeGameHeapEjPS_(0x3b000, 0);
  func_0201a9fc(data_0209d574);
  func_02019440();
  if (!r4)
    func_020134c8();
  func_0203b684();
  data_020a4bb8 = data_02090864;
  func_020233f0();
  _ZN5Scene18PrepareToSpawnBootEv();
  func_020196cc();
  data_0209ee90.f44 = 0x100;
  *(volatile u32*)0x40004c8 = 0x296a5800;
  *(volatile u32*)0x40004cc = 0x7fff;
  *(volatile u32*)0x40004c0 = 0x7fff;
  *(volatile u32*)0x40004c4 = 0;
  func_0201a5cc();
}
