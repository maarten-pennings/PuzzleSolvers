// meshpuzzle.h - https://www.pelikanpuzzles.eu/detail/web-3/ 
// 2023 dec 11 Maarten Pennings
#include <stdio.h>
#include <stdint.h>


// A puzzle of order n has 2n "bars".
// Half of the bars are "rows" placed on the "table" (they "stand").
// The other half are "columns" placed on top of the rows (they "hang").
// The result is a mesh with n² "crossings".
// At each crossing, two bars interlock; they have cutouts at discrete "levels".
//   
//      +-+   +-+   +-+ 
//      | |   | |   | | 
//   +--+ +---+ +---+ +--+
// 0 |                   | stand
//   +--+ +---+ +---+ +--+
//      | |   | |   | | 
//   +--+ +---+ +---+ +--+
// 2 |                   | stand
//   +--+ +---+ +---+ +--+
//      | |   | |   | | 
//   +--+ +---+ +---+ +--+
// 4 |                   | stand
//   +--+ +---+ +---+ +--+
//      | |   | |   | | 
//      +-+   +-+   +-+    
//       1     3     5
//      hang  hang  hang
//
// The diagram shows a puzzle of order 3 (with the 6 slot numbers).
// Note that the slots are ordered in the way the solver places the bars.
//
// A profile is a board encoded in one integer for each of the n by n crossings
// the level at that crossing.


// Order of the puzzle (ie number of row bars and column bars, and thus number of crossings in bars)
#define ORDER 3
// Number of heights at each crossing
#define LEVEL 3
// The character to use for printing a block
#define SQUARE '\xB2' // or '#'
// The characters for the crossing fit
#define CROSSGAP '='
#define CROSSFIT '-'
#define CROSSERR '!'


// This represents one bar (i.e. the levels at each crossing)
typedef struct bar_s { int levels[ORDER]; } bar_t; // each level is [0,LEVEL)


// Returns the normalized (smallest) code of a bar
int bar_code( bar_t * bar ) {
  int v1= 0;
  for(int crossix=0; crossix<ORDER; crossix++ ) {
    v1= v1*10 + bar->levels[crossix];
  }
  int v2= 0;
  for(int crossix=ORDER-1; crossix>=0; crossix-- ) {
    v2= v2*10 + bar->levels[crossix];
  }
  return v1<v2 ? v1 : v2;
}


// Graphically prints a bar (horizontally), with frame at bottom (`standing`) or at top (not `standing` but hanging).
// When `flipped` flips right/left order.
void bar_print(bar_t * bar, int standing, int flipped) {
  // Print crossing levels
  if( !standing ) {
    printf("  ");
    for(int i=0; i<ORDER; i++ ) {
      int crossix = flipped ? (ORDER-1)-i : i;
      printf(" %d",bar->levels[crossix]);
    }
    if( flipped ) printf(" (f)");
    printf("  (=%0*d)\n",LEVEL,bar_code(bar));
  }
  
  // Print crossings graphically
  for(int level=0; level<=LEVEL; level++ ) { // print one row extra
    printf("  %c",SQUARE);
    for(int i=0; i<ORDER; i++ ) {
      int crossix = flipped ? (ORDER-1)-i : i;
      if( standing ) {
        if( LEVEL-level > bar->levels[crossix] ) printf(" "); else printf("%c",SQUARE);
      } else {
        if( level > bar->levels[crossix] ) printf(" "); else printf("%c",SQUARE);
      }
      printf("%c",SQUARE);
    }
    printf("\n");
  }

  // Print crossing levels
  if( standing ) {
    printf("  ");
    for(int i=0; i<ORDER; i++ ) {
      int crossix = flipped ? (ORDER-1)-i : i;
      printf(" %d",bar->levels[crossix]);
    }
    if( flipped ) printf(" (f)");
    bar_code(bar);
    printf("  (=%0*d)\n",LEVEL,bar_code(bar));
  }
}


// This type represents a puzzle, ie a series of bars (the `profile` is not used, only there for reference).
typedef struct puzzle_s { bar_t bars[2*ORDER]; uint64_t profile; } puzzle_t;


// Prints a puzzle (all bars)
// Also prints the levels matrix, but that only makes sens when the the bars are derived from a profile.
void puzzle_print(puzzle_t*puzzle) {
  for( int i=0; i<=ORDER; i++ ) printf("=="); printf("=== puzzle %ld\n",puzzle->profile);
  // Print profile (matrix with levels at the crossing)
  for( int y=0; y<ORDER; y++ ) {
    printf("  ");
    for( int x=0; x<ORDER; x++ ) {
      int lvl1= puzzle->bars[y].levels[x];
      int lvl2= puzzle->bars[ORDER+x].levels[y];
      int lvl= lvl1+lvl2;
      printf("%d", lvl1 );
      if( lvl< LEVEL-1 ) printf("%c",CROSSGAP);
      if( lvl==LEVEL-1 ) printf("%c",CROSSFIT);
      if( lvl> LEVEL-1 ) printf("%c",CROSSERR);
      printf("%d ", lvl2 );
    }
    printf("\n");
  }
  // Print the bars
  for( int barix=0; barix<2*ORDER; barix++ ) {
    for( int i=0; i<=ORDER; i++ ) printf("--"); printf("--- bar%d\n",barix);
    bar_print(&puzzle->bars[barix],barix<ORDER,0);
  }
  for( int i=0; i<=ORDER; i++ ) printf("=="); printf("===\n");
  // Print white line
  printf("\n");
}


// Converts `profile` number to a `puzzle`, returns true if profile is valid (not too large)
int puzzle_fromprofile(uint64_t profile,puzzle_t*puzzle) {
  puzzle->profile = profile;
  for( int y=0; y<ORDER; y++ ) {
    for( int x=0; x<ORDER; x++ ) {
      // One profile level determines the level in two bars
      int level = profile % LEVEL;
      puzzle->bars[y].levels[x] = level; // standing row
      puzzle->bars[ORDER+x].levels[y] = (LEVEL-1)-level; // hanging column
      profile /= LEVEL;
    }
  }
  return profile == 0;
}


// Records a state (solution)
typedef struct {
  int bar_remains[2*ORDER];     // barix:  records if bar is still available (or already placed on the table)
  int slot_barix[2*ORDER];      // slotix: records which bar is placed at the slot
  int slot_barflipped[2*ORDER]; // slotix: records if the bar at the slot is flipped
  int num;                      // index of solution
} state_t;


// Prints a state
//   | | | |
// 0-+-+-+-+-
// 2-+-+-+-+-
// 4-+-+-+-+-
// 6-+-+-+-+-
//   | | | |
//   1 3 5 7
void state_print(puzzle_t*puzzle,state_t*state) {
  // Solution index
  printf("sol %03d:",state->num);
  // Solution bars
  for(int slotix=0; slotix<2*ORDER; slotix+=2 ) printf(" bar%d%s", state->slot_barix[slotix], state->slot_barflipped[slotix]?"f":" " );
  printf(" /");
  for(int slotix=1; slotix<2*ORDER; slotix+=2 ) printf(" bar%d%s", state->slot_barix[slotix], state->slot_barflipped[slotix]?"f":" " );
  printf(" (");
  // Level check
  for( int y=0; y<ORDER; y++ ) {
    for( int x=0; x<ORDER; x++ ) {
      // print the two levels at crossing x,y
      int row_slotix  = y*2;
      int row_barix  = state->slot_barix[row_slotix];
      
      int col_slotix  = x*2+1;
      int col_barix  = state->slot_barix[col_slotix];

      int row_crossix = x; 
      if( state->slot_barflipped[row_slotix] ) row_crossix = (ORDER-1)-row_crossix;

      int col_crossix = y;
      if( state->slot_barflipped[col_slotix] ) col_crossix = (ORDER-1)-col_crossix;

      int lvl1= puzzle->bars[row_barix].levels[row_crossix];
      int lvl2= puzzle->bars[col_barix].levels[col_crossix];
      
      int lvl= lvl1+lvl2;
      printf("%d", lvl1 );
      if( lvl< LEVEL-1 ) printf("%c",CROSSGAP);
      if( lvl==LEVEL-1 ) printf("%c",CROSSFIT);
      if( lvl> LEVEL-1 ) printf("%c",CROSSERR);
      printf("%d ", lvl2 );
    }
  }
  printf(")\n");
}


// A bar is tried in slot `slotix`; does it fit?
//   | | | |
// 0-+-+-+-+-
// 2-+-+-+-+-
// 4-+-+-+-+-
// 6-+-+-+-+-
//   | | | |
//   1 3 5 7
int puzzle_fits(puzzle_t*puzzle,state_t*state,int slotix) {
  if( slotix%2==0 ) {
    // A row is placed, check meshing with already placed columns
    int row_slotix = slotix;
    int row_barix = state->slot_barix[row_slotix];
    for( int col_slotix=1; col_slotix<slotix; col_slotix+=2 ) {
      int col_barix = state->slot_barix[col_slotix];
      int row_crossix= col_slotix/2;
      int col_crossix = slotix/2;
      if( state->slot_barflipped[col_slotix] ) col_crossix = (ORDER-1)-col_crossix;
      if( state->slot_barflipped[row_slotix] ) row_crossix = (ORDER-1)-row_crossix;
      int lvl1= puzzle->bars[row_barix].levels[row_crossix];
      int lvl2= puzzle->bars[col_barix].levels[col_crossix];
      if( lvl1 + lvl2 >= LEVEL ) return 0; // crossing does not fit
    }
    return 1;
  } else {
    // A column is placed, check meshing with already placed rows
    int col_slotix = slotix;
    int col_barix = state->slot_barix[col_slotix];
    for( int row_slotix=0; row_slotix<slotix; row_slotix+=2 ) {
      int row_barix = state->slot_barix[row_slotix];
      int col_crossix= row_slotix/2;
      int row_crossix = slotix/2; 
      if( state->slot_barflipped[col_slotix] ) col_crossix = (ORDER-1)-col_crossix;
      if( state->slot_barflipped[row_slotix] ) row_crossix = (ORDER-1)-row_crossix;
      int lvl1= puzzle->bars[row_barix].levels[row_crossix];
      int lvl2= puzzle->bars[col_barix].levels[col_crossix];
      if( lvl1 + lvl2 >= LEVEL ) return 0; // crossing does not fit
    }
    return 1;
  }  
}

void puzzle_solvestep(puzzle_t*puzzle, state_t*state, int slotix) {
  if( slotix == 2*ORDER ) {
    // All slots filled, we have a solution
    state_print(puzzle,state);
    state->num++;
  } else {
    // Fill the first remaining slot
    if( slotix==ORDER ) {
      int found0 = 0;
      for( int slotix=0; slotix<ORDER; slotix+=2 ) 
        found0 |= state->slot_barix[slotix]==0 && state->slot_barflipped[slotix]==0;
        if( !found0 ) return;
    }
    for(int barix=0; barix<2*ORDER; barix++ ) if( state->bar_remains[barix] ) {
      // Place bar `barix` in slot `slotix` (straight and flipped)
      state->bar_remains[barix]= 0;
      state->slot_barix[slotix]= barix;        
      for(int flipped=0; flipped<2; flipped++ ) {
        state->slot_barflipped[slotix]= flipped;
        if( puzzle_fits(puzzle, state, slotix) ) puzzle_solvestep(puzzle, state, slotix+1 );
      }
      state->bar_remains[barix]= 1;        
    }
  }
}

void puzzle_solve(puzzle_t*puzzle) {
  static state_t state;
  for( int barix=0; barix<2*ORDER; barix++ ) state.bar_remains[barix]=1;
  state.num=0;
  puzzle_solvestep(puzzle,&state,0);
  printf("#solutions %d\n",state.num);
}


puzzle_t puzzle;

void all() {
  uint64_t profile = 0; 
  while( puzzle_fromprofile(profile,&puzzle) ) {
    puzzle_print(&puzzle);
    //puzzle_solve(&puzzle);
    profile++;
    if( profile==0 ) printf("ERROR\n");
  } 
  printf("#puzzles %ld\n",profile);
}  


void main() {
  // puzzle https://www.pelikanpuzzles.eu/detail/web-3/ and https://www.youtube.com/watch?v=QEGd3KW1-oY
  uint64_t profile = 0+3*(0+3*(2+3*(1+3*(2+3*(0+3*(2+3*(1+3*(1))))))));
  puzzle_fromprofile(profile,&puzzle);
  puzzle_print(&puzzle);
  puzzle_solve(&puzzle);
}
