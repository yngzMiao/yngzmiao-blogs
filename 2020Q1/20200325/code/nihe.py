# coding=utf-8
import pylab
import numpy as np
import sys, os
from scipy.optimize import curve_fit

def func(x, a, b, c):
  return b * np.power(a, x) + c

if __name__ == "__main__":
  x = np.arange(1, 31, 1)
  y = np.array([20, 23, 26, 29, 32, 35, 38, 45, 53, 62, 73, 86, 101, 118, 138, 161, 188, 220, 257, 300, 350, 409, 478, 558, 651, 760, 887, 1035, 1208, 1410])
  # z1 = np.polyfit(x, y, 3)
  # p1 = np.poly1d(z1)
  # print(z1)
  # y_pred = p1(x)
  # # print(np.polyval(p1, 29))
  # # print(np.polyval(z1, 29))

  popt, pcov = curve_fit(func, x, y)
  y_pred = [func(i, popt[0], popt[1], popt[2]) for i in x]
  print(popt)
  print(popt)

  plot1 = pylab.plot(x, y, '*', label='original values')
  plot2 = pylab.plot(x, y_pred, 'r', label='fit values')
  pylab.title('')
  pylab.xlabel('')
  pylab.ylabel('')
  pylab.legend(loc=3, borderaxespad=0., bbox_to_anchor=(0, 0))
  pylab.show()
  pylab.savefig('p1.png', dpi=200, bbox_inches='tight')
