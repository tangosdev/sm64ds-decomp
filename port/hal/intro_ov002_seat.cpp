/* THE ov002 MOUNT'S POINTER WORDS, seated with HOST addresses.
 *
 * port/ov002_syms.txt BYTE-COPIES. This lane added twenty ov002 symbols to
 * it during the cast-seating round because the linker named them, and seven
 * of those are POINTER ARRAYS -- so they carried DS addresses into the host.
 * The first one dereferenced handed SharedFilePtr::Load the DS address
 * 0x02130744 and the port's fs refused it by name:
 *     FATAL: fs fileID 0 not in catalog (fileptr 02130744)
 * reached through src/func_ov002_020f6618.cpp:26, which dereferences the
 * `SharedFilePtr** anims` array CutsceneObject::InitResources passes as
 * &data_ov002_0210b60c. relocs.txt names it exactly:
 *     from:0x0210b60c kind:load to:0x02130744 module:overlay(85)
 * The handle it points AT was always fine -- it reads fileID 291 at every
 * checkpoint. It was the POINTER that was a DS address.
 *
 * This is the same class romdata.py's header banks for data_020876e4, and
 * the same fix this lane already applied on the arm9 side in
 * hal/intro_script_records.cpp. ptr_audit.py does not cover it because it
 * audits romdata's NAMED list, not the per-overlay symbol mounts.
 *
 * GENERATED, not hand-written: the closure is 72 words over 88 symbols and
 * hand-picking seven of them is how this was missed the first time. Each
 * word is CHECKED against the ROM address relocs.txt gives before it is
 * rewritten, so a mount that ever changes shape aborts instead of being
 * silently reinterpreted.
 *
 * NOT HERE: data_ov002_0210bae4 and data_ov002_0210bcc4. Those are the Model
 * and ModelAnim vtables and co_seat_model_vtable() in
 * hal/actor_classes_intro.cpp already seats them in MSVC numbering; seating
 * them here in ROM order would undo that fix.
 */
#include <cstdio>
#include <cstdlib>

extern "C" {

void CutsceneObject_Spawn(void);
extern unsigned char data_ov002_0210b600[];
extern unsigned char data_ov002_0210b604[];
extern unsigned char data_ov002_0210b608[];
extern unsigned char data_ov002_0210b60c[];
extern unsigned char data_ov002_0210b610[];
extern unsigned char CutsceneObject_SpawnInfo[];
extern unsigned char data_ov002_0210bcf0[];
extern unsigned char data_ov002_0210bd24[];
extern unsigned char data_ov002_02110b10[];
extern unsigned char data_ov002_02110b18[];
extern unsigned char data_ov002_02110b20[];
extern unsigned char data_ov002_02110b28[];
extern unsigned char data_ov002_02110b30[];
extern unsigned char data_ov002_02110b38[];
extern unsigned char data_ov002_02110b40[];
extern unsigned char data_ov002_02110b48[];
extern unsigned char data_ov002_02110b58[];
extern unsigned char data_ov002_02110b60[];
extern unsigned char data_ov002_02110b68[];
extern unsigned char data_ov002_02110b80[];
extern unsigned char data_ov002_02110b88[];
extern unsigned char data_ov002_02110b90[];
extern unsigned char data_ov002_02110ba0[];
extern unsigned char data_ov002_02110ba8[];
extern unsigned char data_ov002_02110bb0[];
extern unsigned char data_ov002_02110bb8[];
extern unsigned char data_ov002_02110bc0[];
extern unsigned char data_ov002_02110bd0[];
extern unsigned char data_ov002_02110bd8[];
extern unsigned char data_ov002_02110be0[];
extern unsigned char data_ov002_02110be8[];
extern unsigned char data_ov002_02110bf0[];
extern unsigned char data_ov002_02110bf8[];
extern unsigned char data_ov002_02110c00[];
extern unsigned char data_ov002_02110c08[];
extern unsigned char data_ov002_02110c10[];
extern unsigned char data_ov009_02113c28[];
extern unsigned char data_ov085_0213073c[];
extern unsigned char data_ov085_02130744[];

static const struct { unsigned char *holder; unsigned off;
                     unsigned rom; void *host; const char *what; }
g_ov002_ptrs[] = {
    { data_ov002_0210b600, 0x0, 0x02110b10, (void *)&data_ov002_02110b10, "data_ov002_0210b600+0x0" },
    { data_ov002_0210b604, 0x0, 0x02113c28, (void *)&data_ov009_02113c28, "data_ov002_0210b604+0x0" },
    { data_ov002_0210b608, 0x0, 0x0213073c, (void *)&data_ov085_0213073c, "data_ov002_0210b608+0x0" },
    { data_ov002_0210b60c, 0x0, 0x02130744, (void *)&data_ov085_02130744, "data_ov002_0210b60c+0x0" },
    { data_ov002_0210b610, 0x0, 0x02110bf8, (void *)&data_ov002_02110bf8, "data_ov002_0210b610+0x0" },
    { CutsceneObject_SpawnInfo, 0x0, 0x020f8808, (void *)&CutsceneObject_Spawn, "CutsceneObject_SpawnInfo+0x0" },
    { data_ov002_0210bcf0, 0x0, 0x02110c10, (void *)&data_ov002_02110c10, "data_ov002_0210bcf0+0x0" },
    { data_ov002_0210bcf0, 0x4, 0x02110bd0, (void *)&data_ov002_02110bd0, "data_ov002_0210bcf0+0x4" },
    { data_ov002_0210bcf0, 0x8, 0x02110b28, (void *)&data_ov002_02110b28, "data_ov002_0210bcf0+0x8" },
    { data_ov002_0210bcf0, 0xc, 0x02110bf0, (void *)&data_ov002_02110bf0, "data_ov002_0210bcf0+0xc" },
    { data_ov002_0210bcf0, 0x10, 0x02110b68, (void *)&data_ov002_02110b68, "data_ov002_0210bcf0+0x10" },
    { data_ov002_0210bcf0, 0x14, 0x02110ba0, (void *)&data_ov002_02110ba0, "data_ov002_0210bcf0+0x14" },
    { data_ov002_0210bcf0, 0x18, 0x02110bb0, (void *)&data_ov002_02110bb0, "data_ov002_0210bcf0+0x18" },
    { data_ov002_0210bcf0, 0x1c, 0x02110b48, (void *)&data_ov002_02110b48, "data_ov002_0210bcf0+0x1c" },
    { data_ov002_0210bcf0, 0x20, 0x02110b30, (void *)&data_ov002_02110b30, "data_ov002_0210bcf0+0x20" },
    { data_ov002_0210bcf0, 0x24, 0x02110be8, (void *)&data_ov002_02110be8, "data_ov002_0210bcf0+0x24" },
    { data_ov002_0210bcf0, 0x28, 0x02110c08, (void *)&data_ov002_02110c08, "data_ov002_0210bcf0+0x28" },
    { data_ov002_0210bcf0, 0x2c, 0x02110c00, (void *)&data_ov002_02110c00, "data_ov002_0210bcf0+0x2c" },
    { data_ov002_0210bcf0, 0x30, 0x02110bd8, (void *)&data_ov002_02110bd8, "data_ov002_0210bcf0+0x30" },
    { data_ov002_0210bd24, 0x0, 0x02110b60, (void *)&data_ov002_02110b60, "data_ov002_0210bd24+0x0" },
    { data_ov002_0210bd24, 0x4, 0x02110b90, (void *)&data_ov002_02110b90, "data_ov002_0210bd24+0x4" },
    { data_ov002_0210bd24, 0x8, 0x02110b58, (void *)&data_ov002_02110b58, "data_ov002_0210bd24+0x8" },
    { data_ov002_0210bd24, 0xc, 0x02110b88, (void *)&data_ov002_02110b88, "data_ov002_0210bd24+0xc" },
    { data_ov002_0210bd24, 0x10, 0x02110b38, (void *)&data_ov002_02110b38, "data_ov002_0210bd24+0x10" },
    { data_ov002_0210bd24, 0x14, 0x02110ba8, (void *)&data_ov002_02110ba8, "data_ov002_0210bd24+0x14" },
    { data_ov002_0210bd24, 0x18, 0x02110bc0, (void *)&data_ov002_02110bc0, "data_ov002_0210bd24+0x18" },
    { data_ov002_0210bd24, 0x1c, 0x02110b80, (void *)&data_ov002_02110b80, "data_ov002_0210bd24+0x1c" },
    { data_ov002_0210bd24, 0x20, 0x02110b20, (void *)&data_ov002_02110b20, "data_ov002_0210bd24+0x20" },
    { data_ov002_0210bd24, 0x24, 0x02110be0, (void *)&data_ov002_02110be0, "data_ov002_0210bd24+0x24" },
    { data_ov002_0210bd24, 0x28, 0x02110b40, (void *)&data_ov002_02110b40, "data_ov002_0210bd24+0x28" },
    { data_ov002_0210bd24, 0x2c, 0x02110b18, (void *)&data_ov002_02110b18, "data_ov002_0210bd24+0x2c" },
    { data_ov002_0210bd24, 0x30, 0x02110bb8, (void *)&data_ov002_02110bb8, "data_ov002_0210bd24+0x30" },
};

void port_intro_seat_ov002_ptrs(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    unsigned seated = 0, already = 0, foreign = 0;
    for (unsigned i = 0; i < sizeof g_ov002_ptrs / sizeof g_ov002_ptrs[0]; ++i) {
        unsigned *w = (unsigned *)(g_ov002_ptrs[i].holder + g_ov002_ptrs[i].off);
        if (*w == g_ov002_ptrs[i].rom) {
            /* still the ROM's DS address: this is one the per-overlay
               emitter left raw, which is the CROSS-OVERLAY set. Seat it. */
            *w = (unsigned)(size_t)g_ov002_ptrs[i].host;
            ++seated;
        } else if (*w == (unsigned)(size_t)g_ov002_ptrs[i].host) {
            /* already the right host address -- the emitter relocated it
               because the target is inside ov002 itself. Leave it. */
            ++already;
        } else if (*w >= 0x02000000u) {
            /* a DS address that is NOT the one relocs.txt gives: the mount
               has changed shape under us and this seat no longer describes
               it. Refuse rather than write over it. */
            std::fprintf(stderr, "FATAL: ov002 mount %s: holds %08x, the "
                         "ROM's own reloc says %08x -- WRONG BYTES\n",
                         g_ov002_ptrs[i].what, *w, g_ov002_ptrs[i].rom);
            std::abort();
        } else {
            /* some other host address: the emitter or another seat owns
               this word. Not ours to rewrite. */
            ++foreign;
        }
    }
    if (std::getenv("SM64DS_INTRO_WATCH"))
        std::fprintf(stderr, "  [intro] ov002 pointer seat: %u seated, "
                     "%u already relocated by the emitter, %u foreign\n",
                     seated, already, foreign);
}

}  /* extern "C" */
