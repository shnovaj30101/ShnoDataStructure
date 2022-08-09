
import random, string
import json
import sys

if len(sys.argv) >= 1:
    data_num = int(sys.argv[1])
else:
    data_num = 10

t = {
    "example_id": 0,
    "example_str": "",
    "example_str2": "",
}
letters = string.ascii_lowercase

for i in range(data_num):
    t["example_id"] = random.randint(0,100)
    t["example_str"] = ''.join(random.choice(letters) for i in range(8))
    t["example_str2"] = ''.join(random.choice(letters) for i in range(12))
    print(json.dumps(t))
