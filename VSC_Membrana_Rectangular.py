# -*- coding: utf-8 -*-
"""
Created on Fri Feb 26 10:03:16 2016

@author: Alejandro


============================================

Vibraciones de Sistemas Continuos

Master Universitario de Estructuras
Universidad de Granada

Curso Académico 2015-2016


=============================================

Cálculo de las frecuencias naturales y modos de vibración

Membrana Rectangular

"""
from __future__ import division
import numpy as np


#==============================================================================
# Datos de entrada
#==============================================================================

A = 1    # Lado a, dimensión (m)
B = 1    # Lado b, dimensión (m)
H = 0.01 # Espesor de la membrana (m)

Tens = 10    # Tensión de membrana, dato en N / m 
Rho = 100      # Densidad VOLUMETRICA, dato en kg/m3

#==============================================================================
# Funciones
#==============================================================================
def Omegamn(m, n):
    Fmn = (m / A)**2. + (n/B)**2.
    wmn = np.pi * np.sqrt( Tens / (Rho * H) * Fmn)
    return wmn 


print "Frecuencia Angular Fundamental ", Omegamn(1,1) , " (rd/s)" 

print "Frecuencia (Hz) Fundamental", Omegamn(1,1) / ( 2. * np.pi ), " (Hz)"

def Modo(m, n, x, y):
    return np.sin (m * np.pi * x / A) * np.sin (n * np.pi * y / B)
    
#==============================================================================
# Figura
#==============================================================================

Mmodo = 2
Nmodo = 3

T = 2.*np.pi / Omegamn(Mmodo,Nmodo) 
t = np.arange(0, 2 * T, 2* T / 100) # Vector de tiempos.
len(t)

from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import matplotlib.pyplot as plt


fig = plt.figure()
ax = fig.gca(projection='3d')
X = np.arange(0, A, 0.02*A)
Y = np.arange(0, B, 0.02*B)
X, Y = np.meshgrid(X, Y)
Z = Modo(Mmodo, Nmodo, X, Y)
surf = ax.plot_surface(X, Y, Z, rstride=1, cstride=1,cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)
ax.set_zlim(-1.01, 1.01)
ax.set_xlim(0,A)
ax.set_ylim(0,B)
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
