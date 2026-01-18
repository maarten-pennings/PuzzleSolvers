// Knight.js
// 2013-07-16  v2  Maarten Pennings  Added Warnsdorff's rule (http://en.wikipedia.org/wiki/Knight's_tour)
// 2012-07-13  v1  Maarten Pennings  Created


function BoardToTable( size, content ) {
  var table="";
  table+= "<table cellspacing='0' cellpadding='4'>\n";
  for( var row=size-1; row>=0; row-- ) {
    table+= "  <tr>" ;
    table+= "<td>" + (row+1) + "</td>" ;
    for( var col=0; col<size; col++ ) {
      var piece= content(col, row);
      var color= (row+col)%2==0 ? "gray" : "white";
      table+= "<td width='20px' align='center' bgcolor='" + color + "'>" + piece + "</td>" ;
    }
    table+= "</tr>\n" ;
  }
  table+= "  <tr><td></td>" ;
  for( var col=0; col<size; col++ )
    table+= "<td width='20px' align='center' >" + "ABCDEFGH".charAt(col) + "</td>";
  table+= "</tr>\n" ;
  table+= "</table>\n" ;
  return table;
}


function Print( index, msg, board ) {
  var content= function(c,r) { return board.cells[ board.Pos(c,r) ]; };
  var table= BoardToTable( board.size, content );
  document.write( "<h3>Solution " + index + " (" + msg + ")</h3>" );
  document.write( table );
  // Abort
  var abort=1
  //if( index==abort ) throw "Aborting search after finding "+abort+" solutions";
}


function Board( size ) {
  this.size= size;
  this.line= 2+size; // Reserve two blank cells between rows
  this.offset= 2 * this.line + 2; // Reserve two lines and two blank cells before board
  this.num= size*size;
  this.cells = new Array( );
  this.Pos= function (x,y) { // Map two dimensions (x,y) to single array index
    return y*this.line + x + this.offset;
  }
  for( var y=0; y<size; y++ ) for( var x=0; x<size; x++ ) this.cells[ this.Pos(x,y) ]= 0;

  this.moves= new Array( );
  this.moves[0]=  +2 +1*this.line;
  this.moves[1]=  +1 +2*this.line;
  this.moves[2]=  -1 +2*this.line;
  this.moves[3]=  -2 +1*this.line;
  this.moves[4]=  -2 -1*this.line;
  this.moves[5]=  -1 -2*this.line;
  this.moves[6]=  +1 -2*this.line;
  this.moves[7]=  +2 -1*this.line;

  this.solutions= 0;
  this.lfl= this.num;
  this.NumMoves= function ( p ) {
    var nm=0;
    for( var i=0; i<8; i++ ) {
      var q= p + this.moves[i];
      if( this.cells[q]==0 ) nm++;
    }
    return nm;
  }
  this.Solve= function ( p,n ) {
    this.cells[p]= n; // make move
    if( n>=this.num ) {
      this.solutions++;
      var lfl= this.num;
      for( var i=0; i<8; i++ ) {
        var q= p + this.moves[ i ];
        if( this.cells[q]>0 && this.cells[q]<lfl ) lfl= this.cells[q];
      }
      if( lfl<this.lfl ) this.lfl= lfl;
      Print( this.solutions, "lowest-from-last="+lfl, this );
    } else {
      n++;
      // Wandorff's rule: proceed to the square from which knight has the fewest onward moves.
      //for( var nm=1; nm<9; nm++ ) {
      //  for( var i=0; i<8; i++ ) {
      //    var q= p + this.moves[i];
      //    if( this.cells[q]==0 && this.NumMoves(q)==nm )
      //      this.Solve( q,n );
      //  }
      //}

      for( var i=0; i<8; i++ ) {
        var q= p + this.moves[i];
        if( this.cells[q]==0 ) this.Solve( q,n );
      }
    }
    this.cells[p]= 0; // erase move
  }
}

var b= new Board(5);
b.Solve( b.Pos(0,0), 1 );
document.write( "Found " + b. solutions + " solutions, lfl is " + b.lfl );

