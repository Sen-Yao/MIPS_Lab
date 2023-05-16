// 此文件描述的是控制信号发生器 ctr 模块的设计

module ctr(
    input [31:0]instr,
    output reg regDst,  // regDst 决定写入寄存器的数据的地址来自指令的 [20:16] 还是 [15:11]。当 regDst 为 1 时选中前者，为 0 时选中后者
    output reg ALUSrc,  // ALUSrc 决定交给 ALU 进行运算的第二个数据来自 RtData 寄存器还是指令的立即数拓展。当 ALUSrc 为 1 时选中前者，为 0 时选中后者
    output reg memToReg,  // memToReg 决定即将写入寄存器的数据来自于 ALU 的运算结果还是 Dram，当 memToReg 为 1 时选中前者，为 0 时选中后者。
    output reg ALUToReg,  // ALUToReg 决定即将写入寄存器的数据来自于 ALU 的运算结果还是 Dram，当 ALUToReg 为 1 时选中前者，为 0 时选中后者
    // ALUToReg 为 1 且 memToReg 为 1 则数据来自于内存
    // ALUToReg 为 1 且 memToReg 为 0 则数据来自于 ALU 运算结果
    // ALUToReg 为 0 且 memToReg 为 0 则数据来自于 pAddr
    output reg regWrite,  // regWrite 为寄存器写使能信号，高电平有效
    output reg memRead,  // memRead 为内存读使能信号，高电平有效
    output reg memWrite,  // memWrite 为内存写使能信号，高电平有效
    output reg jump,  // jump 为允许跳转信号
    output reg branch,  // branch 为允许分支信号
    output reg [3:0]ALUop
); 
    wire [5:0]opCode;
    wire [5:0]funct;
    wire [11:0]ctr_var1;
    assign opCode = instr[31:26]; // 输入指令的高 6 位作为操作码
    assign funct = instr[5:0]; // ָ输入指令的高 6 位作为功能码
    assign ctr_var1 = {opCode, funct}; // 将操作码和功能码拼接起来，用于产生对应的控制信号
    // 初始化控制信号
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
            12'b000000xxxxxx: begin// 操作码为全零，说明是进行运算
                case(funct)
                    6'b100000: // 功能码为 100000，说明是 add 指令
                        ALUop = 4'b0001;
                    6'b100010: // 功能码为 100010，说明是 sub 指令
                        ALUop = 4'b0010;
                    6'b100100: // 功能码为 100100，说明是 and 指令
                        ALUop = 4'b0011;
                    6'b100101: // 功能码为 100101，说明是 or 指令
                        ALUop = 4'b0100;
                    6'b101010: // 功能码为 101010，说明是 slt 指令
                        ALUop = 4'b0101;
                    default:
                        ALUop = 4'b0000;
                endcase
                regDst = 0; // R 型指令将运算结果写入 [15:11] 描述的寄存器
                ALUSrc = 1; // R 型指令的运算数据来源于 Rt 寄存器
                memToReg = 0; // 存入寄存器的数据来自于 ALU，而非内存
                ALUToReg = 1; // 存入寄存器的数据来自于 ALU，而非内存
                regWrite = 1; // 因为要将结果存进寄存器，因此写使能有效
                memRead = 0; // 不允许读取内存
                memWrite = 0; // 不允许写内存
                branch = 0; // 不需要分支
                jump = 0; // 不需要跳转
            end
            12'b100111xxxxxx: begin// 操作码为 100111，说明是 lw 指令
                ALUop = 4'b0001; // lw 需要调用 ALU 进行地址的加运算
                regDst = 1; // 寄存器地址来自于 [20:16]
                ALUSrc = 0; // ALU 的数据来源于指令立即数，因为需要进行地址偏移量运算
                memToReg = 1; // 寄存器的数据来自于 mem
                ALUToReg = 1; // 寄存器的数据来自于 mem
                regWrite = 1; // lw 肯定要允许寄存器写入
                memRead = 1; // lw 肯定要允许读取内存
                memWrite = 0; // 不需要写内存
                branch = 0;  // 不需要分支
                jump = 0;   // 不需要跳转
            end
            12'b101011xxxxxx: begin  // 操作码为 101011，说明是 sw 指令
                ALUop = 4'b0001; // sw 需要调用 ALU 进行地址的加运算
                regDst = 1;  // 寄存器地址来自于 [20:16]
                ALUSrc = 0;  // ALU 的数据来源于指令立即数，因为需要进行地址偏移量运算
                memToReg = 0;  // 不需要向寄存器中写入数据，置零
                ALUToReg = 0;  // 不需要向寄存器中写入数据，置零
                regWrite = 0;  // 不需要向寄存器中写入数据，置零
                memRead = 0; // 不需要读内存
                memWrite = 1; // 需要写入内存
                branch = 0; // 不需要分支
                jump = 0;  // 不需要跳转
            
            end
            12'b000100xxxxxx: begin // 操作码为 000100，说明是 beq 指令
                ALUop = 4'b0010; // 需要 ALU 将两数相减，若结果为 0，说明相等
                regDst = 0; // 寄存器地址来自于 [15:11]
                ALUSrc = 1; // ALU 的数据来自于寄存器
                memToReg = 0; // 寄存器的数据来自于 ALU
                ALUToReg = 1; // 寄存器的数据来自于 ALU
                regWrite = 0; // 不需要向寄存器中写入数据，置零
                memRead = 0; // 不需要读内存
                memWrite = 0; // 不需要写内存
                branch = 1; // 需要进行分支
                jump = 0; // 不需要跳转
            end
            12'b000010xxxxxx: begin// 操作码为 000100，说明是 j 指令
                ALUop = 4'b0000; // ALU ֱ输出 0 信号
                regDst = 0; // 寄存器地址来自于 [15:11]
                ALUSrc = 0; // 置零
                memToReg = 0; // 寄存器的数据来自于 ALU
                ALUToReg = 1; // 寄存器的数据来自于 ALU
                regWrite = 0; // 不需要向寄存器中写入数据，置零
                memRead = 0; // 不需要读内存
                memWrite = 0; // 不需要写内存
                branch = 1; // 需要进行分支
                jump = 1; // 需要跳转
            end
            12'b000011xxxxxx: begin// 操作码为 000011，说明是 jal 指令
                ALUop = 4'b0000; // ALU ֱ输出 0 信号
                regDst = 0; // 无所谓，置零
                ALUSrc = 0; // 无所谓，置零
                memToReg = 0; // 寄存器的数据来自于 pAddr
                ALUToReg = 0; // 寄存器的数据来自于 pAddr
                regWrite = 1; // 需要将 $ra 写入
                memRead = 0; // 不需要读内存
                memWrite = 0; // 不需要写内存
                branch = 1; // 需要进行分支
                jump = 0; // 需要跳转
            end
            12'b001000xxxxxx: begin// jr ָ��
                ALUop = 4'b0000; // ALU ֱ输出 0 信号
                regDst = 0; // 无所谓，置零
                ALUSrc = 0; // 无所谓，置零
                memToReg = 0; // 寄存器的数据来自于 ALU
                ALUToReg = 1; // 寄存器的数据来自于 ALU
                regWrite = 1; // 需要将 $ra 写入
                memRead = 0; // 不需要读内存
                memWrite = 0; // 写内存
                branch = 1; // 类似 j 指令
                jump = 1; // 类似 j 指令
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

