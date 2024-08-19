import json

list = [('localhost', 8550), ('localhost', 8551)]

b = json.dumps(list)
b = b.encode()
print(b)

r = json.loads(b.decode())
print(r)