extern unsigned int func_0205b5d4(void);

extern int data_020a6488;
extern int data_020a6498;
extern void **data_020a64a8[];
extern void **data_020a6494;
extern void **data_020a6484;
extern int data_020a64a0;

void **func_0205b274(int flag)
{
    void **head;
    void **node;
    int idx;

    if (flag == 0) {
        if (func_0205b5d4() == (unsigned int)data_020a6488) return 0;
    } else {
        while (func_0205b5d4() == (unsigned int)data_020a6488) {}
    }

    idx = data_020a6498;
    head = *(void ***)((int)data_020a64a8 + idx * 4);
    idx = idx + 1;
    data_020a6498 = idx;
    if (idx > 8) {
        data_020a6498 = 0;
    }

    node = head;
    while (*node != 0) node = (void **)*node;

    if (data_020a6494 != 0) {
        *data_020a6494 = head;
    } else {
        data_020a6484 = head;
    }
    data_020a6494 = node;
    data_020a64a0 = data_020a64a0 - 1;
    data_020a6488 = data_020a6488 + 1;
    return head;
}
