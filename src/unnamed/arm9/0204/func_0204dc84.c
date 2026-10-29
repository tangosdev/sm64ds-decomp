/* Clear one 0xc-byte particle tracking record (dPa_c's
 * edStarKiraCallback_c::trackingRecords, and the ov007 sprite list).
 * func_0204dab4 fills them: it binds a record to a particle, has the
 * projection callback (func_0204dcc0 by default) write the screen x / y,
 * stores the callback's weight scaled by the particle's remaining life in
 * fade, keeps the caller's mask in flags bits 0-1 and marks bit 2 for a
 * record it updated this frame. */
typedef struct ParticleTrack {
    short x;             /* 0x00 */
    short y;             /* 0x02 */
    short fade;          /* 0x04 */
    unsigned char flags; /* 0x06 */
    unsigned char pad_07;
    void *particle; /* 0x08 */
} ParticleTrack;

void func_0204dc84(char *self)
{
    ParticleTrack *rec = (ParticleTrack *)self;
    rec->flags &= ~3;
    rec->flags &= ~4;
    rec->fade = 0;
    rec->pad_07 = 0;
    rec->y = 0;
    rec->x = rec->y;
    rec->particle = 0;
}
