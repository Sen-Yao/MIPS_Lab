module signext(
    input [31:0] inst,
    output wire [31:0] data
);
    wire [15:0] middle;
    assign middle = inst[15:0];
    assign data = middle[15:15] ? { 16'hffff,middle} : {16'h0000,middle};
endmodule