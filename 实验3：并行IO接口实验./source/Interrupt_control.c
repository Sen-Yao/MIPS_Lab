// 中断方式 button 按键以及 switch 输入的测试程序

// #include "xparameters.h"    //The hardware configuration describing constants
#include "xintc_l.h"  // Interrupt Controller API functions
#include "stdio.h"
#include "xil_io.h"     // I0 functions
#include "xgpio_l.h"
#include "xtmrctr.h"
// #include "xil_types.h"
// #include "mb_interface.h"
// #include "xgpio.h"      // GPIO API functions


void My_ISR() __attribute__ ((interrupt_handler));

// __attribute__ 为中断服务子程序。
// 当 MicroBlaze 的中断管脚 Interrupt，CPU 就会在内部的寄存器中自动保存断点
// 并且跳转到这里的子函数

void Initialize(); // 初始化函数 (包含中断初始化)

void PushBtnHandler();
void SwitchHandler();
void Delay_500ms();

short flag_Sw,flag_Btn;  // 按键标志位
short sw, btn;  // 按键赋值
int input_1, input_2;
int output;

int main(void)
{
	xil_printf("\r\nRunning CpioInput Interrupt Test(No API)\r\n");
    Initialize();
    while(1)
    {
    	if(flag_Sw)  // 若拨动 switch 开关，则打印相关信息
        {
            xil_printf("flag_Sw = %X, Switch Interrupt Triggered! The result is 0x%X\n\r", flag_Sw, sw);
            flag_Sw = 0;
        }
        if(flag_Btn) // 若按下 btn 按键，则打印相关信息
        {
            xil_printf("Button Interrupt Triggered! The result is 0x%X\n\r", btn);
            flag_Btn = 0;
            if (btn & 0x01) // C
            {
            	input_1 = sw;
            	Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_1);

            }
            if (btn & 0x08) // R
            {
            	input_2 = sw;
            	Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, input_2);

            }
            if (btn & 0x02) // U
            {
                output = input_1 + input_2;
                Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
            }
            if (btn & 0x10) // D
            {
                output = input_1 * input_2;
                Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA2_OFFSET, output);
            }

        }

        // 这里看似是个无用的死循环，因为 flag_sw 和 flag_btn 都没有变化
        // 但实际上，只要硬件上一旦出现中断，就会执行 __attribute__  子函数
        // 从而实现 flag_sw 和 flag_btn 的修改
    }
    return 0;
}

void Initialize()
{
    flag_Sw = 0x00;
    flag_Btn = 0x00;
    sw = 0x00;
    btn = 0x00;

    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xff);  // sw 作为输入
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_IER_OFFSET, 0x01);  // 端口 0 打开
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_GIE_OFFSET, XGPIO_GIE_GINTR_ENABLE_MASK);  // sw 中断启动

    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI_OFFSET, 0xff);  // btn 作为输入
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_IER_OFFSET, 0x01);  // 端口 0 打开
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_GIE_OFFSET, XGPIO_GIE_GINTR_ENABLE_MASK);  // btn 中断启动

    Xil_Out16(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);  // LED 为输出

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07);  // Intr[1:0] 中断启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x03);  /// Intc 中断启动

    microblaze_enable_interrupts(); // CPU 中断启动
}

void My_ISR()
{
	xil_printf("Interrupt Triggered!\n");
	int status;
    status = Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET);  // 读取 ISR
    if (status & 0x01) // ISR[1] = 1, 说明是 switch 中断
    {
    	xil_printf("swTriggered!\n");
    	SwitchHandler();
    }
    if (status & 0x02)  // ISR[0] = 1, 说明是 PushButton 中断
    {
    	PushBtnHandler();
    	xil_printf("btnTriggered!\n");
    }
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET,status); // 写 IAR 清除 INTC 中断标志
}

void SwitchHandler()
{
	sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET);  // 读取 switch 开关的状态值
    flag_Sw = 1;  // 告诉主函数，此时出现了开关的波动状态
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x03); // 写 1 清除中断标志位
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET));
}

void PushBtnHandler()
{
    btn = Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET);  // 读取 Button 按钮的状态值
    flag_Btn = 1;    // 告诉主函数，此时出现了开关的波动状态
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_IER_OFFSET, 0x01);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00); /// 清除中断标志位
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x03);
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_ISR_OFFSET));
}

