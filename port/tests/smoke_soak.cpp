// Gate-4d soak: EVERY model in the catalog through the game's pipeline.
//
// The single-model gates prove the seams on two assets; this drives all
// ~455 catalog models through Construct -> Model::LoadFile -> DoSetFile ->
// Render, per-model SEH so one bad asset is a tally line instead of the end
// of the run. The point is breadth: every texture format, palette size,
// compression flavor and bone shape the game ships, all through the same
// code path a scene would use.
//
// Per-model resets (the game does these per scene): VRAM cursors, the
// common-model-data registry, the GX. Needs SM64DS_HOST_ARENA_MB=256 --
// ModelComponents allocations intentionally leak (no dtor wiring yet).
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"

#include "ntr/gx.h"
#include "ntr/mmio.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" {
void _ZN5ModelC1Ev(void *self);
struct SharedFilePtrC { u16 fileID; u8 numRefs; void *filePtr; };
SharedFilePtrC *_ZN13SharedFilePtr9ConstructEj(SharedFilePtrC *self, u32 ov0FileID);
void _ZN13SharedFilePtr7ReleaseEv(SharedFilePtrC *self);
void *_ZN4Heap13SetupRootHeapEv(void);
extern Matrix4x3 data_0209b3ec;
/* per-model resets */
extern u32 data_020a4bc8, data_020a4be8, data_020a4be0, data_020a4bdc;
extern u32 data_020a4bcc, data_020a4bd8;
extern int data_0209cef8[];
}

static void ident_fx(void *m)
{
    memset(m, 0, 48);
    ((int *)m)[0] = ((int *)m)[4] = ((int *)m)[8] = 0x1000;
}

static void reset_scene()
{
    ntr::gx_reset();
    NTR_MMIO(uint32_t, 0x04000440) = 0;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000440) = 1;
    NTR_MMIO(uint32_t, 0x04000454) = 0;
    NTR_MMIO(uint32_t, 0x04000580) = 0u | (0u << 8) | (255u << 16) | (191u << 24);
    data_020a4bc8 = 0;
    data_020a4be8 = 0x20000u;
    data_020a4be0 = 0x20000u;
    data_020a4bdc = 0x40000u;
    data_020a4bcc = 0;
    data_020a4bd8 = 0x18000u;
    data_0209cef8[0] = 0;
}

/* POD-only so SEH can unwind it */
static int run_one(unsigned handle, size_t *tris_out)
{
    SharedFilePtrC ptr;
    _ZN13SharedFilePtr9ConstructEj(&ptr, handle);
    static char storage[0x50];
    Model *model = (Model *)storage;
    _ZN5ModelC1Ev(storage);
    void *file = Model::LoadFile(*(SharedFilePtr *)&ptr);
    if (!file) return 1;
    if (model->Model::DoSetFile((char *)file, 0, -1) != 1) return 2;
    ident_fx(&model->mat4x3);
    reset_scene();
    model->Model::Render(NULL);
    ntr::gx_polygons(*tris_out);
    _ZN13SharedFilePtr7ReleaseEv(&ptr);
    return 0;
}

static int run_one_seh(unsigned handle, size_t *tris, unsigned *code)
{
    __try {
        return run_one(handle, tris);
    } __except (1) {
        *code = GetExceptionCode();
        return -1;
    }
}

int main(int argc, char **argv)
{
    if (!ntr::io_init()) { fprintf(stderr, "io_init failed\n"); return 2; }
    if (!_ZN4Heap13SetupRootHeapEv()) { fprintf(stderr, "no heap\n"); return 2; }
    ident_fx(&data_0209b3ec);

    char path[512];
    snprintf(path, sizeof path, "%s/build/assets/handles.tsv",
             getenv("SM64DS_ASSET_ROOT") ? getenv("SM64DS_ASSET_ROOT")
                                         : PORT_REPO_ROOT);
    FILE *f = fopen(path, "r");
    if (!f) { fprintf(stderr, "no catalog at %s\n", path); return 2; }

    const int limit = argc > 1 ? atoi(argv[1]) : 0;
    char line[512];
    fgets(line, sizeof line, f);
    int total = 0, ok = 0, empty = 0, load_fail = 0, faulted = 0;
    size_t tri_sum = 0;
    while (fgets(line, sizeof line, f)) {
        unsigned handle;
        char p[300], kind[40];
        if (sscanf(line, "%u\t%*s\t%*u\t%*s\t%299[^\t]\t%39[^\t]", &handle, p, kind) != 3)
            continue;
        if (strcmp(kind, "model") != 0)
            continue;
        if (limit && total >= limit)
            break;
        ++total;
        size_t tris = 0;
        unsigned code = 0;
        int r = run_one_seh(handle, &tris, &code);
        if (r == -1) {
            ++faulted;
            printf("  FAULT %08x  %s\n", code, p);
        } else if (r != 0) {
            ++load_fail;
            printf("  LOADFAIL(%d)  %s\n", r, p);
        } else if (tris == 0) {
            ++empty;
            printf("  EMPTY  %s\n", p);
        } else {
            ++ok;
            tri_sum += tris;
        }
    }
    fclose(f);

    printf("soak: %d models, %d rendered (%zu tris total), %d empty, "
           "%d load-fail, %d faulted\n",
           total, ok, tri_sum, empty, load_fail, faulted);
    /* the gate: a strong majority renders geometry, nothing faults */
    return (faulted == 0 && ok * 10 >= total * 8) ? 0 : 1;
}
