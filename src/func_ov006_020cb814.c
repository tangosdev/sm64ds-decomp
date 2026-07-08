typedef struct { int a, b; } Pair2;
extern Pair2 data_ov006_0213b1c4;
extern void func_ov006_020cb72c(char* c);

void func_ov006_020cb814(char* c){
    *(Pair2*)(c + 0x64) = data_ov006_0213b1c4;
    func_ov006_020cb72c(c);
}
