"""Offline tests for thissweep: the method category, register value kinds and the
generated summary (from sweep.json and sweep.tsv only).

  python -m unittest tools/trace/test_thissweep.py
"""
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import thissweep as ts  # noqa: E402


def row(name, verdict, category, calls, n_samples='4', logged='4', **kw):
    r = {c: '' for c in ts.COLS}
    r.update(module='arm9', addr=name[5:], name=name, thumb='no', calls=str(calls), verdict=verdict,
             category=category, n_keys='1', n_objs='1', n_samples=n_samples, logged=logged,
             r0_used='yes', r0_first_use='base@+0x0 ldr r1, [r0, #8]', evaluations='10')
    r.update(kw)
    return r


class CategoryTest(unittest.TestCase):
    def test_method_needs_r0_read(self):
        self.assertEqual(ts.category('one_class', True), 'method')
        self.assertEqual(ts.category('one_class', False), 'one_class:r0-not-read')
        self.assertEqual(ts.category('one_class', None), 'one_class:r0-unchecked')
        self.assertEqual(ts.category('several_classes', True), 'base-method')
        self.assertEqual(ts.category('several_classes', False), 'several:r0-not-read')
        self.assertEqual(ts.category('not_object', True), 'not_object')


class ValueKindTest(unittest.TestCase):
    def test_kinds(self):
        sp = 0x027e3f00
        self.assertEqual(ts.value_kind(0, sp), 'zero')
        self.assertEqual(ts.value_kind(5, sp), 'int')
        self.assertEqual(ts.value_kind(0xffffffff, sp), 'negint')
        self.assertEqual(ts.value_kind(0x0210_0000, 0x02100010), 'stack')
        self.assertEqual(ts.value_kind(0x0210_0000, sp), 'ram')
        self.assertEqual(ts.value_kind(0x0210_0000, sp, 'daPly_c'), 'obj:daPly_c')
        self.assertEqual(ts.value_kind(0x04000130, sp), 'io')
        self.assertEqual(ts.value_kind(0x06800000, sp), 'vram')


class SummaryTest(unittest.TestCase):
    def test_summary_from_files_only(self):
        res = {'state_key': 'B1', 'state': 'x/B1.ml', 'scenes': ['B1_walk'], 'frames': 300, 'sample': 'reservoir',
               'log_limit': 64, 'arg_bytes': 16, 'resident': [1, 2], 'resident_end': [1, 2], 'secs': 9.5,
               'n_requested': 3,
               'candidates': [{'module': 'arm9', 'addr': a, 'name': 'func_' + a, 'calls': 10}
                              for a in ('02000010', '02000020', '02000030', '02000040')]}
        rows = [row('func_02000010', 'one_class', 'method', 100, **{'class': 'daPly_c', 'n_objs': '1'}),
                row('func_02000020', 'one_class', 'one_class:r0-not-read', 50,
                    r0_used='no', r0_first_use='overwritten@+0x8 add r0, sp, #8', **{'class': 'daBmb_c'}),
                row('func_02000030', 'not_object', 'not_object', 5, n_samples='0', logged='0')]
        with tempfile.TemporaryDirectory() as d:
            ts.write_tsv(os.path.join(d, 'sweep.tsv'), rows)
            back = ts.read_tsv(os.path.join(d, 'sweep.tsv'))
        self.assertEqual([r['name'] for r in back], [r['name'] for r in rows])
        md = ts.summary_md(res, back)
        self.assertIn('| swept | 3 |', md)
        self.assertIn('| swept and sampled | 2 |', md)
        self.assertIn('## method (1)', md)
        self.assertIn('## Flagged: object in r0 but r0 not read (1)', md)
        self.assertIn('overwritten@+0x8', md)
        self.assertIn('candidate func_ never swept (not picked; --n 3 of 4): 1', md)
        self.assertIn('func_02000040(10)', md)
        self.assertIn('Swept, never sampled: `func_02000030`', md)


if __name__ == '__main__':
    unittest.main()
