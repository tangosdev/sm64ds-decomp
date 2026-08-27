struct Node72 {
    struct Node72 *next;
    void (*fn)(int arg, int v);
    int arg;
};
extern struct Node72 *data_020aa3f0;

void func_02072f3c(void) {
    struct Node72 *head = data_020aa3f0;
    if (head == 0) return;
    do {
        data_020aa3f0 = head->next;
        head->fn(head->arg, -1);
        head = data_020aa3f0;
    } while (head != 0);
}
