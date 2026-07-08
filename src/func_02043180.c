extern int ACTOR_PARAM1;
extern short ACTOR_ACTOR_ID;
extern unsigned char ACTOR_UNKNOWN12_ID;
extern int data_020a4b64;

void func_02043180(short a, int b, int c, unsigned char d) {
    ACTOR_PARAM1 = c;
    ACTOR_ACTOR_ID = a;
    ACTOR_UNKNOWN12_ID = d;
    data_020a4b64 = b;
}
