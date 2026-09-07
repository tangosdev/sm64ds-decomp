"""A TU's .text is contiguous, so absorb_unlabelled must not reach across a gap.

The call graph alone cannot tell a file-local helper from an ordinary external
function that happens to have one caller -- both are "called from exactly one
cluster". Absorbing on that evidence placed 128 functions into units they cannot
be in (37 of 547 at the default settings), the worst of them a third of a
megabyte outside the span it was filed under.

Adjacency is the missing half of the argument, and it is what these pin: the
linker lays a unit down in one contiguous run, so a stray joins a cluster only
when it touches that cluster -- and, having joined, extends it, which is how a
contiguous run of strays is taken in.

These are pure unit tests over synthetic clusters: no ROM, no compiler, no config.
"""
import pathlib
import sys
import unittest

TOOLS = pathlib.Path(__file__).resolve().parent
sys.path.insert(0, str(TOOLS))

import tu_map as TM      # noqa: E402


def cluster(start, end, classes=("C",)):
    return {"classes": list(classes), "start": start, "end": end, "funcs": []}


def unit_for(tus, addr):
    """The unit holding `addr`, or None."""
    for t in tus:
        if any(a == addr for (a, _n, _s) in t["funcs"]):
            return t
    return None


class AdjacencyTests(unittest.TestCase):
    def test_a_stray_abutting_the_end_is_absorbed_and_extends_the_span(self):
        c = cluster(0x1000, 0x1100)
        tus = TM.absorb_unlabelled([c], [(0x1100, "helper", 0x40)],
                                   {0x1100: {0x1020}})
        self.assertEqual(len(tus), 1)
        self.assertEqual(tus[0]["end"], 0x1140)
        self.assertIn((0x1100, "helper", 0x40), tus[0]["funcs"])

    def test_a_stray_abutting_the_start_is_absorbed(self):
        c = cluster(0x1000, 0x1100)
        tus = TM.absorb_unlabelled([c], [(0x0fc0, "helper", 0x40)],
                                   {0x0fc0: {0x1020}})
        self.assertEqual(tus[0]["start"], 0x0fc0)

    def test_a_far_stray_is_not_absorbed_even_with_a_single_caller_cluster(self):
        """The defect this file exists for: one caller cluster is not enough."""
        c = cluster(0x21207dc, 0x212231c)
        tus = TM.absorb_unlabelled([c], [(0x20ccd04, "far", 0x60)],
                                   {0x20ccd04: {0x2120800}})
        self.assertEqual(c["end"], 0x212231c, "span must not move")
        self.assertNotIn((0x20ccd04, "far", 0x60), c["funcs"])
        own = unit_for(tus, 0x20ccd04)
        self.assertIsNotNone(own)
        self.assertEqual(own["classes"], [], "a stray becomes its own unlabelled TU")

    def test_a_contiguous_run_is_absorbed_whole(self):
        """Each pass extends the span, so the next function in the run abuts it."""
        c = cluster(0x1000, 0x1100)
        strays = [(0x1100, "a", 0x40), (0x1140, "b", 0x40), (0x1180, "c", 0x40)]
        calls = {0x1100: {0x1020}, 0x1140: {0x1020}, 0x1180: {0x1020}}
        tus = TM.absorb_unlabelled([c], strays, calls)
        self.assertEqual(len(tus), 1)
        self.assertEqual(tus[0]["end"], 0x11c0)
        self.assertEqual(len(tus[0]["funcs"]), 3)

    def test_a_run_stops_at_the_gap(self):
        """Contiguity is the boundary: what is past the hole stays out."""
        c = cluster(0x1000, 0x1100)
        strays = [(0x1100, "a", 0x40), (0x1200, "past_the_gap", 0x40)]
        calls = {0x1100: {0x1020}, 0x1200: {0x1020}}
        tus = TM.absorb_unlabelled([c], strays, calls)
        self.assertEqual(tus[0]["end"], 0x1140)
        self.assertIsNotNone(unit_for(tus, 0x1200))
        self.assertNotEqual(unit_for(tus, 0x1200), tus[0])


class CallerTests(unittest.TestCase):
    def test_an_adjacent_stray_with_two_caller_clusters_is_not_absorbed(self):
        """Adjacency is necessary, not sufficient -- the call-graph rule still runs."""
        a, b = cluster(0x1000, 0x1100, ("A",)), cluster(0x1140, 0x1200, ("B",))
        tus = TM.absorb_unlabelled([a, b], [(0x1100, "shared", 0x40)],
                                   {0x1100: {0x1020, 0x1150}})
        self.assertEqual(a["end"], 0x1100)
        self.assertEqual(b["start"], 0x1140)
        self.assertEqual(unit_for(tus, 0x1100)["classes"], [])

    def test_a_caller_outside_every_cluster_blocks_absorption(self):
        c = cluster(0x1000, 0x1100)
        tus = TM.absorb_unlabelled([c], [(0x1100, "external", 0x40)],
                                   {0x1100: {0x1020, 0x9000}})
        self.assertEqual(c["end"], 0x1100)
        self.assertEqual(unit_for(tus, 0x1100)["classes"], [])

    def test_an_uncalled_stray_is_not_absorbed(self):
        """No callers means no evidence, not evidence of belonging."""
        c = cluster(0x1000, 0x1100)
        tus = TM.absorb_unlabelled([c], [(0x1100, "orphan", 0x40)], {})
        self.assertEqual(c["end"], 0x1100)
        self.assertEqual(unit_for(tus, 0x1100)["classes"], [])


class InvariantTests(unittest.TestCase):
    def test_every_placed_function_lies_inside_its_own_units_span(self):
        """The property the whole guard exists to hold, over a mixed input."""
        a, b = cluster(0x1000, 0x1100, ("A",)), cluster(0x5000, 0x5100, ("B",))
        strays = [(0x1100, "adj_a", 0x40), (0x0800, "far", 0x40),
                  (0x5100, "adj_b", 0x40), (0x9000, "lonely", 0x40)]
        calls = {0x1100: {0x1020}, 0x0800: {0x1020},
                 0x5100: {0x5020}, 0x9000: {0x5020}}
        for t in TM.absorb_unlabelled([a, b], strays, calls):
            for (addr, name, _size) in t["funcs"]:
                self.assertTrue(t["start"] <= addr < t["end"],
                                "%s at %#x escapes %#x..%#x"
                                % (name, addr, t["start"], t["end"]))

    def test_no_function_is_lost_or_duplicated(self):
        a = cluster(0x1000, 0x1100, ("A",))
        a["funcs"].append((0x1000, "member", 0x100))
        strays = [(0x1100, "adj", 0x40), (0x0800, "far", 0x40)]
        tus = TM.absorb_unlabelled([a], strays, {0x1100: {0x1020}, 0x0800: {0x1020}})
        placed = sorted(n for t in tus for (_a, n, _s) in t["funcs"])
        self.assertEqual(placed, ["adj", "far", "member"])


if __name__ == "__main__":
    unittest.main()
