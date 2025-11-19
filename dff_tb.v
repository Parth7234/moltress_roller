module dff_tb;
    reg clk,reset,d;
    wire q;

    dff dd(clk,reset,d,q);

    initial begin
        clk=0;
        forever #5 clk=~clk;
    end

    initial begin
        $display("Time\tclk\treset\td\tq");
        $monitor("%2d\t%b\t%b\t%b\t%b",$time,clk,reset,d,q);
        reset=1;d=0;
        #5 reset=0;d=1;
        #10 d=0;
        #10 d=1;
        #10 reset=1;
        #5 reset=0;
        #10 d=0;
        #10 $finish;
    end
endmodule