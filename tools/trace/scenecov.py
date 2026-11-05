"""scenecov: which functions run in which scene. Execution coverage over every
function start in config/arm9/**/symbols.txt (arm9, itcm and each overlay the
scene has resident), one column per scene.

  python tools/trace/scenecov.py run PLAN.json --out DIR       (needs `ask.py serve --backend ctl`)
  python tools/trace/scenecov.py table DIR                     (offline: rebuild the TSV + summary)

PLAN.json:
  {"scenes": [{"name": "B1_idle", "state": "path/B1.ml", "frames": 300},
              {"name": "B1_walk", "state": "path/B1.ml", "frames": 300,
               "script": [{"at": 0, "frames": 100, "buttons": ["UP"]}]}],
   "align": {...}}          (scenerun's default: CheckInput, file timer + 300)

A scene loads its state, aligns (scenerun), reads the game's loaded-overlay
table, starts coverage on the arm9 + itcm + resident-overlay function starts,
steps `frames` hooked frames with the script's input forced every frame, and
reads the counts. Coverage records the PC, so each hit's first instruction
word is checked against the module's image bytes; a hit whose word does not
match (another overlay's code at that address) is reported as `foreign` and
not counted. The scene also records the overlay table at the end, so a load
or unload during the run shows.

DIR/coverage.tsv: one row per function that ran in any scene: module, addr,
name, named, class (from the mangled name), size, thumb, one count column per
scene, scenes_hit, states_hit. DIR/summary.json + summary.md: per scene, how
many functions and unnamed func_ symbols ran; the unnamed ones that ran in only
one scene, and in only one state.
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


def watch_list(tab, resident):
    """[(module, Sym)] for arm9, itcm and the resident overlays' functions."""
    mods = ['arm9', 'itcm'] + ['ov%03d' % i for i in resident]
    out = []
    for m in mods:
        if m in tab.mods:
            out += [(m, s) for s in tab.functions(m)]
    return out


def run_scene(runner, plan, sc, out):
    path = os.path.join(out, 'scenes', sc['name'] + '.json')
    if os.path.exists(path):
        return None
    tab = symtab.table()
    t = time.time()
    runner.log = []
    runner.load(sc['state'])
    runner.align(sc['state'], sc.get('align') or plan.get('align'))
    resident = runner.resident()
    watch = watch_list(tab, resident)
    addrs = sorted({s.addr for _, s in watch})
    runner.c.fork(cmd='coverage_start', args={'addresses': ['0x%08x' % a for a in addrs], 'reset': True})
    script = sr.Script(sc.get('script') or [])
    n = int(sc.get('frames', 300))
    runner.frames(script, n)
    cov = runner.c.fork(cmd='coverage_stop', args={'nonzero_only': True, 'clear': True})
    resident_end = runner.resident()
    runner.finish()
    bymod = {}
    for m, s in watch:
        bymod.setdefault(s.addr, []).append((m, s))
    hits, foreign = [], []
    for c in cov.get('counts', []):
        a = int(c['address'], 16) & ~1
        word = int(c.get('first_word') or '0', 16)
        cands = bymod.get(a, [])
        owner = None
        for m, s in cands:
            img = tab.code(m, a, 4)
            if img is None:
                owner = (m, s)            # itcm: no image to check, always resident
                break
            want = int.from_bytes(img, 'little')
            mask = 0xffff if s.thumb else 0xffffffff
            if word & mask == want & mask:
                owner = (m, s)
                break
        rec = {'addr': '%08x' % a, 'count': int(c['count']), 'first_frame': c.get('first_frame'),
               'last_frame': c.get('last_frame'), 'first_word': c.get('first_word')}
        if owner is None:
            rec['candidates'] = [m for m, _ in cands]
            foreign.append(rec)
        else:
            rec.update(module=owner[0], name=owner[1].name)
            hits.append(rec)
    res = {'scene': sc['name'], 'state': sc['state'], 'state_key': sc.get('state_key') or sc['name'].split('_')[0],
           'frames': n, 'frames_covered': cov.get('frames_covered'), 'align': runner.align_when,
           'resident': resident, 'resident_end': resident_end, 'watched': len(addrs),
           'hit': len(hits), 'foreign': foreign, 'hits': hits, 'status': runner.status(),
           'script': script.ops, 'secs': round(time.time() - t, 2)}
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'w', encoding='utf-8') as f:
        json.dump(res, f, indent=1)
    return res


def cmd_run(a):
    plan = sr.load_json(a.plan)
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, 'plan.json'), 'w', encoding='utf-8') as f:
        json.dump(plan, f, indent=1)
    runner = sr.Runner(port=a.rpc)
    if runner.backend != 'ctl':
        raise SystemExit('scenecov needs the control-socket backend (ask.py serve --backend ctl)')
    for sc in plan['scenes']:
        r = run_scene(runner, plan, sc, a.out)
        if r:
            print(json.dumps({'scene': r['scene'], 'watched': r['watched'], 'hit': r['hit'],
                              'foreign': len(r['foreign']), 'resident': r['resident'],
                              'resident_end': r['resident_end'], 'secs': r['secs']}), flush=True)
    cmd_table(argparse.Namespace(dir=a.out))


def cmd_table(a):
    out = a.dir
    plan = sr.load_json(os.path.join(out, 'plan.json'))
    tab = symtab.table()
    scenes = []
    for sc in plan['scenes']:
        p = os.path.join(out, 'scenes', sc['name'] + '.json')
        if os.path.exists(p):
            with open(p, encoding='utf-8') as f:
                scenes.append(json.load(f))
    names = [s['scene'] for s in scenes]
    states = sorted({s['state_key'] for s in scenes})
    rows = {}
    for s in scenes:
        for h in s['hits']:
            k = (h['module'], h['addr'])
            if k not in rows:
                sym = tab.function_at(h['module'], int(h['addr'], 16))
                rows[k] = {'module': h['module'], 'addr': h['addr'], 'name': h['name'],
                           'named': symtab.named(h['name']), 'class': symtab.owner_class(h['name']) or '',
                           'size': sym.size if sym else 0, 'thumb': bool(sym and sym.thumb), 'counts': {}}
            rows[k]['counts'][s['scene']] = h['count']
    st_of = {s['scene']: s['state_key'] for s in scenes}
    for r in rows.values():
        r['scenes_hit'] = len(r['counts'])
        r['states_hit'] = sorted({st_of[n] for n in r['counts']})
    order = sorted(rows.values(), key=lambda r: (r['module'] != 'arm9', r['module'], r['addr']))
    with open(os.path.join(out, 'coverage.tsv'), 'w', encoding='utf-8', newline='') as f:
        f.write('\t'.join(['module', 'addr', 'name', 'named', 'class', 'size', 'thumb'] + names +
                          ['scenes_hit', 'states_hit']) + '\n')
        for r in order:
            f.write('\t'.join([r['module'], r['addr'], r['name'], 'yes' if r['named'] else 'no', r['class'],
                               '0x%x' % r['size'], 'yes' if r['thumb'] else 'no'] +
                              [str(r['counts'].get(n, 0)) for n in names] +
                              [str(r['scenes_hit']), ','.join(r['states_hit'])]) + '\n')

    def is_func(r):
        return r['name'].startswith('func_')
    summ = {'scenes': [], 'functions_hit': len(rows), 'unnamed_func_hit': sum(1 for r in rows.values() if is_func(r)),
            'states': states}
    for s in scenes:
        hit = [r for r in rows.values() if s['scene'] in r['counts']]
        summ['scenes'].append({
            'scene': s['scene'], 'state': s['state_key'], 'frames': s['frames'],
            'frames_covered': s['frames_covered'], 'resident': s['resident'], 'resident_end': s['resident_end'],
            'watched': s['watched'], 'hit': len(hit), 'named_hit': sum(1 for r in hit if r['named']),
            'unnamed_func_hit': sum(1 for r in hit if is_func(r)),
            'unnamed_func_by_module': _by_mod([r for r in hit if is_func(r)]),
            'only_this_scene_unnamed': sum(1 for r in hit if is_func(r) and r['scenes_hit'] == 1),
            'foreign_hits': len(s['foreign']), 'host_input': s.get('status')})
    for st in states:
        hit = [r for r in rows.values() if st in r['states_hit']]
        only = [r for r in hit if is_func(r) and r['states_hit'] == [st]]
        summ.setdefault('per_state', []).append({
            'state': st, 'hit': len(hit), 'unnamed_func_hit': sum(1 for r in hit if is_func(r)),
            'unnamed_func_only_this_state': len(only), 'only_by_module': _by_mod(only)})
    summ['only_one_scene_unnamed'] = [
        {'scene': next(iter(r['counts'])), 'module': r['module'], 'addr': r['addr'], 'name': r['name'],
         'count': next(iter(r['counts'].values()))}
        for r in order if is_func(r) and r['scenes_hit'] == 1]
    summ['only_one_state_unnamed'] = [
        {'state': r['states_hit'][0], 'module': r['module'], 'name': r['name'], 'counts': r['counts']}
        for r in order if is_func(r) and len(r['states_hit']) == 1 and len(states) > 1]
    with open(os.path.join(out, 'summary.json'), 'w', encoding='utf-8') as f:
        json.dump(summ, f, indent=1)
    md = _markdown(summ)
    with open(os.path.join(out, 'summary.md'), 'w', encoding='utf-8') as f:
        f.write(md)
    print(md)


def _by_mod(rows):
    d = {}
    for r in rows:
        d[r['module']] = d.get(r['module'], 0) + 1
    return d


def _markdown(s):
    L = ['# Scene coverage', '', '%d functions ran in some scene; %d of them unnamed `func_`.' % (
        s['functions_hit'], s['unnamed_func_hit']), '',
        '| scene | overlays | watched | ran | named | unnamed func_ | only this scene (func_) | foreign |',
        '|---|---|---|---|---|---|---|---|']
    for x in s['scenes']:
        ov = ' '.join('%03d' % i for i in x['resident'])
        if x['resident_end'] != x['resident']:
            ov += ' -> ' + ' '.join('%03d' % i for i in x['resident_end'])
        L.append('| %s | %s | %d | %d | %d | %d | %d | %d |' % (
            x['scene'], ov, x['watched'], x['hit'], x['named_hit'], x['unnamed_func_hit'],
            x['only_this_scene_unnamed'], x['foreign_hits']))
    L.append('')
    for p in s.get('per_state', []):
        L.append('- %s: %d functions ran, %d unnamed func_; %d func_ ran only in this state (%s)' % (
            p['state'], p['hit'], p['unnamed_func_hit'], p['unnamed_func_only_this_state'],
            ', '.join('%s %d' % kv for kv in sorted(p['only_by_module'].items()))))
    L.append('')
    one = s['only_one_scene_unnamed']
    L.append('## Unnamed func_ that ran in only one scene (%d)' % len(one))
    L.append('')
    by = {}
    for r in one:
        by.setdefault(r['scene'], []).append(r)
    for sc, rs in by.items():
        L.append('- %s (%d): %s' % (sc, len(rs), ' '.join('%s:%s(%d)' % (r['module'], r['name'], r['count'])
                                                          for r in rs[:80]) + (' ...' if len(rs) > 80 else '')))
    L.append('')
    return '\n'.join(L)


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = p.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run', help='run every scene in the plan')
    r.add_argument('plan')
    r.add_argument('--out', required=True)
    r.add_argument('--rpc', type=int, default=es.DEFAULT_RPC_PORT)
    r.set_defaults(fn=cmd_run)
    t = sub.add_parser('table', help='rebuild coverage.tsv and the summary from DIR/scenes')
    t.add_argument('dir')
    t.set_defaults(fn=cmd_table)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == '__main__':
    main()
