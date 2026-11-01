//cpp
#include "types.h"
#include "nitro/fx/fx.h"
#include "Ov007MenuState.h"
#include "cstd.h"
#include "Ov007Mesh.h"

extern "C" {

#pragma opt_strength_reduction off

extern short data_02082214[];
extern int func_ov007_020c3ba8(int);
extern void *func_ov007_020c3df4(const char *name, int size);
extern void *func_ov007_020c9388(int initialState);
extern void func_ov007_020c937c(void *state);
extern void func_ov007_020c3d1c(void *);
extern void func_ov007_020c92d0(Ov007MenuState *state);

/* Steps wave i over the mesh: while it holds a line and keepChance wins it
 * adds a travelling sine to the z force of every point on that line;
 * otherwise, once any idle time is served, it re-rolls to idle or to a
 * random line. */
void func_ov007_020ca308(Ov007MeshWave *wave, Ov007Mesh *mesh, int i) {
    Ov007MenuState *state;
    Ov007MeshWaveParams *params;
    int line;
    int amplitude;
    int count;
    int speed;
    int spread;
    int lines;
    int ticks;
    int j;
    int phase;

    state = wave->waves[i];
    params = wave->params;
    speed = params->speed;
    line = state->currentState;
    spread = params->spread;
    amplitude = params->amplitude;

    if (state->stateTicks == 0)
        wave->waves[i]->stateTicks = func_ov007_020c3ba8(0x3c);
    ticks = wave->waves[i]->stateTicks;

    if (i == 0 || i == 2) {
        Ov007Grid *grid = mesh->grid;
        count = grid->rows;
        lines = grid->columns;
    } else if (i == 1 || i == 3) {
        Ov007Grid *grid = mesh->grid;
        count = grid->columns;
        lines = grid->rows;
    }

    if (line >= 0 && line < lines) {
        int keep = params->keepChance;
        if (func_ov007_020c3ba8(10000) <= keep * 10 - 1) {
            u16 base;
            u16 step = (u16)cstd::div(spread, count);
            j = 0;
            base = (u16)(ticks * speed);
            if (count <= 0)
                return;
            phase = j;
            do {
                s16 sine = data_02082214[((u16)(base + phase) >> 4) * 2];
                int val = FX_MUL(amplitude, sine);
                if (i == 0) {
                    mesh->grid->rowLists[j]->items[line]->force.z += val;
                } else if (i == 1) {
                    mesh->grid->rowLists[line]->items[j]->force.z += val;
                }
                j++;
                phase += step;
            } while (j < count);
            return;
        }
    }

    if (line == -1) {
        if (line != -1)
            return;
        if (ticks < params->minIdle)
            return;
    }
    {
        int idle = params->idleChance;
        if (func_ov007_020c3ba8(10000) <= idle * 10 - 1) {
            wave->waves[i]->requestedState = -1;
            return;
        }
    }
    wave->waves[i]->requestedState = func_ov007_020c3ba8(lines);
}

/* Allocates a mesh wave over the template params: four idle waves
 * (func_ov007_020c9388(-2)), no delay. */
Ov007MeshWave *func_ov007_020ca2a8(Ov007MeshWaveParams *params) {
    Ov007MeshWave *wave = (Ov007MeshWave *)func_ov007_020c3df4(0, sizeof(Ov007MeshWave));
    int i;
    for (i = 0; i < 4; i++) {
        wave->waves[i] = (Ov007MenuState *)func_ov007_020c9388(-2);
    }
    wave->params = params;
    wave->delay = 0;
    wave->frame = 0;
    return wave;
}

/* Frees a mesh wave and its four wave states. */
void func_ov007_020ca26c(Ov007MeshWave *wave) {
    int i;
    for (i = 0; i < 4; i++) {
        func_ov007_020c937c(wave->waves[i]);
    }
    func_ov007_020c3d1c(wave);
}

/* One tick of a mesh wave over mesh: once the delay has run out, ticks the
 * four wave states and steps waves 0 and 1 (func_ov007_020ca308); counts
 * frame up every call. */
void func_ov007_020ca1d4(Ov007MeshWave *wave, Ov007Mesh *mesh) {
    int i;
    if (wave->delay <= 0) {
        for (i = 0; i < 4; i++) {
            func_ov007_020c92d0(wave->waves[i]);
            switch (i) {
            case 0:
            case 1:
                func_ov007_020ca308(wave, mesh, i);
                break;
            case 2:
            case 3:
                break;
            }
        }
    } else {
        wave->delay--;
        if (wave->delay <= 0)
            wave->delay = 0;
    }
    wave->frame++;
}
}
