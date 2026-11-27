#ifndef SM64DS_LEVEL_RNG_H
#define SM64DS_LEVEL_RNG_H

/* A random state: one s32 that RandomIntInternal(int *) steps, so callers
   pass &rng.value. The default constructor seeds it to 1, LevelRng(int)
   to the value given; each is defined inline in the files that define an
   object with it. The destructor is the empty func_0201973c. All are only
   declared here, because an inline body in a shared header renumbers every
   consumer's compiler temporaries. */
struct LevelRng {
    int value;
    LevelRng();
    LevelRng(int);
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

/* A random fx32 in [0, 1.0) from the actors' state: the 12 bits below its
   sign bit. RAND_FX_SCALE(n) is that fraction times the fx32 n, a random
   value in [0, n). Macros for the same reason as MG_RAND. */
#define RAND_FX() ((fx32)(((unsigned int)RandomIntInternal(&data_0209e650.value) & 0x7fffffff) >> 19))
#define RAND_FX_SCALE(n) (RAND_FX() * (n) >> 12)

#endif
