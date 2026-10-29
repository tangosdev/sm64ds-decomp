/* __sinit_ov075_0211bb00 -- the static initializer of dScEntry_c's unit. It
 * constructs the scene's graph callback, sets each of the unit's halfword-pair
 * globals and registers its empty destructor, and fills the step sets (enter, behavior,
 * render) that dScEntry_c::SetSteps installs. The steps are copied from the
 * compiler's pointer-to-member literals, so those keep their address names.
 * Blocker: compiler-emitting it needs dScEntry_c's unit joined into one intact
 * object; it is split between intact d_s_entry.cpp and text-only
 * d_s_entry_methods.cpp, and neither owns the .bss step sets. */
#pragma opt_common_subs off

/* dScEntry_c::Step, a member-function pointer as the ARM C++ ABI lays it out. */
struct EntryStep {
    int func;
    int thisAdjust;
};

/* dScEntry_c::StepSet. */
struct EntryStepSet {
    struct EntryStep enter;
    struct EntryStep behavior;
    struct EntryStep render;
};

extern void *_ZN10dScEntry_c15graphCallback_cC1Ev(void *);
extern void func_020731dc(void *, void *, void **);
extern void NoOpDestructor(void);

extern char data_ov075_0211d71c[];

extern int data_ov075_0211d5c0;
extern int data_ov075_0211d5cc;
extern int data_ov075_0211d5d8;
extern int data_ov075_0211d5e4;
extern int data_ov075_0211d5f0;
extern int data_ov075_0211d5fc;
extern int data_ov075_0211d608;
extern int data_ov075_0211d614;
extern int data_ov075_0211d620;
extern int data_ov075_0211d62c;
extern int data_ov075_0211d638;
extern int data_ov075_0211d644;
extern int data_ov075_0211d650;
extern int data_ov075_0211d65c;
extern int data_ov075_0211d668;
extern int data_ov075_0211d674;
extern int data_ov075_0211d680;
extern int data_ov075_0211d68c;
extern int data_ov075_0211d698;
extern int data_ov075_0211d6a4;
extern int data_ov075_0211d6b0;
extern int data_ov075_0211d6bc;
extern int data_ov075_0211d6c8;
extern int data_ov075_0211d6d4;
extern int data_ov075_0211d6e0;
extern int data_ov075_0211d6ec;
extern int data_ov075_0211d6f8;
extern int data_ov075_0211d704;
extern int data_ov075_0211d710;

extern int data_ov075_0211d72c;
extern int data_ov075_0211d730;
extern int data_ov075_0211d734;
extern int data_ov075_0211d738;
extern int data_ov075_0211d73c;
extern int data_ov075_0211d740;
extern int data_ov075_0211d744;
extern int data_ov075_0211d748;
extern int data_ov075_0211d74c;
extern int data_ov075_0211d750;
extern int data_ov075_0211d754;
extern int data_ov075_0211d758;
extern int data_ov075_0211d75c;
extern int data_ov075_0211d760;
extern int data_ov075_0211d764;

extern int data_ov075_0211d948;
extern int data_ov075_0211d94c;
extern int data_ov075_0211d950;
extern int data_ov075_0211d954;
extern int data_ov075_0211d958;
extern int data_ov075_0211d95c;
extern int data_ov075_0211d960;
extern int data_ov075_0211d964;
extern int data_ov075_0211d968;
extern int data_ov075_0211d96c;
extern int data_ov075_0211d970;
extern int data_ov075_0211d974;
extern int data_ov075_0211d978;
extern int data_ov075_0211d97c;

extern struct EntryStep data_02086b58;
extern struct EntryStep data_ov075_0211c6f8;
extern struct EntryStep data_ov075_0211c700;
extern struct EntryStep data_ov075_0211c718;
extern struct EntryStep data_ov075_0211c728;
extern struct EntryStep data_ov075_0211c738;
extern struct EntryStep data_ov075_0211c740;
extern struct EntryStep data_ov075_0211c748;
extern struct EntryStep data_ov075_0211c750;
extern struct EntryStep data_ov075_0211c758;
extern struct EntryStep data_ov075_0211c760;
extern struct EntryStep data_ov075_0211c768;
extern struct EntryStep data_ov075_0211c770;
extern struct EntryStep data_ov075_0211c780;
extern struct EntryStep data_ov075_0211c798;
extern struct EntryStep data_ov075_0211c7a0;
extern struct EntryStep data_ov075_0211c7a8;
extern struct EntryStep data_ov075_0211c7b0;
extern struct EntryStep data_ov075_0211c7b8;
extern struct EntryStep data_ov075_0211c7c8;
extern struct EntryStep data_ov075_0211c7d8;
extern struct EntryStep data_ov075_0211c7e8;
extern struct EntryStep data_ov075_0211c7f0;
extern struct EntryStep data_ov075_0211c800;
extern struct EntryStep data_ov075_0211c808;
extern struct EntryStep data_ov075_0211c810;
extern struct EntryStep data_ov075_0211c818;
extern struct EntryStep data_ov075_0211c820;
extern struct EntryStep data_ov075_0211c828;
extern struct EntryStep data_ov075_0211c830;
extern struct EntryStep data_ov075_0211c838;
extern struct EntryStep data_ov075_0211c840;
extern struct EntryStep data_ov075_0211c848;
extern struct EntryStep data_ov075_0211c850;
extern struct EntryStep data_ov075_0211c858;
extern struct EntryStep data_ov075_0211c860;
extern struct EntryStep data_ov075_0211c868;
extern struct EntryStep data_ov075_0211c878;
extern struct EntryStep data_ov075_0211c888;
extern struct EntryStep data_ov075_0211c898;
extern struct EntryStep data_ov075_0211c8a0;
extern struct EntryStep data_ov075_0211c8a8;
extern struct EntryStep data_ov075_0211c8b8;
extern struct EntryStep data_ov075_0211c8c0;
extern struct EntryStep data_ov075_0211c8c8;
extern struct EntryStep data_ov075_0211c8d0;
extern struct EntryStep data_ov075_0211c8d8;

extern struct EntryStepSet EntryPlayerSelectIntroSteps;
extern struct EntryStepSet EntryCourseSelectSteps;
extern struct EntryStepSet data_ov075_0211d798;
extern struct EntryStepSet EntryMatchLoadingSteps;
extern struct EntryStepSet data_ov075_0211d7c8;
extern struct EntryStepSet data_ov075_0211d7e0;
extern struct EntryStepSet data_ov075_0211d7f8;
extern struct EntryStepSet data_ov075_0211d810;
extern struct EntryStepSet data_ov075_0211d828;
extern struct EntryStepSet EntryDelaySteps;
extern struct EntryStepSet EntryFadeOutSteps;
extern struct EntryStepSet data_ov075_0211d870;
extern struct EntryStepSet data_ov075_0211d888;
extern struct EntryStepSet data_ov075_0211d8a0;
extern struct EntryStepSet data_ov075_0211d8b8;
extern struct EntryStepSet data_ov075_0211d8d0;
extern struct EntryStepSet data_ov075_0211d8e8;
extern struct EntryStepSet data_ov075_0211d900;
extern struct EntryStepSet data_ov075_0211d918;
extern struct EntryStepSet data_ov075_0211d930;

void __sinit_ov075_0211bb00(void) {
    short *p;
    struct EntryStep tmp;
    int ta, tb;

    _ZN10dScEntry_c15graphCallback_cC1Ev(data_ov075_0211d71c);

    p = (short *)(&data_ov075_0211d948);
    ((short *)(&data_ov075_0211d948))[0] = 0x80;
    ((short *)(&data_ov075_0211d948))[1] = 0x10;
    func_020731dc((short *)(&data_ov075_0211d948), (void *)NoOpDestructor,
                  (void **)&data_ov075_0211d5fc);

    p = (short *)(&data_ov075_0211d94c);
    p[0] = 0x3c;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6bc);
    p = (short *)(&data_ov075_0211d950);
    p[0] = 0x1c;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6c8);

    p = (short *)(&data_ov075_0211d954);
    p[0] = 8;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6e0);
    p = (short *)(&data_ov075_0211d958);
    p[0] = 8;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6a4);
    p = (short *)(&data_ov075_0211d95c);
    p[0] = 0x20;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6ec);
    p = (short *)(&data_ov075_0211d960);
    p[0] = 0x30;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6f8);
    p = (short *)(&data_ov075_0211d964);
    p[0] = 0x20;
    p[1] = 0x20;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d710);
    p = (short *)(&data_ov075_0211d968);
    p[0] = 0x28;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d698);
    p = (short *)(&data_ov075_0211d96c);
    p[0] = 0x58;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d5c0);
    p = (short *)(&data_ov075_0211d970);
    p[0] = 0x78;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d5cc);
    p = (short *)(&data_ov075_0211d974);
    p[0] = 0x20;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d5d8);
    p = (short *)(&data_ov075_0211d978);
    p[0] = 0xa;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6b0);
    p = (short *)(&data_ov075_0211d97c);
    p[0] = 0;
    p[1] = 0;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d5e4);

    {
        int sz = 0x20;
        int h = 0x10;
        ((short *)(&data_ov075_0211d72c))[0] = (short)sz;
        ((short *)(&data_ov075_0211d72c))[1] = (short)h;
        func_020731dc((short *)(&data_ov075_0211d72c), (void *)NoOpDestructor,
                      (void **)&data_ov075_0211d5f0);
    }

    p = (short *)(&data_ov075_0211d730);
    p[0] = 0x18;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d68c);
    p = (short *)(&data_ov075_0211d734);
    p[0] = 0x1c;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d608);
    p = (short *)(&data_ov075_0211d738);
    p[0] = 0x1c;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d614);
    p = (short *)(&data_ov075_0211d73c);
    p[0] = 0x20;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d620);

    {
        int sz = 0x30;
        int h = 0x10;
        ((short *)(&data_ov075_0211d740))[0] = (short)sz;
        ((short *)(&data_ov075_0211d740))[1] = (short)h;
        func_020731dc((short *)(&data_ov075_0211d740), (void *)NoOpDestructor,
                      (void **)&data_ov075_0211d62c);
    }

    p = (short *)(&data_ov075_0211d744);
    p[0] = 0x30;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d638);
    p = (short *)(&data_ov075_0211d748);
    p[0] = 0x30;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d644);
    p = (short *)(&data_ov075_0211d74c);
    p[0] = 0x30;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d704);
    p = (short *)(&data_ov075_0211d750);
    p[0] = 0x18;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d650);

    {
        int sz = 0x20;
        int h = 0x10;
        ((short *)(&data_ov075_0211d754))[0] = (short)sz;
        ((short *)(&data_ov075_0211d754))[1] = (short)h;
        func_020731dc((short *)(&data_ov075_0211d754), (void *)NoOpDestructor,
                      (void **)&data_ov075_0211d65c);
    }

    p = (short *)(&data_ov075_0211d758);
    p[0] = 0x20;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d6d4);
    p = (short *)(&data_ov075_0211d75c);
    p[0] = 0x28;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d668);
    p = (short *)(&data_ov075_0211d760);
    p[0] = 0x20;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d674);
    p = (short *)(&data_ov075_0211d764);
    p[0] = 0x30;
    p[1] = 0x10;
    func_020731dc(p, (void *)NoOpDestructor, (void **)&data_ov075_0211d680);

    tmp = data_02086b58;
    ta = tmp.func;
    tb = tmp.thisAdjust;

    EntryDelaySteps.enter = tmp;
    EntryDelaySteps.render.func = tb ? ta : ta;
    EntryDelaySteps.render.thisAdjust = tb;
    EntryDelaySteps.behavior = data_ov075_0211c8d0;

    EntryFadeOutSteps.render.func = tb ? ta : ta;
    EntryFadeOutSteps.render.thisAdjust = tb;
    EntryFadeOutSteps.enter = data_ov075_0211c700;
    EntryFadeOutSteps.behavior = data_ov075_0211c750;

    data_ov075_0211d888.enter = data_ov075_0211c850;
    data_ov075_0211d888.behavior = data_ov075_0211c828;
    data_ov075_0211d888.render = data_ov075_0211c8c8;

    data_ov075_0211d8a0.render.func = tb ? ta : ta;
    data_ov075_0211d8a0.render.thisAdjust = tb;
    data_ov075_0211d8a0.enter = data_ov075_0211c6f8;
    data_ov075_0211d8a0.behavior = data_ov075_0211c7d8;

    data_ov075_0211d8d0.enter = data_ov075_0211c8c0;
    data_ov075_0211d8d0.behavior = data_ov075_0211c8d8;
    data_ov075_0211d8d0.render = data_ov075_0211c718;

    data_ov075_0211d900.render.func = tb ? ta : ta;
    data_ov075_0211d900.render.thisAdjust = tb;
    data_ov075_0211d900.enter = data_ov075_0211c7b0;
    data_ov075_0211d900.behavior = data_ov075_0211c8b8;

    data_ov075_0211d918.enter = data_ov075_0211c878;
    data_ov075_0211d918.behavior = data_ov075_0211c760;
    data_ov075_0211d918.render = data_ov075_0211c780;

    EntryPlayerSelectIntroSteps.render.func = tb ? ta : ta;
    EntryPlayerSelectIntroSteps.render.thisAdjust = tb;
    EntryPlayerSelectIntroSteps.enter = data_ov075_0211c810;
    EntryPlayerSelectIntroSteps.behavior = data_ov075_0211c7e8;

    EntryCourseSelectSteps.enter = data_ov075_0211c768;
    EntryCourseSelectSteps.behavior = data_ov075_0211c898;
    EntryCourseSelectSteps.render = data_ov075_0211c738;

    EntryMatchLoadingSteps.render.func = tb ? ta : ta;
    EntryMatchLoadingSteps.render.thisAdjust = tb;
    EntryMatchLoadingSteps.enter = data_ov075_0211c8a8;
    EntryMatchLoadingSteps.behavior = data_ov075_0211c840;

    data_ov075_0211d7c8.enter = tmp;
    data_ov075_0211d7c8.render.func = tb ? ta : ta;
    data_ov075_0211d7c8.render.thisAdjust = tb;
    data_ov075_0211d7c8.behavior = data_ov075_0211c7b8;

    data_ov075_0211d7e0.enter = data_ov075_0211c728;
    data_ov075_0211d7e0.behavior = data_ov075_0211c830;
    data_ov075_0211d7e0.render = data_ov075_0211c860;

    data_ov075_0211d810.enter = data_ov075_0211c818;
    data_ov075_0211d810.behavior = data_ov075_0211c770;
    data_ov075_0211d810.render = data_ov075_0211c888;

    data_ov075_0211d870.enter = data_ov075_0211c858;
    data_ov075_0211d870.behavior = data_ov075_0211c808;
    data_ov075_0211d870.render = data_ov075_0211c798;

    data_ov075_0211d8b8.render.func = tb ? ta : ta;
    data_ov075_0211d8b8.render.thisAdjust = tb;
    data_ov075_0211d8b8.enter = data_ov075_0211c7a8;
    data_ov075_0211d8b8.behavior = data_ov075_0211c7c8;

    data_ov075_0211d8e8.enter = data_ov075_0211c7f0;
    data_ov075_0211d8e8.render.func = tb ? ta : ta;
    data_ov075_0211d8e8.render.thisAdjust = tb;
    data_ov075_0211d8e8.behavior = data_ov075_0211c748;

    data_ov075_0211d930.behavior = tmp;
    data_ov075_0211d930.render.func = tb ? ta : ta;
    data_ov075_0211d930.render.thisAdjust = tb;
    data_ov075_0211d930.enter = data_ov075_0211c8a0;

    data_ov075_0211d798.enter = data_ov075_0211c758;
    data_ov075_0211d798.behavior = data_ov075_0211c740;
    data_ov075_0211d798.render = data_ov075_0211c7a0;

    data_ov075_0211d7f8.enter = data_ov075_0211c868;
    data_ov075_0211d7f8.behavior = data_ov075_0211c800;
    data_ov075_0211d7f8.render = data_ov075_0211c838;

    data_ov075_0211d828.enter = data_ov075_0211c848;
    data_ov075_0211d828.behavior = data_ov075_0211c820;
    data_ov075_0211d828.render.func = tb ? ta : ta;
    data_ov075_0211d828.render.thisAdjust = tb;
}
