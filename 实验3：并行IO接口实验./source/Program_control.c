#include <stdio.h>
#include "xil_io.h"
#include "xgpio_l.h"
#include "xparameters.h"

// GPIO_0 的 GPIO 为输入，对应 SW15~0
// GPIO_0 的 GPIO2 为输出，对应 LED15~0
// GPIO_2 的 GPIO 为输入，对应按钮的 D, U, L, R, C 

int main()
{
    char button;  // 8 位
    short sw;  // 16 位
    unsigned short input_1,input_2;
    unsigned short output;
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);  // 开关全部为 Input 模式
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0x1f);   // 按键全部为 Input 模式
    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);  // LED 全部为 Output 模式
    while(1)  // 一直执行下面的内层 while 循环
    while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f)!=0)  // 循环检测按钮是否按下，按键没有按下时为低电平，因此只要任何一个按键按下，读入寄存器的值就不为 0，从而进入循环
    {
        button = Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET)&0x1f;  // 读取按键的值
        sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET)&0x1f;  // 读取开关
        while ((Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET) &0x1f)!=0); // 等待按键释放
        if (button == 0b00000001) // 表示按下 C
        {
            input_1 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_1);
        }
        if (button == 0b000001000) // 表示按下 R
        {
            input_2 = sw;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_2);
        }
        if (button == 0b00000010) // 表示按下 U
        {
            output = input_1 + input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        if (button == 0b00010000) // 表示按下 D
        {
            output = input_1 * input_2;
            Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
        }
        xil_printf("The pushed button's code is 0x%x\n", button); // 输出按键的值到stdio
    }
}
