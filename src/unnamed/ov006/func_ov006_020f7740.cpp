//cpp
struct Node {
    virtual void v0();
    virtual void v1(int);
    Node* prev;      // 0x04
    Node* next;      // 0x08
    char pad1[0x2a - 0x0c];
    short f2a;       // 0x2a
    char pad2[0x2d - 0x2c];
    unsigned char f2d;  // 0x2d
};

extern "C" {
extern Node* data_ov006_021424fc;
extern Node* data_ov006_02142500;
extern Node* data_ov006_02142504;
extern Node* data_ov006_02142508;
extern short data_ov006_021424ec;
extern int data_ov006_0213d570;
extern int data_ov006_0213d574;
extern void _Z14ApproachLinearRiii(int& x, int t, int s);
}

extern "C" void func_ov006_020f7740(void) {
    Node* head = data_ov006_02142500;
    Node* n;
    short i;

    if (data_ov006_021424fc != 0) {
        if (data_ov006_02142508 != 0) {
            if (--data_ov006_021424ec == 0) {
                if (data_ov006_021424fc->f2a > data_ov006_02142508->f2a)
                    data_ov006_0213d574 = data_ov006_02142508->f2a << 12;
                else
                    data_ov006_0213d574 = data_ov006_021424fc->f2a << 12;

                {
                    Node* n1 = data_ov006_021424fc;
                    if (data_ov006_02142500 == n1) data_ov006_02142500 = n1->next;
                    if (data_ov006_02142504 == n1) data_ov006_02142504 = n1->prev;
                    if (n1->prev != 0) n1->prev->next = n1->next;
                    if (n1->next != 0) n1->next->prev = n1->prev;
                    n1->next = 0;
                    n1->prev = n1->next;
                }
                {
                    Node* n2 = data_ov006_02142508;
                    if (data_ov006_02142500 == n2) data_ov006_02142500 = n2->next;
                    if (data_ov006_02142504 == n2) data_ov006_02142504 = n2->prev;
                    if (n2->prev != 0) n2->prev->next = n2->next;
                    if (n2->next != 0) n2->next->prev = n2->prev;
                    n2->next = 0;
                    n2->prev = n2->next;
                }

                data_ov006_02142508 = 0;
                data_ov006_021424fc = 0;
                _Z14ApproachLinearRiii(data_ov006_0213d570, 0, 2);
            }
            n = head;
            for (i = 0; i < (data_ov006_0213d574 >> 12); i++) {
                if (n == 0) return;
                if (n->f2d == 1) n->v1(i);
                n = n->next;
            }
            return;
        }
    }

    {
        int t = (short)data_ov006_0213d570;
        if (t > 0x14) t = 0x14;
        _Z14ApproachLinearRiii(data_ov006_0213d574, t << 12, 0x800);
    }
    n = head;
    for (i = 0; i < (data_ov006_0213d574 >> 12); i++) {
        if (n == 0) return;
        n->v1(i);
        n = n->next;
    }
}
