/*
 * RAM.c
 *
 *  Created on: Feb 3, 2021
 *      Author: BAC
 */
#include "RAM.h"
#include "main.h"
#include <stdint.h>
#include <stdio.h>
uint8_t ram[RAM_SIZE]; // 64kb of ram
uint8_t is_addr_in_RAM_space(uint16_t addr);

void ram_initialize(){
	for(int32_t i;i<RAM_SIZE;i++) ram[i]=0;
}
void ram_write(uint16_t addr,uint8_t data){
	/* check if the given address is in RAM space*/
	if(is_addr_in_RAM_space(addr)){
		ram[addr]=data;
	}
}

void ram_read(uint16_t addr){
	if(is_addr_in_RAM_space(addr)) return ram[addr];
	else return 0;
}
/* check if the given address is in RAM space*/
uint8_t is_addr_in_RAM_space(uint16_t addr){
	if(addr<RAM_BEGIN_ADDR || addr>(RAM_BEGIN_ADDR+RAM_SIZE-1)) return 0;
	else return 1;
}
