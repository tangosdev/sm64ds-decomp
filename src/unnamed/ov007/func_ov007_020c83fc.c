/* o: the 0x24-byte follower record func_ov007_020c844c allocates. No class or
 * header names it; this view's names come from its two steppers, which read
 * every slot: func_ov007_020c81a0 (2D) and func_ov007_020c831c (3D). */
typedef struct Ov007Follower {
    int *target;     /* 0x00: the Q12 position each step moves (func_ov007_020c844c's p) */
    int velocity[3]; /* 0x04: added to target[] every step, >> shift */
    int force[3];    /* 0x10: accumulated per frame, cleared at the end of each step */
    short invMass;   /* 0x1c: func_ov007_020c844c stores the reciprocal
                      * (func_02053200) of its q, 0x1000 for q = 1.0; both
                      * steppers take a direct path at 0x1000 and otherwise
                      * scale force by it into velocity; signed, as
                      * func_ov007_020c80c8 reads it (asr) */
    unsigned short pad_1e;
    int flags; /* 0x20: bit 0 skips the step (target held) */
} Ov007Follower;

typedef char Ov007Follower_size_must_be_0x24[sizeof(Ov007Follower) == 0x24 ? 1 : -1];

void func_ov007_020c83fc(int* o){
    Ov007Follower *f = (Ov007Follower *)o;
    f->force[2] = 0;
    f->force[1] = f->force[2];
    f->force[0] = f->force[1];
    f->velocity[2] = f->force[0];
    f->velocity[1] = f->velocity[2];
    f->velocity[0] = f->velocity[1];
    f->target = 0;
    f->invMass = 0x1000;
    f->flags = 0;
}
