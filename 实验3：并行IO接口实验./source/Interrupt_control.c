// 中断方式 button 按键以及 switch 输入的测试程序

#include "xparameters.h"    //The hardware configuration describing constants
#include "xintc.h"  // Interrupt Controller API functions
#include "stdio.h"
#include "xil_io.h"     // I0 functions
#include "xil_types.h"
#include "mb interface.h"
#include "xgpio.h"      // GPIO API functions

#define btn_DATA 0x40000000     // button 数据寄存器地址
#define btn_TRI 0x40000004      // button 控制寄存器地址
#define btn_GIER 0x4000011c      // button 中断全局允许寄存器地址
#define btn_IER 0x40000128      // button 中断通道允许寄存器地址
#define btn_ISR 0x40000120      // button 中断状态寄存器地址

#define sw_DATA 0x40040000        // switch 控制寄存器地址
#define sw_TRI 0x40040004      // switch 控制寄存器地址
#define SW_GIER 0x4004011c      // switch 中断全局允许寄存器地址
#define SW_IER 0x40040128      // switch 中断通道允许寄存器地址
#define SW_ISR 0x40040120      // switch 中断状态寄存器地址

#define intc ISR 0x41200000
#define intc IER 0x41200008
#define intc IAR 0x4120000C
#define intc MER 0x4120001C

void My_ISR (void) __attribute__ ((interrupt handler));  

// __attribute__ 为中断服务子程序。
// 当 MicroBlaze 的中断管脚 Interrupt，CPU 就会在内部的寄存器中自动保存断点
// 并且跳转到这里的子函数

void Initialize(); //初始化函数 (包含中断初始化)

void PushBtnHandler();
void SwitchHandler();
void Delay_50ms();

short flag_sw,flag_Btn;  // 按键标志位
short sw, btn;  // 按键赋值

int main(void)
{
    xil_printf("\r\nRunning CpioInput Interrupt Test(No API)\r\n");
    Initialize();
    while(1)
    {
        if(flag_sw) // 若拨动 switch 开关，则打印相关信息
        {
            xil_printf("Switch Interrupt Triggered! The result is 0x%X\n\r", sw);
        }
        if(flag_btn) // 若按下 btn 按键，则打印相关信息
        {
            xil_printf("Button Interrupt Triggered! The result is 0x%X\n\r", btn);
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

    Xil_Out8(sw_TRI, 0xff);  // sw 作为输入
    Xil_Out8(sw_IER, 0x01);  // 端口 0 打开
    Xil_Out32(sw_GIER, 0x80000000);  // sw 中断启动

    Xil_Out8(btn_TRI, 0xff);  // btn 作为输入
    Xil_Out8(btn_IER, 0x01);  // 端口 0 打开
    Xil_Out32(btn_GIER, 0x80000000);  // btn 中断启动

    Xil_Out32(intc_IAR, 0xffffffff);  // 清除所有中断请求
    Xil_Out32(intc_IER, 0x03);  // Intr[1:0] 中断启动
    Xil_Out32(intc_MER, 0x03);  // Intc 中断启动

    microblaze_enable_interrupts(); // CPU 中断启动
}

void My_ISR(void)
{
    int status;
    status = Xil_In32(intc_ISR);  // 读取 ISR
    if (status & 0x01)  // ISR[0] = 1, 说明是 PushButton 中断
    {
        PushHandler();
    }

    Xil_Out32(intc_IAR,status); // 写 IAR 清除 INTC 中断标志
}

void PushBtnHandler()
{
    btn = Xil_In8(btn_DATA);  // 读取 Button 按钮的状态值
    flag_Btn = 1;  // 告诉主函数，此时出现了开关的波动状态
    Xil_Out32(btn_ISR, 0x01); // 清除中断标志位
    Xil_Out8(btn_IER, 0x01);  // 通道 0 打开
}

void Delay_50ms()
{
    int i;
    for (i = 0; i<5000000; i++);
}