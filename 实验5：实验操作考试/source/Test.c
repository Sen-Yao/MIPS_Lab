#include "xintc_l.h"  // Interrupt Controller API functions
#include "xil_io.h"     // I0 functions
#include "xgpio_l.h"
#include "xtmrctr.h"
#define RESET_VALUE 100000000 - 2  // 这里定义给计时器的时间常数。由于是减计数，因此得到 1 秒的延迟，需要写入 100 M - 2，

void My_ISR() __attribute__ ((interrupt_handler));

// __attribute__ 为中断服务子程序。
// 当 MicroBlaze 的中断管脚 Interrupt，CPU 就会在内部的寄存器中自动保存断点
// 并且跳转到这里的子函数

void Initialize(); // 初始化函数 (包含中断初始化)

void PushBtnHandler();
void SwitchHandler();
void TimerHandler();
void Delay_500ms();

short flag_Sw,flag_Btn;  // 按键标志位
short sw, btn;  // 按键赋值
short flag_L = 0;
int i = 0;
int j = 0;

short segcode[16] = {0xc0, 0xf9,  0xa4, 0xb0, 0x99, 0x92, 0x82, 0xf8, 0x80, 0x90, 0x88, 0x83, 0xc6, 0xa1, 0x86, 0x8e};  // 十六进制显示
// char segcode[8] = {0xc0,0xf9} // 用于数码管显示，有时候还得搞显示缓冲区，如下
// 设置全灭，不知道是啥意思。
// 从右到左分别为 abcdefg点，低电平为亮。

// 二维数组进行查询
// char scancode[5][2] = {{0x1, 0xc6},{0x2, 0xc1},{0x4, 0xc7},{0x8, 0x88},{0x10,0 xa1}}; // 二维数组定义，第一列为按键键值，第二列为对应的段码
char pos[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};  // 用于标识段码位置
int main(void)
{
	xil_printf("\r\nRunning CpioInput Interrupt Test(No API)\r\n");
    Initialize();
    while(1)
    {
    	if(flag_Sw)  // 若拨动 switch 开关，则打印相关信息
        {
    		sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET);
    		xil_printf("flag_Sw = %X, Switch Interrupt Triggered! The result is 0x%X\n\r", flag_Sw, sw);
            flag_Sw = 0;
        }
        if(flag_Btn) // 若按下 btn 按键，则打印相关信息
        {
            xil_printf("Button Interrupt Triggered! The result is 0x%X\n\r", btn);
            flag_Btn = 0;
        	sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET);  // 读取 switch 开关的状态值
			i = sw & 0xf;
            if (btn & 0x01) // C
            {
            	j = 0;
            	Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_DATA_OFFSET, segcode[i]);  // segcode 为提前定义好的八位数
            	Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_DATA2_OFFSET, 0xfe);  // pos 为未知

            }
            if (btn & 0x04) // L
            {
            	if (flag_L == 0)flag_L = 1;
            	xil_printf("L=%X\n\r", flag_L);
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

    Xil_Out16(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_TRI2_OFFSET, 0x00);  // LED 为输出



    // 以下为 timer 初始化
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,tcsr0 & (~ XTC_CSR_ENABLE_TMR_MASK));  // 令 ENT = 0, 停止计时
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET,RESET_VALUE);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值

    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK);  //清除中断标志：TINT = 1；允许 TIMER 中断：ENINT=1;
    // timer 初始化完成

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07);  // Intr[1:0] 中断启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);  /// Intc 中断启动

    microblaze_enable_interrupts(); // CPU 中断启动
}

void My_ISR()
{
	xil_printf("Interrupt Triggered!\n");
	int status;
    status = Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET);  // 读取 ISR
    if (status & 0x01) // ISR[1] = 1, 说明是 switch 中断，也可用 XPAR_AXI_GPIO_0_INTERRUPT_MASK 实现
    {
    	xil_printf("swTriggered!\n");
    	SwitchHandler();
    }
    if (status & 0x02)  // ISR[2] = 1, 说明是 PushButton 中断
    {
    	PushBtnHandler();
    	xil_printf("btnTriggered!\n");
    }
    if (status & 0x04)  // ISR[2] = 1, 说明是  Timer 中断
    {
    	TimerHandler();
    	xil_printf("TimerTriggered!\n");
    }
    // 以下为 timer 初始化
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,tcsr0 & (~ XTC_CSR_ENABLE_TMR_MASK));  // 令 ENT = 0, 停止计时
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET,RESET_VALUE);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值

    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK);  //清除中断标志：TINT = 1；允许 TIMER 中断：ENINT=1;
    // timer 初始化完成

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07);  // Intr[1:0] 中断启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);  /// Intc 中断启动


}

void SwitchHandler()
{

    flag_Sw = 1;  // 告诉主函数，此时出现了开关的波动状态
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // 写 1 清除中断标志位
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET));
}

void PushBtnHandler()
{
    btn = Xil_In8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET);  // 读取 Button 按钮的状态值
    flag_Btn = 1;    // 告诉主函数，此时出现了开关的波动状态
    Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_IER_OFFSET, 0x01);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00); /// 清除中断标志位
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07);
    Xil_Out32(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_ISR_OFFSET));
}

void TimerHandler()
{
	if (flag_L){
		j--;
		if (j == -1){
			j = 7;
		}
		Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_DATA_OFFSET, segcode[i]);  // segcode 为提前定义好的八位数
		Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_DATA2_OFFSET, pos[j]);  // pos 为未知

	}

    // 以下为 timer 初始化
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,tcsr0 & (~ XTC_CSR_ENABLE_TMR_MASK));  // 令 ENT = 0, 停止计时
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET,RESET_VALUE);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值

    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK);  //清除中断标志：TINT = 1；允许 TIMER 中断：ENINT=1;
    // timer 初始化完成

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07);  // Intr[1:0] 中断启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);  /// Intc 中断启动

}
void Delay_500ms()
{
	for (int k = 0; k<1000000; k++);
}


/*
若计时器引起的中断，需要清除中断
Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + XTC_TCSR_OFFSET)); // 写 TCSR，清除中断
*/

/*
七段数码管的输出
Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA2_OFFSET, segcode[i]);  // segcode 为提前定义好的八位数
Xil_Out8(XPAR_AXI_GPIO_1_BASEADDR + XGPIO_DATA_OFFSET, pos);  // pos 为未知
*/

// 显示缓冲区的设计

/*
for (index = 0; index<5; index++)
    if (btncode == scannode[index][0]) // 查询按键键值
    {

    }
Xil_Out32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_2_BASEADDR + XGPIO_ISR_OFFSET)); // 清除中断
*/