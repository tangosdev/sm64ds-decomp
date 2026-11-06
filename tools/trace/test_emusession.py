"""Offline tests for emusession.Images: module lookup for arm9, itcm and overlays.

  python -m unittest tools/trace/test_emusession.py

Builds a fake extracted/dsd tree in a temp directory; no ROM needed.
"""
import os
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import emusession as es  # noqa: E402


def _tree(root, itcm=True):
    os.makedirs(os.path.join(root, 'arm9'))
    os.makedirs(os.path.join(root, 'arm9_overlays'))
    with open(os.path.join(root, 'arm9', 'arm9.yaml'), 'w') as f:
        f.write('base_address: %d\n' % 0x02000000)
    with open(os.path.join(root, 'arm9', 'arm9.bin'), 'wb') as f:
        f.write(bytes(range(256)) * 4)
    if itcm:
        with open(os.path.join(root, 'arm9', 'itcm.yaml'), 'w') as f:
            f.write('base_address: %d\ncode_size: 64\nkind: Itcm\n' % 0x01ff8000)
        with open(os.path.join(root, 'arm9', 'itcm.bin'), 'wb') as f:
            f.write(bytes(range(0x40, 0x80)))
    with open(os.path.join(root, 'arm9_overlays', 'overlays.yaml'), 'w') as f:
        f.write('overlays:\n- id: 2\n  base_address: %d\n' % 0x020ad660)
    with open(os.path.join(root, 'arm9_overlays', 'ov002.bin'), 'wb') as f:
        f.write(b'\xaa' * 32)


class ImagesTest(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)

    def images(self, itcm=True):
        root = os.path.join(self.tmp.name, 'dsd')
        _tree(root, itcm)
        return es.Images(root)

    def test_itcm_module_and_bytes(self):
        im = self.images()
        self.assertEqual(im.mods['itcm'][0], 0x01ff8000)
        self.assertEqual(im.bytes_at('itcm', 0x01ff8004, 4), bytes([0x44, 0x45, 0x46, 0x47]))

    def test_default_module(self):
        im = self.images()
        self.assertEqual(im.default_module(0x01ff8010), 'itcm')
        self.assertEqual(im.default_module(0x02000010), 'arm9')
        self.assertIsNone(im.default_module(0x01ff8040))     # past the ITCM image
        self.assertIsNone(im.default_module(0x020ad660))     # overlays need an explicit ovNNN

    def test_overlay_bytes(self):
        im = self.images()
        self.assertEqual(im.bytes_at('ov002', 0x020ad660, 2), b'\xaa\xaa')

    def test_no_itcm_image(self):
        im = self.images(itcm=False)
        self.assertNotIn('itcm', im.mods)
        self.assertIsNone(im.default_module(0x01ff8010))
        with self.assertRaises(KeyError):
            im.bytes_at('itcm', 0x01ff8000, 4)


if __name__ == '__main__':
    unittest.main()
