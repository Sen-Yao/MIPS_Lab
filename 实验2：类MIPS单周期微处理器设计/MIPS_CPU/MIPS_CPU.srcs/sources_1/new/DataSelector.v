`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/04/11 22:40:35
// Design Name: 
// Module Name: DataSelector
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module DataSelector(
    input memToReg,
    input ALUToReg,
    input [31:0]ALUReg,
    input [31:0]mem,
    input [31:0]pAddr,
    output reg [31:0]wrData
 );
    always @* begin
        if (ALUToReg) begin
            if (memToReg) begin
                wrData = mem; // ALUToReg Ϊ 1��memToReg Ϊ 1����ʾд�������������ڴ�
            end
            else begin
                wrData = ALUReg;  // ALUToReg Ϊ 1��memToReg Ϊ 0����ʾд������������ ALU
            end
        end
        else begin
            wrData = pAddr; // ALUToReg Ϊ 0����ʾд�����������ڳ�����תָ�� pAddr
        end   
    end
endmodule
