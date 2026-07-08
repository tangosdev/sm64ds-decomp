//cpp
extern "C" int WATER_HEIGHT;
struct Node { struct Node* next; char pad[8]; int field0c; char pad2[8]; int field18; };
struct System { char pad8[8]; struct Node* head; char pad[0x10]; int field1c; };
struct Particle { char pad4[4]; int field4; };
extern "C" int _ZN8Particle16FitWaterCallback8OnUpdateERNS_6SystemEb(void* thiz, System* sys, int b);
int _ZN8Particle16FitWaterCallback8OnUpdateERNS_6SystemEb(void* thiz, System* sys, int b) {
    Node* node = sys->head;
    int v;
    if (b) {
        int* flags = (int*)(((long)sys + 0x1c) & 0xFFFFFFFFFFFFFFFF);
        *flags &= ~2;
    } else {
        int* flags = (int*)(((long)sys + 0x1c) & 0xFFFFFFFFFFFFFFFF);
        *flags |= 2;
        if (node == 0) return 0;
    }
    v = (((Particle*)thiz)->field4 + WATER_HEIGHT) >> 3;
    while (node != 0) {
        node->field18 = v - node->field0c;
        node = node->next;
    }
    return 1;
}
