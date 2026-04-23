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

unsigned binaryToDecimal(unsigned *binary, int length) {
    unsigned dec = 0;
    unsigned base = 1;
    
    for (int i = length - 1; i >= 0; i--) {
        if (binary[i] == 1) {
            dec += base;
        }

        base = base * 2;
    }
    
    return dec;
}

/* ALU */
/* 10 Points */
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
    // Recieves decoded instruction from ALU_operations, ALUControl is your instruction
    // it is the responsibility of an instruction function to split the R-type I-type and J-type
    // instructions into A and B for this function.
    // if ALUresult is zero then Zero must be set to 1
    /*
    000 0 - add, addi, lw, sw, j
    001 1 - sub, beq
    010 2 - slt, slti
    011 3 - sltu
    100 4 - and
    101 5 - or
    110 6 - lui
    111 7 - not sure if needed for the assignment
    */ 
    // Unsure if slt, slti, and sltu are exactly the same. However this merges them to the same case
    if (ALUControl == 3) ALUControl = 2; 
     
    switch (ALUControl) {
        case 0:
            *ALUresult = A + B;
            break;
        case 1:
            *ALUresult = A - B;
            break;
        case 2:
            if (A < B) 
                *ALUresult = 1;
            else
                *ALUresult = 0;
            break;
        case 4:
            *ALUresult = A & B;
            break;
        case 5:
            *ALUresult = A || B;
            break;
        case 6:
            *ALUresult = B << 16;
            break;
    }


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
    unsigned rs[5];
    unsigned rt[5];
    unsigned rd[5];
    unsigned iConstant[16];
    unsigned address[26];
    unsigned shamt[5];
    unsigned functa[6];
    // as instructions are fed in as decimals they must be converted to binary
    unsigned *instructionArray = decToBinaryArray(instruction);

    //obtain OP code and define instruction format
    unsigned opArray[6];
    for (int i = 0; i < 6; i++) {
        opArray[i] = instructionArray[i];
        // printf("%d", opArray[i]);
    }

    *op = binaryToDecimal(opArray, 6);
    // 0 : r-type
    // 2-3 : j-type
    // < 3 : i-type
    // an op of 1 is not defined for our assignment, not sure how to handle it.

    char instructionType;
    if (*op > 3)
        instructionType = 'I';
    else if (*op > 1)
        instructionType = 'J';
    else
        instructionType = 'R';

    switch (instructionType) {
        case 'I':
            // 6op 5rs 5rt 16constant/address
            for (int i = 0; i < 5; i++) {
                rs[i] = instructionArray[i+6];
                rt[i] = instructionArray[i+11];
            }
            for (int i = 0; i < 16; i++) 
                iConstant[i] = instructionArray[i+16];

            *r1 = binaryToDecimal(rs, 5);
            *r2 = binaryToDecimal(rt, 5);
            *r3 = binaryToDecimal(iConstant, 16);
            break;
        case 'J':
            // 6op 26address
            for (int i = 0; i < 26; i++) 
                address[i] = instructionArray[i+6];

            *jsec = binaryToDecimal(address, 26);
            break;
        case 'R':
            // 6op 5rs 5rt 5rd 5shamt 6funct
            for (int i = 0; i < 5; i++) {
                rs[i] = instructionArray[i+6];
                rt[i] = instructionArray[i+11];
                rd[i] = instructionArray[i+16];
                shamt[i] = instructionArray[i+21];
            }
            for (int i = 0; i < 6; i++) 
                funct[i] = instructionArray[i+26];

            *r1 = binaryToDecimal(rs, 5);
            *r2 = binaryToDecimal(rt, 5);
            *r3 = binaryToDecimal(rd, 5);
            *shamt = binaryToDecimal(shamt, 5);
            *funct = binaryToDecimal(funct, 6);
            break;
    }
    free(instructionArray);

    // print instruction in partitioned decimal format
    // printf("%d %d %d %d",*op, *r1, *r2, *r3);

    // print binary instruction
    // for (int i = 0; i < 32; i++)
    //     printf("%u", binaryArray[i]);

    // prints decimal instruction
    // printf("%u\n", instruction); 
}



/* instruction decode */
/* 15 Points */
int instruction_decode(unsigned op,struct_controls *controls)
{
    // assign values to controls off op code
    // all instructions by 
    // name, binary, decimal, control value
    // add 000000 0 111 100000 32 000
    // sub 000000 0 111 100010 34 001
    // addi 001000 8 000
    // and 000000 0 111  100100 36 100
    // or 000000 0 111 100101 37 101
    // lw 100011 35 000 | addition is used to add the offset to the address
    // sw 101011 43 000 | addition is used to add the offset to the address
    // lui 001111 15 110
    // beq 000100 4 001 | subtraction as x - y == 0 determines equality
    // slt 000000 0 111 101010 42 010
    // slti 001010 10 010 
    // sltu 000000 0 111 101011 43 011
    // j 000010 2 000 | dont care

    switch (op) {
        case 0:
            controls->ALUOp = '7';
            controls->ALUSrc = '2';
            break;
        case 8:
            controls->ALUOp = '0';
            controls->ALUSrc = '0';
            break;
        case 35:
            controls->ALUOp = '0';
            controls->ALUSrc = '0';
            controls->MemRead = '1';
            controls->RegWrite = '1';
            break;
        case 43:
            controls->ALUOp = '0';
            controls->ALUSrc = '0';
            controls->MemWrite = '1';
            break;
        case 15:
            controls->ALUOp = '6';
            controls->ALUSrc = '0';
            controls->RegWrite = '1';
            break;
        case 4:
            controls->ALUOp = '1';
            controls->ALUSrc = '2';
            controls->Branch = '1';
            break;
        case 10:
            controls->ALUOp = '2';
            controls->ALUSrc = '0';
            break;
        case 2:
            controls->ALUOp = '0';
            controls->ALUSrc = '2';
            controls->Jump = '1';
            controls->Branch = '1';
            break;
    }

    return 0;
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
	if((offset >> 15) == 1){
		*extended_value = offset | 0xffff0000;
	} else {
		*extended_value = offset & 0x0000ffff;
	}
}

/* ALU operations */
/* 10 Points */
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,char *Zero)
{
    // data1 is rt
    // data2 is rd or constant
    // if extended value exists then use it with lui
    // if funct exists then an r-type needs to be send
    // ALUControl will be ALUOp or funct if r-type
    // ALUresult and Zero are managed exclusively by ALU() 
    char ALUControl;
    if (ALUOp != '7')
        ALUControl = ALUOp;
    else
        switch (funct) {
            case 32:
                ALUControl = '0';
                break;
            case 34:
                ALUControl = '1';
                break;
            case 36:
                ALUControl = '4';
                break;
            case 37:
                ALUControl = '5';
                break;
            case 42:
                ALUControl = '2';
                break;
            case 43:
                ALUControl = '3';
                break;
        }

    if (extended_value) data2 = extended_value;

    ALU(data1, data2, ALUControl, ALUresult, Zero);

    //TODO fault conditions
    //TODO Change data1 or data2 to comply with ALUSrc
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




