struct GraphCallbackObj;
typedef void (*VFN)(struct GraphCallbackObj *);
struct GraphCallbackObj {
    VFN *vt;
};
extern struct GraphCallbackObj *data_0209d4a8;
extern unsigned char data_0209d464;
/* The end of the VBlank IRQ (IRQ2.cpp): the scene's GraphCallback3 on
   the dGraph_c::callback_c data_0209d4a8 registers, then clear
   data_0209d464, the flag func_020190b8 raises. */
void func_02019100(void){
    struct GraphCallbackObj *o = data_0209d4a8;
    if (o)
        o->vt[3](o);
    data_0209d464 = 0;
}
