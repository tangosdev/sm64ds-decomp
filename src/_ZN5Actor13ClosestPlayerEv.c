extern int Vec3_Dist(const void* a, const void* b);
extern void* data_0209b458;
extern void* data_0209b450;
extern int data_0208e380;
extern int data_0208e37c;
extern unsigned char NUM_PLAYERS;
extern void* PLAYER_ARR[];
void* _ZN5Actor13ClosestPlayerEv(void* c)
{
    if (!data_0209b458) {
        data_0208e380 = 0x7fffffff;
        data_0208e37c = 0x80000000;
        int i;
        for (i = 0; i < NUM_PLAYERS; i++) {
            void* p = PLAYER_ARR[i];
            if (!p) continue;
            int d = Vec3_Dist((char*)c + 0x5c, (char*)p + 0x5c);
            if (d < data_0208e380) { data_0208e380 = d; data_0209b458 = p; }
            if (d > data_0208e37c) { data_0208e37c = d; data_0209b450 = p; }
        }
    }
    return data_0209b458;
}
