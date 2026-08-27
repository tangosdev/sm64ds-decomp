struct Node { struct Node* next; };
extern struct Node* data_ov006_021421c0;

void func_ov006_020eeef4(struct Node* node) {
    struct Node* prev = data_ov006_021421c0;
    if (prev == node) {
        data_ov006_021421c0 = node->next;
        return;
    }
    struct Node* cur = prev->next;
    while (cur) {
        if (cur == node) {
            prev->next = node->next;
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}
