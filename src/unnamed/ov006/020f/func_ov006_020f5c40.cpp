//cpp
struct C77;
typedef void (C77::*PMF)(int);
extern PMF data_ov006_02142408[];

struct C77 {
    void run() {
        int i;
        char* s = (char*)this;
        for (i = 0; i < 0x14; i++) {
            if (*(unsigned char*)(s + 0x51bb)) {
                (this->*data_ov006_02142408[*(unsigned char*)(s + 0x51bc)])(i);
            }
            s += 0x18;
        }
    }
};

extern "C" void func_ov006_020f5c40(C77* c) { c->run(); }
