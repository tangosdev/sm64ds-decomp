// Native SDAT player: the host's ARM7 sound component.
//
// SEAM: the ARM9/ARM7 command queue. SM64DS's ARM9 side is fully decompiled
// and in slice_gate10 -- Snd_SendCommand builds 5-word nodes from a 256-node
// pool, func_0205b070 flushes the batch over PXI tag 7, and the ARM7 consumes
// it. Nobody has ever decompiled SM64DS's ARM7, so the host plays that role
// natively: same queue, same command ids, same SDAT bytes, a C++ sequencer
// and mixer instead of a 33MHz core.
//
// Everything ABOVE the queue stays the game's own matched code. The SDAT
// root (data_020a5bb8) is seated with a real parsed archive, so
// Sound::InfoSequenceEntry::GetWithID and its five sibling table walkers
// resolve for real, and func_0205b78c patches the SBNK/SWAR wave-link slots
// exactly as it does on hardware (those 32 bytes at +0x18 of a DATA block
// are reserved by the format for precisely that).
//
// HONESTY: anything not implemented prints once and no-ops. It never fakes.
#ifndef SM64DS_PORT_SDAT_H
#define SM64DS_PORT_SDAT_H

#include <stddef.h>

typedef unsigned char sd_u8;
typedef unsigned short sd_u16;
typedef unsigned int sd_u32;
typedef signed char sd_s8;
typedef short sd_s16;
typedef int sd_s32;

// ---- archive ------------------------------------------------------------

// INFO record indices, as the matched accessors index them (offset in the
// INFO block header = 0x08 + rec*4).
enum {
    SDAT_REC_SEQ     = 0,   // +0x08  Sound::InfoSequenceEntry::GetWithID
    SDAT_REC_SEQARC  = 1,   // +0x0c  func_02050c14
    SDAT_REC_BANK    = 2,   // +0x10  Sound::InfoInstrumentBankEntry::GetWithID
    SDAT_REC_WAVEARC = 3,   // +0x14  func_02050b4c
    SDAT_REC_PLAYER  = 4,   // +0x18  func_02050ae8
    SDAT_REC_GROUP   = 5,   // +0x1c  func_02050a84
    SDAT_REC_STRMPLR = 6,
    SDAT_REC_STRM    = 7,
    SDAT_REC_COUNT   = 8
};

struct SdatArchive {
    sd_u8 *base;        // whole file, resident and WRITABLE (func_0205b78c
                        // patches SBNK/SWAR reserved fields in place)
    size_t size;
    sd_u8 *info;        // INFO block base; record offsets are relative to it
    sd_u8 *fat;         // FAT block base; count at +8, entries at +0xc
    sd_u32 fatCount;
    sd_u8 *root;        // the 0x88-byte object data_020a5bb8 points at
};

extern SdatArchive g_sdat;

// Load extracted/dsd/files/data/sound_data.sdat, build the root object and
// seat data_020a5bb8. Pre-seats every FAT entry's runtime "loaded address"
// slot with the resident address, so the game's own residency checks pass
// and its card-load path is correctly skipped. Returns 1 on success.
int sdat_init(void);

// Table walkers, host-side mirrors of the matched accessors. Return a
// pointer into the INFO block or NULL.
sd_u8 *sdat_info_entry(int rec, sd_u32 id);

// FAT lookups.
sd_u8 *sdat_file(sd_u32 fileId);      // resident base of file, or NULL
sd_u32 sdat_file_size(sd_u32 fileId);
// Reverse map: a resident pointer back to its file id, or -1u.
sd_u32 sdat_file_id_of(const void *p);

// The sequence's base channel priority (the SDAT cpr byte), the priority every
// track of the player starts at; 0xC6 overrides it per track. See the body.
int sdat_seq_priority(const sd_u8 *seqBase, sd_u32 startOff);

// ---- SWAV ---------------------------------------------------------------

// Every SWAV is decoded to mono s16 once and cached, keyed by its record
// address. PCM8/PCM16 are widened/copied, IMA-ADPCM is fully decoded, so the
// mixer never carries per-format state across a loop point.
struct SdatWave {
    const sd_s16 *pcm;
    sd_u32 totalSamples;
    int loop;
    sd_u32 loopStart;       // in samples
    sd_u32 sampleRate;
};

// Resolve wave `index` inside a resident SWAR image. Returns 1 on success.
int sdat_swar_wave(const sd_u8 *swar, sd_u32 index, SdatWave *out);

// ---- SBNK ---------------------------------------------------------------

struct SdatNote {
    sd_u16 swav, swar;
    sd_u8 baseNote, attack, decay, sustain, release, pan;
};

// Resolve program `prog` + note `note` in a resident SBNK to its sample and
// envelope. Walks single/drumset/keysplit records. Returns 1 on success.
// `swarOut` receives the linked SWAR image read from the SBNK's reserved
// wave-link slots (SBNK+0x18+i*8), which func_0205b78c fills in.
int sdat_bank_note(const sd_u8 *sbnk, int prog, int note,
                   SdatNote *out, const sd_u8 **swarOut);

// Fill any UNSET wave-link slot in a resident SBNK (SBNK+0x18+i*8) with its
// SWAR's resident address. In the running game func_0205b78c does this; the
// call here is idempotent and only writes empty slots, so it is a backstop
// for paths that reach a bank without going through the loader (the smoke
// test does exactly that). Returns the number of slots it filled.
int sdat_link_bank_waves(sd_u8 *sbnk);

// ---- mixer --------------------------------------------------------------

enum { SD_CHANNELS = 16, SD_MIX_RATE = 32768 };

struct SdatChanKey { int player; int track; int note; };

// Allocate a hardware-style channel. Returns channel index or -1.
int sd_mix_alloc(int priority);
void sd_mix_start(int ch, const SdatWave *w, const SdatNote *n,
                  int volume_db10, int pan, double rate, int priority);
// Both take the REASON they are ending the voice. The reason is the whole
// point of SM64DS_VOICE_TRACE: a per-frame voice count cannot tell "the note
// finished" from "something took the channel away", and that difference is
// the difference between working sound and sound that cuts out.
void sd_mix_release(int ch, const char *why);   // enter the release phase
// How fast a released channel is fading, and how many 192 Hz ticks it still
// needs to reach the silence floor. It holds its slot AND its full allocation
// priority for every one of them, so this is what tells a long tail apart from
// a stuck voice.
int sd_mix_release_rate(int ch);
int sd_mix_release_ticks(int ch);
// Pitch-dump record for a channel being freed. Defined in sseq.cpp, which owns
// the dump and the frame counter.
void sd_pd_end(int ch, int prio, const char *why);
void sd_mix_kill(int ch, const char *why);      // immediate stop
int  sd_mix_active(int ch);
void sd_mix_set(int ch, int volume_db10, int pan, double rate);
void sd_mix_set_pan(int ch, int pan);   // retune a voice already sounding
void sd_mix_set_vol(int ch, int volume_db10);   // ditto, volume only
void sd_mix_set_rate(int ch, double rate);      // ditto, playback rate only
void sd_mix_frame(void);              // advance every envelope one 192Hz frame
void sd_mix_render(sd_s16 *dst, int frames);   // stereo interleaved
// Lane VOICE: an extra fill run over the finished stereo block, AFTER the host
// master volume, at SD_MIX_RATE. Null (the default) is no call at all. One
// registrant, hal/voice_chat.cpp, and only when the player turned voice on.
void sd_mix_set_aux_render(void (*fn)(sd_s16 *dst, int frames));
void sd_mix_reset(void);

// Re-seed the command queue (free list, ring, cursors) to its booted shape.
// Boot calls it through sd_consumer_init; a save-state restore calls it beside
// sd_seq_reset/sd_mix_reset, because the queue's DS-named globals are captured
// while the host cursors next to them are not. See the note on the definition.
void sd_consumer_reset(void);

// dB conversion shared by the sequencer and the mixer: 0..127 -> tenths of
// a dB in -723..0, the DS's own volume range.
int sd_cnv_vol(int v);

// ---- sequencer ----------------------------------------------------------

// Start a sequence. `seqData` points at the SSEQ bytecode (SSEQ base +
// *(u32*)(base+0x18)) -- exactly what command 0 carries. `sbnk` is the
// resident bank image. Returns 1 if a player slot was taken.
// seqBase is the bytecode base EVERY pc in the sequence is measured from --
// for a SEQARC, the archive's whole DATA block, not the entry. startOff is
// where this entry begins inside it. The two must stay separate; see the
// header on the definition.
int sd_seq_start(int player, const sd_u8 *seqBase, sd_u32 startOff,
                 const sd_u8 *sbnk);
void sd_seq_stop(int player);
void sd_seq_set_volume(int player, int vol);      // 0..127
// The OTHER player volume: tenths of a dB in -723..0, straight out of the
// ROM's own 0..127 table at data_02086384. func_0204fafc recomputes it every
// frame for every sounding voice (distance attenuation plus whatever fade
// ramp is running) and sends it as PLAYER_PARAM 6, so unlike the 0..127 one
// this arrives while the sound is already playing and has to reach it.
void sd_seq_set_volume_db10(int player, int db10);
void sd_seq_set_pan(int player, int pan);         // 0..127, 64 centre
// The per-TRACK pair, TRACK_PARAM 0x0a and 0x0c. Same domains as the two
// player calls above them -- tenths of a dB out of data_02086384 for the
// first, 1/64 of a semitone for the second -- applied to the tracks named in
// the mask instead of to the whole player. Both are re-sent every frame by
// the 3D sound updater, so both reach notes that are already sounding. The
// derivation from the five matched TRACK_PARAM emitters is on the
// definitions in sseq.cpp.
void sd_seq_set_track_volume_db10(int player, unsigned trackMask, int db10);
void sd_seq_set_track_pitch(int player, unsigned trackMask, int pitch);
void sd_seq_frame(void);                          // one 192Hz sequencer frame
void sd_seq_reset(void);
int  sd_seq_active(int player);

// Drop the decoded-wave cache. It keys on SWAV record addresses in the game
// arena, so it is only valid while the arena stands still; a save-state
// restore that crossed an area invalidates every key. Called by
// lk6_savestate_load beside the other resets; refills on demand.
void sd_waves_reset(void);

// Re-seat data_020a5bb8 with THIS process's SDAT root. The global is captured
// like any hosted DS symbol, but it holds a host boot pointer, so a disk state
// from an earlier run would restore a heap address that no longer exists.
// Called by lk6_savestate_load after the section copy; idempotent.
void sd_sdat_reseat(void);

// The bit per player the hardware publishes in SNDSharedWork.playerStatus.
// Bit p is set while player p still owns the sequence it was started with.
sd_u32 sd_seq_player_mask(void);

// ---- output device ------------------------------------------------------

// winmm waveOut, loaded dynamically. SM64DS_NO_AUDIO=1 skips the device and
// the mixer still runs (and still feeds the .wav dump).
int  sd_out_open(void);
void sd_out_close(void);
void sd_out_push(void);       // render + submit whatever the ring wants

// Host master volume, 0..100. SM64DS_VOLUME sets it (default 50 = half); it is
// read once and applied as a final linear scalar on the mixed stereo output.
// This is the host output stage; the DS mixer and every envelope are untouched.
// out_set_volume_pct moves it while running: the settings.json watcher in the
// window host pushes the file's Volume through it so the slider works live.
int  out_volume_pct(void);
extern "C" void out_set_volume_pct(int pct);

// SM64DS_WAV_DUMP=<path> captures the mixed stream to a RIFF/WAVE file.
void sd_wav_open(const char *path);
void sd_wav_write(const sd_s16 *pcm, int frames);
void sd_wav_close(void);

// ---- consumer -----------------------------------------------------------

// Seed the command pool and status block. Idempotent.
void sd_consumer_init(void);
// One hosted ARM7 tick: flush the ARM9 pending list, execute every queued
// command, return the nodes, advance the progress counter.
void sd_consumer_tick(void);

// THE call site. Safe before init, safe with no audio device.
extern "C" void sdat_host_tick(void);

// The level-change looping-sound reap, the ROM's Scene::BeforeCleanupResources
// (func_02011974 over data_0209b53c). Stops every live looping handle
// unconditionally, unlike the per-frame reaper which spares refreshed ones. The
// level-change teardown calls it where the ROM's Scene teardown would fire it.
extern "C" void sd_sound_level_reap(void);

// SM64DS_SND_TRACE, shared: the consumer reads the environment and arms both
// its own opcode log and Sound::Play's two cull reports in sound_abi.cpp.
extern "C" int g_snd_trace_play;

// ---- voice trace --------------------------------------------------------

// SM64DS_VOICE_TRACE=1. Latched once in sd_consumer_init, off by default,
// and every call site is guarded by the flag so it costs one predictable
// branch when off. Lines are tagged [vt] and cover the WHOLE chain a sound
// effect travels: the ARM9's own voice records, the queue commands, the
// sequencer's players and tracks, and the 16 mixer channels.
extern "C" int g_voice_trace;
void sd_vtrace(const char *fmt, ...);
#define SD_VT(...) do { if (g_voice_trace) sd_vtrace(__VA_ARGS__); } while (0)

// Snapshot the ARM9-side voice bookkeeping and print it if it moved. Called
// once per hosted tick; also called by name at the interesting moments.
void sd_vtrace_arm9_census(const char *when);

// The same, for func_0201226c's 0x40-entry looping-handle table. Separate
// from the voice census because the two pools drain independently: a handle
// outlives its voice, and only this line shows a handle leak.
void sd_vtrace_loop_census(const char *when);

#endif
