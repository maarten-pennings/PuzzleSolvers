// queens.js - 8-queens problem - partially in Dutch
// 2012-07-11 Maarten Pennings


function BordNaarTable( hok ) {
  // Begin table
  var table="";
  table+= "<table cellspacing='0' cellpadding='4'>\n";
  // Teken rij 8 t/m 1 (het label en 8 hokjes)
  for( var rij=7; rij>=0; rij-- ) {
    table+= "  <tr>" ;
    table+= "<td>" + (rij+1) + "</td>" ;
    for( var kolom=0; kolom<8; kolom++ ) {
      var stuk= hok(kolom, rij );
      var kleur= (rij+kolom)%2==1 ? "gray" : "white";
      table+= "<td width='20px' align='center' bgcolor='" + kleur + "'>" + stuk + "</td>" ;
    }
    table+= "</tr>\n" ;
  } 
  // Teken rij met labels A t/m H
  table+= "  <tr><td></td>" ;
  for( var kolom=0; kolom<8; kolom++ )
    table+= "<td width='20px' align='center' >" + "ABCDEFGH".charAt(kolom) + "</td>";
  table+= "</tr>\n" ;
  // Einde table
  table+= "</table>\n" ;
  return table;
}


var oplossingen= 0;


function PrintBord( bord ) {
  oplossingen++;
  var hok= function(k,r){ return bord[k]==r ? "<b>Q</b>" : " "; };
  var table= BordNaarTable( hok );
  document.write( "<h3>Solutions " + oplossingen + "</h3>" );
  document.write( table );
}


function Past( bord, kolom, rij ) {
  for( var k=0; k<kolom; k++ ) {
    var r= bord[k];
    // Is de koningin op kolom,rij veilig voor die op k,r?
    // De kolommen kunnen nooit gelijk zijn, maar de rijen?
    if( r==rij ) return false;
    // De dalende diagonalen zijn genummerd van 0 t/m 14. 
    // Hok r,k ligt op diagonaal k+r. Ligt rij,kolom op dezelfde?
    if( k+r==kolom+rij ) return false;
    // De stijgende diagonalen zijn genummerd van -7 t/m +7. 
    // Hok r,k ligt op diagonaal k-r. Ligt rij,kolom op dezelfde?
    if( k-r==kolom-rij ) return false;
  }
  return true;
}


function Solve( bord, aantal ) {
  if( aantal==8 ) {
    PrintBord( bord );
  } else {
    var kolom= aantal;
    for( var rij=0; rij<8; rij++ ) {
      if( Past( bord, kolom, rij) ) {
        bord[kolom]= rij;
        Solve(bord,aantal+1);
      }
    }
  }
}


function PrintHelp( ) {
  document.write( "<h2>Help</h2>" );
  var labels = BordNaarTable( function(k,r){ return "ABCDEFGH".charAt(k)+(r+1); } );
  var diag1= BordNaarTable( function(k,r){ return k+r; } );
  var diag2= BordNaarTable( function(k,r){ return k-r; } );
  document.write( "<h3>Labels</h3>" );
  document.write( labels );
  document.write( "<h3>Diagonal c+r</h3>" );
  document.write( diag1 );
  document.write( "<h3>Diagonal c-r</h3>" );
  document.write( diag2 );
}


function PrintSolutions( ) {
  document.write( "<h2>Solutions</h2>" );
  var bord= new Array;
  Solve( bord, 0 );
}


PrintHelp( );
PrintSolutions( );

