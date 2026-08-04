//cpp
struct C71;
typedef void (C71::*PMF)(int);
extern PMF data_ov006_02141f5c[];

struct C71 {
    void run() {
        int i;
        char* s = (char*)this;
        for (i = 0; i < 5; i++) {
            if (*(unsigned char*)(s + 0x5218)) {
                (this->*data_ov006_02141f5c[*(unsigned char*)(s + 0x5219)])(i);
            }
            s += 0x18;
        }
    }
};

extern "C" void func_ov006_020e9374(C71* c) { c->run(); }
