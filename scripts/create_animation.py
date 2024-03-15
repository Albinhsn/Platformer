import json

animations = {
    "woodCoin001": {"textures": ["woodCoin_001", "woodCoin_002"], "timer": 500},
    "waterfallTop001": {"textures": ["water_002", "water_003"], "timer": 500},
    "waterfallMid001": {"textures": ["water_004", "water_005"], "timer": 500},
    "waterfallBot001": {"textures": ["water_008", "water_009"], "timer": 500},
    "lever001": {"textures": ["lever_001", "lever_002", "lever_003"], "timer": 500},
    "flagTop001": {"textures": ["flag_001", "flag_002"], "timer": 500},
    "button001": {"textures": ["button_001", "button_002"], "timer": 500},
    "coin001": {"textures": ["coin_001", "coin_002"], "timer": 500},
}

f = open("../Assets/Animation/animation01.json", "w")
f.write(json.dumps(animations))
