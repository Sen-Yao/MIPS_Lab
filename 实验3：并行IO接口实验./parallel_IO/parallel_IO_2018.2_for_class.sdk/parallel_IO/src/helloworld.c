#include <stdio.h>
#include "xil_io.h"
#include "xgpio_l.h"
#include "xparameters.h"

// GPIO_0 �� GPIO Ϊ���룬��Ӧ SW15~0
// GPIO_0 �� GPIO2 Ϊ�������Ӧ LED15~0
// GPIO_2 �� GPIO Ϊ���룬��Ӧ��ť�� D, U, L, R, C

int main()
{
    char button;  // 8 λ
    short sw;  // 16 λ
    unsigned short input_1,input_2;
    unsigned short output;
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);  // ����ȫ��Ϊ Input ģʽ
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);   // ����ȫ��Ϊ Input ģʽ
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);  // LED ȫ��Ϊ Output ģʽ
    while(1)  // һֱִ��������ڲ� while ѭ��
    while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f)!=0)  // ѭ����ⰴť�Ƿ��£�����û�а���ʱΪ�͵�ƽ�����ֻҪ�κ�һ���������£�����Ĵ�����ֵ�Ͳ�Ϊ 0���Ӷ�����ѭ��
    {
        button = Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f;  // ��ȡ������ֵ
        sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET)&0x1f;  // ��ȡ����
        while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET) &0x1f)!=0); // �ȴ������ͷ�
        if (button == 0b00000001) // ��ʾ���� C
        {
            input_1 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_1);
        }
        if (button == 0b000001000) // ��ʾ���� R
        {
            input_2 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_2);
        }
        if (button == 0b00000010) // ��ʾ���� U
        {
            output = input_1 + input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        if (button == 0b00010000) // ��ʾ���� D
        {
            output = input_1 * input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        xil_printf("The pushed button's code is 0x%x\n", button); // ���������ֵ��stdio
    }
}
