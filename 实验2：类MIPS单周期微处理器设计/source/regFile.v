// 此文件描述了寄存器 regFile 的设计
module regFile(
    input clkin,
    input reset,
    input regWriteEn,
    input [31:0]instr, // 在正常的 regFile 输入基础上，还加上了指令输入，用于判断 jal 指令
    input [31:0]regWriteData,
    input [4:0] RsAddr,
    input [4:0] RtAddr,
    input [4:0] regWriteAddr,
    output reg [31:0]RsData,
    output reg[31:0]RtData
);
    reg [31:0] regs [31:0]; // 初始化 32 个 32 位寄存器
    always @* begin
        if (RsAddr == 5'b00000) begin
            RsData = 32'h00000000;
        end
        else begin
            RsData = regs[RsAddr];
        end
        if (RtAddr == 5'b00000) begin
            RtData = 32'h00000000;
        end
        else begin
            RsData = regs[RsAddr]; RtData = regs[RtAddr];
        end
        if (instr[31:26] == 6'b001000) begin
            RtData = regs[31];
        end
    end

    integer i_reg; 
    always @ (negedge clkin or negedge reset) begin 
        if (!reset) begin// 复位信号低电平有效，按照题目要求赋初值
            for(i_reg = 0; i_reg < 32; i_reg = i_reg + 1) begin
                    regs[i_reg]= 4 * i_reg; 
            end
            i_reg = 0;
        end
        else begin
            if(regWriteEn == 1) begin// 写使能有效
                if (instr[31:26] == 8'b000011)  begin
                    regs[31] = regWriteData; // 说明是 jal 指令，需要将 pAddr 赋给 $ra
                end
                else regs[regWriteAddr] = regWriteData;
            end
        end
    end
    
endmodule