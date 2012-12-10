# Sample elevator AI

import random

def computeNextMove(floors, elevators):
   # the trailing , is required so that python doesn't reduce ((1,2)) to (1,2)
   # the C++ code on the other end of this function expects a tuple of tuples
   # of size 2, not a tuple of ints.
   return ( ( random.randint(1,10), random.randint(1,10) ), ) 
