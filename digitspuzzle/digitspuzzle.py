# Solves the 10-digits jig-saw puzzle
# Chris Ramsay: https://youtu.be/4cJ-zYMxIV4?t=966
# 3D print model: https://www.thingiverse.com/thing:4400600
print("Digits Puzzle")
print("  2020 Dec 27, Maarten Pennings")
import sys
from datetime import datetime


# Note that this solver is used for special pieces. Each piece consists
# of 3x5 squares. Each piece is actually a 7-segment display.
# Here we see an empty piece with col/row indices (left) and #1-no-top (right)
# A piece consists of dots (.), the empty segments (- and |) and used segments (#).
#   012
#  0·-·    ·-·
#  1|·|    |·#
#  2·-·    ·-·
#  3|·|    |·#
#  4·-·    ·-·
# This requires two tricks: the dots (·) above will be pre-filled with walls on the _board_.
# Secondly, if a piece is on the board, a segment is a horizontal or a vertical.
# This will be stored as third value in the layouts tuple.


# This var defines the (list of) pieces - in our case a piece for each of the 10 digits.
# The items in the list are the coordinates of squares that make up the piece.
# In this list, each piece is characterized by one layout, but there might be up to eight layouts.
# (four rotations, and four mirrored rotations). Those other layouts will be computed automatically.
# Secondly, the 2-tuples will be converted to 3-tuples automatically.
# This does assume the layouts here are 3x5 (so that the orientation can be inferred).
layouts2_config = [
  [(1,0),(0,1),(2,1),      (0,3),(2,3),(1,4)], #0
  [            (2,1),            (2,3)      ], #1-no-top
  [(1,0),      (2,1),(1,2),(0,3),      (1,4)], #2
  [(1,0),      (2,1),(1,2),      (2,3),(1,4)], #3
  [      (0,1),(2,1),(1,2),      (2,3),     ], #4
  [(1,0),(0,1),      (1,2),      (2,3),(1,4)], #5
  [(1,0),(0,1),      (1,2),(0,3),(2,3),(1,4)], #6-long-head
# [      (0,1),      (1,2),(0,3),(2,3),(1,4)], #6-short-head
# [(1,0),      (2,1),            (2,3)      ], #7-no-side
  [(1,0),(0,1),(2,1),            (2,3)      ], #7-with-side
  [(1,0),(0,1),(2,1),(1,2),(0,3),(2,3),(1,4)], #8
# [(1,0),(0,1),(2,1),(1,2),      (2,3),(1,4)], #9-long-tail
  [(1,0),(0,1),(2,1),(1,2),      (2,3)      ], #9-short-tail
]


# Squares either occupy a piece, the value of that square will be the index in the pieces array.
# Alternatively a square is empty or blocked ("wall"). 
# In the latter two cases, the following values are used for that square.
empty = len(layouts2_config)+0
wall  = len(layouts2_config)+1
# What character to print for a square, first the pieces, then empty, then wall
symbols="0123456789 ·" # pieces..., empty, wall


# Convert the two-tuple layouts2_config to a 3 tuple layouts3_config.
layouts3_config = []
for pieceix,layout in enumerate(layouts2_config):
  newlayout = []
  for x,y in layout :
    horizontal = x%2 == 1 # Assumes layout is 3x5 so we know orientation
    xyh= (x,y,horizontal)
    assert not( xyh in newlayout ) , f"Duplicate square in piece: piece {pieceix} for ({x},{y})"
    newlayout.append( xyh )
  layouts3_config.append(newlayout)


# Shifts `layout` so that the top-most, left-most used square has coordinates (0,0).
# Normalization also reorders all squares in "reading order"
# (from upper left to upper right, and from uppper to lower).
# The normalized order is needed to find duplicate layouts after mirroring.
# Returns the normalized layout.
def layout_norm(layout) :
  def before( xyh1, xyh2 ):
    if xyh1[1]<xyh2[1] : return True
    if xyh1[1]>xyh2[1] : return False
    return xyh1[0]<xyh2[0]
  top=(99,99,99)
  for xyh in layout :
    if before(xyh,top): 
      top = xyh
  newlayout=[]
  for (x,y,h) in layout :
    newx = x - top[0]
    newy = y - top[1]
    newlayout.append( (newx,newy,h) )
  newlayout.sort(key=lambda xyh:xyh[0]+10*xyh[1]+100*xyh[2])
  return newlayout


# Rotates `layout` 90 degrees counter clock wise.
# Returns the rotated layout (which is normalized).
def layout_rotate(layout) :
  newlayout=[]
  for (x,y,h) in layout :
    newlayout.append( (y,-x,not h) )
  return layout_norm(newlayout)


# Flips `layout` around y-axis.
# Returns the flipped layout (which is normalized).
def layout_flip(layout) :
  newlayout=[]
  for (x,y,h) in layout :
    newlayout.append( (-x,y,h) )
  return layout_norm(newlayout)


# Prints the `layout` (assuming it is normalized).
# All lines are prefixed with string `indent`.
def layout_print(indent,layout) :
  # find upper left
  minx=99
  miny=99
  maxx=0
  maxy=0
  for (x,y,h) in layout :
    if x<minx: minx=x 
    if y<miny: miny=y 
    if x>maxx: maxx=x 
    if y>maxy: maxy=y
  width= 1+maxx-minx    
  height= 1+maxy-miny
  img = [ ['·']*width for _ in range(height) ]
  # start printing
  for (x,y,h) in layout :
    img[y-miny][x-minx]= '-' if h else '|'
    if x==0 and y==0 : img[0-miny][0-minx]= '═' if h else '║' 
  for line in img :
    print( indent+"".join(line))


# Create a list of pieces.
# A piece is a list of layouts for that piece.
# All layouts are normalized.
# There are no duplicate layouts for a piece.
pieces = []
for lay in layouts3_config :
  lay0 = layout_norm(lay)
  lay1 = layout_rotate(lay0)
  lay2 = layout_rotate(lay1)
  lay3 = layout_rotate(lay2)
  lay4 = layout_flip(lay)
  lay5 = layout_rotate(lay4)
  lay6 = layout_rotate(lay5)
  lay7 = layout_rotate(lay6)
  layouts = []
  def append(newlayout) :
    for layout in layouts :
      if newlayout==layout : 
        return # do not add duplicate layout
    layouts.append(newlayout)
  append(lay0)
  append(lay1)
  append(lay2)
  append(lay3)
  # For the digits puzzle, I do not want the pieces mirrored
  #append(lay4)
  #append(lay5)
  #append(lay6)
  #append(lay7)
  pieces.append(layouts)


# Debug function: prints all layouts (as stored in `pieces`) for piece `pieceix`
def layouts_print(pieceix) :
  layouts = pieces[pieceix]
  for lix,layout in enumerate(layouts) :
    print( f"  Piece '{symbols[pieceix]}', layout {lix}/{len(layouts)}" )
    layout_print("    ",layout)


print("Pieces")
for pieceix,layouts in enumerate(pieces) :
  layouts_print(pieceix)


# Create a board with effective size `width` times `height`.
# It will have a border of squares around it, so real size is `width+1` times `height+1`.
# The border, but also on the inside, `wall`s are placed.
# All other cells are filled with `empty`.
# The board is returned.
def board_make(width=11,height=9) :
  width+=2
  height+=2
  board = [ [empty]*width for y in range(height) ]
  for y in range(height):
    board[y][0] = wall
    board[y][width-1] = wall
    for x in range(y%2,width,2):
      board[y][x] = wall
  for x in range(width):
    board[0][x] = wall
    board[height-1][x] = wall
  return board


# Prints the `board` using `indent` as prefix for every line
def board_print(board,indent="") :
  for y in range(1,len(board)-1):
    print( indent, end="")
    for x in range(1,len(board[y])-1):
      pieceix= board[y][x]
      char = symbols[pieceix]
      print( char, end="")
    print()


# Updates `board` by adding `layout` as coordinates `xy`, using `mark` as symbol on the board
# Returns True iff all squares in layout where empty on the board
# If False is returned, some of the fitting squares are still placed on board.
# board_dellayout() will remove the layout even in case of False
def board_addlayout(board,xy,mark,layout) :
  x0,y0 = xy
  for (x,y,h) in layout :
    if y0+y<len(board) and x0+x<len(board[y0+y]) and board[y0+y][x0+x]==empty and h==((x0+x)%2==0):
      board[y0+y][x0+x]= mark
    else :
      return False
  return True


# Updates `board` by removing `layout` as coordinates `xy`, using `mark` as symbol on the board
def board_dellayout(board,xy,mark,layout) :
  x0,y0 = xy
  for (x,y,h) in layout :
    if y0+y<len(board) and x0+x<len(board[y0+y]) and board[y0+y][x0+x]==mark :
      board[y0+y][x0+x]= empty
    else :
      return


# Finds the first empty (x,y) square on the `board`, searching from `xy`
def board_findempty(board,xy) :
  x,y = xy
  while True :
    x += 1
    if x==len(board[y]) :
      x = 1 # skip wall
      y += 1
    if y==len(board) :
      return None
    if board[y][x]==empty :
      return (x,y)


# Finds (x,y) coordinate of center of gravity of pieceix
def board_center(board,pieceix=0) :
  sx = 0
  sy = 0
  n = 0
  for y in range(1,len(board)-1):
    for x in range(1,len(board[y])-1):
      if board[y][x]==pieceix :
        sx += x
        sy += y
        n = n + 1
  return ( sx//n, sy//n )

solutions = 0    
def solver(board,xy,pieceixs) :
  global solutions
  for pieceix in pieceixs :
    newpieceixs = pieceixs.copy()
    newpieceixs.remove(pieceix)
    for layout in pieces[pieceix] :
      fits = board_addlayout(board,xy,pieceix,layout)
      if fits:
        if newpieceixs==[] : 
          solutions+=1
          print( f"  Solution {solutions}:" )
          board_print(board,"    ")
          print( f"  0-center: {board_center(board,0)}" )
        else :
          if pieceix==0 and board_center(board,pieceix)[0]>len(board)/2 :
            pass # only accept solutions with piece 0 on the left
          else :
            newxy = board_findempty(board,xy)
            assert newxy!=None
            solver(board,newxy,newpieceixs)
      board_dellayout(board,xy,pieceix,layout)


def solve(board) :
  xy = board_findempty(board,(0,0))
  pieceixs = [0,1,2,3,4,5,6,7,8,9]
  solver(board,xy,pieceixs)


print("Solutions")
board = board_make()
t0=datetime.now()
solve(board)
t1=datetime.now()
print( f"{t1-t0} seconds for {solutions} solutions by {sys.argv[0]} ")


