// 此文件描述的是内存 Dram 模块的设计

module Dram(
    input clk,
    input we,
    input re,
    input [5:0] a,
    input [31:0] d,
    output reg [31:0]spo
);
    integer i_reg;
    reg [31:0] mems [31:0]; // 初始化 32 个长为 32 的内存空间
    initial begin
        for(i_reg = 0; i_reg < 32; i_reg = i_reg + 1) begin
                    mems[i_reg]= 4 * i_reg; // 按照题目要求赋初值
        end
        i_reg = 0;   
    end
    always @(negedge clk)
        if (we)  begin
        mems[a] = d;end // 若允许写入，则在时钟下降沿将输入信号 d 写入对应的内存空间
    always @(re or a) begin
        if (re) begin spo = mems[a]; end end // 若允许读，则将地址 a 中的数据读给 spo
endmodule