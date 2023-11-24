#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <vector>
#include <string>

#ifndef AST_H_
#define AST_H_

#define CLUSTERWIDTH 4
#define NOPFILL 0
#define LINEWIDTH 32
#define WORDWIDTH 8
#define INPUTFILE "stdin"
#define OUTPUTFILE "stdout"
#define MEMWIDTH 6
#define QUANTIZER_STACKCOUNTING_BITWIDTH 6
#define COUNTER_INFINITE (1<<QUANTIZER_STACKCOUNTING_BITWIDTH) - 1
#define OPCODE_LEN 7
#define ASCII_BITWIDTH 8


typedef enum {CMPLX_EXP,
    OR_EXP, AND_EXP, RANGE_EXP, NOT_EXP, NOT_OR_EXP, OPCP_EXP, OPCPSTAR_EXP,
    OPCPLUS_EXP, OPCPOR_EXP, JIM_EXP, NOOP_EXP, QUANTIFIER_EXP, LAZY_EXP, QUANTIFIER_LAZY_EXP, 
    ANCHOR_SOL_NODE, ANCHOR_EOL_NODE
}ExpKind;

typedef struct node {
    ExpKind        eKind;
    struct node    *child;
   	struct node    *brother;
    char           *reference;
    int            referenceLength;
   	int            absAddress;
    int            jump;
}AstNode;

typedef struct opt {
   	char           *outputFile;
    char           *inputFile;
    int            lineWidth;
   	int            wordWidth;
    int            automata;
    int            clusterWidth;
    int 		   nopFill;
    FILE           *fileInPtr;
    FILE           *fileOutPtr;
    int            numInstr;
}OptStruct;

typedef AstNode *AstNodePtr;

AstNodePtr new_ExprNode(ExpKind kind, char *reference, int referenceLength);
void delete_ExprNode(AstNodePtr node);
void printAST(AstNodePtr root, int nspace);
void printOp(ExpKind kind);

/* da spostare nella nuova apposita libreria*/
void translateAST(OptStruct *opt, AstNodePtr root);
void fixAST(AstNodePtr root, int callOpCounter, int tot);
void backend(OptStruct *opt, AstNodePtr root, ExpKind parentEkind, int negate);
void intOptoStr(OptStruct *opt, char v[], ExpKind ekind, int negate);
void extOptoStr(OptStruct *opt, char v[], ExpKind ekind);
void callOptoStr(OptStruct *opt, char v[], ExpKind ekind);
void integerToAscii(int character, char ascii[], int size);
int isInternalOp(ExpKind kind);
int isCallOp(ExpKind kind);	
void stringToReference(OptStruct *opt, char referenceStr[], int referenceLength, char reference[]);
void OptStructInit(OptStruct *opt);
void nopFill(OptStruct *opt, int numInstr);
int middleend(AstNodePtr root, int instrCount, ExpKind brotherEkind, int callOpCounter, ExpKind parentEkind);
void jumpCalculator(AstNodePtr root, int jumpAddress, ExpKind parentEkind);
int addressMgmt(AstNodePtr root, int idx);
int instructionCounter(AstNodePtr root);
int fixLazy(AstNodePtr root);

#endif