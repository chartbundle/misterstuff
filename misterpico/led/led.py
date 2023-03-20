#! /usr/bin/env python3

# Generate lookup table for converting between perceived LED brightness and PWM

# Adapted from: https://jared.geek.nz/2013/feb/linear-led-pwm
# See also: https://ledshield.wordpress.com/2012/11/13/led-brightness-to-your-eye-gamma-correction-no/

# We need this reversed, input PWM, output brightness

from sys import stdout

TABLE_SIZE = 65536 # Number of steps (brightness)
RESOLUTION = 65536 # PWM resolution (10-bit = 1024)

def cie1931(L):
    L *= 100
    if L <= 8:
        return L / 902.3
    else:
        return ((L + 16) / 116)**3

x = range(0, TABLE_SIZE)
y = [cie1931(float(L) / (TABLE_SIZE - 1)) * (RESOLUTION - 1) for L in x]

#stdout.write('const uint16_t CIE[%d] = {' % TABLE_SIZE)
bigL={}

for i, L in enumerate(y):
#    if i % 16 == 0:
     bigL[int(L/256)]=i/256
#        stdout.write('\n')
#    stdout.write('% 5d,' % round(L))

#stdout.write('\n};\n')
for i in sorted(bigL.keys()):
  print(i,int(bigL[i]))

