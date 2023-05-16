`timescale 1ns / 1ps

module sim();
    reg clk1GHz,reset; // ��λ�źŵ͵�ƽ��Ч
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
