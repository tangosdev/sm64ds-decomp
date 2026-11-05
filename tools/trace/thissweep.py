"""thissweep: what class is `this` when an unnamed function runs?

  python tools/trace/thissweep.py run --coverage DIR/coverage.tsv --state B1.ml --out OUT
          [--scene B1_walk] [--n 50] [--frames 300] [--script ops.json] [--collect-every 10]
  python tools/trace/thissweep.py table OUT                     (offline: rebuild from OUT/sweep.json)

Needs `ask.py serve --backend ctl`. The functions are N unnamed `func_` rows of
a scenecov coverage.tsv that ran in the given scene (default: every scene of
the state's key), spaced evenly by address. Each gets a log-action breakpoint
(this_watch: never halts) that records r0 and the 16 bytes at r0, but only for
a u32[r0] it has not logged yet, so the log holds each distinct vptr once.
this_collect folds the logs every few frames and resolves each word as a vptr:
a _ZTV symbol (a primary vptr is the _ZTV row's own address), the class it demangles
to, the RTTI base chain read from the type_info in RAM, and the actor id
(dBase_c::actorID, +0x0c) where the class derives from dActor_c. A function
whose r0 words exceed --max-keys distinct values is dropped as "not an object
pointer". Coverage over the same addresses gives the call counts.

Verdicts, per function:
  one_class        every call's r0 had one class's vptr: evidence the function
                   is a method of that class (or of a base only this class reaches
                   here: one scene is a sample, not proof)
  several_classes  every r0 was an object, of more than one class: a base-class
                   method or a free function on objects; `common_base` is the
                   nearest RTTI base they share
  mixed            some r0 words were vptrs and some were not
  not_object       no r0 word was a vptr (r0 is no `this`, or a static/free function)
  not_run          no calls in this run

Caveat: at a constructor's entry the object's vptr is not yet the derived one.
"""
import argparse
import json
import os
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import emusession as es     # noqa: E402
import scenerun as sr       # noqa: E402
import symtab               # noqa: E402


def read_coverage(path):
    with open(path, encoding='utf-8') as f:
        head = f.readline().rstrip('\n').split('\t')
        rows = [dict(zip(head, line.rstrip('\n').split('\t'))) for line in f if line.strip()]
    scenes = head[head.index('thumb') + 1:head.index('scenes_hit')]
    return rows, scenes


def pick(rows, scenes, n):
    cand = [r for r in rows if r['name'].startswith('func_') and any(int(r[s]) for s in scenes)]
    cand.sort(key=lambda r: (r['module'] != 'arm9', r['module'], r['addr']))
    if len(cand) <= n:
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


def cmd_run(a):
    rows, scenes = read_coverage(a.coverage)
    key = a.state_key or os.path.basename(a.state).split('_')[0]
    use = [a.scene] if a.scene else [s for s in scenes if s.split('_')[0] == key]
    if not use:
        raise SystemExit('no coverage scene for %s (scenes: %s)' % (key, scenes))
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
    runner.c.this_watch(functions=watch, max_keys=a.max_keys)
    runner.c.fork(cmd='coverage_start', args={'addresses': [w['addr'] for w in watch], 'reset': True})
    collects = []

    def on_frame(f):
        if f and f % a.collect_every == 0:
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
           'align': runner.align_when, 'resident': resident, 'resident_end': resident_end, 'collects': collects,
           'status': runner.status(), 'secs': round(time.time() - t, 2), 'functions': out}
    with open(os.path.join(a.out, 'sweep.json'), 'w', encoding='utf-8') as f:
        json.dump(res, f, indent=1)
    cmd_table(argparse.Namespace(dir=a.out))


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


def cmd_table(a):
    with open(os.path.join(a.dir, 'sweep.json'), encoding='utf-8') as f:
        res = json.load(f)
    fns = res['functions']
    cols = ['module', 'addr', 'name', 'thumb', 'calls', 'verdict', 'class', 'n_keys', 'classes', 'rtti',
            'evaluations', 'logged']
    with open(os.path.join(a.dir, 'sweep.tsv'), 'w', encoding='utf-8', newline='') as f:
        f.write('\t'.join(cols) + '\n')
        for r in sorted(fns, key=lambda r: (r['verdict'], -r['calls'])):
            objs = [k for k in r['keys'] if k.get('rtti')]
            f.write('\t'.join([r['module'], r['addr'], r['name'], 'yes' if r['thumb'] else 'no', str(r['calls']),
                               r['verdict'], r['class'] or '', str(len(r['keys'])), ' '.join(_cls_list(r)),
                               ' > '.join(objs[0]['rtti']) if len(objs) == 1 else '',
                               str(r['evaluations']), str(r['logged'])]) + '\n')
    by = {}
    for r in fns:
        by.setdefault(r['verdict'], []).append(r)
    one = sorted(by.get('one_class', []), key=lambda r: -r['calls'])
    L = ['# `this` sweep: %s, %d functions, %d hooked frames' % (res['state_key'], len(fns), res['frames']), '',
         '- scenes the functions were picked from: %s; overlays %s%s' % (
             ', '.join(res['scenes']), ' '.join('%03d' % i for i in res['resident']),
             '' if res['resident_end'] == res['resident'] else ' -> ' + ' '.join('%03d' % i for i in res['resident_end'])),
         '- verdicts: ' + ', '.join('%s %d' % (k, len(v)) for k, v in sorted(by.items())), '',
         '## One class (method evidence), by call count', '']
    for r in one:
        k = r['keys'][0]
        L.append('- `%s` (%s) -> %s, %d calls%s; RTTI %s' % (
            r['name'], r['module'], r['class'], r['calls'],
            ' [%s]' % ','.join(k['actor_names']) if k.get('actor_names') else '',
            ' > '.join(k.get('rtti') or [])))
    L += ['', '## Several classes (base-class method or free function)', '']
    for r in sorted(by.get('several_classes', []), key=lambda r: -r['calls']):
        L.append('- `%s` (%s), %d calls: %s; common base %s' % (
            r['name'], r['module'], r['calls'], ' '.join(_cls_list(r)), r['class'] or 'none'))
    for v in ('mixed', 'not_object', 'not_run'):
        rs = sorted(by.get(v, []), key=lambda r: -r['calls'])
        if rs:
            L += ['', '## %s (%d)' % (v, len(rs)), '',
                  ' '.join('`%s`(%d%s)' % (r['name'], r['calls'], ', many' if r['many'] else '') for r in rs)]
    L.append('')
    md = '\n'.join(L)
    with open(os.path.join(a.dir, 'sweep.md'), 'w', encoding='utf-8') as f:
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
    r.add_argument('--n', type=int, default=50)
    r.add_argument('--frames', type=int, default=300)
    r.add_argument('--script', help='JSON list of scenerun ops (input during the sweep)')
    r.add_argument('--align', help='JSON align object (default: scenerun.DEFAULT_ALIGN)')
    r.add_argument('--collect-every', type=int, default=10)
    r.add_argument('--max-keys', type=int, default=32)
    r.add_argument('--rpc', type=int, default=es.DEFAULT_RPC_PORT)
    r.set_defaults(fn=cmd_run)
    t = sub.add_parser('table', help='rebuild sweep.tsv and sweep.md')
    t.add_argument('dir')
    t.set_defaults(fn=cmd_table)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == '__main__':
    main()
