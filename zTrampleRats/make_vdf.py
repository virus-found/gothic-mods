import struct
import sys

def pack_vdf(dll_path, vdf_output_path):
    with open(dll_path, "rb") as f:
        dll_data = f.read()

    comment = b"zTrampleRats Gothic II Union Plugin\x1a".ljust(256, b"\x1a")
    sig = b"PSVDSC_V2.00\r\n\r\n"

    dos_date = ((2026 - 1980) << 9) | (7 << 5) | 28
    dos_time = (23 << 11) | (6 << 5) | 0
    dos_stamp = (dos_date << 16) | dos_time

    num_files = 1
    num_dirs = 2

    data_size = len(dll_data)
    root_offset = 296
    version = 0x50

    header = comment + sig + struct.pack("<IIIIII", num_files, num_dirs, dos_stamp, data_size, root_offset, version)

    data_offset = 296 + (num_files + num_dirs) * 80

    def pad_name(name):
        return name.encode("ascii").ljust(64, b" ")

    entry_system = pad_name("SYSTEM") + struct.pack("<IIII", 1, 1, 0x80000000, 0)
    entry_autorun = pad_name("AUTORUN") + struct.pack("<IIII", 2, 1, 0x80000000, 0)
    entry_file = pad_name("ZTRAMPLERATS.DLL") + struct.pack("<IIII", data_offset, len(dll_data), 0x40000000, 0x20)

    vdf_data = header + entry_system + entry_autorun + entry_file + dll_data

    with open(vdf_output_path, "wb") as f:
        f.write(vdf_data)

    print(f"Successfully packed {vdf_output_path} ({len(vdf_data)} bytes)")

if __name__ == "__main__":
    dll = "/home/gargantua/docs/games/gothicii/zTrampleRats/out/build/G2A-MT-Release-msvc-wine/bin/zTrampleRats.dll"
    vdf = "/home/gargantua/games/steam/steamapps/common/TheChroniclesOfMyrtana/Data/Plugins/zTrampleRats.vdf"
    pack_vdf(dll, vdf)
