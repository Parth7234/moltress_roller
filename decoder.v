module decoder(input en,a,b, output d0,d1,d2,d3);
    wire a_inv,b_inv,en_inv;
    nand_gate ng1(a,a,a,a_inv);
    nand_gate ng2(b,b,b,b_inv);
    nand_gate ng3(en,en,en,en_inv);

    nand_gate ng4(a,b,en_inv,d3);
    nand_gate ng5(a,b_inv,en_inv,d2);
    nand_gate ng6(a_inv,b,en_inv,d1);
    nand_gate ng7(a_inv,b_inv,en_inv,d0);

endmodule