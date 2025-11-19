module and_gate_tb;
    reg A,B;
    wire O;
    and_gate andGate(.A(A),.B(B),.O(O));

    initial begin
        $dumpfile("and_gate_output.vcd");
        $dumpvars(0,and_gate_tb);
        $monitor("Time=%2d | A=%b B=%b | O=%b",$time,A,B,O);
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