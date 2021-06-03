from sys import argv

# Leemos y verificamos los argumentos.
assert 1 < len(argv) < 4, "Syntax error."
if len(argv) == 2: M = N = int(argv[1])
else:
  M = int(argv[1])
  N = int(argv[2])
assert M > 0 and N > 0, f'Values M and N must be positive integers.'

# Domain.
psvn = \
f'# {M}x{N} sliding tile puzzle, standard representation.\n\n\
# Domain called "tile" consisting of {M * N} constants where \'b\' is for the blank \n\
DOMAIN tile {M * N} \nb '
for i in range(1, M * N): psvn += f'{i} '

# States.
psvn += \
f'\n\n# A state is a vector of length {M * N} \n\
{M * N} ' + 'tile '*(M * N) + '\n\n'

# Rules.
def add_rule(first_pos, first_val, second_pos, second_val, label):
  # LHC
  lhc = '- ' * (first_pos) + first_val + ' '
  lhc += (second_pos - first_pos - 1) * '- ' + second_val + ' '
  lhc += (M * N - second_pos - 1) * '- '
  # RHC
  rhc = '- ' * (first_pos) + second_val + ' '
  rhc += (second_pos - first_pos - 1) * '- ' + first_val + ' '
  rhc += (M * N - second_pos - 1) * '- '
  return f'{lhc} =>  {rhc}  LABEL {label}\n'
for i in range(M * N):
  if i - N >= 0:      psvn += add_rule(i - M, 'X', i, 'b', 'TOP')
  if i % N != N - 1:  psvn += add_rule(i, 'b', i + 1, 'X', 'RIGHT')
  if i + N < M * N:   psvn += add_rule(i, 'b', i + N, 'X', 'BOT')
  if i % N != 0:      psvn += add_rule(i - 1, 'X', i, 'b', 'LEFT')

# Goal.
psvn += '\nGOAL b '
for i in range(1, M * N): psvn += f'{i} '

print(psvn) 