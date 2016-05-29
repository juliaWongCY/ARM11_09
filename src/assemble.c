#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

//////////////////////////////////STRUCTURE//////////////////////////////////

#include "library/arm11.h"
#include "library/assembler.h"
#include "library/instruction.h"

////////////////////////////////////MACROS/////////////////////////////////////

#include "library/register.h"
#include "library/tokens.h"
#include "library/bitwise.h"



// for numerica constant it's in the form "#x" where x is a natural number
// or in the form "=x" for ldr instr (the expr can be 32 bits after =)

#define max_8bit_represented  256 // 2^8 = 256

int expr_to_num(char *expr)
{
  return (int32_t) strtol(expr, NULL, 0);
}

///////////////////////////// FUNCTION PROTOTYPE //////////////////////////////


char *buffer;
ASSEMBLER_STRUCT *ass = NULL;

TOKEN read_Source(const char *);
void write_File(const char *);

int as_numeric_constant(char *);
int as_shifted_reg_ass(TOKEN *, int);

void funcArray(void);
int32_t assembler_func(TOKEN *line);

int32_t ass_data_proc(TOKEN *, int, int, int, int);
int32_t ass_data_proc_result(TOKEN *);
int32_t ass_data_proc_mov(TOKEN *);
int32_t ass_data_proc_cpsr(TOKEN *);

int32_t ass_multiply(TOKEN *, int, int, int, int, int);
int32_t ass_multiply_mul(TOKEN *);
int32_t ass_multiply_mla(TOKEN *);

int32_t ass_single_data_transfer(TOKEN *, int, char *);
int32_t SDT_num_const(int, int, char *);

int32_t andeq_func(TOKEN *);
int32_t lsl_func(TOKEN *);


///////////////////////Binary file reader //////////////////////////////////////

TOKEN read_Source(const char *sourceFile) {
  FILE *file = fopen(sourceFile, "rs");

  //if (file != NULL) {
    fseek(file, 0, SEEK_END);
    long size = ftell(file); //Size of file in bytes
    fseek(file, 0, SEEK_SET); //go back to start

    buffer = (char *) malloc(size * sizeof(char));
    if(buffer == NULL) {
      perror("Malloc failed.");
    } else {
    fread(buffer, size, 1, file);
    if(ferror(file)) {
      perror("Error reading from sourceFile.\n");
    }

    int sizeBuffer = sizeof(buffer);

    buffer[sizeBuffer - 1]= '\0';

   return tokenise(buffer, ",");

  }

  TOKEN *result = NULL;
  return *result;


//  free(buffer);

//}
//else {
  //perror("Error opening the file.");
//}
// return NULL;
}


void write_File(const char *binaryFile) {
  FILE *file = fopen(binaryFile, "wb"); //w = write b = binary

  int32_t *program = (int32_t*) assemble_generate_bin(ass);
  //get binary code from assembler program

  int size = ass->TOTAL_line * sizeof(int32_t);
  //size of each element that will be written

  fwrite(program, size, 1, file);

  fclose(file);

  free(program);
}

//////////////////////////   Core     //////////////////////////////////////////

//TODO: CHECKKKKKK!!!!!!
int as_numeric_constant(char *value){
  //int num_bit = 0;
  int to_num = expr_to_num(value);
  /*while(num_bit < 32){
    rotate_right(value, 2);
    num_bit += 2;
  }
  */
  if(to_num > max_8bit_represented) {
    perror("numerical constant cannot be represented.");
    exit(EXIT_FAILURE);
  }
  return to_num;
}

// can be either <shiftname><register> or <shiftname><#expression>
//<shiftname> can be either asr, lsl, lsr or ror
//If operand2 is a register the 12-bit is shift(11-4)+Rm(3-0)
  //first case integer(11-7)+shift type(6-5)+0(4)
  //second case shiftReg RS(11-8)+0(7)+shift type(6-5)+1(4)
//TOKEN *elem is a pointer to elems in tokenized line
int as_shifted_reg_ass(TOKEN *line, int Rm)
{
  	char *shift_name = line->tokens[Rm + 1];
  	char *Operand2 = line->tokens[Rm + 2];
  	int  result = 0;

	ShiftReg 	 shiftReg;
  	ShiftRegOptional regOp;
  	int shiftType = expr_to_num(shift_name);

	//in the form <shiftname><#expression>
	if(Is_Expression(Operand2))
	{
  	//+1 to git rid of 'r' but just getting the reg number
 		  shiftReg.Rm = PARSE_REG(Rm - 1);
  		shiftReg.Flag = 0;
  		shiftReg.Type = shiftType;
  		shiftReg.Amount = atoi(Operand2);

  		result = *((int *) &shiftReg);

	} else { //in the form <shiftname><register>
	  //CHECK THE STRUC?!??!
  		regOp.Rm = PARSE_REG(Rm + 2);
  		regOp.Flag1 = 0;
  		regOp.Type = shiftType;
		regOp.Flag2 = 0;
  		regOp.Rs = PARSE_REG(Rm) | (1 << 4);

  //regOp.Rs = atoi(line->tokens[Rm] + 1) << 3; //getting the last bit of Rs

  		result = *((int *) &regOp);
	}

	return result;
}

//to check if operand2 is an expression or a register
int check_op2(TOKEN *line, int op2){
  char *op2 = line->tokens[op2 + 2];

  if(Is_Expression(op2)){
    return as_numeric_constant(op2);
  }
  return as_shifted_reg_ass(line, op2);

}


function_assPtr function_Array[9];

int32_t assembler_func(TOKEN *line) {
  char *mnemonic = line->tokens[0];
  int i = str_to_mnemonic(mnemonic);
  return function_Array[i](line);
}

void funcArray(void) {
  function_Array[0] = ass_data_proc_result;
  function_Array[1] = ass_data_proc_cpsr;
  function_Array[2] = ass_data_proc_mov;
  function_Array[3] = ass_multiply_mul;
  function_Array[4] = ass_multiply_mla;
  //function_Array[5] = ass_branch;
  function_Array[6] = ass_single_data_transfer;
  function_Array[7] = lsl_func;
  function_Array[8] = andeq_func;

}



///////////////////////Instructions ////////////////////////////////////////////


int32_t ass_data_proc(TOKEN *line, int SetCond, int Rn, int Rd, int Operand_2)
{
	char *Operand2 = line->tokens[Operand_2];
	char *mnemonic = line->tokens[0];

	static DataProcessingInstruct *DPInst;

	DPInst->Cond	= AL;
	DPInst->_00	= 0;
	DPInst->ImmOp	= Is_Expression(Operand2);
	DPInst->Opcode	= str_to_mnemonic(mnemonic);
	DPInst->SetCond	= SetCond;
	DPInst->Rn	= PARSE_REG(Rn);
	DPInst->Rd	= PARSE_REG(Rd);
	DPInst->Operand2= check_op2(line, Operand_2);

	return *((int32_t *) &DPInst);
}

int32_t ass_data_proc_result(TOKEN *line)
{
  return ass_data_proc(line, 0, -1, 1, 2);
}

int32_t ass_data_proc_mov(TOKEN *line)
{
  return ass_data_proc(line, 0, -1, 1, 2);
}

int32_t ass_data_proc_cpsr(TOKEN *line)
{
  return ass_data_proc(line, 1,  1, -1, 2);
}

int32_t ass_multiply(TOKEN *line, int Acc, int Rd, int Rm, int Rs, int Rn)
{
	static MultiplyInstruct *MulInst;

	MulInst->Cond	   = AL;
	MulInst->_000000   = 0;
	MulInst->Acc	   = Acc;
	MulInst->SetCond   = 0;
	MulInst->Rd	   = PARSE_REG(Rd);
	MulInst->Rn	   = PARSE_REG(Rn);
	MulInst->Rs	   = PARSE_REG(Rs);
	MulInst->_1001	   = 0;
	MulInst->Rm	   = PARSE_REG(Rm);

	return *((int32_t *) &MulInst);
}

int32_t ass_multiply_mul(TOKEN *line)
{
  return ass_multiply(line, 0, 1, 2, 3, -1);
}

int32_t ass_multiply_mla(TOKEN *line)
{
  return ass_multiply(line, 1, 1, 2, 3,  4);
}

int32_t ass_single_data_transfer(TOKEN *line, int Rd, char *address)
{
  int *Rn    = PARSE_REG(line->tokens[1] + 1);
  char *adr  = line->tokens[2];
  char *mnem = line->tokens[0];

  instr.Cond   = AL;
  instr._01	   = 1;
  instr.I	   = I;
  instr.P	   = P;
  instr.U	   = U;
  instr._00	   = 0;
  instr.L	   = (mnemonic == "ldr")? 0 : 1;
  instr.Rn     = PARSE_REG(2);
  instr.Rd	   = PARSE_REG(1);
  instr.Offset = Offset;


  if (IS_SET(dataL)) {                    // ldr: Load from memory into register
    if (Is_Expression(adr)) {            // In numeric form
      adr++;
      int address = expr_to_num(adr);
      return SDT_num_const(line, Rd, address);
    }

    if (IS_SET(dataP)) {                  // Pre-indexing
      int offset = 0;
      if (Is_Expression(adr[1])) {       //Case offset = <#expression>
        offset = expr_to_num(adr[1]);
      }
      dataRn += (IS_SET(dataU)? dataOffset : -dataOffset);
      IS_SET(dataL) ? (word = MEM_R_32bits(dataRn)) : MEM_W_32bits(dataRn, word);

    } else {                              // Post-indexing
      int offset = expr_to_num(*adr[1]);
      IS_SET(dataL) ? (word = MEM_R_32bits(dataRn)) : MEM_W_32bits(dataRn, word);
      dataRn += (IS_SET(dataU)? dataOffset : -dataOffset);
    }
  }
}

int32_t SDT_num_const(TOKEN *line, int Rd, char *address) {
  if (address <= 0xFF) {                  // Treat as mov Instruction
    adr[0] = '#';
    return ass_data_proc_mov(line);

  } else {
    // use PC to calculate new address
    register[15] += (IS_SET(dataU)? dataOffset : -dataOffset);
  }
}


int32_t ass_branch(TOKEN *line, int Cond, char *expr) {
	char *suffix = "AL";
	if (line->tokens[0] == "b") {
	  *suffix = (line->tokens[0] + 1);
	}
	char *label  = line->tokens[1];
	char *address = PARSE_REG(*expr);

	int Offset = (curr_addr - address + 8) >> 2;  //shift right by 2

	BranchInstr instr;

	instr.Cond   = suffix;
	instr._101   = 101;
	instr._0     = 0;
	instr.Offset = Offset;

	return *(int32_t *) &instr;
}


//////////////////Special Instruction //////////////////////////////////////////

/*andeq func */
//for instr that compute results, the syntax is <opcode> Rd, Rn, <Operand 2>
//andeq is similar to and with cond set to 0000 (eq condition)
//andeq r0, r0, r0
//TOKEN *line is to get 'andeq from the line read'
int32_t andeq_func(TOKEN *line){
  return 0x00000000;
}

/*lsl func */
//note: asprintf() cal the length of the string, allocate that amount of mem and
//write the string into it. it is an implicit malloc need to free afterward
//Compile lsl Rn,<#expression> as mov Rn, Rn, lsl <#expression>
int32_t lsl_func(TOKEN *token_line){
 char *new_token_line = (char* )malloc(511 * sizeof(char));
 sprintf(new_token_line, "mov %s, %s, lsl %s", token_line->tokens[1],
                                                token_line->tokens[1],
                                                token_line->tokens[2]);
TOKEN *new_token = (TOKEN*) malloc(sizeof(TOKEN));
*new_token = tokenise(new_token_line, " ,");
return ass_data_proc_mov(new_token);

//free(new_token_line); TODO: Remeber to free

}

////////////////////A factorial program ////////////////////////////////////////
//ARM stores instructions using Little-endian


///////////////////////// Main /////////////////////////////////////////////////
int main(int argc, char **argv)
{

  if(argc < 3) { // Need two files (+ executer)
    printf("Incomplete number of arguments in input!\n");
    printf("Please type in as first argument : ARM source file\n");
    printf("And as aecond argument : an output ARM binary code file\n");
    exit(EXIT_FAILURE);
  }

  funcArray();

  TOKEN *lines = (TOKEN*) malloc(sizeof(TOKEN));
  *lines = read_Source(argv[1]);
  //get lines of assembly codes

  *ass = assemble(lines, &assembler_func, ",");
   //assemble lines using assembler and get output to write to file

  write_File(argv[2]); //

  //TODO token_free(lines);

  assemble_free(ass);

  return EXIT_SUCCESS;
}
