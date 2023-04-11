module ctr(
    input [31:0]instr,
    output reg regDst, 
    output reg ALUSrc, 
    output reg memToReg,
    output reg ALUToReg,
    output reg regWrite,
    output reg memRead,
    output reg memWrite,
    output reg jump,
    output reg branch,
    output reg [3:0]ALUop
); 
    wire [5:0]opCode;
    wire [5:0]funct;
    wire [11:0]ctr_var1;
    assign opCode = instr[31:26]; // 操作码必定来自指令的高六位
    assign funct = instr[5:0]; // 指令第五位为功能码
    assign ctr_var1 = {opCode, funct}; // 把这两个拼在一起，就知道该执行什么指令
    initial begin
        regDst = 0;
        ALUSrc = 0;
        memToReg = 0;
        regWrite = 0;
        memRead = 0;
        memWrite = 0;
        jump = 0;
        branch = 0;
    end
    always @(*)begin
        casex(ctr_var1)
            12'b000000xxxxxx: begin// R 型指令
                case(funct)
                    6'b100000: // add
                        ALUop = 4'b0001;
                    6'b100010: // sub
                        ALUop = 4'b0010;
                    6'b100100: // and
                        ALUop = 4'b0011;
                    6'b100101: // or
                        ALUop = 4'b0100;
                    6'b101010: //slt
                        ALUop = 4'b0101;
                    default:
                        ALUop = 4'b0000;
                endcase
                regDst = 0; // R 型指令的目标寄存器为 [20:16]
                ALUSrc = 1; // R 型指令执行时， ALU 的输入来自于寄存器
                memToReg = 0; // 写入目标寄存器的输入来自 ALU 而非内存
                ALUToReg = 1; // 写入目标寄存器的输入来自 ALU 而非 pAddr
                regWrite = 1; // 允许写入寄存器
                memRead = 0; // 内存不动
                memWrite = 0; // 内存不动
                branch = 0; // 不需要分支
                jump = 0; // 不需要跳转
            end
            12'b100111xxxxxx: begin// lw 型指令，将内存中数据读到寄存器中 
                ALUop = 4'b0001; // lw 型指令需要让 ALU 计算地址偏移
                regDst = 1; // 目标寄存器为 [20:16]
                ALUSrc = 0; // ALU 输入来自于指令的立即数
                memToReg = 1; // 将内存中数据写入寄存器
                ALUToReg = 1; // 来自内存而非 ALU 或 pAddr
                regWrite = 1; // lw 肯定要写入寄存器
                memRead = 1; // 读取内存
                memWrite = 0; // 内存不许写
                branch = 0; // 不分支
                jump = 0; // 不跳转
            end
            12'b101011xxxxxx: begin// sw 型指令，将寄存器中数据写到内存中
                ALUop = 4'b0001; // sw 型指令需要让 ALU 计算地址偏移
                regDst = 1; // 目标寄存器为 [20:16]
                ALUSrc = 0; // ALU 输入来自于指令的立即数
                memToReg = 0; // 不需要写入寄存器，默认置 0
                ALUToReg = 0; // 不需要写入寄存器，默认置 0
                regWrite = 0; // lw 不需要写入寄存器
                memRead = 0; // 不需要读取内存
                memWrite = 1; // 需要写入内存
                branch = 0; // 不分支
                jump = 0; // 不跳转
            
            end
            12'b000100xxxxxx: begin// beq 指令
                ALUop = 4'b0010; // 做减运算判断是否相等
                regDst = 0; // 目标寄存器为 [15:11]
                ALUSrc = 1; // ALU 输入来自于寄存器
                memToReg = 0; // 数据来自于 ALU
                ALUToReg = 1; // 数据来自于 ALU
                regWrite = 0; // 不需要写入寄存器
                memRead = 0; // 不需要读取内存
                memWrite = 0; // 不需要写入内存
                branch = 1; // 分支
                jump = 0; // 不跳转
            end
            12'b000010xxxxxx: begin// j 指令 
                ALUop = 4'b0000; // ALU 直接输出 0
                regDst = 0; // 目标寄存器为 [15:11]
                ALUSrc = 0; // ALU 来自立即数
                memToReg = 0; // 数据来自 ALU 而非内存
                ALUToReg = 1; // 数据来自 ALU 而非 pAddr
                regWrite = 0; // 不需要写入寄存器
                memRead = 0; // 不需要读取内存
                memWrite = 0; // 不需要写入内存
                branch = 1; // 分支
                jump = 1; // 跳转
            end
            12'b000011xxxxxx: begin// jal 指令
                ALUop = 4'b0000; // 要求 ALU 输出 0
                regDst = 0; // 无所谓
                ALUSrc = 0; // 无所谓
                memToReg = 0; // 让 pAddr 进入寄存器写数据
                ALUToReg = 0; // 让 pAddr 进入寄存器写数据
                regWrite = 1; // 允许写入 $ra 寄存器
                memRead = 0; // 不允许
                memWrite = 0; // 不允许
                branch = 1; // 参考beq
                jump = 0; // 参考 beq
            end
            12'b001000xxxxxx: begin// jr 指令
                ALUop = 4'b0000; // 要求 ALU 输出 0
                regDst = 0; // 无所谓
                ALUSrc = 0; // 无所谓
                memToReg = 0; // 无所谓
                ALUToReg = 1; // 无所谓
                regWrite = 1; // 允许写入 $ra 寄存器
                memRead = 0; // 不允许
                memWrite = 0; // 不允许
                branch = 1; // 参考 j
                jump = 1; // 参考 j
            end
            default: begin
                ALUop = 4'b0000;
                regDst = 0;
                ALUSrc = 0; 
                memToReg = 0; 
                ALUToReg = 1;
                regWrite = 0; 
                memRead = 0; 
                memWrite = 0;
                branch = 0; 
                jump = 0;
            end
        endcase
    end
endmodule