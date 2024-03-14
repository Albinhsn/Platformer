import os
from collections import Counter


def get_number_str(i: int):
    if i // 100 > 0:
        return f"0{i}"
    if i // 10 > 0:
        return f"00{i}"
    return f"000{i}"


f = [
    i.split()[0]
    for i in open("../Assets/variables/tiles_map2.txt", "r").read().strip().split("\n")
]
assert len(set(f)) == len(f), f"How {len(set(f))} vs {len(f)}"


print(f)
filePtr = open("../Assets/variables/tiles_map.txt", "w")
for i, idx in enumerate(f):
    filePtr.write(f"{idx} {i}\n")
