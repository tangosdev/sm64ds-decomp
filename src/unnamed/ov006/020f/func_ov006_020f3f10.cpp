//cpp
struct C75;
typedef void (C75::*PMF)(int);
extern PMF data_ov006_02142334[];

struct C75 {
    void run() {
        int i;
        char* s = (char*)this;
        for (i = 0; i < 0xc; i++) {
            if (*(unsigned char*)(s + 0x51bb)) {
                (this->*data_ov006_02142334[*(unsigned char*)(s + 0x51bc)])(i);
            }
            s += 0x18;
        }
    }
};

extern "C" void func_ov006_020f3f10(C75* c) { c->run(); }
