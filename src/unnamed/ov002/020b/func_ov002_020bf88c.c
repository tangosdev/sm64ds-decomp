extern void func_02022b04(int x, int y, int z);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);
extern int data_0208e430;
void func_ov002_020bf88c(char* c){
    if (*(unsigned char*)(c + 0x707)) return;
    if (data_0208e430 == 0x2e) return;
    if (*(unsigned char*)(c + 0x703))
        func_02022b04(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
    else
        _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
}
