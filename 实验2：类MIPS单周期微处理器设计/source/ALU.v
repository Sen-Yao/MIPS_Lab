// 此文件描述的是 ALU 模块的设计

module ALU(
    input [31:0]input1,
    input [31:0]input2,
    input [3:0] ALUop,
    output reg [31:0]ALURes,
    output zero
);
    assign zero = ALURes ? 0:1; // 若 ALU 运算结果为 0，则令控制信号 zero 为高电平
    initial ALURes = 32'h00000000;
    always @*  begin
        case(ALUop)
            4'b0000:
                ALURes = 32'h00000000;
            4'b0001: // 让 ALU 执行加法运算
                ALURes = input1 + input2;
            4'b0010: // 让 ALU 执行减法运算
                ALURes = input1 - input2; 
            4'b0011: // 让 ALU 执行乘法运算
                ALURes = input1 & input2;
            4'b0100: // 让 ALU 执行按位或法运算
                ALURes = input1 | input2;
            4'b0101: // 进行比较运算
                ALURes = (input1 < input2) ? 32'h00000001 : 32'h00000000;   
        endcase
    end
endmodule