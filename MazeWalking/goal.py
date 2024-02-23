import turtle

class Goal:
  """ Goal class for representing a target/goal inside the maze. """

  def __init__(self, maze, color):
    """ Create a new goal in a maze at the lower right corner of the maze. """

    # default location of the goal - lower right corner at (columns, rows):
    self.column = maze.get_columns()        # the column (x) location (number of squares)
    self.row = maze.get_rows()              # the row (y) location (number of squares)

    self.maze_goal = turtle.Turtle()
    self.maze_goal.shape("circle")
    self.maze_goal.color(color)
    self.maze_goal.setheading(0)
    self.maze_goal.penup()

    self.step = maze.get_square_size()
    self.maze_goal.goto(maze.get_init_x() + (self.column - 0.5) * self.step, maze.get_init_y() - (self.row - 0.5) * self.step)

    #return self.maze_goal


  def get_column(self):
    return self.column

  def get_row(self):
    return self.row
