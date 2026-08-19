/* PORT_HOST_ABI. HOST COPY of cMgSmartball_slot_c's RestoreInitial, the third
 * slot of the vtable at ov006:0x0213ed74, ROM address 0x02110154.
 * Run mg5, lane SMBSEAT.
 *
 * WHY THIS FILE EXISTS. Same defect family as
 * port/unmatched/MgSmartball_Slot18.cpp, and found the same way -- by the
 * linker, then confirmed against the ROM. The matched body is correct for ARM
 * and wrong on the host because it drops an argument that rides through in r0.
 *
 * IT IS ALSO FILED UNDER A WRONG RECOVERED NAME, which is why it takes a
 * second to recognise. config/arm9/overlays/ov006/delinks.txt in this tree
 * gives 0x02110154 the file src/_ZN6Player7ST_WAITE.cpp; the decomp's main has
 * since renamed the same block _ZN19cMgSmartball_slot_c14RestoreInitialEv.cpp.
 * Nothing about the body is Player's. The rename is a byte-gated-tree job and
 * is ROUTED, not taken here.
 *
 * THE ROM, read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *     02110154  stmdb sp!, {r4, lr}
 *     02110158  mov   r4, r0            <- r0 saved, and NOT rewritten
 *     0211015c  bl    #0x2114738        <- so the callee gets the object
 *     02110160  mov   r3, #0
 *     ...                               (the zero pass over r4's own fields)
 *
 * 0x02114738 is cMgSmartball_object_c's own RestoreInitial, the base's copy,
 * and src/func_ov006_02114738.c defines it `void func_ov006_02114738(int *p)`
 * -- it reads [p+0x18]/[p+0x1c] and stores them to [p+8]/[p+0xc]. So the
 * argument is not decorative: dropped, the callee reads whatever the host left
 * at [esp+4] and STORES THROUGH IT. A wild write, not a wild read, so no fault
 * is guaranteed and the corruption would be silent.
 *
 * THE SOURCE PAIR, verbatim from the tree:
 *
 *     src/func_ov006_02114738.c:1        void func_ov006_02114738(int *p)
 *     src/_ZN6Player7ST_WAITE.cpp:3      void data_ov034_02114738(void);
 *     src/_ZN6Player7ST_WAITE.cpp:15     data_ov034_02114738();
 *
 * and there are TWO defects in those three lines, not one. Besides the dropped
 * argument, the callee is spelled data_ov034_02114738 -- an ov034 DATA name for
 * an ov006 FUNCTION. No symbol of that name exists in any config; 0x02114738 is
 * ov006 code and config/arm9/overlays/ov006/symbols.txt has no ov034 anything.
 * That is the same name-spelling defect this lane's Faces file records for
 * data_ov004_020bc864, in its worst form: wrong module AND wrong kind. An alias
 * would have fixed the name and left the arity, which is the half that writes.
 *
 * SO THE ANSWER IS THE HOST COPY AND NOT A FACE, exactly as section 6's remedy
 * says: the caller verbatim with the argument placed, and src is left alone.
 * src/_ZN6Player7ST_WAITE.cpp is off port/slice_smb.txt so the link has one
 * definition of the name, the same split MgSmartball_Slot18.cpp makes for
 * src/func_ov006_02118a8c.cpp.
 *
 * THE NAME IS NOT KEPT, AND THAT IS THE THIRD DEFECT AT THIS ADDRESS.
 * The first draft of this file defined the flat name _ZN6Player7ST_WAITE, on
 * the reasoning that a host copy keeps the caller's spelling.
 * port/tools/alternatename_guard.py refused it in the same build, and the
 * refusal is right:
 *
 *     hal/cxx_aliases.cpp:895
 *       /alternatename:__ZN6Player7ST_WAITE=_data_ov002_02110154
 *       LHS at 0001:0005fe20, RHS 0004:00001864
 *
 * That alias has been in the tree since lane w8-shadows and its own banner
 * spells out why: ov002 and ov006 SHARE A DS OVERLAY LOAD WINDOW, so both
 * cover 0x02110154 and only one is resident at a time on hardware. ov006's
 * symbols.txt calls the address a function; the byte ov002's caller needs
 * there is the Wait State object, _data_ov002_02110154. In the port every
 * overlay is resident at once, so a definition of the flat name does not
 * replace that routing, it DEFEATS it silently -- the R1/R2 arrival shape.
 *
 * So the body gets a port-local name, hal/scene_mg.cpp's kSmartballObjFaces
 * keys the slot on the ROM word 0x02110154 and calls that name, and the ov002
 * alias is left exactly as it was. This is the one place in this seat where
 * keeping the tree's spelling would have been the wrong move, and nothing
 * short of the guard would have said so.
 *
 * THE FIELD LAYOUT BELOW IS src/_ZN6Player7ST_WAITE.cpp's OWN, character for
 * character. Nothing here is re-derived and nothing is improved; the only edit
 * is the argument on the call.
 */

extern "C" {

/* the real arity and the real name, from the definition */
void func_ov006_02114738(int *p);

struct P {
  char p0[0x34];
  struct { int a,b; } A[3];
  struct { int c,d; } B[3];
  int C[3];
  char D[3];
  char e73,e74,e75;
  char pad[2];
  int F78,F7c,F80,F84;
};

void port_mg_smartball_slot_restore(void* c){
  struct P* r4=(struct P*)c;
  func_ov006_02114738((int *)c);
  for(int i=0;i<3;i++){
    r4->A[i].a=0;
    r4->A[i].b=0;
    r4->B[i].c=0;
    r4->B[i].d=0;
    r4->C[i]=0;
    r4->D[i]=0;
  }
  r4->e73=0;
  r4->e74=0;
  r4->e75=0;
  r4->F78=0;
  r4->F7c=0;
  r4->F80=0;
  r4->F84=0;
}

}  /* extern "C" */
