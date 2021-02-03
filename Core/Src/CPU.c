/*
 * CPU.c
 *
 *  Created on: Feb 3, 2021
 *      Author: BAC
 */
//TODO: implement cpu_opcode_to_addr_mode()
//TODO: implement cpu_opcode_to_cycle()
#include "CPU.h"
#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "BUS.h"
/*define all register of CPU*/

uint8_t cpu_A_reg;
uint8_t cpu_X_reg;
uint8_t cpu_Y_reg;
uint8_t cpu_status_reg;
uint16_t cpu_PC; //cpu program counter
uint8_t cpu_SP; // stack pointer
/* internal helper variable*/
uint8_t cpu_remain_cycle=0;
uint8_t cpu_fetched=0;
uint16_t cpu_addr_abs=0;
uint16_t cpu_addr_rel=0;
uint32_t cpu_clk_count=0; // how many clock cpu has excuted since reset
void cpu_set_flag(flag_t flag){
	cpu_status_reg |=(1<<flag);
}
void cpu_clear_flag(flag_t flag){
	cpu_status_reg &=(~(1<<flag));
}
uint8_t cpu_get_flag(flag_t flag){
	return (cpu_status_reg>>flag)&0x1;
}
void cpu_reset(){
#ifdef DEBUG_CPU
	printf("CPU Reset\r\n");
#endif
	/*we find the begin of program, stored in 0xfffc and 0xfffd*/
	uint16_t lo=bus_read(0xfffc);
	uint16_t hi=bus_read(0xfffd);
	cpu_PC=(hi<<8)|lo; // set program counter to begin address
	/* set default state register*/
	cpu_A_reg=0;
	cpu_X_reg=0;
	cpu_Y_reg=0;
	cpu_SP=0xFD;
	status=0|(0x1<<U_FLAG);
	cpu_addr_abs=0;
	cpu_addr_rel=0;
	cpu_fetched=0;
	cpu_clk_count=0;
	cpu_remain_cycle=8; // it takes 8 clock cycle to reset cpu
}
void cpu_clock(){
	/*check if the last instruction is finished*/
	if(cpu_remain_cycle>0){
		cpu_remain_cycle--;
	}
	else{
		uint8_t opcode=bus_read(cpu_PC); // get opcode
		cpu_set_flag(U_FLAG); // u_flag always 1
		cpu_PC++;
		cpu_remain_cycle=cpu_opcode_to_cycle(opcode); // how many cycles need for this opcode
		cpu_remain_cycle=cpu_fetch(opcode); // get data for instruction
		cpu_remain_cycle=cpu_operate(opcode); // execute instruction
	}
	cpu_clk_count++;
}
void cpu_irq(){
	//check if interrupt is allowed
	if(cpu_get_flag(I_FLAG)) return; // 1= not allow
	// get program counter
	uint16_t hi = (pc>>8)&0xff;
	uint16_t lo = pc&0xff;
	//push program counter to stack
	bus_write(0x0100+cpu_SP,(uint8_t)hi);
	cpu_SP--;
	bus_write(0x0100+cpu_SP,(uint8_t)lo);
	cpu_SP--;
	//push status register to stack
	cpu_clear_flag(B_FLAG);
	cpu_set_flag(U_FLAG);
	cpu_set_flag(I_FLAG);
	bus_write(0x0100+cpu_SP,cpu_status_reg);
	cpu_SP--;
	// read the IRQ program address
	lo=bus_read(0xFFFE);
	hi=bus_read(0xFFFE+1);
	pc=(hi<<8)|lo;
	//IRQ takes 7 cycle
	cpu_remain_cycle=7;
}
void cpu_nmi(){
	/*same as cpu_irq, but we dont check I_Flag since this is non-maskable interupt*/
	uint16_t hi = (pc>>8)&0xff;
	uint16_t lo = pc&0xff;
	//push program counter to stack
	bus_write(0x0100+cpu_SP,(uint8_t)hi);
	cpu_SP--;
	bus_write(0x0100+cpu_SP,(uint8_t)lo);
	cpu_SP--;
	//push status register to stack
	cpu_clear_flag(B_FLAG);
	cpu_set_flag(U_FLAG);
	cpu_set_flag(I_FLAG);
	bus_write(0x0100+cpu_SP,cpu_status_reg);
	cpu_SP--;
	// read the IRQ program address
	lo=bus_read(0xFFFA);
	hi=bus_read(0xFFFA+1);
	pc=(hi<<8)|lo;
	//NMI takes 8 cycle
	cpu_remain_cycle=8;
}

void cpu_fetch(uint8_t opcode){
	uint8_t require_cycle=0; // some opcode require additional cycle, we have to return that
	addr_mode_t addr_mode = cpu_opcode_to_addr_mode(opcode);
	switch(addr_mode){
	case IMP: //Implied mode, no need to fetch data
		cpu_fetched=0;
		break;
	case ACC: //Accumulator mode, no need to fetch data
		cpu_fetched=0;
		break;
	case IMM: //immediate mode, next memory is fetched
		cpu_fetched=bus_read(cpu_PC);
		cpu_PC++;
		break;
	case ZP: //zero page
		cpu_addr_abs=bus_read(cpu_PC);
		cpu_PC++;
		cpu_addr_abs&=0xff;
		cpu_fetched=bus_read(cpu_addr_abs);
		break;
	case ZPX: //zero page with X offset
		cpu_addr_abs=bus_read(cpu_PC)+cpu_X;
		cpu_PC++;
		cpu_addr_abs&=0xff;
		cpu_fetched=bus_read(cpu_addr_abs);
		break;
	case ZPY: //zero page with Y offset
		cpu_addr_abs=bus_read(cpu_PC)+cpu_Y;
		cpu_PC++;
		cpu_addr_abs&=0xff;
		cpu_fetched=bus_read(cpu_addr_abs);
		break;
	case REL:

		break;
	case ABS:
		break;
	case ABSX:
		break;
	case ABSY:
		break;
	case IND:
		break;
	case IDX_IND:
		break;
	case IND_IDX:
		break;
	default:
		printf("ErrorCPU opcode to address mode: unknow address mode\r\n");
		printf("Opcode:%d\r\n",opcode);
		cpu_printf_reg();
	}
}


















/*print all register data to console*/
void cpu_printf_reg(){
	printf("A Reg:%d 		X Reg:%d 		Y Reg:%d \r\n",cpu_A_reg,cpu_X_reg,cpu_Y_reg);
	printf("PC:%d\r\n",cpu_PC);
	printf("SP:%d\r\n",cpu_SP);
	printf("Status:%d\r\n",cpu_status_reg);
	printf("Current clock count%ld\r\n",cpu_clk_count);
}
