module tff_tb;
    reg clk,reset,t;
    wire q;

    tff dd(clk,reset,t,q);

    initial begin
        clk=0;
        forever #5 clk=~clk;
    end

    initial begin
        $display("Time\tclk\treset\tt\tq");
        $monitor("%2d\t%b\t%b\t%b\t%b",$time,clk,reset,t,q);
        reset=1;t=0;
        #5 reset=0;t=1;
        #10 t=0;
        #10 t=1;
        #10 reset=1;
        #5 reset=0;
        #10 t=0;
        #10 $finish;
    end
endmodule