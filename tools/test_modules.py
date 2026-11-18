"""An overlay's image base is its lowest delinks start, not its lowest symbol.

A data-only overlay carries an empty `.ctor` section ahead of its first
symbol. Measuring from the symbol shifts every ROM read by up to 0x2c bytes;
the overlay's own relocations are the proof. `data_ov054_021111c0` holds the
word 2 at file offset 0, and the `from:0x021111c4 kind:load to:0x02111258`
row only matches when the base is 0x021111a0, not the lowest symbol
0x021111c0.
"""
import pathlib
import re
import tempfile
import unittest

import modules as MOD


class OverlayImageBase(unittest.TestCase):
    def test_lowest_delinks_start_wins(self):
        with tempfile.TemporaryDirectory() as tmp:
            overlay = pathlib.Path(tmp)
            (overlay / "delinks.txt").write_text(
                "    .ctor start:0x021111a0 end:0x021111a0 kind:rodata align:4\n"
                "    .data start:0x021111c0 end:0x02111d60 kind:data align:32\n",
                encoding="utf-8")
            self.assertEqual(MOD._overlay_image_base(overlay), 0x021111A0)

    def test_no_delinks_falls_back_to_the_lowest_symbol(self):
        with tempfile.TemporaryDirectory() as tmp:
            overlay = pathlib.Path(tmp)
            symbols = overlay / "symbols.txt"
            symbols.write_text(
                "data_a kind:data(any) addr:0x021111c0\n"
                "data_b kind:data(any) addr:0x02111200\n",
                encoding="utf-8")
            self.assertEqual(MOD._overlay_base(symbols), 0x021111C0)

    def test_every_configured_overlay_starts_at_or_below_its_symbols(self):
        seen = 0
        for module in MOD.modules():
            if not module["name"].startswith("ov"):
                continue
            base = module["base"]
            symbols = [int(m.group(1), 16)
                       for line in module["syms"].read_text(errors="ignore").splitlines()
                       if (m := re.search(r"addr:0x([0-9a-fA-F]+)", line))]
            if not symbols:
                continue
            seen += 1
            self.assertLessEqual(base, min(symbols), module["name"])
        if not seen:
            self.skipTest("no overlay modules in this checkout")


if __name__ == "__main__":
    unittest.main()
