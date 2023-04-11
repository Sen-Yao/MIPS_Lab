module ctr(
    input [31:0]instr,
    output reg regDst, 
    output reg ALUSrc, 
    output reg memToReg,
    output reg ALUToReg,
    output reg regWrite,
    output reg memRead,
    output reg memWrite,
    output reg jump,
    output reg branch,
    output reg [3:0]ALUop
); 
    wire [5:0]opCode;
    wire [5:0]funct;
    wire [11:0]ctr_var1;
    assign opCode = instr[31:26]; // ������ض�����ָ��ĸ���λ
    assign funct = instr[5:0]; // ָ�����λΪ������
    assign ctr_var1 = {opCode, funct}; // ��������ƴ��һ�𣬾�֪����ִ��ʲôָ��
    initial begin
        regDst = 0;
        ALUSrc = 0;
        memToReg = 0;
        regWrite = 0;
        memRead = 0;
        memWrite = 0;
        jump = 0;
        branch = 0;
    end
    always @(*)begin
        casex(ctr_var1)
            12'b000000xxxxxx: begin// R ��ָ��
                case(funct)
                    6'b100000: // add
                        ALUop = 4'b0001;
                    6'b100010: // sub
                        ALUop = 4'b0010;
                    6'b100100: // and
                        ALUop = 4'b0011;
                    6'b100101: // or
                        ALUop = 4'b0100;
                    6'b101010: //slt
                        ALUop = 4'b0101;
                    default:
                        ALUop = 4'b0000;
                endcase
                regDst = 0; // R ��ָ���Ŀ��Ĵ���Ϊ [20:16]
                ALUSrc = 1; // R ��ָ��ִ��ʱ�� ALU �����������ڼĴ���
                memToReg = 0; // д��Ŀ��Ĵ������������� ALU �����ڴ�
                ALUToReg = 1; // д��Ŀ��Ĵ������������� ALU ���� pAddr
                regWrite = 1; // ����д��Ĵ���
                memRead = 0; // �ڴ治��
                memWrite = 0; // �ڴ治��
                branch = 0; // ����Ҫ��֧
                jump = 0; // ����Ҫ��ת
            end
            12'b100111xxxxxx: begin// lw ��ָ����ڴ������ݶ����Ĵ����� 
                ALUop = 4'b0001; // lw ��ָ����Ҫ�� ALU �����ַƫ��
                regDst = 1; // Ŀ��Ĵ���Ϊ [20:16]
                ALUSrc = 0; // ALU ����������ָ���������
                memToReg = 1; // ���ڴ�������д��Ĵ���
                ALUToReg = 1; // �����ڴ���� ALU �� pAddr
                regWrite = 1; // lw �϶�Ҫд��Ĵ���
                memRead = 1; // ��ȡ�ڴ�
                memWrite = 0; // �ڴ治��д
                branch = 0; // ����֧
                jump = 0; // ����ת
            end
            12'b101011xxxxxx: begin// sw ��ָ����Ĵ���������д���ڴ���
                ALUop = 4'b0001; // sw ��ָ����Ҫ�� ALU �����ַƫ��
                regDst = 1; // Ŀ��Ĵ���Ϊ [20:16]
                ALUSrc = 0; // ALU ����������ָ���������
                memToReg = 0; // ����Ҫд��Ĵ�����Ĭ���� 0
                ALUToReg = 0; // ����Ҫд��Ĵ�����Ĭ���� 0
                regWrite = 0; // lw ����Ҫд��Ĵ���
                memRead = 0; // ����Ҫ��ȡ�ڴ�
                memWrite = 1; // ��Ҫд���ڴ�
                branch = 0; // ����֧
                jump = 0; // ����ת
            
            end
            12'b000100xxxxxx: begin// beq ָ��
                ALUop = 4'b0010; // ���������ж��Ƿ����
                regDst = 0; // Ŀ��Ĵ���Ϊ [15:11]
                ALUSrc = 1; // ALU ���������ڼĴ���
                memToReg = 0; // ���������� ALU
                ALUToReg = 1; // ���������� ALU
                regWrite = 0; // ����Ҫд��Ĵ���
                memRead = 0; // ����Ҫ��ȡ�ڴ�
                memWrite = 0; // ����Ҫд���ڴ�
                branch = 1; // ��֧
                jump = 0; // ����ת
            end
            12'b000010xxxxxx: begin// j ָ�� 
                ALUop = 4'b0000; // ALU ֱ����� 0
                regDst = 0; // Ŀ��Ĵ���Ϊ [15:11]
                ALUSrc = 0; // ALU ����������
                memToReg = 0; // �������� ALU �����ڴ�
                ALUToReg = 1; // �������� ALU ���� pAddr
                regWrite = 0; // ����Ҫд��Ĵ���
                memRead = 0; // ����Ҫ��ȡ�ڴ�
                memWrite = 0; // ����Ҫд���ڴ�
                branch = 1; // ��֧
                jump = 1; // ��ת
            end
            12'b000011xxxxxx: begin// jal ָ��
                ALUop = 4'b0000; // Ҫ�� ALU ��� 0
                regDst = 0; // ����ν
                ALUSrc = 0; // ����ν
                memToReg = 0; // �� pAddr ����Ĵ���д����
                ALUToReg = 0; // �� pAddr ����Ĵ���д����
                regWrite = 1; // ����д�� $ra �Ĵ���
                memRead = 0; // ������
                memWrite = 0; // ������
                branch = 1; // �ο�beq
                jump = 0; // �ο� beq
            end
            12'b001000xxxxxx: begin// jr ָ��
                ALUop = 4'b0000; // Ҫ�� ALU ��� 0
                regDst = 0; // ����ν
                ALUSrc = 0; // ����ν
                memToReg = 0; // ����ν
                ALUToReg = 1; // ����ν
                regWrite = 1; // ����д�� $ra �Ĵ���
                memRead = 0; // ������
                memWrite = 0; // ������
                branch = 1; // �ο� j
                jump = 1; // �ο� j
            end
            default: begin
                ALUop = 4'b0000;
                regDst = 0;
                ALUSrc = 0; 
                memToReg = 0; 
                ALUToReg = 1;
                regWrite = 0; 
                memRead = 0; 
                memWrite = 0;
                branch = 0; 
                jump = 0;
            end
        endcase
    end
endmodule