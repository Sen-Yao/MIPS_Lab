.data
	a .word 0
	b .word 0
	c .word 040
	message1 .asciiz The address is 
	message2 .asciiz nThe value is 

.text
.globl main # 声明全局变量 main
main # main 的进入点
	la $t0, a #$t0=a 的地址
	la $t1, b #$t1=b 的地址
	li $v0, 5
	syscall
	sw $v0, 0($t0) # a 的新值
	li $v0, 5
	syscall
	sw $v0, 0($t1)# b 的新值
	lw $t0, 0($t0)
	lw $t1, 0($t1)
	la $t4, c # $t4=c 的地址
	mul $t2, $t0, $t1 # $t2=ab
	add $t3, $t0, $t1 # $t3=a+b
	sll $t3, $t3, 2 # 偏移量乘4
	add $t3, $t3, $t4 # 计算偏移量，此时 $t3 应为c[a+b] 的地址
	sw $t2, 0($t3)
	
	la $a0 message1
	li $v0, 4
	syscall
	move $a0, $t3 # $a0=a+b
	li $v0, 34
    	syscall # 以十六进制打印地址
    	li $v0, 4
    	la $a0 message2
    	syscall
    	li $v0, 1
    	lw $a0, 0($t3)
	syscall # 打印 ab
li $v0, 10 # 把 10 赋给 v0，表示 main 函数已经返回，类似于 C 语言中的中括号
syscall
# Exit program
