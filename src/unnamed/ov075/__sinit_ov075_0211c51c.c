/* __sinit_ov075_0211c51c -- the static initializer of the entry-menu mesh's
 * unit, src/game/stages/d_s_entry_entry.cpp. It constructs the mesh's shared
 * texture file handle (file 0x8418), registers the handle's destructor, and
 * fills the handler row EntryMesh::Init, Behavior and Render call through:
 * [0] InitGrid, [1] UpdateRipple, [2] DrawGrid. The three sources are the
 * compiler's pointer-to-member literals, so they keep their address names.
 * Blocker: compiler-emitting it needs d_s_entry_entry.cpp promoted to an intact
 * object owning .data 0x0211d34c..0x0211d364, .bss 0x0211d980..0x0211d9ac and
 * this .init entry. */

/* An EntryMesh member-function pointer as the ARM C++ ABI lays it out. */
typedef struct EntryMeshHandler {
    void *func;
    int thisAdjust;
} EntryMeshHandler;

extern void *func_020178cc(void *file, unsigned int fileID);
extern int func_020178b4(int file);
extern void func_020731dc(void *object, void *dtor, void **link);

extern struct SharedFilePtr EntryMeshTextureFile; /* include/SharedFilePtr.h */
extern void *EntryMeshTextureFileLink[];
extern EntryMeshHandler EntryMeshHandlers[3];

extern EntryMeshHandler data_ov075_0211d34c; /* &EntryMesh::UpdateRipple */
extern EntryMeshHandler data_ov075_0211d354; /* &EntryMesh::InitGrid */
extern EntryMeshHandler data_ov075_0211d35c; /* &EntryMesh::DrawGrid */

void __sinit_ov075_0211c51c(void) {
    func_020178cc(&EntryMeshTextureFile, 0x8418);
    func_020731dc(&EntryMeshTextureFile, (void *)func_020178b4, EntryMeshTextureFileLink);
    EntryMeshHandlers[0] = data_ov075_0211d354;
    EntryMeshHandlers[1] = data_ov075_0211d34c;
    EntryMeshHandlers[2] = data_ov075_0211d35c;
}
