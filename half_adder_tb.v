module half_adder_tb;
    reg A,B;
    wire S,C;
    half_adder halfAdder(.A(A),.B(B),.S(S),.C(C));

    initial begin
        $dumpfile("half_adder_output.vcd");
        $dumpvars(0,half_adder_tb);
        $monitor("Time=%2d | A=%b B=%b | S=%b C=%b",$time,A,B,S,C);

        A=0;B=0;
        #10
        A=0;B=1;
        #10
        A=1;B=0;
        #10
        A=1;B=1;

        $finish;
    end
endmodule
