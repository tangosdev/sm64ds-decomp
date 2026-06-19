extern void func_020566dc(const void *src, unsigned int offset, unsigned int count);
extern void func_02056674(const void *src, unsigned int offset, unsigned int count);
extern unsigned char data_0209e660;
extern int data_0209e674[];
extern int data_0209ea74[];
void _ZN3OAM4LoadEv(void){
  if (data_0209e660){
    func_020566dc(data_0209e674, 0, 0x400);
    return;
  }
  if (((*(unsigned short*)0x4000304 & 0x8000) >> 15) == 1){
    func_020566dc(data_0209e674, 0, 0x400);
    func_02056674(data_0209ea74, 0, 0x400);
  } else {
    func_020566dc(data_0209ea74, 0, 0x400);
    func_02056674(data_0209e674, 0, 0x400);
  }
}
