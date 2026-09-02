/* HOST COPY of src/func_02016ff4.c -- Model::LoadAndSetFile's middle, with
 * its shadow-class virtual call resolved to the slot it names.
 *
 * THE DUAL-NUMBERING HAZARD, and this is the first thing to walk into it.
 * The matched source dispatches through a LOCAL shadow class:
 *
 *     struct Thing { virtual void v0(); virtual void v1();
 *                    virtual int v2(BMD_File *, int, int); };
 *     ret = self->v2(file, c, d);
 *
 * Three virtuals, so the call is vtable slot 2 -- and in ROM/Itanium
 * numbering that IS DoSetFile, because a destructor occupies TWO slots
 * (D1 at 0, D0 at 1). MSVC gives a destructor ONE slot, so the host's
 * _ZTV5Model has DoSetFile at 1 and UpdateVerts at 2, and the shadow's
 * "slot 2" lands on UpdateVerts instead: a fresh Model with no components
 * yet, walking a null vertex list. That is a null-pointer fault inside the
 * first Tree's InitResources, which is exactly where it turned up.
 *
 * hal/cxxname_bridge.cpp already names the same skew for Render (slot 4
 * under Model.h, slot 5 for a shadow TU) and serves both by filling the
 * table twice. That trick cannot work here -- slot 2 has two different
 * meanings and both are live -- so the resolution moves to the caller, the
 * way the four Process wrappers resolve their mwcc pointer-to-members.
 *
 * DoSetFile is slot 1 in every host model table (_ZTV5Model, _ZTV9ModelAnim,
 * _ZTV10ModelAnim2), which is what makes one line enough.
 */
extern "C" {

struct BMD_File;
void _ZN5Model17UpdateFileOffsetsER8BMD_File(BMD_File *file);
int func_02017060(BMD_File *file);
int port_model_shrink_enabled(void);

typedef int(__fastcall *ModelDoSetFile)(void *self, void *dummy,
                                        BMD_File *file, int a, int b);

// PORT_HOST_ABI: the shadow class's slot 2 is DoSetFile only under ROM numbering where the destructor takes two slots; MSVC folds it to one slot so slot 2 lands on UpdateVerts, so this reads host slot 1
int func_02016ff4(void *self, BMD_File *file, int c, int d)
{
    int ret;
    if (!file)
        return 0;
    _ZN5Model17UpdateFileOffsetsER8BMD_File(file);
    ret = ((ModelDoSetFile)(*(void ***)self)[1])(self, 0, file, c, d);
    if (ret && port_model_shrink_enabled())
        func_02017060(file);
    return ret;
}

}
