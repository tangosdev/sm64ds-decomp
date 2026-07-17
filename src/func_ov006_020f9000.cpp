//cpp
typedef short s16;
typedef unsigned char u8;

struct Node {
    virtual void m0();
    virtual void m1(s16 i);
    Node* prev;
    Node* next;
    char pad_c[0x1e];
    s16 key;
    char pad_2c;
    u8 flag;
};

extern "C" void func_ov004_020adb1c(int a);
extern "C" void func_ov006_020f9f40(Node* c, s16 n);

void ApproachLinear2(s16& r, s16 a, s16 b);
void ApproachLinear(int& r, int a, int b);

extern Node* data_ov006_02142570;
extern Node* data_ov006_02142574;
extern Node* data_ov006_0214257c;
extern Node* data_ov006_0214256c;
extern Node* data_ov006_02142578;
extern Node* data_ov006_02142568;
extern s16 data_ov006_02142558;
extern s16 data_ov006_02142560;
extern s16 data_ov006_02142564;
extern s16 data_ov006_0213d6f4;
extern s16 data_ov006_0213d6f8;
extern int data_ov006_0213d700;
extern int data_ov006_02142580[];
extern int data_ov006_0212e97c[][10];

extern "C" void func_ov006_020f9000(void)
{
    Node* head = data_ov006_0214257c;

    if (data_ov006_02142570 != 0 && data_ov006_02142574 != 0) {
        data_ov006_02142558 -= 1;
        if (data_ov006_02142558 == 0) {
            Node* a;
            Node* b;
            Node* t;
            Node* u;
            s16 kb = data_ov006_02142574->key;
            s16 ka = data_ov006_02142570->key;
            if (ka > kb)
                data_ov006_0213d700 = kb << 12;
            else
                data_ov006_0213d700 = ka << 12;

            a = data_ov006_02142570;
            if (data_ov006_0214257c == a) data_ov006_0214257c = a->next;
            if (data_ov006_0214256c == a) data_ov006_0214256c = a->next;
            if (data_ov006_02142578 == a) data_ov006_02142578 = a->prev;
            if (data_ov006_02142568 == a) data_ov006_02142568 = a->prev;
            if (a->prev != 0) a->prev->next = a->next;
            if (a->next != 0) a->next->prev = a->prev;
            a->next = 0;
            a->prev = a->next;

            b = data_ov006_02142574;
            if (data_ov006_0214257c == b) data_ov006_0214257c = b->next;
            if (data_ov006_0214256c == b) data_ov006_0214256c = b->next;
            if (data_ov006_02142578 == b) data_ov006_02142578 = b->prev;
            if (data_ov006_02142568 == b) data_ov006_02142568 = b->prev;
            if (b->prev != 0) b->prev->next = b->next;
            if (b->next != 0) b->next->prev = b->prev;
            b->next = 0;
            b->prev = b->next;

            a = data_ov006_02142570;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = a;
            } else {
                u = t->next;
                if (u != 0) {
                    do { t = u; u = u->next; } while (u != 0);
                }
                t->next = a;
                a->prev = t;
                a->next = 0;
            }

            b = data_ov006_02142574;
            t = data_ov006_0214256c;
            if (t == 0) {
                data_ov006_0214256c = b;
            } else {
                u = t->next;
                if (u != 0) {
                    do { t = u; u = u->next; } while (u != 0);
                }
                t->next = b;
                b->prev = t;
                b->next = 0;
            }

            data_ov006_02142574 = 0;
            data_ov006_02142570 = 0;
            ApproachLinear2(data_ov006_02142564, 0x270f, 2);
            ApproachLinear2(data_ov006_0213d6f8, 0, 2);
            func_ov004_020adb1c(data_ov006_02142564);

            if (data_ov006_0213d6f8 <= 0x14) {
                {
                    int i = 0;
                    s16 idx = *(volatile s16*)&data_ov006_02142560;
                    int* row = (int*)((char*)data_ov006_0212e97c + idx * 0x28);
                    do {
                        data_ov006_02142580[i] = data_ov006_02142580[i] + row[i];
                        i += 1;
                    } while (i < 10);
                }
                {
                    Node* p = data_ov006_0214256c;
                    if (p != 0) {
                        do {
                            Node* nxt = p->next;
                            if (data_ov006_0214257c == p) data_ov006_0214257c = nxt;
                            if (data_ov006_0214256c == p) data_ov006_0214256c = p->next;
                            if (data_ov006_02142578 == p) data_ov006_02142578 = p->prev;
                            if (data_ov006_02142568 == p) data_ov006_02142568 = p->prev;
                            if (p->prev != 0) p->prev->next = p->next;
                            if (p->next != 0) p->next->prev = p->prev;
                            p->next = 0;
                            p->prev = p->next;
                            func_ov006_020f9f40(p, (s16)(data_ov006_02142568->key + 1));
                            {
                                Node* h = data_ov006_02142568;
                                Node* save = h->next;
                                h->next = p;
                                p->prev = h;
                                if (p->next != 0) {
                                    do { p = *(Node* volatile*)&p->next; } while (p->next != 0);
                                }
                                p->next = save;
                                if (p->next == 0) data_ov006_02142568 = p;
                            }
                            p = nxt;
                        } while (p != 0);
                    }
                }
                ApproachLinear2(data_ov006_02142560, 9, 1);
                data_ov006_0213d6f4 += 0x14;
                data_ov006_0213d6f8 += 0x14;
            }
        }
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                if (head->flag == 1)
                    head->m1(i);
                i = i + 1;
                head = head->next;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    } else {
        int v = data_ov006_0213d6f8;
        if (v > 0x14) v = 0x14;
        ApproachLinear(data_ov006_0213d700, v << 12, 0x800);
        {
            s16 i = 0;
            if ((data_ov006_0213d700 >> 12) <= 0) return;
            do {
                if (head == 0) return;
                head->m1(i);
                i = i + 1;
                head = head->next;
            } while (i < (data_ov006_0213d700 >> 12));
        }
    }
}
