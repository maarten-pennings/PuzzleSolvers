// futoshiki.c - Solving futoshiki style puzzles
// gcc futoshiki.c -o futoshiki.exe
// gcc futoshiki.c -o futoshiki.exe && futoshiki.exe


// 2026 Jan 18, Maarten Pennings


/*
The Futoshiki puzzle

In a board of size ORDER, there are ORDER rows and ORDER columns.
The aim is to place the symbols 1..ORDER in each row and each column.
As an extra constraint, sometimes a relation in imposed on two 
neighboring cells (east-west) or north-south). The relation is
either less-than or greater-than.
As a starter, some cell values are already given.

    +--+ +--+ +--+ +--+ +--+ +--+
    |  | |  | |4 | |  | |  |<|  | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
     ^    v         ^    
    +--+ +--+ +--+ +--+ +--+ +--+
    |  | |  |>|  | |6 | |  | |5 | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
          ^                   v
    +--+ +--+ +--+ +--+ +--+ +--+
    |  | |  |<|  | |  |>|  | |  | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
    
    +--+ +--+ +--+ +--+ +--+ +--+
    |  |<|  | |  | |  | |  | |  | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
                    ^   
    +--+ +--+ +--+ +--+ +--+ +--+
    |  | |  | |  |>|  | |2 | |  | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
                    ^   
    +--+ +--+ +--+ +--+ +--+ +--+
    |  | |  |>|  | |4 | |3 | |  | 
    |  | |  | |  | |  | |  | |  | 
    +--+ +--+ +--+ +--+ +--+ +--+
*/


/*
Sample run

Assignment
  |[00,00,10,50,0C,20]
  |
  | .. .. 4. .. ..<.. [10]
  | 00 01 02 03 04 05
  | ^  v     ^
  | .. ..>.. 6. .. 5. [60]
  | 06 07 08 09 10 11
  |    ^           v
  | .. ..<.. ..>.. .. [00]
  | 12 13 14 15 16 17
  |
  | ..<.. .. .. .. .. [00]
  | 18 19 20 21 22 23
  |          ^
  | .. .. ..>.. 2. .. [04]
  | 24 25 26 27 28 29
  |          ^
  | .. ..>.. 4. 3. .. [18]
  | 30 31 32 33 34 35
Solution 1
  | 1. 3. 4. 2. 5.<6.
  | ^  v     ^
  | 3. 2.>1. 6. 4. 5.
  |    ^           v
  | 2. 4.<6. 5.>1. 3.
  |
  | 4.<5. 3. 1. 6. 2.
  |          ^
  | 6. 1. 5.>3. 2. 4.
  |          ^
  | 5. 6.>2. 4. 3. 1.
done (1)

*/


#include <stdio.h>
#include <stdbool.h>


/* 
Implementation

We number the rows and columns 0..ORDER-1. We number the cells 
0..ORDER*ORDER-1. We have a global variable board that holds the cells, 
i.e. it contains the symbols 1..ORDER. To make it easier to test if
we can place a symbol in a row or column, we have two arrays
rowsyms[ORDER], and colsyms[ORDER] that store a mask: bit i is set if
symbol i occurs in that row/column. Finally, we have 4 boolean arrays 
for the constraints. We associate the constraints with the most right-lower 
cell. The 4 arrays have the same size as the board.
 - bool lessthan_northneighbor[]
 - bool greaterthan_northneighbor[]
 - bool lessthan_westneighbor[]
 - bool greaterthan_westneighbor[]

The above puzzle is then numbered as follows
            +--+ +--+ +--+ +--+ +--+ +--+
board[ 2]=4 |  | |  | |4 | |  | |  |<|  | greaterthan_westneighbor[5]
            |00| |01| |02| |03| |04| |05| 
            +--+ +--+ +--+ +--+ +--+ +--+
             ^    v         ^    
            +--+ +--+ +--+ +--+ +--+ +--+ greaterthan_northneighbor[6]
board[ 9]=6 |  | |  |>|  | |6 | |  | |5 | lessthan_northneighbor[7]
board[11]=5 |06| |07| |08| |09| |10| |11| lessthan_westneighbor[8]
            +--+ +--+ +--+ +--+ +--+ +--+ greaterthan_northneighbor[9]
                  ^                   v
            +--+ +--+ +--+ +--+ +--+ +--+ greaterthan_northneighbor[13]
            |  | |  |<|  | |  |>|  | |  | greaterthan_westneighbor[14]
            |12| |13| |14| |15| |16| |17| lessthan_westneighbor[16] 
            +--+ +--+ +--+ +--+ +--+ +--+ lessthan_northneighbor[17]
            
            +--+ +--+ +--+ +--+ +--+ +--+
            |  |<|  | |  | |  | |  | |  | greaterthan_westneighbor[19]
            |18| |19| |20| |21| |22| |23| 
            +--+ +--+ +--+ +--+ +--+ +--+
                            ^   
            +--+ +--+ +--+ +--+ +--+ +--+
board[28]=2 |  | |  | |  |>|  | |2 | |  | greaterthan_northneighbor[27]
            |24| |25| |26| |27| |28| |29| lessthan_westneighbor[27]
            +--+ +--+ +--+ +--+ +--+ +--+
                            ^   
            +--+ +--+ +--+ +--+ +--+ +--+
board[33]=4 |  | |  |>|  | |4 | |3 | |  | lessthan_westneighbor[32]
board[34]=3 |30| |31| |32| |33| |34| |35| greaterthan_northneighbor[33]
            +--+ +--+ +--+ +--+ +--+ +--+
*/


#define ORDER 6
#define NUMCELLS (ORDER*ORDER)

int board[NUMCELLS+1]; // added sentinel
int rowsyms[ORDER];
int colsyms[ORDER];
bool lessthan_northneighbor[NUMCELLS];
bool greaterthan_northneighbor[NUMCELLS];
bool lessthan_westneighbor[NUMCELLS];
bool greaterthan_westneighbor[NUMCELLS];


// Setup some puzzle
void setup() {
  board[ 2]=4; 
  board[ 9]=6; 
  board[11]=5; 
  board[28]=2; 
  board[33]=4; 
  board[34]=3; 
  greaterthan_westneighbor[5]= true;
  greaterthan_northneighbor[6]= true;
  lessthan_northneighbor[7]= true;
  lessthan_westneighbor[8]= true;
  greaterthan_northneighbor[9]= true;
  greaterthan_northneighbor[13]= true;
  greaterthan_westneighbor[14]= true;
  lessthan_westneighbor[16] = true;
  lessthan_northneighbor[17]= true;
  greaterthan_westneighbor[19]= true;
  greaterthan_northneighbor[27]= true;
  lessthan_westneighbor[27]= true;
  lessthan_westneighbor[32]= true;
  greaterthan_northneighbor[33]= true;
  
}


// Printing board, with constraints (but without cell borders)
void print(bool debug) {
  // The column symbols
  if( debug ) {
    printf("  |"); // indent
    for(int x=0; x<ORDER; x++ ) {
      char sep= x==0 ? '[' : ',';
      printf("%c",sep);
      // value: symbol
      printf("%02X",colsyms[x]); 
    }
    printf("]\n");
  }
  
  for(int y=0; y<ORDER; y++ ) {
    
    // The north constraints
    if( y>0 || debug ) {
      printf("  |"); // indent
      for(int x=0; x<ORDER; x++ ) {
        int i= x+y*ORDER;
        // col separator: space (1 char)
        char sep=' ';
        printf("%c",sep);
        // value: space or constraint
        char val=' ';
        if( lessthan_northneighbor[i] ) val='v';
        if( greaterthan_northneighbor[i] ) val='^';
        if( lessthan_northneighbor[i] && greaterthan_northneighbor[i] ) val='*';
        printf("%c ",val); 
      }
      printf("\n");
    }
    
    // The cell symbols
    printf("  |"); // indent
    for(int x=0; x<ORDER; x++ ) {
      int i= x+y*ORDER;
      // col separator: space or constraint (1 char)
      char sep=' ';
      if( lessthan_westneighbor[i] ) sep='>';
      if( greaterthan_westneighbor[i] ) sep='<';
      if( lessthan_westneighbor[i] && greaterthan_westneighbor[i] ) sep='*';
      printf("%c",sep); // one char between cells
      // value: symbol on board (2 chars)
      char sym='.';
      if( board[i]>0 ) sym=board[i]+'0';
      printf("%c.",sym); 
    }
    if( debug ) printf(" [%02X]",rowsyms[y]); // row symbols
    printf("\n");

    // The cell indices
    if( debug ) {
      printf("  |"); // indent
      for(int x=0; x<ORDER; x++ ) {
        int i= x+y*ORDER;
        // col separator: space (1 char)
        char sep=' ';
        printf("%c",sep);
        // value: cell index (2 chars)
        printf("%02d",i);
      }
      printf("\n");
    }
  }
}


// Init the rowsyms[] and colsyms[]
void init() {
  for( int cellindex=0; cellindex<NUMCELLS; cellindex++ ) {
    int x= cellindex % ORDER;
    int y= cellindex / ORDER;
    if( board[cellindex]!=0 ) {
      rowsyms[y] |= 1 << board[cellindex];
      colsyms[x] |= 1 << board[cellindex];
    }
  }
  printf("Assignment\n"); print(true);
}


// Print all solutions
int solutions;
int printindex;
void solve(int cellindex) {
  if( cellindex==NUMCELLS ) { printf("Solution %d\n",++solutions); print(false); return; }
  
  int x= cellindex % ORDER;
  int y= cellindex / ORDER;

  for( int sym=1; sym<=ORDER; sym++ ) {
    if( board[cellindex]!=0   ) continue;
    if( rowsyms[y] & (1<<sym) ) continue;
    if( colsyms[x] & (1<<sym) ) continue;

    if( lessthan_northneighbor[cellindex]    && !(sym<board[cellindex-ORDER]) ) continue;
    if( greaterthan_northneighbor[cellindex] && !(sym>board[cellindex-ORDER]) ) continue;
    if( lessthan_westneighbor[cellindex]     && !(sym<board[cellindex-1    ]) ) continue;
    if( greaterthan_westneighbor[cellindex]  && !(sym>board[cellindex-1    ]) ) continue;
    
    // place
    board[cellindex]= sym;
    rowsyms[y] |= 1<<sym;
    colsyms[x] |= 1<<sym;

    bool violation= false;    
    int nextcellindex= cellindex;
    for(;;) {
      nextcellindex++;
      if( board[nextcellindex]==0 ) break;
      // We are at a pre-filled field. We must skip that.
      // Since we only check constraints "backwards" (to west and to north), we must perform those checks now.
      if( lessthan_northneighbor[nextcellindex]    && !(board[nextcellindex]<board[nextcellindex-ORDER]) ) { violation=true; break; };
      if( greaterthan_northneighbor[nextcellindex] && !(board[nextcellindex]>board[nextcellindex-ORDER]) ) { violation=true; break; };
      if( lessthan_westneighbor[nextcellindex]     && !(board[nextcellindex]<board[nextcellindex-1    ]) ) { violation=true; break; };
      if( greaterthan_westneighbor[nextcellindex]  && !(board[nextcellindex]>board[nextcellindex-1    ]) ) { violation=true; break; };
    }
    
    if( !violation) solve(nextcellindex);
    
    // remove
    board[cellindex]= 0;
    rowsyms[y] &= ~( 1<<sym );
    colsyms[x] &= ~( 1<<sym );
  }
  
}


void main() {
  setup();  // Setup puzzle
  init();   // Init vars
  solve(0); // Print all solutions
  printf("done (%d)\n",solutions);  
}

