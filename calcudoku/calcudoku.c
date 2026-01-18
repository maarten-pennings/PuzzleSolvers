// calcudoku.c 
// gcc  calcudoku.c -O3 -o calcudoku.exe && calcudoku.exe
// Maarten Pennings December 2016

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/time.h>


// Groups - tailor this for each puzzle
#if 0 // Volkskrant 2016 dec 24, Sir Edmund, pagina 56
#define RANK          5

#define GROUPMAP      "ABBCC"\
                      "AADDC"\
                      "EFGDC"\
                      "HFIIJ"\
                      "HKKJJ"

#define GROUPDEFS     {'A', 1,'-'}, \
                      {'B', 3,'-'}, \
                      {'C',11,'+'}, \
                      {'D',10,'+'}, \
                      {'E', 1,'='}, \
                      {'F', 2,'-'}, \
                      {'G', 4,'='}, \
                      {'H', 8,'+'}, \
                      {'I', 1,'-'}, \
                      {'J', 0,'-'}, \
                      {'K', 2,'-'}
#endif

#if 0 // Volkskrant 2016 dec 24, Sir Edmund, pagina 56
#define RANK          6

#define GROUPMAP      "ABCCDD"\
                      "ABBEED"\
                      "FFGEHH"\
                      "FFFIHJ"\
                      "KFLIMJ"\
                      "KLLLMN"

#define GROUPDEFS     {'A',12,'*'}, \
                      {'B',13,'+'}, \
                      {'C', 5,'/'}, \
                      {'D', 2,'-'}, \
                      {'E',12,'+'}, \
                      {'F',17,'+'}, \
                      {'G', 4,'='}, \
                      {'H',10,'+'}, \
                      {'I', 2,'-'}, \
                      {'J',11,'+'}, \
                      {'K', 7,'+'}, \
                      {'L',16,'+'}, \
                      {'M', 3,'-'}, \
                      {'N', 4,'='}, 
#endif

#if 0 // Volkskrant 2017 jul 22, magazine pag 80
#define RANK          5

#define GROUPMAP      "ABBCD"\
                      "AEBBD"\
                      "FEGHH"\
                      "FIJHK"\
                      "LIJJK"
                      

#define GROUPDEFS     {'A', 3,'+'}, \
                      {'B',13,'+'}, \
                      {'C', 3,'='}, \
                      {'D', 1,'-'}, \
                      {'E', 5,'+'}, \
                      {'F', 1,'-'}, \
                      {'G', 5,'='}, \
                      {'H', 7,'+'}, \
                      {'I', 6,'+'}, \
                      {'J', 0,'-'}, \
                      {'K', 2,'-'}, \
                      {'L', 5,'='}, 
#endif

#if 1 // Volkskrant 2018 jul28, magazine pag 74
#define RANK          6

#define GROUPMAP      "AABCCC"\
                      "BBBBBC"\
                      "DDEEFF"\
                      "GDFFFH"\
                      "GDGIIH"\
                      "GGGJJH"
                      

#define GROUPDEFS     {'A', 4,'/'}, \
                      {'B',26,'+'}, \
                      {'C',11,'+'}, \
                      {'D',21,'+'}, \
                      {'E', 2,'*'}, \
                      {'F',21,'+'}, \
                      {'G',12,'+'}, \
                      {'H',13,'+'}, \
                      {'I', 3,'-'}, \
                      {'J',20,'*'}, 
#endif


#if 0 // https://www.calcudoku.org/nl/2018-07-30/9/3
#define RANK          9

#define GROUPMAP      "ABCCDDDEF"\
                      "ABGCHHIEF"\
                      "JBGGHHIIK"\
                      "JLMGNNNIK"\
                      "OMMPNQNRS"\
                      "TMUPVQWXX"\
                      "TYUZVWW[X"\
                      "\\Y]ZZ^^__"\
                      "\\`abZZcc_"
                      

#define GROUPDEFS     {'A',  2,'/'}, \
                      {'B', 12,'+'}, \
                      {'C', 12,'+'}, \
                      {'D',126,'*'}, \
                      {'E', 11,'+'}, \
                      {'F',  8,'/'}, \
                      {'G', 26,'+'}, \
                      {'H',  3,'-'}, \
                      {'I', 19,'+'}, \
                      {'J',  2,'-'}, \
                      {'K', 15,'+'}, \
                      {'L',  8,'='}, \
                      {'M', 54,'*'}, \
                      {'N', 17,'+'}, \
                      {'O',  1,'='}, \
                      {'P',  1,'-'}, \
                      {'Q', 15,'+'}, \
                      {'R',  7,'='}, \
                      {'S',  5,'='}, \
                      {'T', 11,'+'}, \
                      {'U',  2,'-'}, \
                      {'V', 11,'+'}, \
                      {'W', 12,'+'}, \
                      {'X',  9,'+'}, \
                      {'Y',  3,'/'}, \
                      {'Z',5760,'*'}, \
                      {'[',  8,'='}, \
                      {'\\', 11,'+'}, \
                      {']',  4,'='}, \
                      {'^', 18,'*'}, \
                      {'_',  3,'-'}, \
                      {'`',  7,'='}, \
                      {'a',  1,'='}, \
                      {'b',  2,'='}, \
                      {'c', 17,'+'}, 
#endif


// Temp storage for the group defs                      
struct groupdef { int name, result, op; } groupdefs[] = {GROUPDEFS};
#define GROUPCOUNT (sizeof(groupdefs)/sizeof(struct groupdef))

// Complete storage for the group defs
typedef struct {
    // public (defined by the exercise)
    char name;     // names of the group, e.g. 'A'
    int  result;   // required result when applying the operation to the group members (cell values)
    char op;       // operation specified for the group, e.g. '-'
    // private (needed internally)
    int  size;     // number of cells in the group
    int  cixs[99]; // the indexes of the cell belonging to the group [0..size).
} group_t;

group_t groups[GROUPCOUNT];

int board[RANK*RANK];
int fills[GROUPCOUNT]; // number of cells in the group that is already assigned
int count;

// Prints 'board' without borders
void print1(void) {
    for(int cix=0; cix<RANK*RANK; cix++) {
        if( cix%RANK==0 ) printf("  ");
        char groupname= GROUPMAP[cix];
        int gix= groupname-'A'; // We assume a one-on-one mapping from names to indexes
        //printf("%02d(%c~%02d)%d ",cix,groupname,gix,board[cix]);
        printf("%d  ",board[cix]);
        if( (cix+1)%RANK==0 ) printf("\n");
    }    
}

// Prints 'board' nicely in a grid with borders
void print2(void) {
    for( int row=0; row<RANK; row++) {
        printf("  ");
        for(int col=0; col<RANK; col++) {
            int cix= row*RANK+col;
            int gix= GROUPMAP[cix]-'A';
            char hsep = row==0 || GROUPMAP[cix-RANK]!=GROUPMAP[cix] ? '-' : ' ';
            int printed;
            if( groups[gix].cixs[0]==cix ) printed= printf("+[%d%c]",groups[gix].result,groups[gix].op); else printed=printf("+");
            for( int i=printed; i<=7; i++) printf("%c",hsep);
        }
        printf("+\n");
        printf("  ");
        for(int col=0; col<RANK; col++) {
            int cix= row*RANK+col;
            int gix= GROUPMAP[cix]-'A';
            char vsep= col==0 || GROUPMAP[cix-1]!=GROUPMAP[cix] ? '|' : ' ';
            printf("%c  %2d   ",vsep,board[cix]);
        }
        printf("|\n");
    
    }
    printf("  ");
    for(int i=0; i<RANK; i++) printf("+-------"); printf("+\n");
}

void group_init(void) {
    // Check rank
    int len = strlen(GROUPMAP);
    if( RANK*RANK!=len ) { printf("Fatal: GROUPMAP is not square; is %d, should be %dx%d\n",len,RANK,RANK); exit(1); }
    printf("Groups on board   name~gix(cix) name~gix(cix) name~gix(cix)\n");
    // Scan GROUPMAP
    int groupcount= 0;
    for(int cix=0; cix<RANK*RANK; cix++) {
        char groupname= GROUPMAP[cix];
        if( groupname<'A' ) { printf("Fatal: GROUPMAP must have letters, not '%c'\n",groupname); exit(1); }
        int gix= groupname-'A'; // We assume a one-on-one mapping from names to indexes
        if( cix%RANK==0 ) printf("  ");
        printf("%c~%02d(%02d) ",groupname,gix,cix);
        if( gix>groupcount ) { 
            printf("Fatal: GROUPMAP has gap in group names: '%c' at %d (expected %c)\n",groupname,cix, groupcount+'A'); 
            exit(1);
        } else if( gix==groupcount) {
            // First occurrence of a new group name
            // Is there a next groupdef?
            if( groupcount==GROUPCOUNT ) { printf("Fatal: GROUPMAP uses group '%c' at %d, but highest GROUPDEF is '%c'\n",groupname,cix,GROUPCOUNT+'A'-1); exit(1); }
            
            if( groupdefs[gix].name!=groupname ) { 
                // Is it the next up?
                printf("Fatal: GROUPDEF must have names in order: %d must be '%c' is '%c'\n",gix,groupname,groupdefs[gix].name); exit(1); 
            }
            // Init the group
            groups[gix].name= groupdefs[gix].name;
            groups[gix].result= groupdefs[gix].result;
            groups[gix].op= groupdefs[gix].op;
            groups[gix].size=0;
            groupcount++;
        }
        groups[gix].cixs[groups[gix].size]= cix; 
        groups[gix].size++;
        if( (cix+1)%RANK==0 ) printf("\n");
    }
    // Check groups
    printf("Group definitions   name~gix: result op size: cix cix cix\n");
    for( int gix=0; gix<GROUPCOUNT; gix++) {
        char name= groups[gix].name;
        char op= groups[gix].op;
        int  result= groups[gix].result;
        int  size= groups[gix].size;
        if( size<=0 )  { 
            printf("Fatal: group '%c' has funny size %d\n",name,size); exit(1); 
        } else if( size==1 ) {
            if( op!='=' ) { printf("Fatal: group '%c' has size 1, but op is not '=' but '%c'\n",name,op); exit(1); }
        } else {
            if( op!='+' && op!='-' && op!='*' && op!='/' && op!='?' ) { printf("Fatal: group '%c' has funny op '%c'\n",name,op); exit(1); }
        }
        printf("  %c~%02d: %5d%c %d:",name,gix,result,op,size);
        for(int i=0; i<size; i++) printf("%3d",groups[gix].cixs[i]);
        printf("\n");
    }
    // Print empty board
    printf("Empty board\n");
    for(int cix=0; cix<RANK*RANK; cix++ ) board[cix]=cix; // put cell numbers, nice for printout
    print2();
}

bool safe(int cix, int val) {
    board[cix]= val;
    count++;
    //bool trace= board[ 0]==4 && board[ 1]==1 && board[ 2]==6 && board[ 3]==2 && board[ 4]==5 && board[ 5]==3 ;
    //if( trace ) printf("debug %d:%d\n",cix,val);
    //if( trace ) print1();
    // Row safe - or does the row already contain a `val`?
    int row= cix/RANK;
    for(int i=row*RANK; i<cix; i++ )
        if( board[i]==val ) { 
            //if( trace ) printf("Row not safe\n");
            return false; 
        }
    // Col safe - or does the col already contain a `val`?
    int col= cix%RANK;
    for(int i=col; i<cix; i+=RANK )
        if( board[i]==val ) { 
            //if( trace ) printf("Col not safe\n");
            return false; 
        }
    // Group safe - or does the group sum differ from the result (or, for incomplete groups, already exceed the result)
    int  gix= GROUPMAP[cix]-'A'; 
    char op= groups[gix].op;
    int  result= groups[gix].result;
    int  size= groups[gix].size;
    int  fill= fills[gix]+1;
    if( op=='+') {
        int sum=0;
        for( int i=0; i<fill; i++) { int v=board[groups[gix].cixs[i]]; sum+=v; }
        //if( trace ) printf("fill %d, size %d, sum %d, result %d\n",fill,size,sum,result);
        if( fill==size ) return sum==result; else return sum<result; 
    } else if( op=='-') {
        int sum=0,max=0;
        for( int i=0; i<fill; i++) { int v=board[groups[gix].cixs[i]]; sum+=v; if(v>max) max=v; }
        int r= max+max-sum;
        //if( trace ) printf("fill %d, size %d, sum %d, max %d, r %d, result %d\n",fill,size,sum,max,r,result);
        if( fill==size ) return r==result; else return true;       
    } else if( op=='*') {
        int prod=1;
        for( int i=0; i<fill; i++) { int v=board[groups[gix].cixs[i]]; prod*=v; }
        //if( trace ) printf("fill %d, size %d, prod %d, result %d\n",fill,size,prod,result);
        if( fill==size ) return prod==result; else return prod<=result;        
    } else if( op=='/') {
        int prod=1,max=0;
        for( int i=0; i<fill; i++) { int v=board[groups[gix].cixs[i]]; prod*=v; if(v>max) max=v; }
        int r= max*max/prod;
        //if( trace ) printf("fill %d, size %d, prod %d, max %d, r %d, result %d\n",fill,size,prod,max,r,result);
        if( fill==size ) return r==result; else return true;               
    } else if( op=='=') {
        // Singleton
        return result==val;
    } else if( op=='?') {
        // Wildcard
        return true;
    } else {
        printf("Should not happen\n");
        return false;
    }
}

void move(int cix, int val) {
    int gix= GROUPMAP[cix]-'A'; 
    fills[gix]++; // add value to group
    board[cix]= val;
}

void unmove(int cix, int val) {
    int gix= GROUPMAP[cix]-'A'; 
    fills[gix]--; // remove value from group
}

int solution_count;
void solve(int cix) {
    // Generate all boards, keeping [0..cix) as is, varying [cix..RANK*RANK).
    // Print them if they match the rules
    if( cix==RANK*RANK ) {
        printf("\nSolution %d\n", solution_count);    
        print2();
        solution_count++;
    } else {
        for(int val=1; val<=RANK; val++) {
            if( safe(cix,val) ) {
                move(cix,val);
                solve(cix+1);
                unmove(cix,val);
            }
        }
    }
}

void main(void) {
    struct timeval t1, t2;
    double delta;

    group_init();

    gettimeofday(&t1, NULL);
    solve(0);
    gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    delta = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    delta += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("\nFound\n  %d solutions in %f ms (%d calls)\n", solution_count, delta, count);
}


