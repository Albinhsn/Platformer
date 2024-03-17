f = [
    i.split()[0]
    for i in open("../Assets/variables/tiles_map.txt").read().strip().split("\n")
]
f2 = open("../Assets/variables/tiles_map2.txt", "w")
for idx, ch in enumerate(f):
    f2.write(f"{ch} {idx}\n")
