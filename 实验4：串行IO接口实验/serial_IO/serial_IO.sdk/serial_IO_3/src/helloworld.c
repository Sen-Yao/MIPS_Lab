/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

// timer 接中断0，SPI 接中断1，switch 接中断2
// DA 接 Pmod JA
//SS0,SCLK0,MOSI
// sw 为  GPIO_0的0

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
void SPIHandler();
void SwitchHandler();

short flag_Sw,flag_SPI;  // 按键标志位
short sw;  // 按键赋值

int main()
{
	xil_printf("\r\nRunning!\r\n");
	Initialize();
    while(1)
    {
    	if(flag_Sw)  // 若拨动 switch 开关，则打印相关信息
        {
            xil_printf("flag_Sw = %X, Switch Interrupt Triggered! The result is 0x%X\n\r", flag_Sw, sw);
            flag_Sw = 0;
        }
    	if(flag_SPI)
        {
            xil_printf("flag_SPI = %X, SPI Interrupt Triggered!\n\r", flag_SPI);
            flag_SPI = 0;
        }


    }
}
void Initialize()
{
    flag_Sw = 0x00;
    sw = 0x00;
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xff);  // sw 作为输入
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_IER_OFFSET, 0x01);  // 端口 0 打开
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_GIE_OFFSET, XGPIO_GIE_GINTR_ENABLE_MASK);  // sw 中断启动

    // 以下为 SPI 初始化
    // 设置 SPI 为主设备，CPOL = 1, CPHA = 0，自动方式，高位优先传送
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_CR_OFFSET, XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK|XSP_CR_CLK_POLARITY_MASK);
    // 设置 SSR 寄存器
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_SSR_OFFSET, 0xfffffffe);
    // 开放 SPI 发送寄存器中断
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IIER_OFFSET, XSP_INTR_TX_EMPTY_MASK);
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DGIER_OFFSET, XSP_GINTR_ENABLE_MASK);

    // 中断控制器 intr0 中断源使能
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // intc[2:0] 启动
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);// intc[2:0] 启动

    microblaze_enable_interrupts(); // CPU 中断启动
    // 启动传输，发送数据
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET,0);
}


void My_ISR(){
	xil_printf("Interrupt Triggered!\n");
	int status;
	status = Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET);  // 读取 ISR
	if (status & 0x04) // ISR[2] = 1, 说明是 switch 中断
	{
		xil_printf("swTriggered!\n");
		SwitchHandler();
	}
	if (status & 0x02) //ISR[1] = 1, 说明是  SPI 中断
	{
		xil_printf("SPITriggered!\n");
		SPIHandler();
	}
    // 写 INTC 中断响应寄存器
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET));
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
	// 清楚 SPI 中断装起寄存器
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET, Xil_In32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET));
    volt++;
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET, volt&0xffff);//启动SPI 使能，产生时钟和片选信号
    xil_printf("SPI voltage++!\n");
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // 写 1 清除中断标志位
}
