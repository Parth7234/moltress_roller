module half_adder(input A,input B,output S,output C);
    and_gate andGate(.A(A),.B(B),.O(C));
    assign S=A^B;
endmodule