typedef int Fix12;
struct Node { char pad[8]; struct Node* next; };
extern void func_ov006_020f82d0(struct Node* o, int n);
extern Fix12 data_ov006_0213d574;
extern struct Node* data_ov006_02142500;
void func_ov006_020f7994(void){
    struct Node* node = data_ov006_02142500;
    short i = 0;
    if((data_ov006_0213d574>>12) <= 0) return;
    do {
        if(node == 0) return;
        func_ov006_020f82d0(node, i);
        i = i+1;
        node = node->next;
    } while(i < (data_ov006_0213d574>>12));
}
