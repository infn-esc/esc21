#!/usr/bin/python
""" 
    inspiered to DIRAC Benchmark 2012 by Ricardo Graciani, and wrapper functions to
    run multiple copies in parallel by Andrew McNab.
"""

import os
import math
import sys
import random
import urllib
import multiprocessing


def singleRun( iterations = 1, extraIteration = False ):
  """ Get Normalized Power of one CPU in DIRAC Benchmark 2012 units (DB12)
  """

  # This number of iterations corresponds to 1kHS2k.seconds, i.e. 250 HS06 seconds

  n = int( 1000 * 1000 * 12.5 )
  calib = 250.0

  m = 0.0
  m2 = 0.0
  nn = 0
  # Do one iteration extra to allow CPUs with variable speed (we ignore zeroth iteration)
  # Possibly do one extra iteration to avoid tail effects when copies run in parallel
  for i in range( iterations + 1 + (2 if extraIteration else 0)):
    if i == 1:
      start = os.times()

    # Now the iterations
    for _j in xrange( n ):
      t = random.gauss( 10., 1. )
      m += t
      m2 += t * t
      nn += 1

    if i == iterations:
      end = os.times()

  cput = sum( end[:4] ) - sum( start[:4] )
  wall = end[4] - start[4]
  

  return { 'CPU' : cput, 'WALL' : wall, 'BENCH' : calib * iterations / cput, 'N' : nn, 'AVE' : m/nn , 'RMS' : math.sqrt((m2-m*m/nn)/(nn-1)) }

#
# If we run as a command
#   
if __name__ == "__main__":

  iterations     = 1
  extraIteration = False

  for arg in sys.argv[1:]:
    if arg.startswith('--iterations='):
      iterations = int(arg[13:])
    elif arg == '--extra-iteration':
      extraIteration = True

  print singleRun(iterations = iterations, extraIteration = extraIteration)
