typedef int Fix12i;
typedef struct Vector3 { int x, y, z; } Vector3;
extern Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern unsigned char NUM_PLAYERS[];
extern char* PLAYER_ARR[];

int func_ov081_021245e8(void* thiz)
{
    char* sl = (char*)thiz;
    int min = 0x2710000;
    int i = 0;
    Vector3 v;
    *(int*)(sl + 0x40c) = -1;
    if ((int)NUM_PLAYERS[0] > 0) {
        do {
            char* obj = PLAYER_ARR[i];
            if (obj != 0) {
                int dist;
                int* s = (int*)(((long long)(obj + 0x5c)) & 0xFFFFFFFFFFFFFFFF);
                v.x = s[0];
                v.y = s[1];
                v.z = s[2];
                dist = Vec3_Dist((Vector3*)(sl + 0x5c), &v);
                if (i == 0) {
                    min = dist;
                    *(int*)(sl + 0x40c) = 0;
                } else if (dist < min) {
                    min = dist;
                    *(int*)(sl + 0x40c) = i;
                }
            }
            i++;
        } while (i < (int)NUM_PLAYERS[0]);
    }
    return min;
}
