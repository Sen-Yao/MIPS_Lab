module ALU(
    input [31:0]input1,
    input [31:0]input2,
    input [3:0] ALUop,
    output reg [31:0]ALURes,
    output zero
);
    assign zero = ALURes ? 0:1; // 运算结果为零，输出高电平
    initial ALURes = 32'h00000000;
    always @*  begin
        case(ALUop)
            4'b0000:
                ALURes = 32'h00000000;
            4'b0001: // 加法
                ALURes = input1 + input2;
            4'b0010: // 减法
                ALURes = input1 - input2; 
            4'b0011: // 与运算
                ALURes = input1 & input2;
            4'b0100: // 或运算
                ALURes = input1 | input2;
            4'b0101: // 小于等于
                ALURes = (input1 < input2) ? 32'h00000001 : 32'h00000000;   
        endcase
    end
endmodule