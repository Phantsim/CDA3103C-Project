#include "spimcore.h"

// Takes instructions in decimal and outputs an array representing the 32bit instruction
unsigned *decToBinaryArray(unsigned decimalNum)
{
    unsigned tempArray[1000];
    unsigned *binaryArray;

    int i = 0;
    while (decimalNum > 0) {
        tempArray[i] = decimalNum % 2;
        decimalNum = decimalNum / 2;
        i++;
    }

    binaryArray = calloc(32, sizeof(unsigned));

    for (int j = i - 1; j >= 0; j--) {
        binaryArray[i-j+1] = tempArray[j];
    }

    return binaryArray;
}

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Recieves decoded instruction from control, ALUControl is your instruction
    // A and B represent rt, rd for R-type or rt, constant for I-type
    // ALUresult is rs
    // it is the responsibility of an instruction function to split the R-type I-type and J-type
    // instructions into A and B for this function.
    // if ALUresult is zero then Zero must be set to 1?
    /*
    add
    R-type
    
    sub
    addi
    and
    or
    lw
    sw
    lui
    beq
    slt
    slti
    sltu
    sltiu
    j

    */ 
     


}

/* instruction fetch */
/* 10 Points */
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{   // Halt if either of the first two bits are 1, as this indicates that PC is not word aligned.
	if (PC & 0x3) 
		return 1;

    // Halt if PC exceeds 64k bytes
	if (PC > 0xFFFF)
		return 1;

    // right shifting returns the decimal value for the instruction
	*instruction = Mem[PC >> 2];
	return 0;
}


/* instruction partition */
/* 10 Points */
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,unsigned *r2, unsigned *r3, unsigned *funct, unsigned *offset, unsigned *jsec)
{
    // as instructions are fed in as decimals they must be converted to binary
    unsigned *binaryArray = decToBinaryArray(instruction);

    // print binary instruction
    // for (int i = 0; i < 32; i++)
    //     printf("%d", binaryArray[i]);

    // prints decimal instruction
    // printf("%d\n", instruction); 

// unsigned op,	// instruction [31-26]
// 	r1,	// instruction [25-21]
// 	r2,	// instruction [20-16]
// 	r3,	// instruction [15-11]
// 	funct,	// instruction [5-0]
// 	offset,	// instruction [15-0]
// 	jsec;	// instruction [25-0]



    

}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{

}

/* Read Register */
/* 5 Points */
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,unsigned *data2)
{
	*data1 = Reg[r1];
	*data2 = Reg[r2];
}


/* Sign Extend */
/* 10 Points */
void sign_extend(unsigned offset,unsigned *extended_value)
{

}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{

}

/* Read / Write Memory */
/* 10 Points */
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,unsigned *memdata,unsigned *Mem)
{

}


/* Write Register */
/* 10 Points */
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
	if(RegWrite==1){
		if(MemtoReg==1&&RegDst==0){
			Reg[r2] = memdata;
		} else if(MemtoReg==1&&RegDst==1){
			Reg[r3] = memdata;
		}
	}

	if(RegWrite==0){
		if(MemtoReg==0&&RegDst==0){
			Reg[r2] = ALUresult;
		} else if(MemtoReg==0&&RegDst==1){
			Reg[r3] = ALUresult;
		}
	}
}

/* PC update */
/* 10 Points */
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,char Zero,unsigned *PC)
{
	unsigned pc_plus_4 = *PC + 4;
	unsigned pc_next = pc_plus_4;

	if (Branch && Zero)
		pc_next = pc_plus_4 + (extended_value << 2);

	if (Jump)
		pc_next = (pc_plus_4 & 0xF0000000) | (jsec << 2);

	*PC = pc_next;
}

