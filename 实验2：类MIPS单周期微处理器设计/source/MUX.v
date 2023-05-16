// 此文件描述了两个简单的数据选择器。
module MUX5(
    input [4:0]A,
    input [4:0]B,
    input set,
    output [4:0]Y
);
    assign Y = set ? A : B;
endmodule

module MUX32(
    input [31:0]A,
    input [31:0]B,
    input set,
    output [31:0]Y
);
   assign Y = set ? A : B;
endmodule