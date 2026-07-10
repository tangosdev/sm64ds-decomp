extern int Vec3_Dist(void* a, void* b);
extern void* FindWithActorID(unsigned int id, void* prev);
extern int SetPolygonID(void* model, int id);

#pragma opt_propagation off
int _ZN5Cloud8BehaviorEv(char *c) {
    int d = Vec3_Dist(c+0x74, c+0x5c);
    int rank = 1;
    int base = 2;
    void *a = FindWithActorID(0x13a, 0);
    while (a) {
        if (a != (void*)c) {
            int d2 = Vec3_Dist(c+0x74, (char*)a+0x5c);
            if (d > d2) rank++;
        }
        a = FindWithActorID(0x13a, a);
    }
    SetPolygonID(c+0xd4, (base + rank) & 0xff);
    return 1;
}
