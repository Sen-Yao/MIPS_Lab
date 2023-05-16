// 此文件是主文件，将所有模块串在一起。
`include "ALU.v"
`include "Dram.v"
`include "MUX.v"
`include "PC.v"
`include "ctr.v"
`include "irom.v"
`include "regFile.v"
`include "signext.v"

module MIPS(
    input clk,
    input reset
);
    wire [31:0]instr;
    wire [31:0]mem;

    wire [31:0]wrData;
    wire [31:0]RtData;
    wire [31:0]RsData;
    wire [4:0]RdAdd;

    wire [31:0] ALU_in1;
    wire [31:0] ALU_in2;

    wire regDst; // regDst 决定写入寄存器的数据的地址来自指令的 [20:16] 还是 [15:11]。当 regDst 为 1 时选中前者，为 0 时选中后者
    wire ALUSrc; // ALUSrc 决定交给 ALU 进行运算的第二个数据来自 RtData 寄存器还是指令的立即数拓展。当 ALUSrc 为 1 时选中前者，为 0 时选中后者
    wire memToReg; // memToReg 决定即将写入寄存器的数据来自于 ALU 的运算结果还是 Dram，当 memToReg 为 1 时选中前者，为 0 时选中后者。
    wire ALUToReg; // ALUToReg 决定即将写入寄存器的数据来自于 ALU 的运算结果还是 Dram，当 ALUToReg 为 1 时选中前者，为 0 时选中后者。
    // ALUToReg 为 1 且 memToReg 为 1 则数据来自于内存
    // ALUToReg 为 1 且 memToReg 为 0 则数据来自于 ALU 运算结果
    // ALUToReg 为 0 且 memToReg 为 0 则数据来自于 pAddr
    wire regWrite;  // regWrite 为寄存器写使能信号，高电平有效
    wire memRead;  // memRead 为内存读使能信号，高电平有效
    wire memWrite;  // memWrite 为内存写使能信号，高电平有效
    wire jump; /// jump 为允许跳转信号
    wire branch; // branch 为允许分支信号 
    wire [3:0]ALUop;
    wire [31:0]ALURes;
    wire zero;
    wire [31:0]pAddr;

    wire [31:0]sign;

    wire [31:0]inData;

    PC PC_1(
        clk,
        reset,
        jump,
        branch,
        zero,
        instr,
        RtData,
        pAddr
    );
    regFile regFile_1 (
        clk,
        reset,
        regWrite,
        instr,
        wrData,
        instr[25:21],
        instr[20:16],
        RdAdd[4:0],
        ALU_in1,
        RtData
    );
    ctr ctr_1(
        instr,
        regDst,
        ALUSrc,
        memToReg,
        ALUToReg,
        regWrite,
        memRead, 
        memWrite,
        jump, 
        branch,
        ALUop 
    );
    ALU ALU_1(
        ALU_in1,
        ALU_in2,
        ALUop,
        ALURes,
        zero
    );

    MUX5 MUX5_1(
        instr[20:16],
        instr[15:11],
        regDst,
        RdAdd[4:0]
    );
    
    MUX32 MUX32_1(
        RtData,
        sign,
        ALUSrc,
        ALU_in2
    );
    DataSelector Da(
        memToReg,
        ALUToReg,
        ALURes,
        inData,
        pAddr,
        wrData
    );
    signext signext_1(
        instr,
        sign
    );
    irom irom_1(
        clk,
        pAddr[8:2] ,
        instr
    );
    Dram Dram_1(
        clk,
        memWrite,
        memRead,
        ALURes[7:2],
        RtData,
        inData
    );
endmodule
   





