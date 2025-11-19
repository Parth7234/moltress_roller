module decoder_tb;
    reg en,a,b;
    wire d0,d1,d2,d3;
    integer i;

    decoder d(en,a,b,d0,d1,d2,d3);

    initial begin
        $dumpfile("decoder_output.vcd");
        $dumpvars(0,decoder_tb);

        $monitor("Time=%2d | En=%b A=%b B=%b | D3=%b D2=%b D1=%b D0=%b",$time,en,a,b,d3,d2,d1,d0);

        for(i=0;i<8;i=i+1)begin
            {en,a,b}=i;
            #10;
        end

        $finish;
    end
endmodule
        