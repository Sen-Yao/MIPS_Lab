// timer 接中断0，SPI 接中断1
// DA 接Pmod JA
//SS0,SCLK0,MOSI

#include "xil_io.h"
#include "xintc_l.h"
#include "xspi_l.h"

void My_ISR() __attribute__ ((interrupt_handler));

u16 volt,lastvolt;

int main()
{
    // 设置 SPI 为主设备，CPOL = 1, CPHA = 0，自动方式，高位优先传送
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_CR_OFFSET, XSP_CR_ENABLE_MASK | XSP_CR_MASTER_MODE_MASK|XSP_CR_CLK_POLARITY_MASK);
    // 设置 SSR 寄存器
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_SSR_OFFSET, 0xfffffffe);
    // 开放 SPI 发送寄存器中断
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IIER_OFFSET, XSP_INTR_TX_EMPTY_MASK);
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DGIER_OFFSET, XSP_GINTR_ENABLE_MASK);


    // 中断控制器 intr0 中断源使能
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IER_OFFSET, XPAR_AXI_QUAD_SPI_0_IP2INTC_IRPT_MASK);
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_MER_OFFSET, XIN_INT_MASTER_ENABLE_MASK|XIN_INT_HARDWARE_ENABLE_MASK);

    // 处理器中断使能
    microblaze_enable_interrupts();
    // 启动传输，发送数据
    Xil_OUT16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTROFFSET,0);
    while(1);
}

void My_ISR(){
    // 清楚 SPI 中断装起寄存器
    Xil_Out32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET, Xil_In32(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_IISR_OFFSET));
    // 写 INTC 中断响应寄存器
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + XIN_IAR_OFFSET, xil_In32(XPAR_AXI_INTC_0_BASEADDR + XSP_ISR_OFFSET));
    volt++;
    Xil_Out16(XPAR_AXI_QUAD_SPI_0_BASEADDR + XSP_DTR_OFFSET, volt&0xffff);//启动SPI 使能，产生时钟和片选信号
}