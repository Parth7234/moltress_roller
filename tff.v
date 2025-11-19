module tff(input clk,reset,t, output q);
    wire in;
    assign in=q^t;
    dff d(clk,reset,in,q);
endmodule