#include <stdio.h>
#include "xil_io.h"
#include "xgpio_l.h"
#include "xparameters.h"

// GPIO_0 鐨� GPIO 涓鸿緭鍏ワ紝瀵瑰簲 SW15~0
// GPIO_0 鐨� GPIO2 涓鸿緭鍑猴紝瀵瑰簲 LED15~0
// GPIO_2 鐨� GPIO 涓鸿緭鍏ワ紝瀵瑰簲鎸夐挳鐨� D, U, L, R, C

int main()
{
    char button; // 8 浣�
    short sw; // 16 浣�
    unsigned short input_1,input_2;
    unsigned short output;
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0x1f); // 寮�鍏冲叏閮ㄤ负 Input 妯″紡
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0x1f); // 鎸夐敭鍏ㄩ儴涓� Input 妯″紡
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);  // LED 鍏ㄩ儴涓� Output 妯″紡
    while(1) // 涓�鐩存墽琛屼笅闈㈢殑鍐呭眰 while 寰幆
    while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f)!=0) // 寰幆妫�娴嬫寜閽槸鍚︽寜涓嬶紝鎸夐敭娌℃湁鎸変笅鏃朵负浣庣數骞筹紝鍥犳鍙浠讳綍涓�涓寜閿寜涓嬶紝璇诲叆瀵勫瓨鍣ㄧ殑鍊煎氨涓嶄负 0锛屼粠鑰岃繘鍏ュ惊鐜�
    {
        button = Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f; // 璇诲彇鎸夐敭鐨勫��
        sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET)&0x1f; // 璇诲彇鎸夐敭鐨勫��
        while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET) &0x1f)!=0); // 绛夊緟鎸夐敭閲婃斁
        if (button == 0b00000001) // 琛ㄧず鎸変笅 C
        {
            input_1 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_1);
        }
        if (button == 0b000001000) // 琛ㄧず鎸変笅 R
        {
            input_2 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_2);
        }
        if (button == 0b00000010) // 琛ㄧず鎸変笅 U
        {
            output = input_1 + input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        if (button == 0b00010000) // 琛ㄧず鎸変笅 D
        {
            output = input_1 * input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        xil_printf("The pushed button's code is 0x%x\n", button); // 杈撳嚭鎸夐敭鐨勫�煎埌stdio
    }
}
