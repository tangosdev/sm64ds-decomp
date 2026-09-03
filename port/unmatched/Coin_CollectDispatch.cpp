/* HOST COPY of src/func_ov002_020b19dc.c -- the coin's collect dispatch.
 *
 * WHY A HOST COPY (the calling-convention seam):
 *
 * func_ov002_020b1884 takes TWO arguments -- the coin `self` (r0) and the
 * collecting player `r4` (r1) -- and reads the player at +0x706, +0x6d8 to
 * pick the pickup sound and to index GiveCoins by player number. On the DS
 * (ARM) func_ov002_020b19dc leaves the player live in r1: it does
 *
 *     mov  r4, r0                 ; self
 *     ldr  r0, [r4, #0x19c]       ; the coin's target-actor id
 *     bl   Actor::FindWithID      ; r0 = the player that touched the coin
 *     movs r1, r0                 ; r1 = player, and it STAYS in r1 ...
 *     ...
 *     mov  r0, r4                 ; r0 = self
 *     bl   func_ov002_020b1884    ; ... all the way to here (r0=self, r1=player)
 *
 * so the second argument arrives "for free" in the register the ABI passes it
 * in. The matched C for func_ov002_020b19dc spells the call with a single
 * argument (func_ov002_020b1884(self)); that is byte-identical on ARM because
 * r1 is already the found player.
 *
 * On the host MSVC uses the cdecl STACK convention. The one-argument call in
 * the matched source pushes only `self`; func_ov002_020b1884's host object then
 * reads its second parameter from [ebp+0Ch], a stack slot nothing wrote, so the
 * "player" is stack garbage. GiveCoins(garbage[+0x6d8], 1) indexes
 * data_0209f394 out of bounds, hands func_ov002_020b18f0 a bogus base (4), and
 * that function's `[base+0x6d8]` read faults at 0x6dc -- the g5_COIN_B /
 * g1_COIN_B signature, c0000005 at func_ov002_020b18f0 +0x3b, in every star
 * group's collect path.
 *
 * THE FIX is to pass the found player explicitly, exactly the value the ROM
 * leaves in r1: FindWithID(self->[0x19c]). This host copy is the matched
 * source line for line; only the func_ov002_020b1884 call gains its real second
 * argument. The other caller of func_ov002_020b1884 (func_ov002_020b2a34,
 * OnTurnIntoEgg) already passes two arguments in its matched C, so it is
 * correct as-is and is left untouched.
 *
 * src/func_ov002_020b19dc.c is dropped from slice_gate33.txt in favour of this
 * file; the byte-locked source is unchanged.
 */

extern "C" {

void *_ZN5Actor10FindWithIDEj(unsigned int id);
void func_ov002_020b16c4(char *self, char *p);   /* derefs the player at +0x706/+0x6d8 */
void func_ov002_020b1674(char *self, char *p);
void func_ov002_020b1884(char *self, char *p);   /* the real two-arg shape */

// PORT_HOST_ABI: implicit-register-arg (callee func_ov002_020b1884 is (self, player); ARM rode the player in r1, the host passes it).
int func_ov002_020b19dc(char *self)
{
    unsigned int id = *(unsigned int *)(self + 0x19c);
    if (id != 0) {
        char *p = (char *)_ZN5Actor10FindWithIDEj(id);
        if (p != 0) {
            if (*(int *)(self + 0x198) & 0x400000) {
                *(unsigned short *)(self + 0x3a8) = 0;
                if (*(int *)(self + 0x3a0) == 1)
                    func_ov002_020b16c4(self, p);   /* <-- the player, ROM's r1 */
                else if (*(int *)(self + 0x3a0) == 2)
                    func_ov002_020b1674(self, p);
                else
                    func_ov002_020b1884(self, p);   /* <-- the player, ROM's r1 */
                return 1;
            }
        }
    }
    return 0;
}

}  /* extern "C" */
