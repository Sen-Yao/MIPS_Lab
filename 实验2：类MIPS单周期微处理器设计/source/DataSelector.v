// 此文件描述的是数据选择器 DataSeletor 模块的设计，描述了由控制信号 memToReg 和 ALUToReg 共同决定写入寄存器的数据的来源的过程
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
                wrData = mem; // ALUToReg 为 1 且 memToReg 为 1 则数据来自于内存
            end
            else begin
                wrData = ALUReg;  // ALUToReg 为 1 且 memToReg 为 0 则数据来自于 ALU 运算结果
            end
        end
        else begin
            wrData = pAddr; // ALUToReg 为 0 且 memToReg 为 0 则数据来自于 pAddr
        end   
    end
endmodule

