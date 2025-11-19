module updown_counter(input clk,reset,en,dir,output [3:0] q);
    wire in0,in1,in2,in3;
    assign in0=1'b1&en;
    assign in1=((dir&q[0])|(~dir&(~q[0])))&en;
    assign in2=((dir&(q[0]&q[1]))|(~dir&(~q[0]&~q[1])))&en;
    assign in3=((dir&(q[0]&q[1]&q[2]))|(~dir&(~q[0]&~q[1]&~q[2])))&en;

    tff t0(clk,reset,in0,q[0]);
    tff t1(clk,reset,in1,q[1]);
    tff t2(clk,reset,in2,q[2]);
    tff t3(clk,reset,in3,q[3]);

endmodule
