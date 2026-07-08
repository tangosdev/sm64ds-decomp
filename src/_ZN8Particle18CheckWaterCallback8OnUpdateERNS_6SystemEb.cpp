//cpp
extern "C" int WATER_HEIGHT;
struct Node { struct Node* next; char pad[8]; int field0c; char pad2[8]; int field18; char pad3[0x10]; unsigned short field2c; unsigned short field2e; };
struct System { char pad8[8]; struct Node* head; char pad[0x10]; int field1c; };
extern "C" int _ZN8Particle18CheckWaterCallback8OnUpdateERNS_6SystemEb(void* thiz, System* sys, int b);
int _ZN8Particle18CheckWaterCallback8OnUpdateERNS_6SystemEb(void* thiz, System* sys, int b) {
    Node* node = sys->head;
    if (b) {
        int* flags = (int*)(((long)sys + 0x1c) & 0xFFFFFFFFFFFFFFFF);
        *flags &= ~2;
    } else {
        int* flags = (int*)(((long)sys + 0x1c) & 0xFFFFFFFFFFFFFFFF);
        *flags |= 2;
        if (node == 0) return 0;
    }
    while (node != 0) {
        if (((node->field18 + node->field0c) << 3) > WATER_HEIGHT) {
            node->field2e = node->field2c;
        }
        node = node->next;
    }
    return 1;
}
