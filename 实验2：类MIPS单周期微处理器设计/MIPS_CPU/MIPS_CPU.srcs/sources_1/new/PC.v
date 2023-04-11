module PC(
    input clkin,
    input reset,
    input jump,
    input branch, // ��֧
    input zero, // �������Ƿ�Ϊ 0
    input [31:0]instr,
    input [31:0]RtData,
    output reg[31:0]pAddr // ����ָ�룬ֻ��Ҫ��ʮ����ָ��������ﻹ�Ƕ�����˼�λ������ͽ̲Ľ��вο�
);
    wire[31:0] offset; // ƫ��
    assign offset = instr[15:15] ? { 16'hffff,instr[15:0]} : {16'h0000,instr[15:0]}; // 32 λ�з�����չ

    always @(negedge clkin or negedge reset) begin 
        if(!reset) begin // ��ʼ����ָ��� 0 ��ָ��
            pAddr = 0;
        end
        else begin
            if(jump) begin// ��Ҫ��ת
                if (instr[31:26] == 6'b001000) begin
                    pAddr = RtData + 4; // jr ָ��
                end
                else begin 
                    pAddr = {pAddr[31:28],(instr[25:0] << 2),2'b00}; 
                end
            end
            else begin // ������ת���ǾͿ����Ƿ�֧
                if(branch && zero) begin// ��ʾ��Ҫִ�е� beq �� jal��
                    pAddr = pAddr + 4 + (offset << 2);
                end
                else begin// ʲô�����ǣ�ִ����һ��ָ��
                    pAddr = pAddr + 4; 
                end 
            end //
        end
        if(pAddr >= 128) begin // ָ��Խ��
            pAddr = 0; // ǿ��ָ��� 0 ��ָ��
        end
    end
endmodule
