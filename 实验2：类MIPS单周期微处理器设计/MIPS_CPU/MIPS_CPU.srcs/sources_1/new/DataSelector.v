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
                wrData = mem; // ALUToReg 为 1，memToReg 为 1，表示写入数据来自于内存
            end
            else begin
                wrData = ALUReg;  // ALUToReg 为 1，memToReg 为 0，表示写入数据来自于 ALU
            end
        end
        else begin
            wrData = pAddr; // ALUToReg 为 0，表示写入数据来自于程序跳转指针 pAddr
        end   
    end
endmodule
