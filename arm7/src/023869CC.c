/* storage 0x023869CC runtime 0x037FE864 size 0x28 + one pool word, wram ARM.
 * Clears the first two bytes of eight 0x40-byte rows at SNDi_Work+0xF80
 * (0x03808544). Written as struct-field access: row[i].a/b keeps the 0xF80
 * field offset in the strb displacement and lands the row pointer in r0,
 * the zero in r2. A flat byte_3808544[...] index gives the same prologue
 * but scaled-index stores instead. */

struct Row { unsigned char a; unsigned char b; char pad[0x3E]; };
struct Work { char pad1[0xF80]; struct Row rows[8]; };
extern struct Work SNDi_Work;

void fn_023869CC(void) {
    int i;
    for (i = 0; i < 8; i++) {
        SNDi_Work.rows[i].a = 0;
        SNDi_Work.rows[i].b = 0;
    }
}
