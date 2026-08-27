struct Obj { int pad[2]; int f8; int fc; };
struct Tbl { struct Obj *o[8]; };
extern char data_ov007_0210342c[];

void func_ov007_020bca40(int sel){
  struct Tbl *t = *(struct Tbl **)(*(char **)(*(char **)data_ov007_0210342c + 0x74) + 0x34);
  struct Obj *o;
  switch (sel) {
  case 0:
    o = t->o[0]; o->fc = 0; o->f8 = o->fc;
    o = t->o[1]; o->fc = 1; o->f8 = o->fc;
    o = t->o[2]; o->fc = 1; o->f8 = o->fc;
    o = t->o[3]; o->fc = 1; o->f8 = o->fc;
    o = t->o[4]; o->fc = 0; o->f8 = o->fc;
    o = t->o[5]; o->fc = 0; o->f8 = o->fc;
    o = t->o[6]; o->fc = 1; o->f8 = o->fc;
    o = t->o[7]; o->fc = 1; o->f8 = o->fc;
    break;
  case 1:
    o = t->o[1]; o->fc = 0; o->f8 = o->fc;
    break;
  case 2:
    o = t->o[0]; o->fc = 1; o->f8 = o->fc;
    break;
  case 3:
    o = t->o[2]; o->fc = 0; o->f8 = o->fc;
    o = t->o[3]; o->fc = 0; o->f8 = o->fc;
    o = t->o[4]; o->fc = 1; o->f8 = o->fc;
    o = t->o[5]; o->fc = 1; o->f8 = o->fc;
    o = t->o[6]; o->fc = 1; o->f8 = o->fc;
    o = t->o[7]; o->fc = 1; o->f8 = o->fc;
    o = t->o[1]; o->fc = 0; o->f8 = o->fc;
    break;
  }
}
