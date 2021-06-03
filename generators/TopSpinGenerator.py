from sys import argv

# Leemos y verificamos los argumentos.
assert len(argv) == 3, "Syntax error."
N = int(argv[1])
K = int(argv[2])
assert K > 1 and N > K, f'K must be greater than 1 and N must be grater than K.'

# Description.
psvn = \
f'# TopSpin is a commercial puzzle made by Binary Arts. There is a circular ring \n\
# of N tokens from 0 to N-1 and a special section of the ring (the "turntable" or \n\
# "turnstile") of length K that can be "spun" to reverse the order of the K tokens\n\
# in it. In this case, N = {N} and K = {K}. In the physical puzzle, the ring of tokens \n\
# has to be rotated to get the token you want to reverse into the turntable. In \n\
# most computer encodings (including the ones here), the rotation of the ring is \n\
# not counted as a move, any K adjacent locations can be rotated, including locations\n\
# that are adjacent by "wrapping around" from the end of the vector to the beginning, \n\
# e.g. there is an operator that reverses the tokens in location ..., N-1, 0, 1, ... \n\
# Because only the circular order of the tokens matters, in this encoding there are \n\
# {N} goal states which differ only in their cyclic rotation.\n\n'

# Domain.
psvn += \
f'# Domain called "token" consisting of {N} constants. \n\
DOMAIN token {N} \n'
for i in range(N): psvn += f'{i} '

# States.
psvn += \
f'\n\n# A state is a vector of length {N} \n\
{N} ' + 'token '* N + '\n\n'

# Rules.
for i in range(N):
  if i + K <= N:
    psvn += '-  ' * i
    for j in range(K): psvn += f'X{j} '
    psvn += '-  ' * (N - K - i) + ' =>  '
    psvn += '-  ' * i
    for j in range(K-1, -1, -1): psvn += f'X{j} '
    psvn += '-  ' * (N - K - i)
  else:
    for j in range(i + K - N): psvn += f'X{N - i + j} '
    psvn += '-  ' * (N - K)
    for j in range(N - i): psvn += f'X{j} '
    psvn += ' =>  '
    for j in range(i + K - N - 1, -1, -1): psvn += f'X{j} '
    psvn += '-  ' * (N - K)
    for j in range(K - 1, i + K - N - 1, -1): psvn += f'X{j} '
  psvn += f'  LABEL R{i}_{(i + K - 1) % N} \n'

# Goal.
psvn += '\nGOAL '
for i in range(N): psvn += f'{i} '

print(psvn)