# -*- coding: utf-8 -*-
"""
============================================

Vibraciones de Sistemas Continuos

Master Universitario de Estructuras
Universidad de Granada

Curso Académico 2015-2016


=============================================

Cálculo de las frecuencias naturales de vibración de una membrana circular

"""

from __future__ import division
from scipy.special import jn_zeros # Ceros de las funciones de Bessel

d_max = 8 # Número de modos en la dirección radial
c_max = 3 # Número de modos en la dirección circular

modes = {}

fundamental = jn_zeros(0, 1)[0]

for d in range(d_max):
    zeros = jn_zeros(d, c_max)
    for c in range(c_max):
        modes.update({(d, c+1): zeros[c] / fundamental})

# Print the modes sorted by frequency
        
for w in sorted(modes, key=modes.get):
    print repr(w) + ': ' + str(modes[w])