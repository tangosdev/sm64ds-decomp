struct Node { struct Node* next; };
extern struct Node* data_ov006_02142f64;

void func_ov006_02120b7c(struct Node* node) {
    struct Node* prev = data_ov006_02142f64;
    if (prev == node) {
        data_ov006_02142f64 = node->next;
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
