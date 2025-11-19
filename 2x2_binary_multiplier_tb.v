module binary_multiplier_tb;
    reg A0,A1,B0,B1;
    wire M0,M1,M2,M3;
    integer i;

    binary_multiplier bm(A0,A1,B0,B1,M0,M1,M2,M3);

    initial begin

        $dumpfile("2x2_binary_multiplier_output.vcd");
        $dumpvars(0,binary_multiplier_tb);

        $monitor("Time=%2d | A1A0=%b%b B1B0=%b%b | S3S2S1S0=%b%b%b%b",$time,A1,A0,B1,B0,M3,M2,M1,M0);

        for(i=0;i<16;i=i+1)begin
            {A0,A1,B0,B1}=i;
            #10;
        end

        $finish;
    end
endmodule

