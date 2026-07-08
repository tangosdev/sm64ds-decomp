//cpp
typedef unsigned char u8;
extern "C" {
void func_02012d64(void);
void func_02013524(int *a, int b, int c);
void func_0204fa2c(int *a, int b);
int func_02013078(void);
void func_020494cc(void *p, int s);
void func_020490b0(void *p);
void func_0204f03c(void);
void func_020119c8(void *base);
}
void ApproachLinear(int &val, int target, int step);
extern u8 data_0209b480;
extern int data_0208e430;
extern signed char data_0209b470;
extern int data_0209b494;
extern int MESSAGE_SOUND_VOLUME_LSL_12;
extern int data_0209b4b0;
extern int _ZN5Sound7FileRef5PTR_0E;
extern int data_0209b53c;
extern "C" void func_020132d8(void) {
    if (data_0209b480 == 0) return;
    func_02012d64();
    if (data_0208e430 >= 0) {
        ApproachLinear(MESSAGE_SOUND_VOLUME_LSL_12, data_0209b470 << 0xc, data_0209b494);
        func_02013524(&data_0209b4b0, MESSAGE_SOUND_VOLUME_LSL_12 >> 0xc, 0);
        if (MESSAGE_SOUND_VOLUME_LSL_12 == 0) {
            data_0208e430 = -1;
            func_0204fa2c(&data_0209b4b0, 0);
        }
    }
    int s = func_02013078();
    if (s >= 0) {
        func_020494cc(&_ZN5Sound7FileRef5PTR_0E, s);
    }
    func_020490b0(&_ZN5Sound7FileRef5PTR_0E);
    func_0204f03c();
    func_020119c8(&data_0209b53c);
}
