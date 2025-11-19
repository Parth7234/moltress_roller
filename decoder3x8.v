module decoder3x8(input en,a, b, c, output d0, d1, d2, d3, d4, d5, d6, d7);

    wire a_inv;
    nand_gate inv_a(a, a, a, a_inv);
    decoder d_low(en|a, b, c, d0, d1, d2, d3);
    decoder d_high(en|a_inv, b, c, d4, d5, d6, d7);

endmodule