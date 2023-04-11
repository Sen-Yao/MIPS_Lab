`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/03/15 19:08:34
// Design Name: 
// Module Name: CPU
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


module CPU(
    input [31:0] instr,
    input [31:0] mem,
    input clk,
    input reset
    );
endmodule

module PC(
    input clk,
    input reset,
    input jump,
    input branch,
    inout zero,
    input [31:0] instr,
    output reg[31:0] pAddr
    );
    wire[31:0] offset;
    assign offset = instr[15:15] ? {16'hffff,instr} : {16'h0000, instr}
endmodule