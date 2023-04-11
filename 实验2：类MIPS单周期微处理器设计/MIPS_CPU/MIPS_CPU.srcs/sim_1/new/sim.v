`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2023/03/28 20:56:43
// Design Name: 
// Module Name: sim
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


module sim();
    reg clk1GHz,reset; // 复位信号低电平有效
    MIPS CPU(clk1GHz,reset);
    
    always begin
        #1
        clk1GHz = ~ clk1GHz;
    end
    initial begin
        clk1GHz = 0;
        reset = 0;
        #1
        reset =1;
        #300 $finish;
    end
endmodule
