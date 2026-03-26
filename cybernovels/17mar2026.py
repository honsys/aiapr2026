#!/usr/bin/env python3
ss = 2068.90
z = 1424.95 # feb 2026 div report
fid = (0.8*3762.94 + 1821.29) / 12
ml = 0.8*(402 + 402 + 242.8) / 3
totmo = ss + z + fid + ml
print(ss, z, fid, ml, totmo)
sobe = 2800 - 800 - 400 - 275
totmo = ss + z + fid + ml + sobe
print(ss, z, fid, ml, sobe, totmo)

