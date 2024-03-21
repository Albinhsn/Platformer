import json

f = open("../Assets/variables/tiles_02.json", "r")
f2 = [
    i.split()
    for i in open("../Assets/variables/tiles_map2.txt", "r").read().strip().split("\n")
]

f2 = {i[0]: int(i[1]) for i in f2}


M = json.load(f)
for k, v in M.items():
    if "animation" in v:
        del v["textureIdx"]
    else:
        v["textureIdx"] = f2[k]


f.close()

f = open("../Assets/variables/tiles_03.json", "w")

f.write(json.dumps(M, indent=4))

# f = [
#     i.split()
#     for i in open("../Assets/variables/tiles_map2.txt", "r").read().strip().split("\n")
# ]

# f2 = json.load(open("../Assets/Animation/animation01.json"))
# m = {}
# for idx, i in enumerate(f):
#     m[i[0]] = {
#         "entityType": 0,
#         "tileType": 0,
#         "textureIdx": idx,
#         "boundsWidth": 0,
#         "boundsHeight": 0,
#     }
#     if i[0] in f2:
#         m[i[0]]["animation"] = f2[i[0]]

# __import__('pprint').pprint(m)
