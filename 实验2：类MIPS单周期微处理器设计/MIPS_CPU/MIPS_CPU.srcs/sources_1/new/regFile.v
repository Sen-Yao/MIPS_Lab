module regFile(
    input clkin,
    input reset,
    input regWriteEn,
    input [31:0]instr, // 干脆把指令直接给它，让他判断是否需要 jal
    input [31:0]regWriteData,
    input [4:0] RsAddr,
    input [4:0] RtAddr,
    input [4:0] regWriteAddr,
    output reg [31:0]RsData,
    output reg[31:0]RtData
);
    reg [31:0] regs [31:0]; // 定义 32 个寄存器
    /*
    assign RsData = (RsAddr == 5'b00000) ? 32'h00000000 : regs[RsAddr];
    assign RtData = (RtAddr == 5'b00000) ? 32'b0 : regs[RtAddr]; 
    assign RtData = (instr[31:26] == 6'b001000) ? regs[31] : regs[RtAddr]; // 用于 jr 指令;
    */
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
        if (!reset) begin// 复位信号低电平时，按照题目要求对寄存器初值化
            for(i_reg = 0; i_reg < 32; i_reg = i_reg + 1) begin
                    regs[i_reg]= 4 * i_reg; 
            end
            i_reg = 0;
        end
        else begin
            if(regWriteEn == 1) begin// 允许对寄存器进行写操作
                if (instr[31:26] == 8'b000011)  begin
                    regs[31] = regWriteData; // 执行jal 指令，把pAddr 存进 $ra
                end
                else regs[regWriteAddr] = regWriteData;
            end
        end
    end
    
endmodule