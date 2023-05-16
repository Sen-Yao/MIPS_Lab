// 此文件描述的是指令控制器 PC 的工作过程
module PC(
    input clkin,
    input reset,
    input jump,
    input branch, 
    input zero, 
    input [31:0]instr,
    input [31:0]RtData,
    output reg[31:0]pAddr // PC 输出一个指令指针 pAddr，用于指示下一条指令
);
    wire[31:0] offset; // ƫ��
    assign offset = instr[15:15] ? { 16'hffff,instr[15:0]} : {16'h0000,instr[15:0]}; // 对立即数的低 16 位进行 32 位扩展

    always @(negedge clkin or negedge reset) begin 
        if(!reset) begin // 复位信号低电平有效
            pAddr = 0;
        end
        else begin
            if(jump) begin// 说明需要进行程序跳转
                if (instr[31:26] == 6'b001000) begin
                    pAddr = RtData + 4; // jr 指令
                end
                else begin 
                    pAddr = {pAddr[31:28],(instr[25:0] << 2),2'b00}; 
                end
            end
            else begin // 不需要进行程序跳转
                if(branch && zero) begin// 满足执行条件的 beq 指令或 jal 指令
                    pAddr = pAddr + 4 + (offset << 2);
                end
                else begin// 其他情况，直接执行下一条指令
                    pAddr = pAddr + 4; 
                end 
            end //
        end
        if(pAddr >= 128) begin // 指针越界
            pAddr = 0; // 强制指向第 0 条指令
        end
    end
endmodule
