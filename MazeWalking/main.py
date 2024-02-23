import maze, walker, goal
from random import randint, choice
  
#########################
# Maze Creation part
#########################

def block_line(start_column, start_row, length, direction, block_type):
  # direction of the line can be: "down", "right", "down-left" (diagonal), "down-right" (diagonal)
  # I created 4 if-statements that incremented or decremented the row and col based on the direction of the line
  curr_col = start_column
  curr_row = start_row
  for i in range(length):
    maze.block_square(curr_col, curr_row, block_type) #this line colors the specific block of the row and column
    if direction == "down":
      curr_row += 1
    elif direction == "right":
      curr_col += 1
    elif direction == "down-right":
      curr_row += 1
      curr_col += 1
    elif direction == "down-left":
      curr_row += 1
      curr_col -= 1
      
# create the maze:
columns = 30
rows = 10
maze = maze.Maze(columns, rows)      # size of maze (columns, rows)

# Create the goal:
goal = goal.Goal(maze, "gold")    # (maze, color)

# Create a maze walker:
walker = walker.Walker(maze, 1, 1,0, "blue")     # (maze, column, row, heading, color)


# Placing blocking walls/tiles in the maze:

# the letter K
block_line(3, maze.get_rows() - 5, 6, "down", "wall")   
block_line(6, maze.get_rows() - 5, 3, "down-left", "wall")   
block_line(5, maze.get_rows() - 2, 1, "down", "wall") 
block_line(6, maze.get_rows() - 1, 2, "down", "wall")   

# the letter A
block_line(9, maze.get_rows() - 2, 3, "down", "wall")   
block_line(11, maze.get_rows() - 5, 3, "down-left", "wall")   
block_line(10, maze.get_rows() - 2, 3, "right", "wall")   
block_line(12, maze.get_rows() - 4, 2, "down-right", "wall")   
block_line(13, maze.get_rows() - 2, 3, "down", "wall")   

# the letter T
block_line(15, maze.get_rows() - 5, 5, "right", "wall")   
block_line(17, maze.get_rows() - 4, 5, "down", "wall")   

# the letter H
block_line(22, maze.get_rows() - 5, 6, "down", "wall")   
block_line(23, maze.get_rows() - 3, 2, "right", "wall")   
block_line(25, maze.get_rows() - 5, 6, "down", "wall")   

maze.wrapup()   # finish up the maze creation by refreshing/updating the screen


#########################
# Maze Walking part
#########################

'''
My pseudocode:

while walker has not reached the goal yet:
  if tile on the right is not blocked or off the maze:
    turn right
  if tile ahead is open:
    move forward
  else:
    turn left
'''

num_of_moves = 0  #initialize number of moves counter

while (walker.get_row() != goal.get_row() or walker.get_column() != goal.get_column()) and (num_of_moves <= 300):
  if not walker.is_direction_blocked(maze, "on_right"):
    walker.turn_right()
  if not walker.is_direction_blocked(maze, "ahead"):
    walker.move_forward()
  else:
    walker.turn_left()
  num_of_moves += 1   #increment counter and print
  print num_of_moves, "out of 300 moves"
  
