typedef unsigned short u16;
extern u16 data_02099fcc;
extern u16 data_020a6084;
void _ZN2GX6DispOnEv(void){
 data_02099fcc = 1;
 u16 t = data_020a6084;
 if(t!=0){
   volatile unsigned *r = (volatile unsigned*)0x4000000;
   *r = (*r & ~0x30000u) | ((unsigned)t<<16);
   return;
 }
 *(volatile unsigned*)0x4000000 |= 0x10000;
}
