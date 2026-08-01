int func_ov100_02145014(void);
int _ZN6Player12GetTalkStateEv(void* p);
int func_ov100_021452e4(char* a, char* b);
void func_ov100_021453d8(char* c, void* p, char* a2);

extern char data_ov100_02148924[];

int func_ov100_02144c6c(char* r0, char* r1)
{
    func_ov100_02145014();
    if (_ZN6Player12GetTalkStateEv(r1) == -1) {
        if (func_ov100_021452e4(r0, r1) == 0) {
            func_ov100_021453d8(r0, data_ov100_02148924, r1);
        }
    }
    return 1;
}
