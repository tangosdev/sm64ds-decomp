typedef unsigned short u16;
extern u16 data_02099fcc;
extern u16 data_020a6084;
void _ZN2GX15SetGraphicsModeEiii(int a, int b, int c){
 u16 t = data_02099fcc;
 unsigned reg = *(volatile unsigned*)0x4000000;
 data_020a6084 = (u16)a;
 if(t==0) a=0;
 reg = (unsigned)b | ((reg & 0xfff0fff0) | ((unsigned)a<<16));
 reg = ((unsigned)c<<3) | reg;
 *(volatile unsigned*)0x4000000 = reg;
 if(data_020a6084==0) data_02099fcc=0;
}
