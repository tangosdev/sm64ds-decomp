/* run link100, lane SEAT5 -- the dispatch census the three seated rows share.
   Definition and the exit dump are in port/hal/pmf_seat5.cpp; the dump only
   prints when SM64DS_SEAT5_LOG names a file. */
#ifndef PORT_HAL_PMF_SEAT5_H
#define PORT_HAL_PMF_SEAT5_H

#define PORT_SEAT5_COUNTERS 14

/* index order, fixed: 0..4 Unagi MAIN halves (d8c d9c dac dbc d7c),
   5..12 the QUESTION_BLOCK content bodies (021494cc 02149478 02149428 02149384
   021493dc 021492d4 02149288 02149220), 13 MirrorLuigi's TICK half. */
#ifdef __cplusplus
extern "C" {
#endif
void port_seat5_count(int i);
#ifdef __cplusplus
}
#endif

#endif
