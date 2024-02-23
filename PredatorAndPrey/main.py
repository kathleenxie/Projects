# filename: OO - Super Turtles - Project 1 - WIP
from random import randint
from superturtle import *
import time

# make sure to implement a class that supports the following:

prey = Superturtle(100, -40, 90, 5, 'blue')       # (x, y, heading, step, color)
prey.start_trace()

predator = Superturtle(60, 90, 90, 10, 'red')    # (x, y, heading, step, color)
predator.start_trace()                          # lower the pen to leave a trace as the superturtle moves

# The Pseudo Code:
'''
while the distance between the predator and the prey is bigger than the predator step size:
  the predator moves forward - based on its step size
  if the smell of the prey is "weaker" after this step, compared to the smell before:
    the predator turns left by a random amount in the range of 1-90 degrees
'''

# Your code here:

prev_smell = 0    
curr_smell = predator.distance_to(prey)

while curr_smell > predator.get_step():   #this loop runs while the distance between the predator and prey is larger than the step number
  predator.move_forward(predator.get_step())   
  prey.move_in_circle(6,5)   #the prey moves forward by 5 and turns left by 5 so it goes in the shape of a circle
  curr_smell = predator.distance_to(prey)
  print curr_smell    #prints the distance between predator and prey each time
  if curr_smell > prev_smell:   
    predator.turn_left(randint(1, 90))    #turn left by a random number between 1 and 90
  prev_smell = curr_smell
    
predator.stop_trace()
