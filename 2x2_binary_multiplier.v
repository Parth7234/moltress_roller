module binary_multiplier(input A0,A1,B0,B1 ,output M0,M1,M2,M3);

    and_gate ag1(A0,B0,M0);
    wire x,y;
    and_gate ag2(A1,B0,x);
    and_gate ag3(A0,B1,y);
    wire c;
    half_adder ha1(x,y,M1,c);
    wire z;
    and_gate ag4(B1,A1,z);
    half_adder ha2(z,c,M2,M3);

endmodule