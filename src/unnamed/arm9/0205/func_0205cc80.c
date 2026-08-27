extern unsigned int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(unsigned int saved);
extern int func_0205d23c(void *a, void *b);
extern void *func_0205d304(void *a, void *b);

typedef struct Node {
    void *field_0;
    struct Node *next;   /* field_4 */
    struct Node *field_8;
} Node;

extern Node *data_020a8048;

typedef struct {
    void *field_0;
    short field_4;
    short field_6;
    int field_8;
} Tail;

extern Tail data_020a804c;

int func_0205cc80(Node *thiz, void *a, void *b)
{
    unsigned int saved;
    int ret = 0;

    saved = _ZN3IRQ7DisableEv();

    if (func_0205d23c(a, b) == 0) {
        if (data_020a8048 == 0) {
            data_020a8048 = thiz;
            data_020a804c.field_0 = thiz;
            data_020a804c.field_8 = 0;
            data_020a804c.field_6 = 0;
            data_020a804c.field_4 = 0;
        } else {
            Node *cur = data_020a8048;
            while (cur->next != 0)
                cur = cur->next;
            cur->next = thiz;
            thiz->field_8 = cur;
        }
        thiz->field_0 = func_0205d304(a, b);
        ret = 1;
    }

    _ZN3IRQ7RestoreEj(saved);
    return ret;
}
