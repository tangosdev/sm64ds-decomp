//cpp
#include "Sound.h"

extern "C" void func_0203da2c(int v);
extern void func_02030500(void);
extern "C" {

extern void func_02019a58(void);
}

extern "C" void func_020132d8(void);
extern "C" {

extern void func_0201ffcc(void);
extern void func_020199a4(void);
}

extern unsigned char data_02092778;
extern int data_0209fc68;
extern int data_0209277c;
extern unsigned char data_0209e64c;
extern unsigned char gSoundInitialized;

/* End the VS session on an error: session state data_0209fc68 becomes 6,
   which dScEntry_c::HandlePlayerDisconnect reads as a connection error. If
   a session was running (data_02092778, which d_s_entry_methods.cpp sets),
   drop to local link mode (func_0203da2c(0)), put the BG1 picture up on
   both screens (func_02030500), and with the link started (data_0209e64c)
   and gSoundInitialized set, play sound 0x123 and stop the loaded music.
   func_0201ffcc then runs one more link step with the watchdog held, and
   func_020199a4 follows. The state handlers func_02030b58
   and func_02030c38 and the error checks in func_02030e4c call it after
   setting their disconnect bits in data_0209fc58. */
extern "C" void func_02030790(void)
{
    data_0209fc68 = 6;
    if (data_02092778 == 0)
        return;
    func_0203da2c(0);
    func_02030500();
    data_0209277c = 0;
    func_02019a58();
    if (data_0209e64c != 0 && gSoundInitialized != 0) {
        func_02012790(0x123);
        Sound::StopLoadedMusic_Layer1(4);
        func_020132d8();
    }
    func_0201ffcc();
    func_020199a4();
}
