extern void* _ZN5Actor13ClosestPlayerEv(void);

struct ActorObj {
    char pad[0x8c8];
    void* closestPlayer;
};

void func_ov034_02112330(char* c) {
    void* player = _ZN5Actor13ClosestPlayerEv();
    ((struct ActorObj*)c)->closestPlayer = player;
}
