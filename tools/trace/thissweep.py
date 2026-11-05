"""thissweep: what class is `this` when an unnamed function runs?

  python tools/trace/thissweep.py run --coverage DIR/coverage.tsv --state B1.ml --out OUT
          [--scene B1_walk] [--n 50] [--frames 300] [--script ops.json]
          [--sample reservoir|distinct] [--log-limit 64] [--arg-bytes 16]
  python tools/trace/thissweep.py table OUT                     (offline: rebuild from OUT/sweep.json)

Needs `ask.py serve --backend ctl`. The functions are N unnamed `func_` rows of
a scenecov coverage.tsv that ran in the given scene (default: every scene of
the state's key), spaced evenly by address (--n 0: all of them). Nothing halts:
each function gets one log-action breakpoint in the fork.

--sample reservoir (the default): the fork keeps a uniform sample of --log-limit
hits out of every hit of the run (reservoir sampling), each with r0-r3, sp, lr and
--arg-bytes bytes at each of r0-r3. The samples spread evenly over the run; a
halting probe armed in waves samples the first calls after each arm instead,
which over-weights whatever object the game visits first.
--sample distinct: the older mode. The breakpoint logs r0 and 16 bytes at r0 only
for a u32[r0] it has not logged yet, so the log holds each distinct vptr once and
every class that ever reaches the function is seen, but nothing says how often.

Each r0 word is resolved as a vptr: a _ZTV symbol (a primary vptr is the _ZTV row's
own address), the class it demangles to, the RTTI base chain read from the
type_info in RAM, and the actor id (dBase_c::actorID, +0x0c) where the class
derives from dActor_c. A function whose r0 words exceed --max-keys distinct values
is "not an object pointer". Coverage over the same addresses gives the call counts.

Verdicts, per function:
  one_class        every sampled r0 had one class's vptr
  several_classes  every r0 was an object, of more than one class; `class` is the
                   nearest RTTI base they share
  mixed            some r0 words were vptrs and some were not
  not_object       no r0 word was a vptr (r0 is no `this`, or a static/free function)
  not_run          no calls in this run

A vptr in r0 is evidence only if the function reads r0. `table` disassembles each
function from the extracted module image (r0use.py) and records r0_used and
r0_first_use; the `method` category needs one_class AND r0_used. func_020092c4
(`add r0, sp, #8` first) is the case this guards against: its r0 at entry is the
caller's leftover.

Caveat: at a constructor's entry the object's vptr is not yet the derived one.

OUT/sweep.json is the run's record; OUT/sweep.tsv and OUT/summary.md are generated
from it (summary.md reads only sweep.tsv and sweep.json, so every count in it is
recomputable).
"""
import argparse
import collections
import json
import os
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import emusession as es     # noqa: E402
import r0use                # noqa: E402
import scenerun as sr       # noqa: E402
import symtab               # noqa: E402

COLS = ['module', 'addr', 'name', 'thumb', 'calls', 'verdict', 'category', 'class', 'n_keys', 'n_objs',
        'n_samples', 'sample_classes', 'classes', 'rtti', 'r0_used', 'r0_first_use', 'r1', 'r2', 'r3',
        'first_frame', 'last_frame', 'evaluations', 'logged']


def read_coverage(path):
    with open(path, encoding='utf-8') as f:
        head = f.readline().rstrip('\n').split('\t')
        rows = [dict(zip(head, line.rstrip('\n').split('\t'))) for line in f if line.strip()]
    scenes = head[head.index('thumb') + 1:head.index('scenes_hit')]
    return rows, scenes


def candidates(rows, scenes):
    cand = [r for r in rows if r['name'].startswith('func_') and any(int(r[s]) for s in scenes)]
    cand.sort(key=lambda r: (r['module'] != 'arm9', r['module'], r['addr']))
    return cand


def pick(rows, scenes, n):
    cand = candidates(rows, scenes)
    if n <= 0 or len(cand) <= n:
        return cand
    step = len(cand) / float(n)
    return [cand[int(i * step)] for i in range(n)]


def verdict(rec):
    keys = rec['keys']
    if rec['calls'] == 0 and not keys:
        return 'not_run', None
    if rec['many']:
        return 'not_object', None
    objs = [k for k in keys if k.get('cls')]
    if not objs:
        return 'not_object', None
    if len(objs) < len(keys):
        return 'mixed', None
    classes = sorted({k['cls'] for k in objs})
    if len(classes) == 1:
        return 'one_class', classes[0]
    chains = [k.get('rtti') or [k['cls']] for k in objs]
    common = next((c for c in chains[0] if all(c in ch for ch in chains[1:])), None)
    return 'several_classes', common


def category(verdict_, r0_used):
    """The method claim needs the static r0 read as well as the runtime class."""
    if verdict_ == 'one_class':
        return 'method' if r0_used else ('one_class:r0-not-read' if r0_used is False else 'one_class:r0-unchecked')
    if verdict_ == 'several_classes':
        return 'base-method' if r0_used else ('several:r0-not-read' if r0_used is False else 'several:r0-unchecked')
    return verdict_


def cmd_run(a):
    rows, scenes = read_coverage(a.coverage)
    key = a.state_key or os.path.basename(a.state).split('_')[0]
    use = [a.scene] if a.scene else [s for s in scenes if s.split('_')[0] == key]
    if not use:
        raise SystemExit('no coverage scene for %s (scenes: %s)' % (key, scenes))
    cand = candidates(rows, use)
    funcs = pick(rows, use, a.n)
    tab = symtab.table()
    runner = sr.Runner(port=a.rpc)
    if runner.backend != 'ctl':
        raise SystemExit('thissweep needs the control-socket backend (ask.py serve --backend ctl)')
    script = sr.Script(sr.load_json(a.script) if a.script else [])
    os.makedirs(a.out, exist_ok=True)
    t = time.time()
    runner.load(a.state)
    runner.align(a.state, sr.load_json(a.align) if a.align else None)
    resident = runner.resident()
    watch = []
    for r in funcs:
        s = tab.function_at(r['module'], int(r['addr'], 16))
        watch.append({'addr': '0x' + r['addr'], 'module': r['module'], 'thumb': bool(s and s.thumb), 'name': r['name']})
    reservoir = a.sample == 'reservoir'
    runner.c.this_watch(functions=watch, max_keys=a.max_keys, sample='reservoir' if reservoir else None,
                        log_limit=a.log_limit, arg_bytes=a.arg_bytes)
    runner.c.fork(cmd='coverage_start', args={'addresses': [w['addr'] for w in watch], 'reset': True})
    collects = []

    def on_frame(f):
        # distinct mode re-arms every few frames; a reservoir is read once, at the end
        if not reservoir and f and f % a.collect_every == 0:
            collects.append(runner.c.this_collect().get('new', 0))
    runner.frames(script, a.frames, on_frame=on_frame)
    collects.append(runner.c.this_collect().get('new', 0))
    rep = runner.c.this_unwatch()
    cov = runner.c.fork(cmd='coverage_stop', args={'clear': True})
    resident_end = runner.resident()
    runner.finish()
    calls = {}
    for c in cov.get('counts', []):
        a0 = int(c['address'], 16) & ~1
        calls[a0] = (int(c['count']), c.get('first_word'))
    out = []
    for w in rep:
        a0 = int(w['addr'], 16)
        n, word = calls.get(a0, (0, None))
        img = tab.code(w['module'], a0, 4)
        if n and img is not None and word is not None:
            mask = 0xffff if w['thumb'] else 0xffffffff
            if int(word, 16) & mask != int.from_bytes(img, 'little') & mask:
                n = 0                       # another overlay's code ran at this address
        rec = dict(w, calls=n, coverage_first_word=word)
        rec['verdict'], rec['class'] = verdict(rec)
        out.append(rec)
    res = {'state': a.state, 'state_key': key, 'scenes': use, 'frames': a.frames, 'script': script.ops,
           'sample': a.sample, 'log_limit': a.log_limit if reservoir else None,
           'arg_bytes': a.arg_bytes if reservoir else None, 'n_requested': a.n,
           'candidates': [{'module': r['module'], 'addr': r['addr'], 'name': r['name'],
                           'calls': sum(int(r[s]) for s in use)} for r in cand],
           'align': runner.align_when, 'resident': resident, 'resident_end': resident_end, 'collects': collects,
           'status': runner.status(), 'secs': round(time.time() - t, 2), 'functions': out}
    with open(os.path.join(a.out, 'sweep.json'), 'w', encoding='utf-8') as f:
        json.dump(res, f, indent=1)
    cmd_table(argparse.Namespace(dir=a.out))


# ------------------------------------------------------------------ offline table
def _cls_list(rec):
    out = []
    for k in rec['keys']:
        c = k.get('cls') or ('?' + k['key'])
        if k.get('vtable_offset') not in (None, 0):
            c += '@+%d' % k['vtable_offset']
        if k.get('actor_names'):
            c += '[%s]' % ','.join(k['actor_names'])
        out.append(c)
    return out


def value_kind(v, sp, vt=None):
    """Coarse kind of a register value: zero, int, negint, stack, ram, obj:<class>, dtcm, io, vram,
    large (e.g. an fx32 above 16.0) or neglarge."""
    v &= 0xffffffff
    if v == 0:
        return 'zero'
    if v < 0x10000:
        return 'int'
    if v >= 0xffff0000:
        return 'negint'
    if 0x02000000 <= v < 0x02400000:
        if sp is not None and sp - 0x80 <= v < sp + 0x2000:
            return 'stack'
        return ('obj:' + vt) if vt else 'ram'
    if 0x027e0000 <= v < 0x02800000:
        return 'dtcm'
    if 0x04000000 <= v < 0x05000000:
        return 'io'
    if 0x06000000 <= v < 0x07000000:
        return 'vram'
    return 'neglarge' if v >= 0x80000000 else 'large'


def _shape(kinds):
    c = collections.Counter(kinds)
    n = sum(c.values())
    if not n:
        return ''
    return ', '.join(k + ('' if m == n else ' %d%%' % round(100.0 * m / n)) for k, m in c.most_common(3)) + \
        (', ...' if len(c) > 3 else '')


def sample_columns(rec, tab, mods):
    """From reservoir samples: per-class sample counts, distinct object pointers, arg shapes."""
    S = rec.get('samples') or []
    if not S:
        objs = set()
        for k in rec['keys']:
            if k.get('cls'):
                objs.update(k.get('objs') or [k.get('obj')])
        return {'n_samples': 0, 'sample_classes': '', 'n_objs': len(objs), 'r1': '', 'r2': '', 'r3': '',
                'first_frame': '', 'last_frame': ''}
    bykey = {k['key']: k for k in rec['keys']}
    cls_n = collections.Counter()
    objs = set()
    regs = {1: [], 2: [], 3: []}
    for s in S:
        k = bykey.get(s['key'], {})
        if k.get('cls'):
            cls_n[k['cls']] += 1
            objs.add(s['r0'])
        else:
            cls_n['non-object'] += 1
        sp = int(s['sp'], 16) if s.get('sp') else None
        for i in (1, 2, 3):
            v = int(s['r%d' % i], 16)
            vt = None
            m = s.get('m%d' % i)
            if m and m != 'ERR' and len(m) >= 8 and 0x02000000 <= v < 0x02400000:
                sym, off = tab.vtable(int.from_bytes(bytes.fromhex(m[:8]), 'little'), mods)
                if sym is not None and off == 0:
                    vt = symtab.vtable_class(sym.name) or sym.name
            regs[i].append(value_kind(v, sp, vt))
    frames = [s['frame'] for s in S if s.get('frame') is not None]
    return {'n_samples': len(S), 'n_objs': len(objs),
            'sample_classes': ', '.join('%s x%d' % kv for kv in cls_n.most_common(6)) + (' ...' if len(cls_n) > 6 else ''),
            'r1': _shape(regs[1]), 'r2': _shape(regs[2]), 'r3': _shape(regs[3]),
            'first_frame': str(min(frames)) if frames else '', 'last_frame': str(max(frames)) if frames else ''}


def table_rows(res, tab=None):
    tab = tab or symtab.table()
    mods = ['arm9'] + ['ov%03d' % i for i in res.get('resident') or []]
    rows = []
    for r in res['functions']:
        objs = [k for k in r['keys'] if k.get('rtti')]
        chk = r0use.check_symbol(tab, r['module'], int(r['addr'], 16))
        row = {'module': r['module'], 'addr': r['addr'], 'name': r['name'], 'thumb': 'yes' if r['thumb'] else 'no',
               'calls': str(r['calls']), 'verdict': r['verdict'], 'class': r['class'] or '',
               'category': category(r['verdict'], chk['r0_used']),
               'n_keys': str(len(r['keys'])), 'classes': ' '.join(_cls_list(r)),
               'rtti': ' > '.join(objs[0]['rtti']) if len(objs) == 1 else '',
               'r0_used': {True: 'yes', False: 'no', None: '?'}[chk['r0_used']],
               'r0_first_use': chk['r0_first_use'],
               'evaluations': str(r['evaluations']), 'logged': str(r['logged'])}
        row.update({k: str(v) for k, v in sample_columns(r, tab, mods).items()})
        rows.append(row)
    rows.sort(key=lambda r: (r['category'], -int(r['calls']), r['module'], r['addr']))
    return rows


def write_tsv(path, rows):
    with open(path, 'w', encoding='utf-8', newline='') as f:
        f.write('\t'.join(COLS) + '\n')
        for r in rows:
            f.write('\t'.join(str(r.get(c, '')).replace('\t', ' ').replace('\n', ' ') for c in COLS) + '\n')


def read_tsv(path):
    with open(path, encoding='utf-8') as f:
        head = f.readline().rstrip('\n').split('\t')
        return [dict(zip(head, line.rstrip('\n').split('\t'))) for line in f if line.strip()]


def summary_md(res, rows):
    """summary.md from sweep.json (`res`) and sweep.tsv (`rows`) only."""
    n = len(rows)
    by_v = collections.Counter(r['verdict'] for r in rows)
    by_c = collections.Counter(r['category'] for r in rows)
    sampled = {(r['module'], r['addr']) for r in rows if int(r['n_samples'] or 0) or int(r['logged'] or 0)}
    picked = {(r['module'], r['addr']) for r in rows}
    cand = res.get('candidates') or []
    not_picked = [c for c in cand if (c['module'], c['addr']) not in picked]
    picked_unsampled = [r for r in rows if (r['module'], r['addr']) not in sampled]
    mode = res.get('sample') or 'distinct'
    L = ['# `this` sweep: %s' % res['state_key'], '',
         'Generated by `thissweep.py table` from sweep.json and sweep.tsv; every count below is recomputed '
         'from those two files.', '',
         '| field | value |', '|---|---|',
         '| state | `%s` |' % os.path.basename(res['state']),
         '| scenes | %s |' % ', '.join(res['scenes']),
         '| hooked frames | %d |' % res['frames'],
         '| sampling | %s%s |' % (mode, ', %s samples per function, %s bytes at r0-r3' % (
             res.get('log_limit'), res.get('arg_bytes')) if mode == 'reservoir' else ''),
         '| overlays | %s%s |' % (' '.join('%03d' % i for i in res['resident']),
                                  '' if res['resident_end'] == res['resident'] else
                                  ' -> ' + ' '.join('%03d' % i for i in res['resident_end'])),
         '| candidate func_ (ran in the scenes) | %d |' % len(cand),
         '| swept | %d |' % n,
         '| swept and sampled | %d |' % len(sampled),
         '| run time | %s s |' % res.get('secs'), '',
         '## Verdicts and categories', '',
         '| verdict | functions |', '|---|---|'] + \
        ['| %s | %d |' % kv for kv in sorted(by_v.items())] + \
        ['', '| category | functions |', '|---|---|'] + \
        ['| %s | %d |' % kv for kv in sorted(by_c.items())] + \
        ['', '`method` = one_class and the function reads r0 before writing it (r0use.py). '
         '`one_class:r0-not-read` = the runtime class is the caller\'s leftover in r0, not evidence.', '']
    meth = sorted([r for r in rows if r['category'] == 'method'], key=lambda r: -int(r['calls']))
    L += ['## method (%d), by call count' % len(meth), '',
          '| function | module | class | calls | n_objs | samples | r0 first use | r1 | r2 | r3 |',
          '|---|---|---|---|---|---|---|---|---|---|']
    for r in meth:
        L.append('| `%s` | %s | %s | %s | %s | %s | `%s` | %s | %s | %s |' % (
            r['name'], r['module'], r['class'], r['calls'], r['n_objs'], r['n_samples'], r['r0_first_use'],
            r['r1'], r['r2'], r['r3']))
    base = sorted([r for r in rows if r['category'] == 'base-method'], key=lambda r: -int(r['calls']))
    L += ['', '## base-method (%d)' % len(base), '',
          '| function | module | common base | calls | n_objs | sample classes |', '|---|---|---|---|---|---|']
    for r in base:
        L.append('| `%s` | %s | %s | %s | %s | %s |' % (r['name'], r['module'], r['class'] or 'none', r['calls'],
                                                     r['n_objs'], r['sample_classes'] or r['classes']))
    flagged = [r for r in rows if r['category'].endswith(':r0-not-read')]
    L += ['', '## Flagged: object in r0 but r0 not read (%d)' % len(flagged), '']
    for r in sorted(flagged, key=lambda r: -int(r['calls'])):
        L.append('- `%s` (%s) %s: r0 held %s; `%s`' % (r['name'], r['module'], r['verdict'], r['class'] or r['classes'],
                                                     r['r0_first_use']))
    for v in ('mixed', 'not_object', 'not_run'):
        rs = sorted([r for r in rows if r['category'] == v], key=lambda r: -int(r['calls']))
        if rs:
            L += ['', '## %s (%d)' % (v, len(rs)), '',
                  ' '.join('`%s`(%s)' % (r['name'], r['calls']) for r in rs)]
    L += ['', '## Coverage gaps', '',
          '- candidate func_ never swept (not picked; --n %s of %d): %d' % (res.get('n_requested'), len(cand),
                                                                          len(not_picked)),
          '- swept but never sampled (no logged call): %d' % len(picked_unsampled), '']
    if picked_unsampled:
        L.append('Swept, never sampled: ' + ' '.join('`%s`' % r['name'] for r in picked_unsampled))
        L.append('')
    if not_picked:
        L.append('Never swept (%d), by module:' % len(not_picked))
        L.append('')
        bym = collections.OrderedDict()
        for c in not_picked:
            bym.setdefault(c['module'], []).append(c)
        for m, cs in bym.items():
            L.append('- %s (%d): %s' % (m, len(cs), ' '.join('%s(%d)' % (c['name'], c['calls']) for c in cs)))
        L.append('')
    return '\n'.join(L)


def cmd_table(a):
    with open(os.path.join(a.dir, 'sweep.json'), encoding='utf-8') as f:
        res = json.load(f)
    write_tsv(os.path.join(a.dir, 'sweep.tsv'), table_rows(res))
    write_summary(a.dir)


def write_summary(d):
    with open(os.path.join(d, 'sweep.json'), encoding='utf-8') as f:
        res = json.load(f)
    md = summary_md(res, read_tsv(os.path.join(d, 'sweep.tsv')))
    with open(os.path.join(d, 'summary.md'), 'w', encoding='utf-8') as f:
        f.write(md)
    print(md)


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = p.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run', help='sweep N unnamed functions from a coverage table')
    r.add_argument('--coverage', required=True, help='scenecov coverage.tsv')
    r.add_argument('--state', required=True)
    r.add_argument('--state-key', help='coverage scene prefix (default: the state file name up to "_")')
    r.add_argument('--scene', help='pick from this coverage scene only')
    r.add_argument('--out', required=True)
    r.add_argument('--n', type=int, default=50, help='functions to sweep, evenly by address (0: all candidates)')
    r.add_argument('--frames', type=int, default=300)
    r.add_argument('--script', help='JSON list of scenerun ops (input during the sweep)')
    r.add_argument('--align', help='JSON align object (default: scenerun.DEFAULT_ALIGN)')
    r.add_argument('--sample', choices=('reservoir', 'distinct'), default='reservoir')
    r.add_argument('--log-limit', type=int, default=64, help='reservoir: samples kept per function')
    r.add_argument('--arg-bytes', type=int, default=16, help='reservoir: bytes logged at each of r0-r3')
    r.add_argument('--collect-every', type=int, default=10, help='distinct: frames between log folds')
    r.add_argument('--max-keys', type=int, default=32)
    r.add_argument('--rpc', type=int, default=es.DEFAULT_RPC_PORT)
    r.set_defaults(fn=cmd_run)
    t = sub.add_parser('table', help='rebuild sweep.tsv and summary.md')
    t.add_argument('dir')
    t.set_defaults(fn=cmd_table)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == '__main__':
    main()
