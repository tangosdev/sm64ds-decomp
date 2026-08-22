/* dScMgTeresa_c, "Hide and Boo Seek". The class's sprite draw: the sixth and
   last call vtable slot 9 (Render) makes, after func_ov004_020b1e34,
   func_ov006_0211ddcc, _0211e29c, _0211e460 and _0211e118
   (src/func_ov006_02120348.c). This is the pass that puts the Boos on the
   screen; nothing else in the class draws them.

   THE LOOP IS THE SIXTEEN-RECORD ARRAY at this + 0x4660, stride 0x24 -- the
   same records func_ov006_0211f6fc ticks and src/func_ov006_0211ebdc.c and
   _0211ee34.c update. Per record:

     +0x1a  the RENDER gate. Zero means the Boo is not drawn at all, and it is
            the byte func_ov006_0211ebdc clears when a Boo has finished fading.
     +0x00  x, 20.12; +0x04 y, 20.12. Shifted down by 12 for the sprite.
     +0x16  the fade level. Nonzero picks 1 for OAM::Render's mode and zero
            picks -1, which is the same two-hoisted-constants shape
            src/func_ov006_020f5b98.c's card draw uses for its flag.
     +0x1b  the sprite priority.
     +0x1d  the animation frame and +0x1e the facing, together the index into
            data_ov006_0213a964.

   THESE GO ON THE SUB SCREEN. OAM::Render's first parameter is `bool sub` --
   src/_ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii.cpp names it -- and this loop
   passes 1 where every other caller in the tree passes 0. So a capture of the
   TOP screen alone cannot show this body running, however many Boos are live;
   SM64DS_SCENE_BMP_STACKED is the artifact that can. The rest of the argument
   list is unremarkable and is written out because a reader should not have to
   count commas: palette -1, priority from the record, unity scale on both
   axes, no rotation.

   THE SPRITE TABLE IS TWO ROWS OF SEVEN and the ROM says so twice. The symbol
   span 0x0213a964..0x0213a99c is 0x38 = fourteen words, and
   config/arm9/overlays/ov006/relocs.txt carries fourteen `from:` rows over
   exactly that span and none past it, so every one of the fourteen is a
   relocated OamAttr pointer and there is no fifteenth. The index is
   +0x1e * 7 + +0x1d, and src/func_ov006_0211ebdc.c writes +0x1e as 0 or 1 off
   the sign of the sine table, so the row is which way the Boo faces and the
   column is its seven-frame animation.

   THE NINTH ARGUMENT IS WRITTEN (int)(long long)0 AND THAT IS DELIBERATE. It
   is a plain zero and every caller in the tree passes one. Spelt `0`, mwcc
   hoists the loop's constants in the order [0, 1, -1, 0x1000] and the ROM's is
   [-1, 0, 1, 0x1000] -- the whole allocation comes out rotated by one register
   with every instruction otherwise identical, seventeen words differing by a
   register number and nothing else. The wider-typed zero moves -1 to the front
   of that list. src/func_ov002_020f1fcc.c uses the same long-long laundering
   on an address for the same class of reason. */

extern void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
    int sub, void *attr, int xOff, int yOff, int palette, int priority,
    int scaleX, int scaleY, int rotation, int mode);
extern void *data_ov006_0213a964[];

void func_ov006_0211e72c(char *c)
{
    int i;
    int idx;
    int mode;
    int prio;
    int x;
    int y;

    for (i = 0; i < 0x10; i++) {
        if (*(unsigned char *)(c + 0x467a) != 0) {
            x = *(int *)(c + 0x4660) >> 12;
            idx = *(unsigned char *)(c + 0x467d) +
                  *(unsigned char *)(c + 0x467e) * 7;
            prio = *(unsigned char *)(c + 0x467b);
            mode = -1;
            y = *(int *)(c + 0x4664) >> 12;
            if (*(unsigned char *)(c + 0x4676) != 0)
                mode = 1;
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
                1, data_ov006_0213a964[idx], x, y, -1, prio,
                0x1000, 0x1000, (int)(long long)0, mode);
        }
        c += 0x24;
    }
}
