/*
 * CPU.h
 *
 *  Created on: Feb 3, 2021
 *      Author: BAC
 */

#ifndef INC_CPU_H_
#define INC_CPU_H_

typedef enum {C_FLAG=0, Z_FLAG,I_FLAG,D_FLAG,B_FLAG,U_FLAG,V_FLAG,N_FLAG} flag_t;
void cpu_set_flag(flag_t flag);
void cpu_clear_flag(flag_t flag);
#endif /* INC_CPU_H_ */
