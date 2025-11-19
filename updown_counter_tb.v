module updown_counter_tb;
    reg clk,reset,en,dir;
    wire[3:0] q;
    updown_counter uc(clk,reset,en,dir,q);

    initial begin
        clk=0;
        forever #5 clk=~clk;
    end

    initial begin
        $display("Time\tclk\treset\ten\tdir\tq[3]q[2]q[1]q[0]");
        $monitor("%2d\t%b\t%b\t%b\t%b\t%b%b%b%b",$time,clk,reset,en,dir,q[3],q[2],q[1],q[0]);

        en=1;reset=0;dir=1;
        #45 reset=1;
        #75 en=0;
        #95 en=1;
        #125 dir=0;
        #200 en=0;
    $finish;
    end
endmodule 
