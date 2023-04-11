module PC(
    input clkin,
    input reset,
    input jump,
    input branch, // 分支
    input zero, // 计算结果是否为 0
    input [31:0]instr,
    input [31:0]RtData,
    output reg[31:0]pAddr // 程序指针，只需要三十二条指令，但是这里还是多设计了几位，方便和教材进行参考
);
    wire[31:0] offset; // 偏置
    assign offset = instr[15:15] ? { 16'hffff,instr[15:0]} : {16'h0000,instr[15:0]}; // 32 位有符号拓展

    always @(negedge clkin or negedge reset) begin 
        if(!reset) begin // 初始化，指向第 0 条指令
            pAddr = 0;
        end
        else begin
            if(jump) begin// 需要跳转
                if (instr[31:26] == 6'b001000) begin
                    pAddr = RtData + 4; // jr 指令
                end
                else begin 
                    pAddr = {pAddr[31:28],(instr[25:0] << 2),2'b00}; 
                end
            end
            else begin // 不是跳转，那就可能是分支
                if(branch && zero) begin// 表示需要执行的 beq 或 jal，
                    pAddr = pAddr + 4 + (offset << 2);
                end
                else begin// 什么都不是，执行下一条指令
                    pAddr = pAddr + 4; 
                end 
            end //
        end
        if(pAddr >= 128) begin // 指针越界
            pAddr = 0; // 强制指向第 0 条指令
        end
    end
endmodule
