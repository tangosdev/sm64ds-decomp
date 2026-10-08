struct Node72 {
    struct Node72 *next;
    void (*fn)(void *object, int v);
    void *object;
};
extern struct Node72 *data_020aa3f0;

void func_02072f3c(void) {
    struct Node72 *head = data_020aa3f0;
    if (head == 0) return;
    do {
        data_020aa3f0 = head->next;
        head->fn(head->object, -1);
        head = data_020aa3f0;
    } while (head != 0);
}
