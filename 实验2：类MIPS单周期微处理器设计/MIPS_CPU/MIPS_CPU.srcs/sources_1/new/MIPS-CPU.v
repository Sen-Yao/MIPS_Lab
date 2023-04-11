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

    wire regDst; // 决定目标寄存器来自指令机器码的高位还是低位，1 表示来自 inst[20:16]，0 表示来自 inst[15:11]
    wire ALUSrc; // ALU 的第二个输入数来自哪里，1 表示来自 RtData，0 表示来自指令的立即数
    wire memToReg; // 存到寄存器的数据是什么，1 表示内存，0 表示来自 ALU 或 pAddr
    wire ALUToReg; // 存到寄存器的数据是什么，1 表示 ALU，0 表示来自 pAddr
    wire regWrite; // 寄存器的写使能信号，1 表示允许写入
    wire memRead; // 内存的读使能信号，1 表示允许读
    wire memWrite; // 内存的写使能信号，1 表示允许写
    wire jump; // 跳转
    wire branch; // 鍒嗘敮分支
    wire [3:0]ALUop; // ALU 执行什么运算
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
   





