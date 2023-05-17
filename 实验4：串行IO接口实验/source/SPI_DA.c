// 所有实验任务要求采用中断方式实现，中断方式时，GPIO 输入、延时、UART\SPI 接口通信都采用中断方式实现。
// 利用 SPI IP 核，timer IP 核以及 DA 模块，控制 DA 模块输出周期可变锯齿波，且锯齿波周期由 switch 控制。
// 锯齿波周期最长约为 1 s，最短约为 60 ms 利用 SPI IP 核，timer IP 核以及 DA 模块，控制 DA 模块输出周期可变锯齿波，且锯齿波周期由 switch 控制。
// 锯齿波周期最长约为 1 s，最短约为 60 ms
// 提示：switch 输入的数据，控制定时计数器的定时时间，定时计数器定时时间到，输出一个新数据到 DA 转换器。


// 说明
// SPI 用于连接 DA 模块，在板子上，将 DA 接 Pmod JA

// 在硬件平台搭建中，将 timer 接中断 0，SPI 接中断 1，switch 接中断 2；sw 为 GPIO_0 的 0
//
// DA 模块的三根线分别为 SS0,SCLK0,MOSI


#include "xil_io.h"
#include "xintc_l.h"
#include "xspi_l.h"
#include "xintc_l.h"  // Interrupt Controller API functions
#include "stdio.h"
#include "xgpio_l.h"
#include "xtmrctr.h"

void My_ISR() __attribute__ ((interrupt_handler));

u16 volt,lastvolt;
void Initialize();
void TimerHandler();
void SPIHandler();
void SwitchHandler();

short flag_Sw,flag_SPI,flag_Timer;  // 按键标志位
short sw = 15;  // 按键赋值

int to_set_value; // 在本次试验中，采用减计数。由于本次实验开发板采用的是 Nexys4 DDR，其频率为 100 MHz，因此我们在配置数时，需要做对应的调整。

int main()
{
	// xil_printf("\r\nRunning!\r\n");
	Initialize();
    while(1)
    {
    	if(flag_Sw)  // 若拨动 switch 开关，则打印相关信息
        {
            xil_printf("flag_Sw = %X, Switch Interrupt Triggered! The result is 0x%X\n", flag_Sw, sw);
            // 开关拨动，说明此时应写入的数据 to_set_value 发生了改变。首先算出需要的对应的 to_set_value。
            // 题目要求周期最长为 1s，最短为 60ms 左右。由于此 DA 芯片输入最大为 4096，如果周期为 1s，而计时周期为 100 MHz，则每过 1s/4096 = 244 约等于 256 个周期。因此我们可以将 to_set_value 设置为：
            xil_printf("Now the period is %d\n", to_set_value * 4);
            // 重新设置完周期后，在下一个 Timer 中断时，就会装在新的 to_set_value，从而开始采用新的周期
            flag_Sw = 0;
        }
        if(flag_Timer)
        {
            //xil_printf("time's up!\n");
            flag_Timer = 0;
            // xil_printf("Now the period is %d\n", to_set_value * 4);
        }
    	if(flag_SPI)
        {
            // xil_printf("SPI Interrupt Triggered!\n");
            flag_SPI = 0;
    	    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET,volt);//启动 SPI 使能，产生时钟和片选信号。数字信号对应的有效位为低 12 位，因此在这里将volt 与 0xfff 相与之后再输出
    	    // while((Xil_In32(XPAR_AXI_QUAD_SPI_0_BASEADDR + 0x64)&0x4) == 0x4);
    	    for (int i = 0; i<10000; i++);
        }
    }
}
void Initialize()
{
    flag_Sw = 0x00;
    sw = 0x00;

    // 以下为 switch 初始化
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xff);  // sw 作为输入
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_IER_OFFSET, 0x01);  // 端口 0 打开
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_GIE_OFFSET, XGPIO_GIE_GINTR_ENABLE_MASK);  // sw 中断启动
    // switch 初始化完成

    // 以下为 SPI 初始化
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_CR_OFFSET, XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK|XSP_CR_CLK_POLARITY_MASK);  // 首先设置 SPI 接口的通信模式。设置 SPI 为主设备，CPOL = 1, CPHA = 0，自动方式，高位优先传送。空闲时始终为高电平，因此在上升沿采样数据
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_SSR_OFFSET, 0xfffffffe);  // 设置 SSR 寄存器。由于只连接了一个从设备，因此只需要 SS0 为 0，其余为高电平
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IIER_OFFSET, XSP_INTR_TX_EMPTY_MASK);  // 开放 SPI 发送寄存器中断，若发送完数字信号，则 SPI 产生中断
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DGIER_OFFSET, XSP_GINTR_ENABLE_MASK);  // 开启 SPI 接口的中断输出。

    // 以下为 timer 初始化
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET,tcsr0 & (~ XTC_CSR_ENABLE_TMR_MASK));  // 令 ENT = 0, 停止计时
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET, to_set_value);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值

    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK);  //清除中断标志：TINT = 1；允许 TIMER 中断：ENINT=1;
    // timer 初始化完成

    // 中断控制器 intr0 中断源使能
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // intc[2:0] 启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);// intc[2:0] 启动

    microblaze_enable_interrupts(); // CPU 中断启动
    // 启动传输，发送数据。由于 SPI 接口的中断是来自于发送寄存器空，因此需要向 SPI 的发送寄存器写数据，引起 SPI 接口开始传输。当传输完成后，发送寄存器就为空，才能产生中断
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET,0);
}


void My_ISR(){
	int status;
	// xil_printf("Now the period is %d\n", to_set_value * 4);
	status = Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET);  // 读取 ISR
	if (status & 0x04) // ISR[2] = 1, 说明是 switch 中断
	{
		xil_printf("swTriggered!\n");
		SwitchHandler();
	}
	if (status & 0x02) //ISR[1] = 1, 说明是 SPI 中断
	{
		//xil_printf("SPITriggered!\n");
		SPIHandler();

	}
    if (status & 0x01) //ISR[0] = 1, 说明是 Timer 中断
	{
		//xil_printf("TimerTriggered!\n");
		TimerHandler();

	}
    // 写 INTC 中断响应寄存器
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET));
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // intc[2:0] 启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);// intc[2:0] 启动
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET, to_set_value);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值
    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TLR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK); // 清除中断标志：写 TINT=1（这是 Timer 的特定寄存器功能）;

}

void SwitchHandler()
{
	sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET);  // 读取 switch 开关的状态值
    flag_Sw = 1;  // 告诉主函数，此时出现了开关的波动状态

    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // 写 1 清除中断标志位
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET));
}
void SPIHandler()
{
	flag_SPI = 1;
	// 清除 SPI 中断寄存器
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET, Xil_In32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET));
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET));
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_CR_OFFSET, XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK|XSP_CR_CLK_POLARITY_MASK);  // 首先设置 SPI 接口的通信模式。设置 SPI 为主设备，CPOL = 1, CPHA = 0，自动方式，高位优先传送。空闲时始终为高电平，因此在上升沿采样数据
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_SSR_OFFSET, 0xfffffffe);  // 设置 SSR 寄存器。由于只连接了一个从设备，因此只需要 SS0 为 0，其余为高电平
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IIER_OFFSET, XSP_INTR_TX_EMPTY_MASK);  // 开放 SPI 发送寄存器中断，若发送完数字信号，则 SPI 产生中断
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DGIER_OFFSET, XSP_GINTR_ENABLE_MASK);  // 开启 SPI 接口的中断输出。


}

void TimerHandler()
{
    flag_Timer = 1;
	// 清除 SPI 中断寄存器
    volt++; // 由于要求输出锯齿波，因此每输出一次，电压++，
    if (volt == 4095)
    {
    	volt = 0;
    }

    to_set_value = 960 * (sw + 1);
    //xil_printf("Now sw is %d, the period is %d\n", sw, to_set_value * 4);
    // 每次 Timer 中断触发后，理论上需要重新装载计时器
    int tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET);  // tcsr0 意思是 定时器 0 控制寄存器，有 32 位，但只有 0～10 位有效。这里我们读出 TCSR 寄存器，写入这里的变量 tcsr0
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TLR_OFFSET, to_set_value);  // TLR 为预置数寄存器，这里需要写 LDR 寄存器，将其赋值为 to_set_value 配置计数初始值
    tcsr0 = Xil_In32(XPAR_TMRCTR_0_BASEADDR+XTC_TLR_OFFSET);  // 更新 tcsr0 中的数据
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | XTC_CSR_LOAD_MASK);  // LOAD = 1，将 TLR 装载到 TCR，准备开始计数
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, tcsr0 | ((XTC_CSR_ENABLE_TMR_MASK | XTC_CSR_DOWN_COUNT_MASK | XTC_CSR_AUTO_RELOAD_MASK) & (~XTC_CSR_LOAD_MASK)));  // ENT = 1，运行定时器，UDT = 1, 减计数 ，自动装载:ARHT=1,LOAD =0;
    Xil_Out32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET, Xil_In32(XPAR_TMRCTR_0_BASEADDR + XTC_TCSR_OFFSET) | XTC_CSR_INT_OCCURED_MASK|XTC_CSR_ENABLE_INT_MASK); // 清除中断标志：写 TINT=1（这是 Timer 的特定寄存器功能）;
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // 写 1 清除中断标志位

}
