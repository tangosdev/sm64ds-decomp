#ifndef SM64DS_LEVEL_RNG_H
#define SM64DS_LEVEL_RNG_H

/* A random state: one s32 that RandomIntInternal(int *) steps, so callers
   pass &rng.value. The constructor seeds it to 1 and is defined inline in
   each file that defines an object. The destructor is the empty
   func_0201973c. Both are only declared here, because an inline body in a
   shared header renumbers every consumer's compiler temporaries. */
struct LevelRng {
    int value;
    LevelRng();
    ~LevelRng();
};

/* The actors' state, defined in func_0201fec8.cpp. */
extern LevelRng data_0209e650;
/* The minigames' state, defined in Timer.cpp. */
extern LevelRng data_0209d4b8;

/* A random integer in [0, n) from the minigames' state: the generator's
   top 15 bits, scaled by n. A macro, not an inline, because the minigames
   expand it in place: at several sites only the expanded form reproduces
   their code. The user includes math/CalcLib.h for RandomIntInternal. */
#define MG_RAND(n) ((((unsigned int)RandomIntInternal(&data_0209d4b8.value) >> 16) & 0x7fff) * (n) >> 15)

#endif
