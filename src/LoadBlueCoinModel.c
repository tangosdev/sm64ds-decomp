typedef struct { int unk[1]; } SharedFilePtr;

extern SharedFilePtr data_ov002_0210da00;   /* 0x0210da00 */
extern SharedFilePtr data_ov002_0210d9c8;  /* 0x0210d9c8 */

extern void _ZN5Model8LoadFileER13SharedFilePtr(SharedFilePtr* ptr); /* 0x02017a3c */

/* Takes the calling actor in r0. The body ignores it -- the first instruction is
 * `ldr r0,[pc,#0x14]`, so r0 is overwritten before any read and the parameter is
 * invisible in this function's own bytes (it compiles identically either way). The
 * evidence is the call sites: all ten in the cartridge set r0 immediately before the
 * bl, nine of them as `mov r0, r4`. Spelling it `(void)` here is harmless for this
 * file but wrong, and it cost MrI::InitResources a match for two campaigns, because a
 * caller that declares it `(void)` lets its own incoming r0 die early and reorders the
 * caller's entry block. */
void LoadBlueCoinModel(void *actor)
{
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da00);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210d9c8);
}
