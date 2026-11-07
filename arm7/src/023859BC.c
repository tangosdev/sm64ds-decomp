/* storage 0x023859BC runtime 0x037FD854 size 0x10 wram ARM.
 * Tail call. Pool word is fn_02385AB0. */

extern int fn_02385AB0(int a1, int a2, int a3);

int fn_023859BC(int a1, int a2) {
    return fn_02385AB0(a1, a2, -1);
}
