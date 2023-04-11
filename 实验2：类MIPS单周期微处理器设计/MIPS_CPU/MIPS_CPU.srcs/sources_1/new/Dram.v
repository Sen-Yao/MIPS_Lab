module Dram(
    input clk,
    input we,
    input re,
    input [5:0] a,
    input [31:0] d,
    output reg [31:0]spo
);
    integer i_reg;
    reg [31:0] mems [31:0]; // 初始化 32 行内存空间
    initial begin
        for(i_reg = 0; i_reg < 32; i_reg = i_reg + 1) begin
                    mems[i_reg]= 4 * i_reg; // 按照题目要求进行初始化
        end
        i_reg = 0;   
    end
    always @(negedge clk)
        if (we)  begin
        mems[a] = d;end // 按照输入进行写入
    always @(re or a) begin
        if (re) begin spo = mems[a]; end end // 读取数据到输出
endmodule