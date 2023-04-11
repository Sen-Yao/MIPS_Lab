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

    wire regDst; // ����Ŀ��Ĵ�������ָ�������ĸ�λ���ǵ�λ��1 ��ʾ���� inst[20:16]��0 ��ʾ���� inst[15:11]
    wire ALUSrc; // ALU �ĵڶ����������������1 ��ʾ���� RtData��0 ��ʾ����ָ���������
    wire memToReg; // �浽�Ĵ�����������ʲô��1 ��ʾ�ڴ棬0 ��ʾ���� ALU �� pAddr
    wire ALUToReg; // �浽�Ĵ�����������ʲô��1 ��ʾ ALU��0 ��ʾ���� pAddr
    wire regWrite; // �Ĵ�����дʹ���źţ�1 ��ʾ����д��
    wire memRead; // �ڴ�Ķ�ʹ���źţ�1 ��ʾ�����
    wire memWrite; // �ڴ��дʹ���źţ�1 ��ʾ����д
    wire jump; // ��ת
    wire branch; // 分支��֧
    wire [3:0]ALUop; // ALU ִ��ʲô����
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
   





