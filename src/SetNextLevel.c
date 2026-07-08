// NONMATCHING: register allocation (div=31). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef unsigned char u8;
typedef signed char s8;

extern s8 LEVEL_ID;
extern s8 RETURN_LEVEL_ID;
extern u8 RETURN_ENTRANCE_ID;
extern u8 NEXT_LEVEL_ID;
extern u8 NEXT_ENTRANCE_ID;
extern u8 STAR_ID;
extern u8 NEXT_STAR_ID;
extern u8 RETURN_STATE;

struct Entry { s8 a; s8 b; u8 c; };
extern struct Entry data_02075638[];

int SublevelToLevel(int i);
void SetNextStar(void);

void SetNextLevel(int arg) {
    int v = LEVEL_ID;
    unsigned i;
    struct Entry* p;
    if (v == 0x22) {
        if (arg != 1) {
            RETURN_LEVEL_ID = 1;
            RETURN_ENTRANCE_ID = 0xa;
        }
    } else {
        p = data_02075638;
        for (i = 0; i < 8; i++) {
            if (v == p->a) {
                RETURN_LEVEL_ID = p->b;
                RETURN_ENTRANCE_ID = p->c;
                break;
            }
            p++;
        }
    }
    if (RETURN_LEVEL_ID >= 0) {
        if (SublevelToLevel(0) == 0x1d) {
            NEXT_LEVEL_ID = 1;
            NEXT_ENTRANCE_ID = 0xd;
        } else {
            NEXT_LEVEL_ID = RETURN_LEVEL_ID;
            NEXT_ENTRANCE_ID = RETURN_ENTRANCE_ID;
        }
    }
    RETURN_LEVEL_ID = -1;
    NEXT_STAR_ID = STAR_ID;
    SetNextStar();
    RETURN_STATE = arg;
}
