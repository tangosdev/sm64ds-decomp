typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct {
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char pad : 4;
} Flags;

typedef struct CapNode {
    u8 pad0[4];
    int field4;
    int field8;
    u8 padC[4];
    struct CapNode *next;
    int soundHandle;
    u8 field18;
    u8 field19;
    u8 pad1A;
    Flags flags;
} CapNode;

typedef struct CapRequest {
    u8 pad0[8];
    int field8;
} CapRequest;

extern CapNode *data_ov001_020ad634[];
extern u8 data_ov001_020ad628[];
extern u8 data_ov001_020ad630[];
extern u8 data_0208a0e0;
extern CapRequest *data_0209f394[];

extern int func_ov001_020aa79c(int x);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int func_0202a8e0(int a, u8 b);
extern void func_ov001_020aa6b0(void *p, int flag);
extern void func_ov001_020ab110(void *p);
extern int func_ov001_020aa7b8(int idx, void *p);
extern void func_ov001_020aa6e4(int idx, unsigned int val, void *obj);
extern int _ZN6Player15IsCollectingCapEv(void *self);

void func_ov001_020aa420(void) {
    CapRequest *player;
    CapNode *best;
    int handled;
    u8 *flagByte;
    u8 reqFlag;
    CapNode *node;
    u8 maxPriority;
    int spC;
    int sp10;
    unsigned char i;
    void *found;
    u8 cnt;
    int idx;

    maxPriority = 0;
    i = 0;
    spC = 0;
    sp10 = 0;

    for (; i < 3; i++) {
        best = 0;
        node = data_ov001_020ad634[i];
        handled = (int)best;
        flagByte = &data_ov001_020ad628[i];
        reqFlag = data_ov001_020ad628[i];

        if (reqFlag == 0) {
            if (node != 0) {
                do {
                    *(u8 *)(((long long)(int)((char *)node + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2;
                    if (node->field19 == 1) {
                        *flagByte = 1;
                    }
                    node = node->next;
                } while (node != 0);
            }
            if (*flagByte == 0) {
                *flagByte = 2;
            } else {
                *flagByte = 1;
            }
        }

        node = *(CapNode *volatile *)&data_ov001_020ad634[i];

        if (func_ov001_020aa79c(i) != 0) continue;
        if (*flagByte == 2) continue;
        if (data_ov001_020ad630[i] == 0) continue;

        if (node != 0) {
            do {
                found = _ZN5Actor10FindWithIDEj(node->field8);
                if (found != 0 || (int)found != node->field4) {
                    if (node->flags.b0) {
                        if (node->soundHandle == -1) {
                            node->soundHandle = func_0202a8e0(node->field4, node->field18);
                            *(u8 *)(((long long)(int)((char *)node + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
                            func_ov001_020aa6b0(node, 1);
                        }
                        handled = 1;
                        maxPriority = node->field19;
                    } else if (node->field19 != 0) {
                        if (maxPriority < node->field19) {
                            maxPriority = node->field19;
                            best = node;
                        }
                    } else if (maxPriority == 0) {
                        if (node->flags.b3) {
                            best = node;
                        } else {
                            *(u8 *)(((long long)(int)((char *)node + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL) |= 8;
                        }
                    }
                } else {
                    func_ov001_020ab110(node);
                }
                node = node->next;
            } while (node != 0);
        }

        if (func_ov001_020aa7b8(i, best) != 0) {
            *(u8 *)(((long long)(int)((char *)best + 0x1b)) & 0xFFFFFFFFFFFFFFFFLL) |= 2;
            best->soundHandle = func_0202a8e0(best->field4, best->field18);
            func_ov001_020aa6b0(best, 1);
            func_ov001_020aa6e4(i, best->field19, best);
            continue;
        }

        if (handled == 0) continue;

        cnt = data_0208a0e0;
        for (idx = spC; idx < cnt; idx++) {
            player = data_0209f394[idx];
            if (i == player->field8) break;
            player = 0;
        }
        if (player != 0 && _ZN6Player15IsCollectingCapEv(player) != 0) {
            func_ov001_020aa6e4(i, maxPriority, (void *)sp10);
        }
    }
}
