// solve1567.c - Make an expression with 1, 5, 6, and 7 and +, -, *, and / and see if you can form 21
// gcc  solve1567.c  -o solve1567.exe  &&  solve1567.exe > solve1567.log

// 2020 April 6, Maarten Pennings


#include <stdio.h>


struct frac {
  int num;
  int den;
};


char op[]={'+', '-', '*', '/' };
char bo[]={' ','('};
char bc[]={' ',')'};


// Make a fraction from an int
struct frac num( int n ) {
  struct frac r;
  r.num= n;
  r.den= 1;
  return r;
}


// Combine two fractions using operator op (0=+, 1=-, 2=*, 3=/)
struct frac calc(struct frac f1, int op, struct frac f2 ) {
  struct frac r;
  switch( op ) {
    case 0 /* + */: r.num= f1.num*f2.den + f2.num*f1.den; r.den= f1.den*f2.den; break;
    case 1 /* - */: r.num= f1.num*f2.den - f2.num*f1.den; r.den= f1.den*f2.den; break;
    case 2 /* * */: r.num= f1.num*f2.num                ; r.den= f1.den*f2.den; break;
    case 3 /* / */: r.num= f1.num*f2.den                ; r.den= f1.den*f2.num; break;
    default : printf("Error %d\n",op); break;
  }
  return r;
}


// Simplify fraction 
struct frac simplify(struct frac f) {
  if( f.den==0 ) return f; // Illegal fraction: denominator is 0
  if( f.num==0 ) { f.den=1; return f; } // Number 0, normalized
  // Find GCD of numerator and denominator
  int a= f.num;
  int b= f.den;
  // Make a and b positive
  if( a<0 ) a= -a;
  if( b<0 ) b= -b;  
  // Find GCD
  while( a!=b ) {
    if( a>b ) a= a-b;
    else if( b>a ) b= b-a;
  }
  // Divide numerator and denominator by GCD
  f.num /= a;
  f.den /= a;
  // Move -1 from denominator to numerator
  if( f.den<0 ) { f.num*=-1; f.den*=-1; } 
  return f;
}


// Test if two fractions are equal
int equal( struct frac f1, struct frac f2 ) {
  return f1.num*f2.den == f1.den*f2.num;
}


// Print fraction (plain and simplified)
void fracprint( struct frac f) {
  printf("%d/%d",f.num,f.den);
  printf(" = ");
  struct frac r= simplify(f);
  if( r.den==0 ) printf("NAN");
  else if( r.den==1 ) printf("%d",r.num);
  else printf("%d/%d",r.num,r.den);
}


// Bracket example
//  498: 1-  5- 6 -7    // b1=0  b2=0  b3=0
//  506: 1-  5-(6 -7)   // b1=0  b2=0  b3=1
//  682: 1- (5- 6)-7    // b1=0  b2=1  b3=0
//  xxx: 1- (5-(6)-7)   // b1=0  b2=1  b3=1 NOT ALLOWED
//  786: 1-( 5- 6 -7 )  // b1=1  b2=0  b3=0
//  794: 1-( 5-(6 -7))  // b1=1  b2=0  b3=1
//  xxx: 1-((5- 6)-7 )  // b1=1  b2=1  b3=0 same as 786
//  xxx: 1-((5-(6)-7))  // b1=1  b2=1  b3=1 NOT ALLOWED


void main() {
  struct frac s1, s2, r;
  int n=0;
  for(int n1=1; n1<=7; n1++ ) {
    if( n1==2 || n1==3 || n1==4 ) continue;
    for( int o1=0; o1<=3; o1++ ) {
      for( int b1=0; b1<=1; b1++ ) {
        for( int b2=0; b2<=1; b2++ ) {
          if( b1==1 && b2==1 ) continue;
          for(int n2=1; n2<=7; n2++ ) {
            if( n2==2 || n2==3 || n2==4 || n2==n1 ) continue;
            for( int o2=0; o2<=3; o2++ ) {
              for( int b3=0; b3<=1; b3++ ) {
                if( b2==1 && b3==1 ) continue;
                for(int n3=1; n3<=7; n3++ ) {
                  if( n3==2 || n3==3 || n3==4 || n3==n1 || n3==n2 ) continue;
                  for( int o3=0; o3<=3; o3++ ) {
                    for(int n4=1; n4<=7; n4++ ) {
                      if( n4==2 || n4==3 || n4==4 || n4==n1 || n4==n2 || n4==n3 ) continue;
                        n++;
                        printf("%5d: ",n);
                        printf("%d",n1);
                        printf("%c",op[o1]);
                        printf("%c",bo[b1]);
                        printf("%c",bo[b2]);
                        printf("%d",n2);
                        printf("%c",op[o2]);
                        printf("%c",bo[b3]);
                        printf("%d",n3);
                        printf("%c",bc[b2]);
                        printf("%c",op[o3]);
                        printf("%d",n4);
                        printf("%c",bc[b3]);
                        printf("%c",bc[b1]);
                        //  498: 1-  5- 6 -7    // b1=0  b2=0  b3=0
                        //  506: 1-  5-(6 -7)   // b1=0  b2=0  b3=1
                        //  682: 1- (5- 6)-7    // b1=0  b2=1  b3=0
                        //  xxx: 1- (5-(6)-7)   // b1=0  b2=1  b3=1 NOT ALLOWED
                        //  786: 1-( 5- 6 -7 )  // b1=1  b2=0  b3=0
                        //  794: 1-( 5-(6 -7))  // b1=1  b2=0  b3=1
                        //  xxx: 1-((5- 6)-7 )  // b1=1  b2=1  b3=0 same as 786
                        //  xxx: 1-((5-(6)-7))  // b1=1  b2=1  b3=1 NOT ALLOWED
                        if( b1==0 ) {
                          //  498: 1-  5- 6 -7    // b1=0  b2=0  b3=0
                          //  506: 1-  5-(6 -7)   // b1=0  b2=0  b3=1
                          //  682: 1- (5- 6)-7    // b1=0  b2=1  b3=0
                          if( b3==1 ) {
                            //  506: 1-  5-(6 -7)   // b1=0  b2=0  b3=1
                            s1= calc(num(n3),o3,num(n4));
                            s2= calc(num(n1),o1,num(n2));
                            r= calc(s2,o2,s1);                            
                          } else if( b2==1 ) {
                            //  682: 1- (5- 6)-7    // b1=0  b2=1  b3=0
                            s1= calc(num(n2),o2,num(n3));
                            s2= calc(num(n1),o1,s1);
                            r= calc(s2,o3,num(n4));
                          } else {
                            //  498: 1-  5- 6 -7    // b1=0  b2=0  b3=0
                            s1= calc(num(n1),o1,num(n2));
                            s2= calc(s1,o2,num(n3));
                            r= calc(s2,o3,num(n4));
                          }
                        } else {
                          //  786: 1-( 5- 6 -7 )  // b1=1  b2=0  b3=0
                          //  794: 1-( 5-(6 -7))  // b1=1  b2=0  b3=1
                          if( b3==1 ) {
                            //  794: 1-( 5-(6 -7))  // b1=1  b2=0  b3=1
                            s1= calc(num(n3),o3,num(n4));
                            s2= calc(num(n2),o2,s1);
                            r= calc(num(n1),o1,s2);
                          } else {
                            //  786: 1-( 5- 6 -7 )  // b1=1  b2=0  b3=0
                            s1= calc(num(n2),o2,num(n3));
                            s2= calc(s1,o3,num(n4));
                            r= calc(num(n1),o1,s2);
                          }
                        }
                        printf(" = ");
                        fracprint(r);
                        if( equal(r,num(21)) ) printf(" <<== EQUAL" );
                        printf("\n");
                    } // n4
                  } // o3
                } //n3
              } // b3
            } // o2
          } // n2 
        } // b2
      } // b1
    } // o1
  } //n1

}

