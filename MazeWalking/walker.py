import turtle

class Walker:
  ''' a maze-walking turtle. '''

  def __init__(self, maze, column, row, heading, color):
    if column > maze.get_columns() or row > maze.get_rows():
      raise ValueError("Maze_walker(): The number of columns/rows doesn't match the size of the maze.")
    self.maze = maze
    self.column = column
    self.row = row
    self.angle = heading

    self.maze_walker = turtle.Turtle()
    self.maze_walker.shape("turtle")
    self.maze_walker.color(color)
    self.maze_walker.setheading(heading)
    self.maze_walker.penup()

    self.step = maze.get_square_size()
    self.maze_walker.goto(maze.get_init_x() + (column - 0.5) * self.step, maze.get_init_y() - (row - 0.5) * self.step)

    #return self.maze_walker


  def move_forward(self):
    self.maze_walker.pd()
    self.maze_walker.fd(self.step)
    self.maze_walker.pu()
    if self.angle == 0:                           # pointing "east" (right)
      self.column = self.column + 1
    if self.angle == 90 or self.angle == -270:    # pointing "north" (up)
      self.row = self.row - 1
    if self.angle == -90 or self.angle == 270:    # pointing "south" (down)
      self.row = self.row + 1
    if self.angle == 180 or self.angle == -180:   # pointing "west" (left)
      self.column = self.column - 1
    #print self.column, self.row
  
  def turn_left(self):
    self.maze_walker.left(90)
    self.angle = (self.angle + 90) % 360

  def turn_right(self):
    self.maze_walker.right(90)
    self.angle = (self.angle - 90) % 360

  def is_tile_blocked(self, maze, tile_column, tile_row):
    # tiles outside the maze are "blocked" by definition:
    if tile_column < 1 or tile_row < 1 or tile_column > maze.columns or tile_row > maze.rows:
      return True
    
    blocked_tiles = maze.get_blockers()
    #print blocked_tiles
    wall_tiles = blocked_tiles[maze.wall_color]
    if [tile_column, tile_row] in wall_tiles:
      return True
    return False

  def is_direction_blocked(self, maze, direction):
    # possible directions to look at: "ahead", "on_left", "on_right"
    angle = self.angle
    #print "is_direction_blocked(), direction, angle:", direction, angle
    if direction == "ahead":
      if angle == 0:
        tile_column = self.column + 1
        tile_row = self.row
      if angle == 90 or angle == -270:
        tile_column = self.column
        tile_row = self.row - 1
      if angle == 180 or angle == -180:
        tile_column = self.column - 1
        tile_row = self.row
      if angle == -90 or angle == 270:
        tile_column = self.column
        tile_row = self.row + 1
    if direction == "on_left":
      if angle == 0:
        tile_column = self.column
        tile_row = self.row - 1
      if angle == 90 or angle == -270:
        tile_column = self.column - 1
        tile_row = self.row
      if angle == 180 or angle == -180:
        tile_column = self.column
        tile_row = self.row + 1
      if angle == -90 or angle == 270:
        tile_column = self.column + 1
        tile_row = self.row
    if direction == "on_right":
      if angle == 0:
        tile_column = self.column
        tile_row = self.row + 1
      if angle == 90 or angle == -270:
        tile_column = self.column + 1
        tile_row = self.row
      if angle == 180 or angle == -180:
        tile_column = self.column
        tile_row = self.row - 1
      if angle == -90 or angle == 270:
        tile_column = self.column - 1
        tile_row = self.row
    return(self.is_tile_blocked(self.maze, tile_column, tile_row))
  
  def get_column(self):
    return self.column
  
  def get_row(self):
    return self.row
  
  
