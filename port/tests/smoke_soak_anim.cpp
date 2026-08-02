// Gate-5b soak: every compatible animation through the game's sampler.
//
// 481 catalog BCAs sit next to a model; for each one this finds a sibling
// BMD with a MATCHING BONE COUNT (the game never plays an animation onto a
// mismatched skeleton, so neither does the soak), then runs the game's
// per-frame path -- UpdateBones at first/middle/last frame, re-skin,
// render -- with per-pair SEH. Breadth is the point: every track shape,
// channel layout and keyframe encoding Nintendo shipped goes through
// func_020453c0/func_0204547c and both keyframe samplers.
//
// Needs SM64DS_HOST_ARENA_MB=512: file images and ModelComponents leak by
// design (no dtor wiring), and this loads ~500 pairs.
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "Model.h"
#include "Animation.h"

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
void _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(void *c, void *bca, int frame);
void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void *c);
extern Matrix4x3 data_0209b3ec;
extern u32 data_020a4bc8, data_020a4be8, data_020a4be0, data_020a4bdc;
extern u32 data_020a4bcc, data_020a4bd8;
extern int data_0209cef8[];
}

enum { MAX_ROWS = 4096 };
struct Row { unsigned handle; char path[300]; char kind[24]; };
static Row g_rows[MAX_ROWS];
static int g_nrows;

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

/* POD-only for SEH */
static int run_pair(unsigned mh, unsigned ah, size_t *tris, int *frames_out)
{
    SharedFilePtrC mp;
    _ZN13SharedFilePtr9ConstructEj(&mp, mh);
    static char storage[0x50];
    Model *model = (Model *)storage;
    _ZN5ModelC1Ev(storage);
    reset_scene();
    void *file = Model::LoadFile(*(SharedFilePtr *)&mp);
    if (!file) return 1;
    unsigned model_bones = *(unsigned *)((char *)file + 4);

    SharedFilePtrC ap;
    _ZN13SharedFilePtr9ConstructEj(&ap, ah);
    void *bca = Animation::LoadFile(*(SharedFilePtr *)&ap);
    if (!bca) { _ZN13SharedFilePtr7ReleaseEv(&mp); return 1; }
    unsigned anim_bones = *(unsigned short *)bca;
    int num_frames = *(unsigned short *)((char *)bca + 2);
    *frames_out = num_frames;
    if (anim_bones != model_bones || num_frames <= 0) {
        _ZN13SharedFilePtr7ReleaseEv(&ap);
        _ZN13SharedFilePtr7ReleaseEv(&mp);
        return 3;   /* incompatible: skeleton mismatch, skip */
    }

    if (model->Model::DoSetFile((char *)file, 0, -1) != 1) return 2;
    ident_fx(&model->mat4x3);

    int probes[3] = { 0, num_frames / 2, num_frames - 1 };
    for (int k = 0; k < 3; ++k) {
        _ZN15ModelComponents11UpdateBonesEP8BCA_Filei(&model->data, bca, probes[k]);
        _ZN15ModelComponents21UpdateVertsUsingBonesEv(&model->data);
        reset_scene();
        model->Model::Render(NULL);
    }
    ntr::gx_polygons(*tris);
    _ZN13SharedFilePtr7ReleaseEv(&ap);
    _ZN13SharedFilePtr7ReleaseEv(&mp);
    return 0;
}

static int run_pair_seh(unsigned mh, unsigned ah, size_t *tris, int *fr, unsigned *code)
{
    __try {
        return run_pair(mh, ah, tris, fr);
    } __except (1) {
        *code = GetExceptionCode();
        return -1;
    }
}

static const char *dir_of(const char *p, char *buf)
{
    strcpy(buf, p);
    char *s = strrchr(buf, '/');
    if (s) *s = 0; else buf[0] = 0;
    return buf;
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
    char line[512];
    fgets(line, sizeof line, f);
    while (fgets(line, sizeof line, f) && g_nrows < MAX_ROWS) {
        Row *r = &g_rows[g_nrows];
        if (sscanf(line, "%u\t%*s\t%*u\t%*s\t%299[^\t]\t%23[^\t]",
                   &r->handle, r->path, r->kind) == 3)
            ++g_nrows;
    }
    fclose(f);

    const int limit = argc > 1 ? atoi(argv[1]) : 0;
    int total = 0, ok = 0, skipped = 0, load_fail = 0, faulted = 0, empty = 0;
    for (int i = 0; i < g_nrows; ++i) {
        if (strcmp(g_rows[i].kind, "animation") != 0)
            continue;
        char adir[300], mdir[300];
        dir_of(g_rows[i].path, adir);
        /* sibling model with a matching skeleton: try every model in the dir
           until one is compatible (run_pair returns 3 on mismatch) */
        int r = 3;
        size_t tris = 0;
        int frames = 0;
        unsigned code = 0;
        for (int j = 0; j < g_nrows && r == 3; ++j) {
            if (strcmp(g_rows[j].kind, "model") != 0)
                continue;
            if (strcmp(dir_of(g_rows[j].path, mdir), adir) != 0)
                continue;
            r = run_pair_seh(g_rows[j].handle, g_rows[i].handle, &tris, &frames, &code);
        }
        if (r == 3)
            continue;           /* no compatible sibling: not a pair */
        if (limit && total >= limit)
            break;
        ++total;
        if (r == -1) {
            ++faulted;
            printf("  FAULT %08x  %s\n", code, g_rows[i].path);
        } else if (r > 0) {
            ++load_fail;
            printf("  LOADFAIL(%d)  %s\n", r, g_rows[i].path);
        } else if (tris == 0) {
            ++empty;
            printf("  EMPTY  %s\n", g_rows[i].path);
        } else {
            ++ok;
        }
    }
    (void)skipped;

    printf("anim soak: %d compatible pairs, %d animated+rendered, %d empty, "
           "%d load-fail, %d faulted\n", total, ok, empty, load_fail, faulted);
    return (faulted == 0 && ok * 10 >= total * 8) ? 0 : 1;
}
