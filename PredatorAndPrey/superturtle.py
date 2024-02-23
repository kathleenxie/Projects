from turtle import *

class Superturtle():
  def __init__(self, init_x, init_y, init_heading, step, color):
    self.__turtle = Turtle()
    self.__turtle.color(color)
    self.__turtle.penup()
    
    self.__x = init_x
    self.__y = init_y
    self.__turtle.goto(init_x, init_y)
    
    self.__turtle.setheading(init_heading)
    self.__step = step
    
  def get_x(self):
    return self.__x
    
  def get_y(self):
    return self.__y
    
  def set_x(self, x):
    self.__x = x
  
  def set_x(self, y):
    self.__y = y
  
  def get_step(self):
    return self.__step
  
  def start_trace(self):
    self.__turtle.pendown()
  
  def stop_trace(self):
    self.__turtle.penup()

  def move_forward(self, step):
    self.__turtle.forward(step)
    
  def distance_to(self, other_st):
    dX = other_st.__turtle.xcor() - self.__turtle.xcor()
    dY = other_st.__turtle.ycor() - self.__turtle.ycor()
    return (dY**2 + dX**2) ** 0.5  
    
  def turn_left(self, angle):
    self.__turtle.left(angle)
  
  def move_in_circle(self, length, angle):
    self.__turtle.forward(length)
    self.__turtle.left(angle)
  
