from random import randint
from superturtle import *
import time

prey = Superturtle(100, -40, 90, 5, 'blue')       # (x, y, heading, step, color)
prey.start_trace()

predator = Superturtle(60, 90, 90, 10, 'red')    # (x, y, heading, step, color)
predator.start_trace()                          # lower the pen to leave a trace as the superturtle moves

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
