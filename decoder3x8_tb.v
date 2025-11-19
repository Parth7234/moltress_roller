module decoder3x8_tb;
    reg en,a,b,c;
    wire d0,d1,d2,d3,d4,d5,d6,d7;
    integer i;

    decoder3x8 d(en,a,b,c,d0,d1,d2,d3,d4,d5,d6,d7);

    initial begin
        $dumpfile("decoder3x8_output.vcd");
        $dumpvars(0,decoder3x8_tb);

        $monitor("Time=%2d | En=%d A=%d B=%d C=%d | D7=%d D6=%d D5=%d D4=%d D3=%d D2=%d D1=%d D0=%d",$time,en,a,b,c,d7,d6,d5,d4,d3,d2,d1,d0);

        for(i=0;i<16;i=i+1)begin
            {en,a,b,c}=i;
            #10;
        end
    $finish;

    end
endmodule