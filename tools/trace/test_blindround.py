"""Offline tests for blindround: spec scrambling, the registration gate, jitter,
PNG stripping, selfcheck, dryrun flags and score's refusal. A fake client stands
in for the emulator.

  python -m unittest tools/trace/test_blindround.py
"""
import argparse
import contextlib
import hashlib
import io
import json
import os
import shutil
import struct
import sys
import tempfile
import unittest
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import blindround as br  # noqa: E402


def cond(label, real, expect, value, count=2):
    return {'label': label, 'count': count, 'real': real, 'expect': expect,
            'steps': [{'op': 'step', 'n': 2}, {'op': 'shot', 'as': 'A'},
                      {'op': 'set', 'n': 3, 'pokes': [{'expr': '0x2000000', 'op': 'write', 'value': value}]},
                      {'op': 'shot', 'as': 'B'}]}


def design(choices=('0', '1', '2')):
    return {'round': 't', 'seed': 't1', 'specs': [{
        'name': 'sa', 'state': 'state.ml', 'hook': {'addr': '0x02019390', 'module': 'arm9'}, 'settle': 4,
        'screen': 'top', 'guards': [{'name': 'g', 'expr': '0x2000004', 'size': 1, 'want': 7}],
        'question': {'text': 'How many icons are new in B?', 'choices': list(choices)},
        'how': 'Each Qxx has one image: the left half is A, the right half is B.',
        'conditions': [cond('value one', True, '1', 1), cond('value two', True, '2', 2),
                       cond('zero', False, '0', 0, count=1)],
        'prediction': {'hypothesis': 'h', 'expected': 'e', 'risks': []}}]}


class FakeClient:
    """Memory is a dict; every shot is a unique 2x1 PNG (frame count in its pixels)."""

    def __init__(self):
        self.mem = {}
        self.frame = 0

    def clear_bps(self):
        pass

    def load_state_file(self, path):
        self.mem = {0x2000004: 7}
        self.frame = 0
        return {'verdict': 'loaded'}

    def set_frame(self, addr, module, name):
        pass

    def step(self, n):
        self.frame += n
        return {}

    def locate(self, locate):
        return None

    def snapshot(self, fields):
        return {f['name']: self.mem.get(int(f['expr'], 16), 0) for f in fields}

    def poke(self, expr, op, mask, value, size):
        a = int(expr, 16)
        old = self.mem.get(a, 0)
        new = {'write': (old & ~mask) | (value & mask), 'set': old | mask, 'clear': old & ~mask,
               'flip': old ^ mask, 'same': old}[op]
        self.mem[a] = new
        return {'addr': '%08x' % a, 'old': old, 'new': new, 'size': size}

    def shot(self, path, which, settle_ms):
        v = self.mem.get(0x2000000, 0)
        br.write_png(path, 2, 1, bytes([self.frame & 0xff, v, 0, self.frame >> 8, 0, 0]))
        return {}


def fake_compose(a, b, out):
    # a 2x2 PNG whose pixels hash both inputs, plus a tEXt chunk for strip_png to drop
    with open(a, 'rb') as fa, open(b, 'rb') as fb:
        px = hashlib.sha256(fa.read() + fb.read()).digest()[:12]
    br.write_png(out, 2, 2, px)
    with open(out, 'rb') as f:
        data = f.read()
    t = b'tEXt'
    d = b'Comment\x00made by a test'
    ch = struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    with open(out, 'wb') as f:
        f.write(data[:33] + ch + data[33:])


class RoundTest(unittest.TestCase):
    def setUp(self):
        self.rd = tempfile.mkdtemp()
        with open(os.path.join(self.rd, 'design.json'), 'w') as f:
            json.dump(design(), f)
        self._compose = br.compose_pair
        br.compose_pair = fake_compose
        self._quiet = contextlib.redirect_stdout(io.StringIO())
        self._quiet.__enter__()

    def tearDown(self):
        self._quiet.__exit__(None, None, None)
        br.compose_pair = self._compose
        shutil.rmtree(self.rd)

    def ns(self, **kw):
        kw.setdefault('round', self.rd)
        return argparse.Namespace(**kw)

    def spec(self, discard=False):
        br.cmd_spec(self.ns(discard=discard))

    def test_spec_hides_labels(self):
        self.spec()
        s = json.load(open(os.path.join(self.rd, 'specs', 'sa.json')))
        self.assertEqual(sorted(c['name'] for c in s['conditions']), ['c1', 'c2', 'c3'])
        self.assertNotIn('value one', json.dumps(s))
        labels = json.load(open(os.path.join(self.rd, 'labels.json')))['sa']
        self.assertEqual(sorted(labels.values()), ['value one', 'value two', 'zero'])
        # set pokes default to every frame
        p = s['conditions'][0]['steps'][2]['pokes'][0]
        self.assertEqual((p['every'], p['mask'], p['size']), ('frame', 0xff, 1))

    def test_offsets_unique(self):
        self.spec()
        s = json.load(open(os.path.join(self.rd, 'specs', 'sa.json')))
        n = len(br.plan_for(s))
        offs = br.offsets_for(s, n)
        self.assertEqual(len(set(offs)), n)
        self.assertTrue(all(0 <= o < 2 * n for o in offs))

    def test_run_refuses_unregistered(self):
        self.spec()
        with self.assertRaises(br.Refused):
            br.cmd_run(self.ns(spec=None), client=FakeClient())

    def test_respec_invalidates_registration(self):
        self.spec()
        br.cmd_register(self.ns())
        self.assertTrue(os.path.exists(os.path.join(self.rd, 'REGISTERED.sha256')))
        self.spec()
        self.assertFalse(os.path.exists(os.path.join(self.rd, 'REGISTERED.sha256')))
        with self.assertRaises(br.Refused):
            br.cmd_run(self.ns(spec=None), client=FakeClient())

    def test_edit_after_register_refused(self):
        self.spec()
        br.cmd_register(self.ns())
        with open(os.path.join(self.rd, 'predictions', 'sa.md'), 'a') as f:
            f.write('late edit\n')
        with self.assertRaises(br.Refused) as e:
            br.cmd_run(self.ns(spec=None), client=FakeClient())
        self.assertIn('predictions/sa.md changed', str(e.exception))

    def test_unregistered_spec_refused(self):
        self.spec()
        br.cmd_register(self.ns())
        shutil.copy(os.path.join(self.rd, 'specs', 'sa.json'), os.path.join(self.rd, 'specs', 'sb.json'))
        with self.assertRaises(br.Refused) as e:
            br.cmd_run(self.ns(spec=None), client=FakeClient())
        self.assertIn('specs/sb.json not registered', str(e.exception))

    def test_full_round(self):
        self.spec()
        br.cmd_pilot(self.ns(spec=None), client=FakeClient())
        pilot = json.load(open(os.path.join(self.rd, 'PILOT.json')))
        self.assertTrue(pilot['all_pass'])
        self.assertEqual(set(pilot['pilot']['sa/c1']), {'guards', 'hook', 'pokes_landed', 'pass'})
        br.cmd_register(self.ns())
        br.cmd_run(self.ns(spec=None), client=FakeClient())
        jd = os.path.join(self.rd, 'judge')
        self.assertEqual(sorted(os.listdir(jd)), ['JUDGE.md', 'sa'])
        self.assertEqual(sorted(os.listdir(os.path.join(jd, 'sa'))), ['answers.json', 'pairs'])
        for f in os.listdir(os.path.join(jd, 'sa', 'pairs')):
            with open(os.path.join(jd, 'sa', 'pairs', f), 'rb') as fh:
                self.assertEqual(set(br.png_chunks(fh.read())), {'IHDR', 'IDAT', 'IEND'})
        self.assertTrue(br.selfcheck(self.rd, quiet=True))
        # a second run of the same registration is refused
        with self.assertRaises(br.Refused):
            br.cmd_run(self.ns(spec=None), client=FakeClient())
        # score refuses until the judgments exist
        with self.assertRaises(br.Refused):
            br.cmd_score(self.ns())
        ans = json.load(open(os.path.join(jd, 'sa', 'answers.json')))
        key = json.load(open(os.path.join(self.rd, 'key', 'sa', 'key.json')))
        for q in ans['answers']:
            ans['answers'][q] = {'answer': key['events'][q]['expected'], 'confidence': 'high'}
        with open(os.path.join(jd, 'sa', 'my_judgments.json'), 'w') as f:
            json.dump(ans, f)
        self.assertTrue(br.selfcheck(self.rd, quiet=True))
        br.cmd_score(self.ns())
        sc = json.load(open(os.path.join(self.rd, 'scores', 'sa.json')))
        self.assertEqual((sc['agree'], sc['of']), (5, 5))
        self.assertTrue(os.path.exists(os.path.join(self.rd, 'scores', 'SUMMARY.md')))
        # a respec after the run must discard it explicitly
        with self.assertRaises(br.Refused):
            self.spec()
        self.spec(discard=True)
        self.assertTrue(os.path.isdir(os.path.join(self.rd, 'discarded_1', 'key')))

    def test_selfcheck_catches_leaks(self):
        self.spec()
        br.cmd_register(self.ns())
        br.cmd_run(self.ns(spec=None), client=FakeClient())
        jd = os.path.join(self.rd, 'judge', 'sa')
        a = json.load(open(os.path.join(jd, 'answers.json')))
        a['how'] += ' value two at 0x0209f310, see C:/tmp/x and c2'
        with open(os.path.join(jd, 'answers.json'), 'w') as f:
            json.dump(a, f)
        pairs = sorted(os.listdir(os.path.join(jd, 'pairs')))
        shutil.copy(os.path.join(jd, 'pairs', pairs[0]), os.path.join(jd, 'pairs', pairs[1]))
        open(os.path.join(self.rd, 'judge', 'notes.txt'), 'w').close()
        buf = io.StringIO()
        with contextlib.redirect_stdout(buf):
            self.assertFalse(br.selfcheck(self.rd))
        text = buf.getvalue()
        for want in ("label 'value two'", 'address', 'path', 'c-name', 'identical judge images', 'stray judge/notes.txt'):
            self.assertIn(want, text)


class DryrunTest(unittest.TestCase):
    def spec(self, choices, conds):
        return {'name': 'sx', 'seed': 'x', 'question': {'text': 'q', 'choices': choices},
                'conditions': [{'name': 'c%d' % (i + 1), 'count': 2, 'real': r, 'expect': e, 'steps': []}
                               for i, (r, e) in enumerate(conds)]}

    def flags(self, choices, conds):
        return [f['flag'] for f in br.dryrun_spec(self.spec(choices, conds))[1]]

    def test_real_same_as_null(self):
        fl = self.flags(['moved', 'no change', 'other'], [(True, 'moved'), (True, 'no change'), (False, 'no change')])
        self.assertTrue(any('cannot tell the effect from no effect' in f for f in fl), fl)

    def test_view_changed_vs_frozen(self):
        # the s14d case: a frozen view also reads as "the view changed" against the expected motion
        fl = self.flags(['the view changed', 'the view froze', 'no change'],
                        [(True, 'the view froze'), (True, 'the view changed'), (False, 'no change')])
        self.assertTrue(any("'the view froze' and 'the view changed'" in f for f in fl), fl)
        self.assertTrue(any("'the view froze' and 'no change'" in f for f in fl), fl)

    def test_clean_design_has_no_choice_flags(self):
        fl = self.flags(['the arrow moved left', 'the arrow moved up', 'something else changed', 'no change'],
                        [(True, 'the arrow moved left'), (True, 'the arrow moved up'), (False, 'no change')])
        self.assertFalse(any('could both fit' in f for f in fl), fl)

    def test_expect_not_a_choice(self):
        out = []
        s = self.spec(['a', 'b'], [(True, 'c'), (False, 'a')])
        for c in s['conditions']:
            c['steps'] = [{'op': 'shot', 'as': 'A'}, {'op': 'shot', 'as': 'B'}]
        br.lint_spec(s, out)
        self.assertTrue(any(f['level'] == 'error' and 'not one of the choices' in f['flag'] for f in out))

    def test_once_poke_flagged(self):
        out = []
        s = self.spec(['a', 'b'], [(True, 'b'), (False, 'a')])
        for c in s['conditions']:
            c['steps'] = [{'op': 'shot', 'as': 'A'},
                          {'op': 'set', 'n': 4, 'pokes': [{'expr': '0x1', 'op': 'set', 'mask': 1, 'every': 'once'}]},
                          {'op': 'shot', 'as': 'B'}]
        br.lint_spec(s, out)
        self.assertTrue(any('poked once' in f['flag'] for f in out), out)


class PngTest(unittest.TestCase):
    def test_strip(self):
        d = tempfile.mkdtemp()
        try:
            a, b, o = (os.path.join(d, x) for x in ('a.png', 'b.png', 'o.png'))
            br.write_png(a, 2, 1, bytes(6))
            br.write_png(b, 2, 1, bytes(range(6)))
            fake_compose(a, b, o)
            self.assertIn('tEXt', br.png_chunks(open(o, 'rb').read()))
            self.assertEqual(br.strip_png(o), ['tEXt'])
            self.assertEqual(br.png_chunks(open(o, 'rb').read()), ['IHDR', 'IDAT', 'IEND'])
        finally:
            shutil.rmtree(d)


if __name__ == '__main__':
    unittest.main()
