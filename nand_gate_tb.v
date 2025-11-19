module nand_gate_tb;
    reg a,b,c;
    wire o;
    integer i;

    nand_gate ng(a,b,c,o);

    initial begin
        $dumpfile("nand_gate_output.vcd");
        $dumpvars(0,nand_gate_tb);

        $monitor("Time=%2d | A=%b B=%b C=%b | O=%b",$time,a,b,c,o);

        for(i=0;i<8;i=i+1)begin
            {a,b,c}=i;
            #10;
        end

        $finish;
    end
endmodule