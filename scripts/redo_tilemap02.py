import json

f = json.load(open("../Assets/variables/tiles_04.json"))
f2 = open("../Assets/variables/tiles_05.json", "w")
for idx, (key, val) in enumerate(f.items()):
    print(idx, key, val)
    f[key]["tileIdx"] = idx


json.dump(f, f2, indent=2)
