"""ramdiff: RAM-diff screening. Which bytes does an input event change, compared
with doing nothing from the same aligned frame?

  python tools/trace/ramdiff.py run PLAN.json --out DIR     (needs `ask.py serve`)
  python tools/trace/ramdiff.py analyze DIR                 (offline; reads DIR/plan.json)

PLAN.json:
  {"states":  {"C1": "path/C1.ml", "B1": "path/B1.ml"},
   "align":   {"pc": "0x0202c2ac", "when": "[0x020a0db0] == {file:0x020a0db0} + 300"},   (default)
   "offsets": [1, 5, 15, 30, 60, 120],      hooked frames after the align hit (0 is always taken)
   "ranges":  ["arm9", "overlays", "player"],   (default) arm9 .data+.bss, every resident
                                             overlay's .data+.bss, 0x800 bytes of the player;
                                             or {"name", "addr" (expression), "size"} entries
   "reps": 2, "idle_reps": 2,
   "events":  {"jump_B": [{"at": 0, "frames": 8, "buttons": ["B"]}],
               "coin_to_player": {"states": ["B1"], "ops": [...]}},     ops: see scenerun.py
   "expect":  [{"state": "B1", "event": "pause_start", "addr": "0x0209f2c4"}]}   (optional checks)

Every run reloads the state and aligns, so all runs share frame 0. That is
checked: a byte that differs at frame 0 between any run and idle r1 is load
residue (RAM the savestate load leaves as the previous run had it; on the
2026-11 runs, 7 bytes of the idle thread's stack area, data_020a6188) and is
treated as noisy. The idle runs classify every byte as
constant, deterministic-changing (changes over time, the same in every idle run)
or noisy (differs between idle runs; never reported). An event's bytes are
diffed against idle at the same frame offset, so deterministic-changing bytes
still count when the event changes them differently.

analyze writes DIR/candidates.tsv (one row per state, event and symbol: named
rows are kept and marked), DIR/summary.json and DIR/summary.md. Symbols come
from config/arm9/**/symbols.txt; readers/writers are the functions whose literal
pools load the symbol's address (relocs.txt), classified by how the loaded
pointer is used (symtab.xref: R read, W write, A passed on).
"""
import argparse
import json
import os
import sys
import time
from collections import defaultdict

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

import emusession as es     # noqa: E402
import scenerun as sr       # noqa: E402
import symtab               # noqa: E402

DEFAULT_OFFSETS = [1, 5, 15, 30, 60, 120]
DEFAULT_RANGES = ['arm9', 'overlays', 'player']
PLAYER_SIZE = 0x800


def _events(plan, state):
    out = {}
    for name, ev in plan.get('events', {}).items():
        if isinstance(ev, dict):
            if ev.get('states') and state not in ev['states']:
                continue
            ops = ev.get('ops', [])
        else:
            ops = ev
        out[name] = sr.Script(ops)
    return out


def _ranges(plan, runner):
    tab = symtab.table()
    out = []
    for r in plan.get('ranges', DEFAULT_RANGES):
        if r == 'arm9':
            lo, hi = tab.data_range('arm9')
            out.append({'name': 'arm9', 'addr': '0x%08x' % lo, 'size': hi - lo})
        elif r == 'overlays':
            for i in runner.resident():
                mod = 'ov%03d' % i
                dr = tab.data_range(mod) if mod in tab.mods else None
                if dr:
                    out.append({'name': mod, 'addr': '0x%08x' % dr[0], 'size': dr[1] - dr[0]})
        elif r == 'player':
            out.append({'name': 'player', 'addr': '[0x%08x]' % sr.PLAYER_PTR, 'size': PLAYER_SIZE})
        elif isinstance(r, dict):
            out.append({'name': r['name'], 'addr': r['addr'], 'size': es.parse_int(r['size'])})
        else:
            raise ValueError('unknown range %r' % (r,))
    return out


def run_one(runner, plan, state_key, label, script, rep, out):
    d = os.path.join(out, 'runs', state_key, '%s_r%d' % (label, rep))
    if os.path.exists(os.path.join(d, 'run.json')):
        return None
    os.makedirs(d, exist_ok=True)
    state = plan['states'][state_key]
    offsets = sorted(set([0] + [int(x) for x in plan.get('offsets', DEFAULT_OFFSETS)]))
    t = time.time()
    runner.log = []
    runner.load(state)
    runner.align(state, plan.get('align'))
    st = runner.status()
    ranges = _ranges(plan, runner)
    resident = runner.resident()
    timers = {}

    def snap(f):
        if f in offsets:
            r = runner.c.ram_snapshot(ranges=ranges, path=os.path.join(d, 'f%03d.bin' % f))
            timers[f] = runner.u32(sr.FRAME_TIMER)
            return r
    runner.frames(script, max(offsets), on_frame=snap)
    resident_end = runner.resident()
    runner.finish()
    meta = {'state': state_key, 'path': state, 'event': label, 'rep': rep, 'offsets': offsets,
            'align': runner.align_when, 'timers': timers, 'resident': resident, 'resident_end': resident_end,
            'status': st, 'log': runner.log, 'ops': script.ops, 'backend': runner.backend,
            'secs': round(time.time() - t, 2)}
    with open(os.path.join(d, 'run.json'), 'w', encoding='utf-8') as f:
        json.dump(meta, f, indent=1)
    return meta


def cmd_run(a):
    plan = sr.load_json(a.plan)
    os.makedirs(a.out, exist_ok=True)
    with open(os.path.join(a.out, 'plan.json'), 'w', encoding='utf-8') as f:
        json.dump(plan, f, indent=1)
    runner = sr.Runner(port=a.rpc)
    states = a.only.split(',') if a.only else list(plan['states'])
    want = set(a.events.split(',')) if a.events else None
    for sk in states:
        evs = _events(plan, sk)
        todo = [('idle', sr.Script([]), r) for r in range(1, int(plan.get('idle_reps', 2)) + 1)]
        for r in range(1, int(plan.get('reps', 2)) + 1):
            todo += [(n, s, r) for n, s in evs.items() if want is None or n in want]
        for label, script, rep in todo:
            m = run_one(runner, plan, sk, label, script, rep, a.out)
            if m:
                print(json.dumps({'state': sk, 'event': label, 'rep': rep, 'secs': m['secs'],
                                  'timers': [m['timers'][k] for k in sorted(m['timers'])],
                                  'resident': m['resident'] == m['resident_end']}), flush=True)


# ------------------------------------------------------------------ analysis
def _diff(a, b, step=256):
    """Offsets where a and b differ (same length)."""
    if a == b:
        return []
    out = []
    for i in range(0, len(a), step):
        x, y = a[i:i + step], b[i:i + step]
        if x != y:
            out.extend(i + k for k in range(len(x)) if x[k] != y[k])
    return out


class Run:
    def __init__(self, d):
        with open(os.path.join(d, 'run.json'), encoding='utf-8') as f:
            self.meta = json.load(f)
        self.snaps = {}
        self.layout = None
        for off in self.meta['offsets']:
            p = os.path.join(d, 'f%03d.bin' % off)
            with open(p + '.json', encoding='utf-8') as f:
                idx = json.load(f)
            with open(p, 'rb') as f:
                blob = f.read()
            self.snaps[off] = {r['name']: blob[r['offset']:r['offset'] + r['size']] for r in idx['ranges']}
            if self.layout is None:
                self.layout = [(r['name'], int(r['addr'], 16), r['size']) for r in idx['ranges']]


def _load_runs(out):
    runs = defaultdict(lambda: defaultdict(dict))
    base = os.path.join(out, 'runs')
    for sk in sorted(os.listdir(base)) if os.path.isdir(base) else ():
        for d in sorted(os.listdir(os.path.join(base, sk))):
            p = os.path.join(base, sk, d)
            if os.path.exists(os.path.join(p, 'run.json')):
                label, rep = d.rsplit('_r', 1)
                runs[sk][label][int(rep)] = Run(p)
    return runs


def _where(tab, name, base, i):
    """(module, Sym or None, display name, symbol start) for byte i of range name."""
    a = base + i
    if name in tab.mods:
        s = tab.sym_at(name, a)
        if s is not None:
            return name, s, s.name, s.addr
        return name, None, '?%08x' % a, a
    w = i & ~3
    return name, None, '%s+0x%03x' % (name, w), base + w


def analyze_state(tab, sk, runs, events_order):
    idles = runs.get('idle', {})
    if not idles:
        raise RuntimeError('%s: no idle runs' % sk)
    ref = idles[min(idles)]
    offs = ref.meta['offsets']
    layout = ref.layout
    later = [o for o in offs if o != 0]
    res = {'state': sk, 'resident': ref.meta['resident'], 'ranges': [[n, '%08x' % b, s] for n, b, s in layout],
           'host_input': ref.meta.get('status'), 'align': ref.meta.get('align'), 'events': {}}
    # alignment: every run's frame 0 should equal idle r1's frame 0. Bytes that
    # differ there anyway are load residue (what the run before left in RAM the
    # savestate does not cover, e.g. below a thread's stack pointer): reported, and
    # treated as noisy, so they never count as an event's change.
    al, residue = {}, {nm: set() for nm, _, _ in layout}
    for label, reps in runs.items():
        for r, run in reps.items():
            if run.layout and [x[0] for x in run.layout] != [x[0] for x in layout]:
                raise RuntimeError('%s %s_r%d: range layout differs from idle r1: %s'
                                   % (sk, label, r, run.layout))
            n = 0
            for nm, _, _ in layout:
                d = _diff(ref.snaps[0][nm], run.snaps[0][nm])
                residue[nm].update(d)
                n += len(d)
            if n:
                al['%s_r%d' % (label, r)] = n
    res['align_mismatch'] = al
    res['n_runs'] = sum(len(v) for v in runs.values())
    res['load_residue'] = sorted({_where(tab, nm, b, i)[2] for nm, b, _ in layout for i in residue[nm]})
    # idle classes
    noisy, changing = {}, {}
    for nm, _, _ in layout:
        ns = set(residue[nm])
        for r, run in idles.items():
            for o in offs:
                ns.update(_diff(ref.snaps[o][nm], run.snaps[o][nm]))
        ch = set()
        for o in later:
            ch.update(_diff(ref.snaps[0][nm], ref.snaps[o][nm]))
        noisy[nm], changing[nm] = ns, ch - ns
    res['idle'] = {nm: {'bytes': sz, 'noisy': len(noisy[nm]), 'changing': len(changing[nm]),
                        'constant': sz - len(noisy[nm]) - len(changing[nm])} for nm, _, sz in layout}
    res['idle_reps'] = sorted(idles)
    rows = []
    for label in events_order:
        reps = runs.get(label)
        if not reps:
            continue
        per = {}
        for r, run in sorted(reps.items()):
            for o in later:
                for nm, base, _ in layout:
                    for i in _diff(ref.snaps[o][nm], run.snaps[o][nm]):
                        if i in noisy[nm]:
                            continue
                        mod, s, disp, start = _where(tab, nm, base, i)
                        d = per.setdefault((nm, disp), {'mod': mod, 'sym': s, 'start': start, 'by_rep': {}})
                        d['by_rep'].setdefault(r, {}).setdefault(o, set()).add(i)
        # whole-run divergence between the reps
        rl = sorted(reps)
        div = None
        if len(rl) >= 2:
            div = {}
            for r in rl[1:]:
                n = 0
                first = None
                for o in offs:
                    k = sum(len(_diff(reps[rl[0]].snaps[o][nm], reps[r].snaps[o][nm])) for nm, _, _ in layout)
                    if k and first is None:
                        first = o
                    n += k
                div['r%d' % r] = {'bytes': n, 'first_offset': first}
        ev = {'reps': rl, 'rep_divergence': div, 'symbols': 0, 'unnamed': 0,
              'log': reps[rl[0]].meta.get('log'), 'resident_stable': all(
                  x.meta['resident'] == x.meta['resident_end'] for x in reps.values())}
        for (nm, disp), d in sorted(per.items(), key=lambda kv: kv[1]['start']):
            s = d['sym']
            r1 = min(d['by_rep'])
            f1 = min(d['by_rep'][r1])
            ch = sorted(d['by_rep'][r1][f1])
            base = [b for n2, b, _ in layout if n2 == nm][0]
            if s is not None:
                lo = max(s.addr, base + ch[0] - ((base + ch[0] - s.addr) % 4))
                hi = min(s.addr + s.size, lo + 16)
            else:
                lo, hi = d['start'], d['start'] + 4
            i0, i1 = lo - base, hi - base
            agree = None
            if len(rl) >= 2:
                agree = True
                for o in later:
                    sets = [d['by_rep'].get(r, {}).get(o, set()) for r in rl]
                    if any(x != sets[0] for x in sets):
                        agree = False
                        break
                    if any(len({reps[r].snaps[o][nm][i] for r in rl}) > 1 for i in sets[0]):
                        agree = False
                        break
            allb = set()
            for x in d['by_rep'].values():
                for y in x.values():
                    allb |= y
            idle_ch = bool(allb & changing[nm])
            rows.append({'state': sk, 'event': label, 'module': d['mod'], 'addr': '%08x' % d['start'],
                         'symbol': disp, 'named': symtab.named(disp) and not disp.startswith('?') and
                         d['mod'] in tab.mods, 'kind': s.kind if s else '', 'size': s.size if s else 4,
                         'changed_bytes': len(allb), 'first_offset': f1,
                         'offsets': sorted({o for x in d['by_rep'].values() for o in x}),
                         'reps_changed': sorted(d['by_rep']), 'value_at': '%08x' % lo,
                         'idle_value': ref.snaps[f1][nm][i0:i1].hex(),
                         'event_value': reps[r1].snaps[f1][nm][i0:i1].hex(),
                         'repeats_agree': agree, 'idle_class': 'changes_in_idle' if idle_ch else 'constant_in_idle',
                         'sym': s})
            ev['symbols'] += 1
            ev['unnamed'] += 0 if rows[-1]['named'] else 1
        res['events'][label] = ev
    # how many events change each symbol (specificity)
    nev = defaultdict(set)
    for r in rows:
        nev[(r['module'], r['symbol'])].add(r['event'])
    for r in rows:
        r['n_events'] = len(nev[(r['module'], r['symbol'])])
        r['events_all'] = sorted(nev[(r['module'], r['symbol'])])
    return res, rows


def _xref(tab, r, cache):
    s = r['sym']
    if s is None:
        return '', '', ''
    key = (s.mod, s.addr)
    if key not in cache:
        R, W, A = [], [], []
        for fm, fn, acc in tab.xref(s.mod, s.addr, s.addr + s.size):
            tag = fn if fm == s.mod or fm == 'arm9' else '%s:%s' % (fm, fn)
            if 'R' in acc:
                R.append(tag)
            if 'W' in acc:
                W.append(tag)
            if 'A' in acc or acc == '?':
                A.append(tag)
        cache[key] = (','.join(R), ','.join(W), ','.join(A))
    return cache[key]


COLS = ['state', 'event', 'module', 'addr', 'symbol', 'named', 'kind', 'size', 'changed_bytes', 'first_offset',
        'offsets', 'value_at', 'idle_value', 'event_value', 'repeats_agree', 'idle_class', 'n_events',
        'events_all', 'readers', 'writers', 'passed_on']


def cmd_analyze(a):
    out = a.dir
    plan = sr.load_json(a.plan or os.path.join(out, 'plan.json'))
    tab = symtab.table()
    runs = _load_runs(out)
    states, allrows = [], []
    for sk in runs:
        order = [e for e in plan.get('events', {}) if e in runs[sk]]
        res, rows = analyze_state(tab, sk, runs[sk], order)
        states.append(res)
        allrows += rows
    cache = {}
    for r in allrows:
        r['readers'], r['writers'], r['passed_on'] = _xref(tab, r, cache)
    allrows.sort(key=lambda r: (r['state'], r['named'], r['n_events'], r['event'], r['module'], r['addr']))
    with open(os.path.join(out, 'candidates.tsv'), 'w', encoding='utf-8', newline='') as f:
        f.write('\t'.join(COLS) + '\n')
        for r in allrows:
            f.write('\t'.join(_cell(r[c]) for c in COLS) + '\n')
    checks = []
    for e in plan.get('expect', []):
        a0 = es.parse_int(e['addr'])
        hit = [r for r in allrows if r['state'] == e['state'] and r['event'] == e['event'] and r['sym'] is not None
               and r['sym'].addr <= a0 < r['sym'].addr + r['sym'].size]
        checks.append(dict(e, found=bool(hit), rows=[{k: h[k] for k in ('symbol', 'idle_value', 'event_value',
                                                                          'repeats_agree', 'first_offset')}
                                                     for h in hit]))
    top = [r for r in allrows if not r['named'] and r['repeats_agree'] is not False
           and r['idle_class'] == 'constant_in_idle' and r['module'] in tab.mods]
    top.sort(key=lambda r: (r['n_events'], r['state'], r['addr']))
    seen, tops = set(), []
    for r in top:
        k = (r['state'], r['module'], r['symbol'])
        if k in seen:
            continue
        seen.add(k)
        tops.append({c: r[c] for c in ('state', 'module', 'symbol', 'events_all', 'idle_value', 'event_value',
                                        'readers', 'writers')})
    summ = {'states': states, 'rows': len(allrows), 'checks': checks, 'top_unnamed': tops[:60]}
    with open(os.path.join(out, 'summary.json'), 'w', encoding='utf-8') as f:
        json.dump(summ, f, indent=1, default=str)
    with open(os.path.join(out, 'summary.md'), 'w', encoding='utf-8') as f:
        f.write(_markdown(summ))
    print(_markdown(summ))


def _cell(v):
    if isinstance(v, bool):
        return 'yes' if v else 'no'
    if v is None:
        return ''
    if isinstance(v, (list, tuple)):
        return ','.join(str(x) for x in v)
    return str(v)


def _markdown(s):
    L = ['# RAM-diff summary', '']
    for st in s['states']:
        L.append('## %s (overlays %s)' % (st['state'], ' '.join('%03d' % i for i in st['resident'])))
        L.append('')
        L.append('- align: `%s`; runs whose frame 0 differs from idle r1: %d of %d (%d bytes in all, in %s; '
                 'treated as noisy)' % (st['align'], len(st['align_mismatch']), st['n_runs'],
                                        sum(st['align_mismatch'].values()), ', '.join(st['load_residue']) or '-'))
        L.append('- host input: %s' % (st['host_input'] or 'n/a'))
        idle = st['idle']
        L.append('- idle reps %s: noisy bytes %d, deterministic-changing %d, constant %d' % (
            st['idle_reps'], sum(v['noisy'] for v in idle.values()), sum(v['changing'] for v in idle.values()),
            sum(v['constant'] for v in idle.values())))
        L.append('')
        L.append('| event | reps | rep divergence (bytes, first offset) | symbols | unnamed |')
        L.append('|---|---|---|---|---|')
        for e, v in st['events'].items():
            dv = v['rep_divergence']
            dtxt = ', '.join('%s %d @%s' % (k, x['bytes'], x['first_offset']) for k, x in dv.items()) if dv else '-'
            L.append('| %s | %s | %s | %d | %d |' % (e, v['reps'], dtxt, v['symbols'], v['unnamed']))
        L.append('')
    if s['checks']:
        L.append('## Checks')
        L.append('')
        for c in s['checks']:
            L.append('- %s %s %s: %s %s' % (c['state'], c['event'], c['addr'], 'FOUND' if c['found'] else 'MISSING',
                                           c['rows'][:2]))
        L.append('')
    L.append('## Unnamed candidates (constant in idle, repeats agree; fewest events first)')
    L.append('')
    for t in s['top_unnamed'][:40]:
        L.append('- %s %s %s: %s -> %s on %s; R %s; W %s' % (
            t['state'], t['module'], t['symbol'], t['idle_value'], t['event_value'], ','.join(t['events_all']),
            t['readers'][:120] or '-', t['writers'][:120] or '-'))
    L.append('')
    return '\n'.join(L)


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = p.add_subparsers(dest='cmd', required=True)
    r = sub.add_parser('run', help='run the plan against the daemon')
    r.add_argument('plan')
    r.add_argument('--out', required=True)
    r.add_argument('--only', help='comma-separated state keys')
    r.add_argument('--events', help='comma-separated event names (idle always runs)')
    r.add_argument('--rpc', type=int, default=es.DEFAULT_RPC_PORT)
    r.set_defaults(fn=cmd_run)
    z = sub.add_parser('analyze', help='diff the runs in DIR')
    z.add_argument('dir')
    z.add_argument('--plan', help='default DIR/plan.json')
    z.set_defaults(fn=cmd_analyze)
    a = p.parse_args(argv)
    a.fn(a)


if __name__ == '__main__':
    main()
