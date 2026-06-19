typedef unsigned short u16;
extern unsigned char data_0209f2d8;
extern int data_0209fc68;
extern unsigned char data_0209f2bc;
extern u16 data_ov002_02111188;
extern unsigned char data_0209f204;
int _ZN5Stage8CanPauseEv(void){
    if (((unsigned)(data_0209f2d8 == 1)) == 0) return 1;
    if (data_0209fc68 != 0) return 0;
    if (data_0209f2bc != 0 || (data_ov002_02111188 == 0 && data_0209f204 != 0)) return 0;
    return 1;
}
