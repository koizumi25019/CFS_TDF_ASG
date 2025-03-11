module s27 (G0, G1, G2, G3, CLK
    , G17) ;

input   G0, G1, G2, G3, CLK ;

output  G17 ;

INV     gate0  (.A(G11), .Z(G17) ) ;
NOR2    gate1  (.A(G14), .B(G11), .Z(G10) ) ;
DFF     gate2  (.D(G10), .CP(CLK), .Q(G5) ) ;
NOR2    gate3  (.A(G5), .B(G9), .Z(G11) ) ;
DFF     gate4  (.D(G11), .CP(CLK), .Q(G6) ) ;
NOR2    gate5  (.A(G2), .B(G12), .Z(G13) ) ;
DFF     gate6  (.D(G13), .CP(CLK), .Q(G7) ) ;
INV     gate7  (.A(G0), .Z(G14) ) ;
AND2    gate8  (.A(G14), .B(G6), .Z(G8) ) ;
NOR2    gate9  (.A(G1), .B(G7), .Z(G12) ) ;
OR2     gate10  (.A(G12), .B(G8), .Z(G15) ) ;
OR2     gate11  (.A(G3), .B(G8), .Z(G16) ) ;
NAND2   gate12  (.A(G16), .B(G15), .Z(G9) ) ;

endmodule
