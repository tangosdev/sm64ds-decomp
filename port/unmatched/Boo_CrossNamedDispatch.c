/* HOST COPIES of three ov063 TUs whose matched sources spell a callee by
 * ANOTHER overlay's name -- the shared-window naming race (the exact-share
 * trio is ov063/ov065/ov066; ov075 merely overlaps the window; dsd's
 * export race handed three ov063
 * addresses their ov065/ov066 spellings, the /alternatename:_data_ov059_...
 * =_func_ov045_... case from hal/actor_classes_ov045.cpp in source form):
 *
 *     src/unnamed/ov063/func_ov063_02116d38.c   case 0 spelled daYurei_Mucho_c_classInit
 *         -- ov065's name for 0x02116f48; ov063's own body there is
 *         func_ov063_02116f48 (0x64 bytes vs daYurei_Mucho_c_classInit's 0x50: different
 *         content, so an /alternatename would go WRONG the moment lane 5-B
 *         links the real ov065 TU -- the alias only fires while undefined).
 *     src/unnamed/ov063/func_ov063_021192d4.c   case 6 spelled
 *         _ZN15TtcRotatingCube8BehaviorEv -- ov065's name for 0x02119a50;
 *         ov063's body is func_ov063_02119a50 (on the slice).
 *     src/unnamed/ov063/func_ov063_02116f48.cpp calls data_ov066_0211ad00 AS
 *         A FUNCTION -- ov066's data name for 0x0211ad00; ov063's body is
 *         func_ov063_0211ad00 (on the slice). Mounting ov066's word there
 *         one day would hand this call host DATA to jump into; the by-name
 *         call below is immune.
 *
 * The three sources are byte-matched and stay untouched in src/; these copies
 * change ONLY the callee spellings, corrected BY ADDRESS against the raw
 * image (extracted/overlays/overlay_0063.bin) and each dispatcher's own
 * jump-table disassembly: 02116d38 case 0 is `bl 0x2116f48` and 021192d4
 * case 6 is `bl 0x2119a50`, both with the actor still in r0, so every case
 * passes c here (the recovered daYurei_Mucho_c_classInit() no-arg spelling was arity
 * noise from the misnamed extern). The decomp-side fix -- renaming the
 * externs by address in src/ -- belongs to a src-owning lane; flagged in the
 * lane report.
 *
 * These two dispatchers are Boo state handlers (+0x5cc sub-state below the
 * +0x5cf state the Behavior switches on); 0x02116f48 re-arms the Boo scale
 * and calls the 0x0211ad00 approach test.
 */

extern void func_ov063_02116e14(void *c);
extern void func_ov063_02116df0(void *c);
extern void func_ov063_02116dbc(void *c);
extern void func_ov063_02116d98(void *c);
extern int func_ov063_0211aa34(void *c);
extern int func_ov063_02116f48(char *c);
extern void func_ov063_02119bb0(void *c);
extern void func_ov063_02119960(void *c);
extern void func_ov063_02119870(void *c);
extern void func_ov063_0211975c(void *c);
extern void func_ov063_02119a2c(void *c);
extern void func_ov063_0211934c(void *c);
extern void func_ov063_02119a50(void *c);
extern int func_ov063_0211ad00(char *c);

/* PORT_HOST_ABI: shared-window naming race -- the matched TU spells the
   0x02116f48 callee by ov065's daYurei_Mucho_c_classInit; corrected by address here. */
void func_ov063_02116d38(char *c)
{
    switch (*(unsigned char *)(c + 0x5cc)) {
        case 0: func_ov063_02116f48(c); break;
        case 1: func_ov063_02116e14(c); break;
        case 2: func_ov063_02116df0(c); break;
        case 3: func_ov063_02116dbc(c); break;
        case 4: func_ov063_02116d98(c); break;
    }
    func_ov063_0211aa34(c);
}

/* PORT_HOST_ABI: shared-window naming race -- the matched TU spells the
   0x02119a50 callee by ov065's _ZN15TtcRotatingCube8BehaviorEv; corrected by
   address here. */
void func_ov063_021192d4(char *c)
{
    switch (*(unsigned char *)(c + 0x5cc)) {
        case 0: func_ov063_02119bb0(c); break;
        case 1: func_ov063_02119960(c); break;
        case 2: func_ov063_02119870(c); break;
        case 3: func_ov063_0211975c(c); break;
        case 4: func_ov063_02119a2c(c); break;
        case 5: func_ov063_0211934c(c); break;
        case 6: func_ov063_02119a50(c); break;
    }
    func_ov063_0211aa34(c);
}

/* PORT_HOST_ABI: shared-window naming race -- the matched TU calls ov066's
   data_ov066_0211ad00 as a function; the body at 0x0211ad00 in ov063's own
   image is func_ov063_0211ad00, called by name here. */
int func_ov063_02116f48(char *c)
{
    *(int *)(c + 0x490) = 0;
    *(unsigned char *)(c + 0x5c9) = 0xff;
    *(int *)(c + 0x584) = 0x2000;
    *(int *)(c + 0x80) = 0x2000;
    *(int *)(c + 0x84) = 0x2000;
    *(int *)(c + 0x88) = 0x2000;
    *(int *)(c + 0x188) = *(int *)(c + 0x590) * *(int *)(c + 0x80);
    *(int *)(c + 0x18c) = *(int *)(c + 0x594) * *(int *)(c + 0x80);
    {
        int r = func_ov063_0211ad00(c);
        if (r) {
            *(unsigned char *)(c + 0x5cc) = 1;
            r = 1;
        }
        return r;
    }
}
