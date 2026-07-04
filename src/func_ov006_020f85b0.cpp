//cpp
struct Node {
    virtual void f0();
    Node* next;
    char pad[0x18];
    int field20;
};
extern "C" {
void func_ov006_020c0aa8(char* p);
void func_ov004_020b1bc8(char* a0, int a1, int a2, int a3);
void func_ov004_020b6430(void);
void func_ov004_020b1e34(char* a0, int a1, int a2, int a3);
void func_ov006_020f8540(char* p);
void func_ov006_020c1804(char* p);
}
extern Node* data_ov006_02142504;

extern "C" int func_ov006_020f85b0(char* c)
{
    func_ov006_020c0aa8(c + 0x4660);
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    {
        Node* n = data_ov006_02142504;
        int i = 0;
        for (;;) {
            if (n == 0) break;
            if (n->field20 > 0) n->f0();
            i++;
            n = n->next;
            if (i >= 0x14) break;
        }
    }
    {
        int i = 0;
        Node* n = data_ov006_02142504;
        for (;;) {
            if (n == 0) break;
            if (n->field20 == 0) n->f0();
            i++;
            n = n->next;
            if (i >= 0x14) break;
        }
    }
    func_ov006_020f8540(c);
    func_ov006_020c1804(c + 0x4f38);
    return 1;
}
