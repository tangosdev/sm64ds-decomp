#!/usr/bin/env python3
"""The address cross-reference between this tree's config and main's.

WHY THIS EXISTS.  main renamed most of the ROM's symbols after the port forked.
Reconciling the port means re-spelling every ROM name the port files carry.  A
tool that does that BY NAME is wrong and fails silently: for 131 of the
spellings the port uses, the old spelling is still a live symbol on main at a
DIFFERENT address, so a name substitution moves that row onto another class's
code and nothing in the build complains.  Every row below is therefore keyed on
(module, ROM address), read out of config/**/symbols.txt at the two revisions,
and every vtable row is additionally anchored on the string the cartridge
itself writes.

THE RTTI ANCHOR.  A vtable at V has its typeinfo pointer at V-4, and that
object's +4 word points at the NUL-terminated type name.  Read straight out of
extracted/overlays/overlay_NNNN.bin (never the dsd copy -- those are shifted),
or extracted/arm9_dec.bin for arm9.  Where main carries `_ZTV<that name>` at
the same address, that is the target: it is the spelling the ROM itself
carries, main has it too, and it cannot be confused with the legacy
class-facing spellings, which this diff moved by exactly one class.  main's
own ov018 0x021138cc is the proof that the legacy spellings are the moving
part: main calls it `_ZTV7SkiLift` and the cartridge's RTTI there reads
`14daObjSm_Lift_c`.

TARGET SELECTION, in order:
  1. vtable row and main carries _ZTV<ROM RTTI name> at the address
  2. exactly one main-only spelling at the address of the same kind
  3. exactly one main-only spelling of any kind
  4. no main-only spelling (main dropped the port's extra) -> fold onto the
     spelling of that kind the two sides already share
  5. otherwise AMBIGUOUS, which is written down and not guessed

FOUR PROOFS the map has to pass, all re-run by --verify:
  1. no old spelling maps to two different new spellings
  2. no (module, address, old) key appears twice
  3. after ONE SIMULTANEOUS substitution no target spelling is still carried by
     an unrenamed port symbol at another address -- this is what makes the
     three-link chains resolvable at all (ov064: data_ov064_0211c1d8 ->
     _ZTV19BowserPuzzleManager -> _ZTV16daObjFl_Puzzle_c, with
     _ZTV17BowserPuzzlePiece -> _ZTV18daWater_Hakidasi_c alongside it)
  4. every DANGER row resolves to a spelling main actually has AT THAT ADDRESS

--apply REFUSES until the definitions exist.  The rename is not landable ahead
of the merge: 2,337 slice enrollment rows and 135 CMakeLists source paths name
src/<oldname>.c, 2,465 of those bodies exist in this tree only under the old
name, and 73 of the 79 target headers are not here at all.  Re-spelling the
references before the bodies arrive points them at files and symbols that do
not exist, so CMake configure fails and the link cannot resolve.  The refusal
is the sequencing rule made executable: this map lands in the same commit that
brings main's src/ and include/ across, not before it.
"""
import argparse
import collections
import csv
import glob
import os
import re
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(HERE)
ROOT = os.path.dirname(PORT)
MAP = os.path.join(HERE, 'reconcile_rename_map.tsv')
HDRMAP = os.path.join(HERE, 'reconcile_header_map.tsv')
BS = chr(92)

SKIP_EXT = {'.png', '.bmp', '.jpg', '.zip', '.bin', '.exe', '.dll', '.obj',
            '.pdb', '.ico', '.wav', '.nds'}

# This tool's own artifacts carry every old AND new spelling by construction.
# Counting them as port files that "reference" a name would make the map
# self-sustaining -- a row would stay alive because the map itself names it --
# and would poison the per-place adjudication with rows that are data, not code.
OWN = {'reconcile_rename_map.tsv', 'reconcile_header_map.tsv',
       'reconcile_danger_rows.tsv', 'reconcile_xref.py'}

# The places where the surrounding prose names the address the OLD spelling
# still occupies on main, so the automatic context test cannot tell which body
# the line means.  Each was read by hand; the reason is the sentence in the file
# that settles it.  Keyed on (file, symbol) because line numbers move.
HAND = {
    ('port/slice_gate40.txt', '_ZTV4Door'):
        "the file's own words: config's _ZTV4Door is the table whose RTTI reads "
        "12daStarGate_c, not the real door at 0x02148188 -- so it means the "
        "renamed address, and the rename is what stops the name misleading",
    ('port/hal/actor_classes.cpp', '_ZTV4Door'):
        "gate 22's header says config hands _ZTV4Door to the STAR door "
        "(RTTI daStarGate_c) while the real door at 0x02148188 stayed unnamed",
    ('port/hal/actor_classes_ov030.cpp', '_ZTV13RollingLogTtm'):
        "the declaration's own comment reads 'vtspan: data_ov030_02115bfc, "
        "really _ZTV7daMky_c' -- the renamed address, named outright",
    ('port/hal/actor_classes_ov091.cpp', '_ZTV5Stump'):
        "the header reads 'the config symbol _ZTV5Stump sits at 0x021353ac with "
        "RTTI 10daHyuhyu_c'; 0x021352bc in the same paragraph is actor 27's own "
        "table, a different class",
    ('port/hal/actor_classes_ov064_w9.cpp', '_ZTV12MetalNetLift'):
        "gate 178's header pins it: '_ZTV12MetalNetLift 0x0211bd2c' -- the "
        "renamed address; 0x0211bc68 in the same paragraph is id 69's table",
    ('port/hal/actor_classes_ov070.cpp', '_ZTV18PoppingLavaBubbles'):
        "the sentence is 'NOT the class at _ZTV18PoppingLavaBubbles', i.e. the "
        "table at the renamed address; 0x021093e0 named beside it is the "
        "UNNAMED table it contrasts with",
}


# ---------------------------------------------------------------- config I/O

def modname(path):
    p = path.replace(BS, '/')
    m = re.search(r'/overlays/(ov[0-9]+)/', p)
    if m:
        return m.group(1)
    if '/itcm/' in p:
        return 'itcm'
    if '/dtcm/' in p:
        return 'dtcm'
    return 'arm9'


def parse_symbols(lines, mod, a2n, n2a):
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        m = re.search(r'addr:(0x[0-9a-fA-F]+)', line)
        if not m:
            continue
        nm = line.split()[0]
        ad = m.group(1).lower()
        a2n[(mod, ad)].append(nm)
        n2a[nm].append((mod, ad))


def load_worktree(root):
    a2n = collections.defaultdict(list)
    n2a = collections.defaultdict(list)
    for path in glob.glob(os.path.join(root, 'config', '**', 'symbols.txt'), recursive=True):
        with open(path, encoding='utf-8', errors='replace') as f:
            parse_symbols(f, modname(path), a2n, n2a)
    return a2n, n2a


def load_rev(root, rev):
    a2n = collections.defaultdict(list)
    n2a = collections.defaultdict(list)
    ls = subprocess.run(['git', '-C', root, 'ls-tree', '-r', '--name-only', rev, '--', 'config/'],
                        capture_output=True, text=True)
    if ls.returncode != 0:
        return None, None
    for f in ls.stdout.split('\n'):
        if not f.endswith('symbols.txt'):
            continue
        blob = subprocess.run(['git', '-C', root, 'show', rev + ':' + f],
                              capture_output=True, text=True)
        parse_symbols(blob.stdout.split('\n'), modname(f), a2n, n2a)
    return a2n, n2a


# ------------------------------------------------------------- the ROM image

class Rom(object):
    def __init__(self, root):
        self.root = root
        self.cache = {}

    def image(self, mod):
        if mod in self.cache:
            return self.cache[mod]
        got = None
        if mod == 'arm9':
            base = self._text_start(os.path.join(self.root, 'config', 'arm9', 'delinks.txt'))
            path = os.path.join(self.root, 'extracted', 'arm9_dec.bin')
            if base is not None and os.path.exists(path):
                got = (base, open(path, 'rb').read())
        elif re.match(r'^ov\d+$', mod):
            d = os.path.join(self.root, 'config', 'arm9', 'overlays', mod, 'delinks.txt')
            base = self._text_start(d)
            path = os.path.join(self.root, 'extracted', 'overlays',
                                'overlay_%04d.bin' % int(mod[2:]))
            if base is not None and os.path.exists(path):
                got = (base, open(path, 'rb').read())
        self.cache[mod] = got
        return got

    @staticmethod
    def _text_start(delinks):
        if not os.path.exists(delinks):
            return None
        for line in open(delinks, encoding='utf-8', errors='replace'):
            m = re.match(r'\s*\.text\s+start:(0x[0-9a-fA-F]+)', line)
            if m:
                return int(m.group(1), 16)
        return None

    def word(self, mod, addr):
        im = self.image(mod)
        if im is None:
            return None
        base, data = im
        off = addr - base
        if off < 0 or off + 4 > len(data):
            return None
        return struct.unpack('<I', data[off:off + 4])[0]

    def rtti_name(self, mod, vaddr):
        """The type-name string a vtable's RTTI points at, or None."""
        ti = self.word(mod, vaddr - 4)
        if not ti:
            return None
        nm = self.word(mod, ti + 4)
        if not nm:
            return None
        im = self.image(mod)
        base, data = im
        off = nm - base
        if off < 0 or off >= len(data):
            return None
        end = data.find(b'\0', off)
        if end < 0 or end - off > 80:
            return None
        s = data[off:end].decode('ascii', 'replace')
        return s if re.match(r'^\d+[A-Za-z_][A-Za-z0-9_]*$', s) else None


# ------------------------------------------------------------------ the map

def kind(s):
    if s.startswith('_ZTV'):
        return 'vtable'
    if s.startswith('_ZTI'):
        return 'typeinfo'
    if s.startswith('_ZTS'):
        return 'typename'
    if s.startswith('_Z'):
        return 'mangled'
    if s.startswith('func_'):
        return 'func'
    if s.startswith('data_'):
        return 'data'
    return 'plain'


def port_tokens(root):
    """token -> the set of files under port/ that name it"""
    out = collections.defaultdict(set)
    for dirpath, dirnames, filenames in os.walk(os.path.join(root, 'port')):
        dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
        for fn in filenames:
            if os.path.splitext(fn)[1].lower() in SKIP_EXT or fn in OWN:
                continue
            fp = os.path.join(dirpath, fn)
            rel = os.path.relpath(fp, root).replace(BS, '/')
            try:
                txt = open(fp, encoding='utf-8', errors='replace').read()
            except OSError:
                continue
            for t in set(re.findall(r'[A-Za-z_][A-Za-z0-9_]*', txt)):
                out[t].add(rel)
    return out


FIELDS = ['module', 'address', 'old_name', 'new_name', 'kind', 'basis', 'rom_rtti',
          'danger_old_still_live_on_main', 'port_at_address', 'main_at_address',
          'n_files', 'port_files']


def derive(root, rev):
    P_a2n, P_n2a = load_worktree(root)
    M_a2n, M_n2a = load_rev(root, rev)
    if M_a2n is None:
        return None, None, 'cannot read %s -- fetch it first' % rev
    rom = Rom(root)
    toks = port_tokens(root)

    rows = []
    for (mod, ad), pnames in sorted(P_a2n.items()):
        mnames = M_a2n.get((mod, ad), [])
        if not mnames:
            continue                      # main has no symbol here at all
        mset, pset = set(mnames), set(pnames)
        main_only = [x for x in mnames if x not in pset]
        shared = [x for x in mnames if x in pset]
        for p in pnames:
            if p in mset:
                continue                  # both sides already agree
            files = toks.get(p, set())
            if not files:
                continue                  # nothing under port/ names it
            tgt = basis = None
            rt = None
            if kind(p) == 'vtable':
                rt = rom.rtti_name(mod, int(ad, 16))
                if rt and ('_ZTV' + rt) in mset:
                    tgt = '_ZTV' + rt
                    basis = "ROM RTTI at V-4 reads '%s' and main carries _ZTV%s here" % (rt, rt)
            if tgt is None:
                same = [x for x in main_only if kind(x) == kind(p)]
                if len(same) == 1:
                    tgt, basis = same[0], 'one main-only spelling of the same kind'
                elif len(main_only) == 1:
                    tgt, basis = main_only[0], 'the one main-only spelling at the address'
                elif not main_only:
                    sh = [x for x in shared if kind(x) == kind(p)] or shared
                    if len(sh) == 1:
                        tgt, basis = sh[0], 'main dropped it; folds onto the shared spelling'
                    elif sh:
                        tgt, basis = sh[0], 'AMBIGUOUS: main dropped it, several shared spellings'
                    else:
                        tgt, basis = '-', 'AMBIGUOUS: no candidate at the address'
                elif same:
                    tgt, basis = same[0], 'AMBIGUOUS: several main-only spellings of the same kind'
                else:
                    tgt, basis = main_only[0], 'AMBIGUOUS: several main-only, none same-kind'
            live = [t for t in M_n2a.get(p, []) if t != (mod, ad)]
            rows.append(dict(module=mod, address=ad, old_name=p, new_name=tgt,
                             kind=kind(p), basis=basis, rom_rtti=(rt or '-'),
                             danger_old_still_live_on_main=(
                                 ';'.join('%s:%s' % t for t in live) or '-'),
                             port_at_address=';'.join(sorted(pset)),
                             main_at_address=';'.join(mnames),
                             n_files=len(files),
                             port_files=';'.join(sorted(files))))
    return rows, P_n2a, None


def proofs(rows, P_n2a):
    """the four obligations; returns a list of failure strings"""
    bad = []
    byold = collections.defaultdict(set)
    for r in rows:
        byold[r['old_name']].add(r['new_name'])
    for k, v in sorted(byold.items()):
        if len(v) > 1:
            bad.append('PROOF 1: %s maps to %s' % (k, sorted(v)))
    keys = collections.Counter((r['module'], r['address'], r['old_name']) for r in rows)
    for k, c in sorted(keys.items()):
        if c > 1:
            bad.append('PROOF 2: duplicate key %s' % (k,))
    renamed = set(r['old_name'] for r in rows)
    for r in rows:
        if r['new_name'] in renamed:
            continue
        for t in P_n2a.get(r['new_name'], []):
            if t != (r['module'], r['address']):
                bad.append('PROOF 3: %s -> %s but the port still spells %s at %s:%s'
                           % (r['old_name'], r['new_name'], r['new_name'], t[0], t[1]))
    for r in rows:
        if r['danger_old_still_live_on_main'] == '-':
            continue
        if r['new_name'] not in r['main_at_address'].split(';'):
            bad.append('PROOF 4: DANGER %s -> %s which main does not have at %s %s'
                       % (r['old_name'], r['new_name'], r['module'], r['address']))
    for r in rows:
        if r['basis'].startswith('AMBIGUOUS'):
            bad.append('UNRESOLVED: %s %s %s -- %s'
                       % (r['module'], r['address'], r['old_name'], r['basis']))
    return bad


def write_map(rows, path):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, delimiter='\t', lineterminator='\n', fieldnames=FIELDS)
        w.writeheader()
        for r in rows:
            w.writerow(r)


def read_map(path):
    with open(path, encoding='utf-8') as f:
        return list(csv.DictReader(f, delimiter='\t'))


# ------------------------------------------------------------------ headers

def derive_headers(root, rev):
    """Every renamed include/ header, resolved by the class's vtable address."""
    P_a2n, P_n2a = load_worktree(root)
    M_a2n, M_n2a = load_rev(root, rev)
    if M_a2n is None:
        return None, 'cannot read %s' % rev
    rom = Rom(root)
    main_inc = set(f.split('/')[-1] for f in subprocess.run(
        ['git', '-C', root, 'ls-tree', '-r', '--name-only', rev, '--', 'include/'],
        capture_output=True, text=True).stdout.split('\n') if f.endswith('.h'))
    port_inc = set(os.listdir(os.path.join(root, 'include')))

    # Reverse index: the ROM's own RTTI type name -> the address whose vtable
    # points at it.  Built over every address MAIN spells _ZTV*, which is what
    # lets an RTTI-named port header (daDoor_c.h) find the class main gave a
    # legacy name to (Door.h) even though the port config never spells
    # _ZTV8daDoor_c at all.
    rtti_index = {}
    for nm, places in M_n2a.items():
        if not nm.startswith('_ZTV'):
            continue
        for mod, ad in places:
            s = rom.rtti_name(mod, int(ad, 16))
            if s:
                rtti_index.setdefault(re.sub(r'^\d+', '', s), (mod, ad))

    rows = []
    for h in sorted(port_inc):
        if not h.endswith('.h') or h.startswith('decl_'):
            continue
        cls = h[:-2]
        if h in main_inc:
            continue                       # main still has this header
        ztv = '_ZTV%d%s' % (len(cls), cls)
        anchors = P_n2a.get(ztv, [])
        kindtag = 'vtable'
        if not anchors and '__' in cls:
            # a nested class: Outer__Inner is _ZTVN<l>Outer<l>InnerE
            outer, _, inner = cls.partition('__')
            nested = '_ZTVN%d%s%d%sE' % (len(outer), outer, len(inner), inner)
            if nested in P_n2a:
                anchors, ztv = P_n2a[nested], nested
        if not anchors:
            pref = '_ZN%d%s' % (len(cls), cls)
            cands = sorted(n for n in P_n2a if n.startswith(pref))
            if cands:
                anchors = [P_n2a[cands[0]][0]]
                ztv = cands[0]
                kindtag = 'method'
            elif cls in rtti_index:
                anchors = [rtti_index[cls]]
                ztv = '(ROM RTTI %s)' % cls
                kindtag = 'rom-rtti'
            else:
                rows.append(dict(port_header=h, anchor='-', anchor_kind='none', module='-',
                                 address='-', rom_rtti='-', main_at_address='-',
                                 main_class='-', new_header='-',
                                 verdict='NOT A RENAME (no address anchor and no '
                                         'counterpart on main)'))
                continue
        mod, ad = anchors[0]
        psyms = set(P_a2n.get((mod, ad), []))
        msyms = M_a2n.get((mod, ad), [])
        rt = rom.rtti_name(mod, int(ad, 16)) if kindtag == 'vtable' else None
        main_only = [s for s in msyms if s not in psyms]
        shared = [s for s in msyms if s in psyms]

        def cls_of(s):
            m = re.match(r'^_ZT[VIS](\d+)(.+)$', s) or re.match(r'^_ZN(\d+)(.+)$', s)
            if not m:
                return None
            n = int(m.group(1))
            return m.group(2)[:n] if len(m.group(2)) >= n else None

        cands = [cls_of(s) for s in main_only] + [cls_of(s) for s in shared]
        cands = [c for c in cands if c]
        newh = '-'
        mc = '-'
        for c in cands:
            if c + '.h' in main_inc:
                newh, mc = 'include/' + c + '.h', c
                break
        verdict = 'RESOLVED' if newh != '-' else 'UNRESOLVED'
        rows.append(dict(port_header=h, anchor=ztv, anchor_kind=kindtag, module=mod,
                         address=ad, rom_rtti=(rt or '-'), main_at_address=';'.join(msyms),
                         main_class=mc, new_header=newh, verdict=verdict))
    return rows, None


HDR_FIELDS = ['port_header', 'anchor', 'anchor_kind', 'module', 'address', 'rom_rtti',
              'main_at_address', 'main_class', 'new_header', 'verdict']


# ------------------------------------------------------------------- apply

def apply_ready(root):
    """Can the rename land yet?  Only once the bodies it points at are here."""
    reasons = []
    rows = read_map(MAP) if os.path.exists(MAP) else []
    srcdir = os.path.join(root, 'src')
    have = set()
    if os.path.isdir(srcdir):
        for fn in os.listdir(srcdir):
            b, e = os.path.splitext(fn)
            if e in ('.c', '.cpp'):
                have.add(b)
    missing = [r for r in rows if r['old_name'] in have and r['new_name'] not in have]
    if missing:
        reasons.append('%d renamed bodies still exist only under the OLD name in src/ '
                       '(e.g. %s); the slice rows and CMake source paths that name them '
                       'cannot be re-spelled yet'
                       % (len(missing), ', '.join(r['old_name'] for r in missing[:3])))
    hdrs = read_map(HDRMAP) if os.path.exists(HDRMAP) else []
    inc = set(os.listdir(os.path.join(root, 'include'))) if os.path.isdir(
        os.path.join(root, 'include')) else set()
    hmiss = [h for h in hdrs if h['new_header'] != '-'
             and h['new_header'].split('/')[-1] not in inc]
    if hmiss:
        reasons.append('%d target headers do not exist in this tree (e.g. %s)'
                       % (len(hmiss), ', '.join(h['new_header'] for h in hmiss[:3])))
    return reasons


def do_apply(root, force):
    rows = read_map(MAP)
    reasons = apply_ready(root)
    if reasons and not force:
        print('REFUSED -- the rename is not landable in this tree yet:')
        for r in reasons:
            print('  * ' + r)
        print('Land it in the same commit that brings main\'s src/ and include/ over,'
              ' or pass --force if you know better than this check.')
        return 2
    sub = {r['old_name']: r['new_name'] for r in rows}
    pat = re.compile(r'(?<![A-Za-z0-9_])(' +
                     '|'.join(sorted((re.escape(k) for k in sub), key=len, reverse=True)) +
                     r')(?![A-Za-z0-9_])')
    changed = hits = 0
    for dirpath, dirnames, filenames in os.walk(os.path.join(root, 'port')):
        dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
        for fn in filenames:
            if os.path.splitext(fn)[1].lower() in SKIP_EXT or fn in OWN:
                continue
            fp = os.path.join(dirpath, fn)
            try:
                txt = open(fp, encoding='utf-8', errors='replace').read()
            except OSError:
                continue
            n = [0]

            def rep(m):
                n[0] += 1
                return sub[m.group(1)]
            new = pat.sub(rep, txt)
            if n[0]:
                with open(fp, 'w', encoding='utf-8', newline='') as f:
                    f.write(new)
                changed += 1
                hits += n[0]
    print('rewrote %d occurrences across %d files under port/' % (hits, changed))
    return 0


# ------------------------------------------------------------------- driver

def cmd_verify(args):
    rows, P_n2a, err = derive(args.root, args.rev)
    if err:
        print('SKIP: ' + err)
        return 0
    bad = proofs(rows, P_n2a)
    if not os.path.exists(MAP):
        print('no committed map at %s' % MAP)
        return 1
    old = read_map(MAP)
    a = {(r['module'], r['address'], r['old_name']): r['new_name'] for r in old}
    b = {(r['module'], r['address'], r['old_name']): r['new_name'] for r in rows}
    for k in sorted(set(a) | set(b)):
        if a.get(k) != b.get(k):
            bad.append('DRIFT %s: committed=%s derived=%s' % (k, a.get(k), b.get(k)))
    if bad:
        print('reconcile_xref: %d problem(s)' % len(bad))
        for x in bad[:60]:
            print('  ' + x)
        return 1
    dang = sum(1 for r in rows if r['danger_old_still_live_on_main'] != '-')
    romr = sum(1 for r in rows if r['basis'].startswith('ROM RTTI'))
    print('reconcile_xref: %d rows re-derived and identical to the committed map; '
          '%d DANGER rows, %d settled by the ROM\'s own RTTI string; 4/4 proofs OK'
          % (len(rows), dang, romr))
    return 0


def cmd_selftest(args):
    """Proofs over the committed map, plus rows pinned by hand from the ROM."""
    if not os.path.exists(MAP):
        print('selftest: no committed map')
        return 1
    rows = read_map(MAP)
    _, P_n2a = load_worktree(args.root)
    bad = proofs(rows, P_n2a)
    idx = {(r['module'], r['address'], r['old_name']): r['new_name'] for r in rows}
    # Every one of these was read out of the overlay binary by hand.  They are
    # the class-shift traps: main carries the OLD spelling at the neighbouring
    # address, so a by-name substitution lands on the wrong class.
    pinned = [
        ('ov002', '0x020b6e08', '_ZN18PoppingLavaBubblesD1Ev', '_ZN16daObjWaterfall_cD1Ev'),
        ('ov007', '0x020c3d1c', '_ZN6Player17St_EndingFly_MainEv', 'func_ov007_020c3d1c'),
        ('ov018', '0x021139bc', '_ZTV7SkiLift', '_ZTV10daPgMthr_c'),
        ('ov064', '0x0211c25c', '_ZTV19BowserPuzzleManager', '_ZTV16daObjFl_Puzzle_c'),
        ('ov064', '0x0211c334', '_ZTV17BowserPuzzlePiece', '_ZTV18daWater_Hakidasi_c'),
        ('ov100', '0x021483cc', '_ZTV4Door', '_ZTV12daStarGate_c'),
        ('ov045', '0x02112dbc', '_ZTV15FireSeaElevator', '_ZTV18daObjKm2_Ami_Bou_c'),
        ('ov045', '0x02112e80', '_ZTV8PoleLift', '_ZTV17daObjKm2_Nobiru_c'),
        ('ov036', '0x02113cf8', '_ZTV8ShipWing', '_ZTV16daObjRc_Tikuwa_c'),
        ('ov002', '0x02109c74', '_ZTV7Seaweed', '_ZTV12daObjHeart_c'),
    ]
    for mod, ad, old, want in pinned:
        got = idx.get((mod, ad, old))
        if got != want:
            bad.append('PINNED %s %s %s: want %s got %s' % (mod, ad, old, want, got))
    # and the ROM still has to say what the pins claim
    rom = Rom(args.root)
    for mod, ad, want in [('ov002', '0x02109c74', '12daObjHeart_c'),
                          ('ov064', '0x0211c25c', '16daObjFl_Puzzle_c'),
                          ('ov018', '0x021138cc', '14daObjSm_Lift_c'),
                          ('ov100', '0x021483cc', '12daStarGate_c')]:
        got = rom.rtti_name(mod, int(ad, 16))
        if got != want:
            bad.append('ROM %s %s: RTTI reads %r, expected %r' % (mod, ad, got, want))
    if bad:
        print('selftest FAIL (%d)' % len(bad))
        for x in bad[:40]:
            print('  ' + x)
        return 1
    print('selftest PASS (%d rows, 4 proofs, %d pinned rows, 4 ROM reads)'
          % (len(rows), len(pinned)))
    return 0


DANGER_OUT = os.path.join(HERE, 'reconcile_danger_rows.tsv')
DANGER_FIELDS = ['module', 'address', 'old_name', 'new_name', 'file', 'line', 'basis', 'text']


def cmd_danger(args):
    """One row per PLACE a DANGER spelling is written, with the evidence that
    settles which address that place means.

    Bases, strongest first:
      A  an address literal in the line or its +-4 lines equals the renamed
         address -- the file says outright which body it means
      P  the token sits inside a src/<old>.c|cpp path, so it names the port's
         own one-function file, whose delink address is the renamed address
      U  the port config spells this name at exactly ONE address and it is the
         renamed one, so a port file written against that config means it
      X  an address literal equals the address where the OLD spelling survives
         on main and the renamed address is absent -- read this one by hand
    """
    rows = [r for r in read_map(MAP) if r['danger_old_still_live_on_main'] != '-']
    if not rows:
        print('no danger rows in the map')
        return 1
    _, P_n2a = load_worktree(args.root)
    names = set(r['old_name'] for r in rows)
    pat = re.compile(r'(?<![A-Za-z0-9_])(' +
                     '|'.join(sorted((re.escape(n) for n in names), key=len, reverse=True)) +
                     r')(?![A-Za-z0-9_])')
    addr_re = re.compile(r'0x0[12][0-9a-fA-F]{6}')
    byname = {r['old_name']: r for r in rows}
    out = []
    for dirpath, dirnames, filenames in os.walk(os.path.join(args.root, 'port')):
        dirnames[:] = [d for d in dirnames if d not in ('.git', '__pycache__')]
        for fn in filenames:
            if os.path.splitext(fn)[1].lower() in SKIP_EXT or fn in OWN:
                continue
            fp = os.path.join(dirpath, fn)
            rel = os.path.relpath(fp, args.root).replace(BS, '/')
            try:
                lines = open(fp, encoding='utf-8', errors='replace').read().split('\n')
            except OSError:
                continue
            for i, line in enumerate(lines):
                if not pat.search(line):
                    continue
                ctx = '\n'.join(lines[max(0, i - 4):i + 5])
                found = set(a.lower() for a in addr_re.findall(ctx))
                for m in pat.finditer(line):
                    r = byname[m.group(1)]
                    surv = [c.split(':')[1].lower()
                            for c in r['danger_old_still_live_on_main'].split(';') if ':' in c]
                    has_ren = r['address'] in found
                    has_surv = any(s in found for s in surv)
                    onepath = re.search(r'src/' + re.escape(r['old_name']) + r'\.(c|cpp)', line)
                    uniq = P_n2a.get(r['old_name'], []) == [(r['module'], r['address'])]
                    if (rel, r['old_name']) in HAND:
                        basis = 'H'
                    elif has_surv and not has_ren:
                        basis = 'X'
                    elif has_ren:
                        basis = 'A'
                    elif onepath:
                        basis = 'P'
                    elif uniq:
                        basis = 'U'
                    else:
                        basis = 'X'
                    note = HAND.get((rel, r['old_name']), '')
                    out.append(dict(module=r['module'], address=r['address'],
                                    old_name=r['old_name'], new_name=r['new_name'],
                                    file=rel, line=i + 1, basis=basis,
                                    text=(note or line.strip())[:220]))
    with open(DANGER_OUT, 'w', encoding='utf-8', newline='') as f:
        w = csv.DictWriter(f, delimiter='\t', lineterminator='\n', fieldnames=DANGER_FIELDS)
        w.writeheader()
        for r in sorted(out, key=lambda x: (x['module'], x['address'], x['file'], x['line'])):
            w.writerow(r)
    c = collections.Counter(r['basis'] for r in out)
    print('wrote %s: %d places across %d danger spellings; bases %s'
          % (DANGER_OUT, len(out), len(rows), dict(c)))
    unresolved = [r for r in out if r['basis'] == 'X']
    for r in unresolved:
        print('  READ BY HAND %s:%s  %s' % (r['file'], r['line'], r['old_name']))
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--root', default=ROOT)
    ap.add_argument('--rev', default='origin/main')
    g = ap.add_mutually_exclusive_group(required=True)
    g.add_argument('--derive', action='store_true', help='recompute and rewrite the map')
    g.add_argument('--headers', action='store_true', help='recompute the header map')
    g.add_argument('--verify', action='store_true', help='re-derive and diff against the map')
    g.add_argument('--selftest', action='store_true')
    g.add_argument('--apply', action='store_true', help='rewrite port/ (refuses too early)')
    g.add_argument('--danger', action='store_true',
                   help='per-place adjudication of every DANGER spelling')
    ap.add_argument('--force', action='store_true')
    args = ap.parse_args(argv)

    if args.selftest:
        return cmd_selftest(args)
    if args.verify:
        return cmd_verify(args)
    if args.danger:
        return cmd_danger(args)
    if args.apply:
        return do_apply(args.root, args.force)
    if args.derive:
        rows, P_n2a, err = derive(args.root, args.rev)
        if err:
            print(err)
            return 1
        bad = proofs(rows, P_n2a)
        write_map(rows, MAP)
        print('wrote %s (%d rows); %d proof problems' % (MAP, len(rows), len(bad)))
        for x in bad[:40]:
            print('  ' + x)
        return 1 if bad else 0
    if args.headers:
        rows, err = derive_headers(args.root, args.rev)
        if err:
            print(err)
            return 1
        with open(HDRMAP, 'w', encoding='utf-8', newline='') as f:
            w = csv.DictWriter(f, delimiter='\t', lineterminator='\n', fieldnames=HDR_FIELDS)
            w.writeheader()
            for r in rows:
                w.writerow(r)
        un = [r for r in rows if r['verdict'] != 'RESOLVED']
        print('wrote %s (%d headers, %d unresolved)' % (HDRMAP, len(rows), len(un)))
        for r in un:
            print('  ' + r['port_header'] + '  ' + r['verdict'])
        return 0
    return 1


if __name__ == '__main__':
    sys.exit(main())
