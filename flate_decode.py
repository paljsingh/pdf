import re
import zlib
import sys

print(sys.argv[1])
pdf = open(sys.argv[1], "rb").read()
stream = re.compile(r'.*?FlateDecode.*?stream(.*?)endstream', re.S)

for s in stream.findall(pdf):
    s = s.strip('\r\n')
    print(zlib.decompress(s))
