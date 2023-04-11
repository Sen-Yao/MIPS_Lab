module Dram(
    input clk,
    input we,
    input re,
    input [5:0] a,
    input [31:0] d,
    output reg [31:0]spo
);
    integer i_reg;
    reg [31:0] mems [31:0]; // ��ʼ�� 32 ���ڴ�ռ�
    initial begin
        for(i_reg = 0; i_reg < 32; i_reg = i_reg + 1) begin
                    mems[i_reg]= 4 * i_reg; // ������ĿҪ����г�ʼ��
        end
        i_reg = 0;   
    end
    always @(negedge clk)
        if (we)  begin
        mems[a] = d;end // �����������д��
    always @(re or a) begin
        if (re) begin spo = mems[a]; end end // ��ȡ���ݵ����
endmodule