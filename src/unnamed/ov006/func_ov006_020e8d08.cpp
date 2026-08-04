//cpp
struct C70;
typedef void (C70::*PMF)(int);
extern PMF data_ov006_02141f74[];

struct C70 {
    void run() {
        int i;
        char* s = (char*)this;
        for (i = 0; i < 3; i++) {
            if (*(unsigned char*)(s + 0x5290)) {
                (this->*data_ov006_02141f74[*(unsigned char*)(s + 0x5291)])(i);
            }
            s += 0x14;
        }
    }
};

extern "C" void func_ov006_020e8d08(C70* c) { c->run(); }
