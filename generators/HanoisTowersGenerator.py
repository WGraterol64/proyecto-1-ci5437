from sys import argv

# Leemos y verificamos los argumentos.
assert len(argv) == 3, "Syntax error."
P = int(argv[1])
D = int(argv[2])
assert P > 2 and D > 1, f'P must be greater than 2 and D must be grater than 1.'

# Description
psvn = \
f'# In the Towers of Hanoi puzzle there are P distinct pegs and D disks, each disk \n\
# a different size. In a given state, each disk is on exactly one peg and the disks \n\
# on a peg are in increasing size from top to bottom. An operator moves the top disk\n\
# from one peg to the top of another peg. An operator can only be applied if the \n\
# disk being moved is smaller than the top disk on the peg to which it is being \n\
# moved. In this case P = {P} and D = {D}. In this encoding, for each disk there will\n\
# be {P} binary state variables indicating which peg the disk is on (if disk d is on\n\
# peg i, the i^th state variable for d will be 1 and the others will be 0). The \n\
# variables for the smallest disk are first, then the variables for the second \n\
# smallest disk, etc. For each disk d and for each pair of pegs i and j one PSVN \n\
# operator is needed for moving d from the top of peg i to the top of peg j only \n\
# if all previous disks not is in the peg j.\n\n'

# States.
psvn += \
f'# A state is a binary vector of length {P * D} \n\
{P * D}\n' + ('2 ' * P + ' ') * D + '\n\n'

# Rules.
for i in range(D):
  psvn += f'# {i}^th disk \n'
  # Moves the i^th disk
  for j in range(P):
    # from the j^th peg
    for k in range(P):
      # to the k^th peg with j != k.
      if j != k:
        # lhc
        if j > k: 
          psvn += ('- ' * k + '0 ' + '- ' * (j - k - 1) + '0 ' + '- ' * (P - j - 1) + ' ') * i
          psvn += '- ' * k + '0 ' + '- ' * (j - k - 1) + '1 ' + '- ' * (P - j - 1) + ' '
        else: 
          psvn += ('- ' * j + '0 ' + '- ' * (k - j - 1) + '0 ' + '- ' * (P - k - 1) + ' ') * i
          psvn += '- ' * j + '1 ' + '- ' * (k - j - 1) + '0 ' + '- ' * (P - k - 1) + ' '
        psvn += ('- ' * P + ' ') * (D - i - 1) + ' =>   '

        # rhc
        if j > k: 
          psvn += ('- ' * k + '0 ' + '- ' * (j - k - 1) + '0 ' + '- ' * (P - j - 1) + ' ') * i
          psvn += '- ' * k + '1 ' + '- ' * (j - k - 1) + '0 ' + '- ' * (P - j - 1) + ' '
        else: 
          psvn += ('- ' * j + '0 ' + '- ' * (k - j - 1) + '0 ' + '- ' * (P - k - 1) + ' ') * i
          psvn += '- ' * j + '0 ' + '- ' * (k - j - 1) + '1 ' + '- ' * (P - k - 1) + ' '
        psvn += ('- ' * P + ' ') * (D - i - 1) 

        # label
        psvn += f'  LABEL D{i}_From{j}_To{k}\n'


# Goal
psvn += '\n# The goal is that all disks are in the last peg.\n'
psvn += 'GOAL ' + ('1 ' + '0 ' * (P - 1) + ' ') * D

print(psvn)