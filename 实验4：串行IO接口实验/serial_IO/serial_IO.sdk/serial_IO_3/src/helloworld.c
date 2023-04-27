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

// timer ���ж�0��SPI ���ж�1��switch ���ж�2
// DA �� Pmod JA
//SS0,SCLK0,MOSI
// sw Ϊ  GPIO_0��0

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

short flag_Sw,flag_SPI;  // ������־λ
short sw;  // ������ֵ

int main()
{
	xil_printf("\r\nRunning!\r\n");
	Initialize();
    while(1)
    {
    	if(flag_Sw)  // ������ switch ���أ����ӡ�����Ϣ
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
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_TRI_OFFSET, 0xff);  // sw ��Ϊ����
    Xil_Out8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_IER_OFFSET, 0x01);  // �˿� 0 ��
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_GIE_OFFSET, XGPIO_GIE_GINTR_ENABLE_MASK);  // sw �ж�����

    // ����Ϊ SPI ��ʼ��
    // ���� SPI Ϊ���豸��CPOL = 1, CPHA = 0���Զ���ʽ����λ���ȴ���
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_CR_OFFSET, XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK|XSP_CR_CLK_POLARITY_MASK);
    // ���� SSR �Ĵ���
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_SSR_OFFSET, 0xfffffffe);
    // ���� SPI ���ͼĴ����ж�
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IIER_OFFSET, XSP_INTR_TX_EMPTY_MASK);
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DGIER_OFFSET, XSP_GINTR_ENABLE_MASK);

    // �жϿ����� intr0 �ж�Դʹ��
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, 0xffffffff);  // ��������ж�����
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // intc[2:0] ����
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, 0x07);// intc[2:0] ����

    microblaze_enable_interrupts(); // CPU �ж�����
    // �������䣬��������
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET,0);
}


void My_ISR(){
	xil_printf("Interrupt Triggered!\n");
	int status;
	status = Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET);  // ��ȡ ISR
	if (status & 0x04) // ISR[2] = 1, ˵���� switch �ж�
	{
		xil_printf("swTriggered!\n");
		SwitchHandler();
	}
	if (status & 0x02) //ISR[1] = 1, ˵����  SPI �ж�
	{
		xil_printf("SPITriggered!\n");
		SPIHandler();
	}
    // д INTC �ж���Ӧ�Ĵ���
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, Xil_In32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET));
}

void SwitchHandler()
{
	sw = Xil_In8(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_DATA_OFFSET);  // ��ȡ switch ���ص�״ֵ̬
    flag_Sw = 1;  // ��������������ʱ�����˿��صĲ���״̬
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // д 1 ����жϱ�־λ
    Xil_Out32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET, Xil_In32(XPAR_AXI_GPIO_0_BASEADDR + XGPIO_ISR_OFFSET));
}
void SPIHandler()
{
	flag_SPI = 1;
	// ��� SPI �ж�װ��Ĵ���
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET, Xil_In32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET));
    volt++;
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET, volt&0xffff);//����SPI ʹ�ܣ�����ʱ�Ӻ�Ƭѡ�ź�
    xil_printf("SPI voltage++!\n");
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_ISR_OFFSET, 0x00);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, 0x07); // д 1 ����жϱ�־λ
}
