struct Node58 {
    char pad[0x64];
    int f64;
    struct Node58 *next;
};
struct Root58 {
    char pad[0xc];
    struct Node58 *head;
};
extern struct Root58 data_020a6134;
struct Node58 *func_0205801c(void) {
    struct Node58 *node = data_020a6134.head;
    while (node && node->f64 != 1)
        node = node->next;
    return node;
}
