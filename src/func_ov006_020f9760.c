typedef short s16;
typedef unsigned char u8;

typedef struct Node {
    void* vt;
    struct Node* prev;
    struct Node* next;
    char pad_c[0x1e];
    s16 key;
    char pad_2c;
    u8 flag;
} Node;

extern void func_ov006_020f9f40(Node* c, s16 n);
extern int func_ov006_020f96e0(void);

extern Node* data_ov006_0214257c;
extern Node* data_ov006_0214256c;
extern Node* data_ov006_02142570;
extern Node* data_ov006_02142574;
extern Node* data_ov006_02142568;
extern s16 data_ov006_02142558;
extern s16 data_ov006_0214255c;
extern s16 data_ov006_02142560;
extern s16 data_ov006_02142564;
extern s16 data_ov006_0213d6f4;
extern s16 data_ov006_0213d6f8;
extern int data_ov006_0213d700;
extern int data_ov006_02142580[];
extern int data_ov006_0212e954[];

void func_ov006_020f9760(Node* base)
{
    do {
        {
            s16 i = 0;
            do {
                data_ov006_02142580[i] = data_ov006_0212e954[i];
                i = i + 1;
            } while (i < 10);
        }
        {
            s16 j = 0;
            Node* p = base;
            do {
                func_ov006_020f9f40(p, j);
                p = p + 1;
                j = j + 1;
            } while (j < 0x28);
        }
        data_ov006_02142568 = 0;
        data_ov006_0214257c = base;
        data_ov006_0214256c = 0;
        data_ov006_02142570 = 0;
        data_ov006_02142574 = 0;
        data_ov006_02142558 = 0;
        data_ov006_0214255c = 0;
        data_ov006_0213d6f8 = 0x28;
        data_ov006_0213d6f4 = 0x28;
        data_ov006_02142560 = 0;
        data_ov006_02142564 = 0;
        data_ov006_0213d700 = 0;
        {
            s16 i = 0;
            Node* h = base;
            do {
                Node* t = &base[i + 1];
                Node* save = h->next;
                h->next = t;
                t->prev = h;
                {
                    Node* u = t->next;
                    if (u != 0) {
                        do { t = u; u = u->next; } while (u != 0);
                    }
                }
                t->next = save;
                if (t->next == 0) data_ov006_02142568 = t;
                h = h + 1;
                i++;
            } while (i < 0x27);
        }
    } while (func_ov006_020f96e0() == 0);
}
