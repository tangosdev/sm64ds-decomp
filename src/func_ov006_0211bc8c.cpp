//cpp
/* dScMgSound_c ("Boom Box", actor 0x16f / scene 367): state 1 of the ten-record
   pad array, slot 1 of data_ov006_02142df8. This is the state a TAPPED pad sits
   in -- src/func_ov006_0211bf44.c (state 0) is the stylus hit test and writes 1
   into the record's +0x50f4 on a hit.

   It runs the pad's press animation on a per-step timer, and on the last step it
   commits the note: the record moves to state 2, the note is appended to the
   16-bit play log at +0x5610 with the pad index packed into the high byte,
   func_ov006_0211b654 spawns the note entities, and one of three arm9 sound
   entry points is called. Which one depends on vtable slot 35 and on the mode
   byte at +0x5627.

   The slot-35 call is a real virtual through a 36-slot shadow class, so the
   receiver arrives in r0 exactly as the ROM's `ldr r1,[r0] / ldr r1,[r1,#0x8c] /
   blx r1` expects. src/func_ov006_0211b9c8.cpp spells the same call the same way.

   data_ov006_0212ef3c is the eight-entry per-step frame count; 0x0213f794 is the
   per-note kind (1 or 2) and 0x0213f7e8 the per-note sound id, 42 entries each.
   Entries 0x1a..0x29 of the sound table are four groups of four, which is what
   the subtraction loop below indexes: the quotient picks the group and the
   remainder is func_02012174's first argument. */

struct Obj {
    virtual void p00(); virtual void p01(); virtual void p02(); virtual void p03();
    virtual void p04(); virtual void p05(); virtual void p06(); virtual void p07();
    virtual void p08(); virtual void p09(); virtual void p10(); virtual void p11();
    virtual void p12(); virtual void p13(); virtual void p14(); virtual void p15();
    virtual void p16(); virtual void p17(); virtual void p18(); virtual void p19();
    virtual void p20(); virtual void p21(); virtual void p22(); virtual void p23();
    virtual void p24(); virtual void p25(); virtual void p26(); virtual void p27();
    virtual void p28(); virtual void p29(); virtual void p30(); virtual void p31();
    virtual void p32(); virtual void p33(); virtual void p34();
    virtual int check();
};

extern "C" {
extern unsigned short data_ov006_0212ef3c[];
extern unsigned short data_ov006_0213f794[];
extern unsigned short data_ov006_0213f7e8[];
void func_ov006_0211b654(void *c, int idx);
void func_ov006_020c2300(void *c);
unsigned int func_02012174(unsigned int a, unsigned int b);
unsigned int func_02012790(unsigned int a);
unsigned int _ZN5Sound12PlayBank2_2DEj(unsigned int a);
}

extern "C" void func_ov006_0211bc8c(Obj *obj, int idx)
{
    char *c = (char *)obj;
    int o = idx * 0x14;
    char *cb = c + 0x50f0;
    unsigned char step;
    unsigned char note;
    unsigned char mode;

    *(unsigned short *)(cb + o) = (unsigned short)(*(unsigned short *)(cb + o) + 1);
    step = ((unsigned char *)(c + 0x50f7))[o];
    if (*(unsigned short *)(cb + o) < data_ov006_0212ef3c[step]) return;
    *(unsigned short *)(cb + o) = 0;
    ((unsigned char *)(c + 0x50f7))[o] =
        (unsigned char)(((unsigned char *)(c + 0x50f7))[o] + 1);
    if (((unsigned char *)(c + 0x50f7))[o] <= 6) return;
    ((unsigned char *)(c + 0x50f7))[o] = 6;
    ((unsigned char *)(c + o))[0x50f4] = 2;

    *(unsigned short *)(c + *(unsigned char *)(c + 0x5625) * 2 + 0x5610) =
        (unsigned short)(((unsigned char *)(c + 0x50f8))[o] + 1);
    *(unsigned short *)(c + 0x5610 + *(unsigned char *)(c + 0x5625) * 2) |=
        idx << 8;
    {
        unsigned char *q = (unsigned char *)(c + 0x5625);
        *q = (unsigned char)(*q + 1);
    }
    *(unsigned short *)(c + 0x5614) = 0x20;

    func_ov006_0211b654(c, idx);

    note = ((unsigned char *)(c + 0x50f8))[o];
    if (obj->check()) {
        mode = *(unsigned char *)(c + 0x5627);
        if (mode == 0 || mode == 2) {
            if (data_ov006_0213f794[note] == 2) {
                func_02012790(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(*(unsigned int *)(c + 0x560c),
                              data_ov006_0213f7e8[note]);
            }
        } else if (mode == 1) {
            int v = note - 0x1a;
            int k = 0;
            while (v >= 4) { v -= 4; k++; }
            func_02012174(v, data_ov006_0213f7e8[k * 4 + 0x1a]);
        } else {
            _ZN5Sound12PlayBank2_2DEj(data_ov006_0213f7e8[note]);
        }
    } else {
        mode = *(unsigned char *)(c + 0x5627);
        if (mode == 0 || mode == 2) {
            func_02012790(data_ov006_0213f7e8[note]);
        } else if (mode == 1) {
            func_02012174(*(unsigned int *)(c + 0x560c),
                          data_ov006_0213f7e8[note]);
        } else if (mode == 4) {
            if (data_ov006_0213f794[note] == 2) {
                _ZN5Sound12PlayBank2_2DEj(data_ov006_0213f7e8[note]);
            } else {
                func_02012174(*(unsigned int *)(c + 0x560c),
                              data_ov006_0213f7e8[note]);
            }
        } else {
            _ZN5Sound12PlayBank2_2DEj(data_ov006_0213f7e8[note]);
        }
    }

    if (*(unsigned char *)(c + 0x5624) != 1) return;
    func_ov006_020c2300(c + 0x4f38);
}
