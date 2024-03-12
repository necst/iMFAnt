#include "ast.hpp"
#include "../re2automata/automata.hpp"
#include<cstdio>
#include<cstdlib>
#include<string.h>
#include<ctype.h>

using namespace std;

//creates a new expression node
AstNodePtr new_ExprNode(ExpKind kind, char *reference, int referenceLength){
	AstNodePtr node = NULL;
	int i;
	node = (AstNodePtr)malloc(sizeof(AstNode));
	if(node!=NULL){
		node->eKind = kind;
		node->reference = reference;
		node->referenceLength = referenceLength;
		node->child = NULL;
		node->brother = NULL;
		node->absAddress = 0;
		node->jump = 0;
	}else{
		printf("Error during creation of a new node!\n");
	}
	return node;
}

void delete_ExprNode(AstNodePtr node){
		node->reference = NULL;
		node->child = NULL;
		node->brother = NULL;
		free(node);
}

void printAST(AstNodePtr root, int nspace){
	int i, len;
	if(root==NULL)
		return;
	printf("%*s", nspace,"");
	printf("%d ", root->absAddress);
	printf(" %d ", root->jump);
	printOp(root->eKind);
	if(root->reference != NULL){
		len = root->referenceLength;
		for(i = 0; i < len; i++){
			if(!isprint(*(root->reference + i))){
				if(*(root->reference + i) == '\n')
					printf("\\n");
				else
					if(*(root->reference + i) == '\t')
						printf("\\t");
					else
						if(*(root->reference + i) == '\r')
							printf("\\r");
						else
							printf(" ");
			}
			else{
				printf("%c ", *(root->reference + i));
			}
		}
	}
	printf("\n");
	printAST(root->child, nspace + 1);
	printAST(root->brother, nspace);
}

int instructionCounter(AstNodePtr root){
	int i = 0;
	if(root==NULL)
		return i;
	else {
		return instructionCounter(root->brother) + instructionCounter(root->child) + 1;
	}
}

int addressMgmt(AstNodePtr root, int idx)
{
	if(root==NULL)
		return idx;
		
		idx++;

	root->absAddress = idx-1;
	int n=-1;
		n = addressMgmt(root->child, idx);
	
	n = addressMgmt(root->brother, n);

	return n;

}

int fixLazy(AstNodePtr root){
	if(root==NULL) 
		return 0;
	int actualAdress;
	int callOpCounterTmp;
	AstNodePtr brother, nextBrother, child;
	const char s[2] = ",";
	char *content, *refTmp;

	if(root->eKind == LAZY_EXP && root->child->brother == NULL && root->child->eKind == QUANTIFIER_EXP){
		child = root->child;
		if(!(refTmp = (char *)malloc(sizeof(char)*child->referenceLength)))
			return -1;
		memcpy(refTmp, child->reference, child->referenceLength);
		content = strtok(refTmp, s);
		content = strtok(NULL, s);
		root->child = root->child->child;
		root->reference = child->reference;
		root->referenceLength = child->referenceLength;
		delete_ExprNode(child);
		child = NULL;
		// if(content != NULL){
		// 	root->eKind = QUANTIFIER_LAZY_EXP;
		// } 
		// else{
		root->eKind = QUANTIFIER_EXP;
		// }
		free(refTmp);
		refTmp = NULL;
	}
	// if(root->eKind == QUANTIFIER_LAZY_EXP){
	// 	root->eKind = QUANTIFIER_EXP;
	// }
	fixLazy(root->child);
	fixLazy(root->brother);
}

void fixAST(AstNodePtr root, int callOpCounter, int tot){

	

	if (root == NULL)
		return; 

	int callOpCounterTmp;
	
	// four rules with different priority

	// 1: remove CMPLX operator
	if(root->eKind == CMPLX_EXP){
		root->eKind = OPCP_EXP;
	}
	if(root->eKind == OPCP_EXP && root->child->eKind == CMPLX_EXP){
		AstNodePtr tmp = root->child;
		root->child = root->child->child;
		delete_ExprNode(tmp);
		tmp = NULL;
	}

	// 2 - 3: adjust NOR and OR expressions (bring OR children on the same level when possible)
	if(root->eKind == NOT_EXP){
		// printAST(root, 0);
		root->eKind = NOT_OR_EXP;
		if(root->child->eKind!=RANGE_EXP){
			// cout<<"here is "<<endl; 
			// printAST(root, 0);
			// cout<<"child is "<<endl; 
			
			// printAST(root->child, 0);

			// not expressions' child is always an or, whose children are the actual chars/ranges
			AstNodePtr tmp = root->child, n = NULL, last = NULL;
			while(tmp!=NULL){
				// cout<<"TMP:"<<endl;
				AstNodePtr tmpBro = tmp->brother;
				if(tmp->eKind == OPCP_EXP)
					tmp = tmp->child; 
				// printAST(tmp, 0);
				
				// cout<<"here child is "<<endl; 
				// printAST(tmp, 0);
				if(tmp->eKind==OR_EXP && tmp->referenceLength == 0){
					if(n==NULL){
						n = tmp->child; 
						last = n; 
					} else {
						last->brother = tmp->child; 
						last = last->brother; 
					}
				} else {
					// printAST(tmp, 0);
					if(n==NULL){
						n = tmp; 
						last = n; 
					} else {
						last->brother = tmp; 
						last = last->brother; 
					}
				}
				tmp = tmpBro;
			}
			root->child = n;
			// cout<<"after fixing not_or_exp"<<endl;
			// printAST(root, 0);
		}
		
	} else if(root->eKind == OR_EXP){
		AstNodePtr tmp = root->child, n = NULL, last = NULL;
		while(tmp!=NULL){
			AstNodePtr tmpBro = tmp->brother;
			if(tmp->eKind==OR_EXP){
				fixAST(tmp, callOpCounter, tot);
				if(n==NULL){
					n = tmp->child; 
					last = n; 
				} else {
					last->brother = tmp->child; 
					last = last->brother; 
				}
			} else {
				fixAST(tmp, callOpCounter, tot);
				if(n==NULL){
					n = tmp; 
					last = n; 
				} else {
					last->brother = tmp; 
					last = last->brother; 
				}
			}

			tmp = tmpBro;
		}
		root->child = n;
	}

	// fix lazy operator

	// printAST(root, 0);

	fixLazy(root);

	if(isCallOp(root->eKind)){
	   callOpCounterTmp = (root->brother == NULL) ? callOpCounter + 1 : 1;
		if(root->eKind == OPCPSTAR_EXP){
			root->reference = strdup("0,inf");
			root->referenceLength = strlen(root->reference);
			root->eKind = QUANTIFIER_EXP;
  	   }else
  	   	if(root->eKind == OPCPLUS_EXP){
				root->reference = strdup("1,inf");
				root->referenceLength = strlen(root->reference);
				root->eKind = QUANTIFIER_EXP;
			}
	}
	else
		callOpCounterTmp = 0;
	
	fixAST(root->child, callOpCounter, tot);
	fixAST(root->brother, callOpCounter, tot);

	return;
}


void translateAST(OptStruct *opt, AstNodePtr root){
	// printAST(root, 0);
	int numInstr;
	if(root==NULL)
		return;
	numInstr = instructionCounter(root);
	numInstr = addressMgmt(root, 0);
	jumpCalculator(root, numInstr, NOOP_EXP);
	printf("Number of instructions %d\n", numInstr);
	// numInstr = middleend(root, -1, NOOP_EXP, 0, NOOP_EXP) + 1;
	// jumpCalculator(root, numInstr, NOOP_EXP);
	// printf("\nOptimized AST: \n");
	// printAST(root, 0);
	// printf("Number of instructions %d\n", numInstr);
	// if(opt->automata)
		// thompson(root, opt->outputFile);
	// backend(opt, root, NOOP_EXP, 0);
	// no merging, just translation, eps removal and printing 
	singleCOO *s = new singleCOO;
	s = addAutomaton(root, 0);
	
	removeEps(s);

	or2range_s(s);

	loopExpansion_s(s); 

	delete s;

	// printANML_s(s, opt->outputFile);

	if(opt->nopFill)
		nopFill(opt, numInstr);
}



void jumpCalculator(AstNodePtr root, int jumpAddress, ExpKind parentEkind){
	int callOpCounterTmp;
	int jumpAddressTmp;
	AstNodePtr brother;
	if(root == NULL)
		return;
	if(isCallOp(root->eKind)){
		brother = root->brother;	
		if(root->eKind == OPCPOR_EXP){
			while(brother->eKind != OPCP_EXP){
				brother = brother->brother;
			}
			brother = brother->brother;
		}
		if(brother == NULL){ 
			jumpAddressTmp = (!isCallOp(parentEkind)) ? jumpAddress : jumpAddress -1;
		}else
			jumpAddressTmp = brother->absAddress;
		
		if(root->eKind != OPCP_EXP){
			if(brother != NULL)
				root->jump = brother->absAddress - root->absAddress;
			else	
				root->jump = jumpAddressTmp - root->absAddress;
		}
	}
	else
		jumpAddressTmp = jumpAddress;
	jumpCalculator(root->child, jumpAddressTmp, root->eKind);
	jumpCalculator(root->brother, jumpAddress, parentEkind);
}

void nopFill(OptStruct *opt, int numInstr){
	int n = opt->numInstr - numInstr;
	int i;
	int referenceWidth = opt->lineWidth;
	char *reference;
    if(!(reference = (char *)malloc(sizeof(char)*referenceWidth)))
    	return;
	for(i = 0; i < referenceWidth; i++)
		*(reference + i) = '0';
	for(i = 0; i < n; i++){
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, reference);
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, reference);
	}
	free(reference);
}

int middleend(AstNodePtr root, int instrCount, ExpKind brotherEkind, int callOpCounter, ExpKind parentEkind){
	int actualAdress;
	int callOpCounterTmp;
	AstNodePtr brother, nextBrother, child;
	const char s[2] = ",";
	char *content, *refTmp;

	if(root == NULL)
		return instrCount;
	if(root->eKind != NOT_EXP && root->eKind != LAZY_EXP)
	  	  root->absAddress = ++instrCount;
	else{
		root->absAddress = instrCount + 1;
	}

	if(root->eKind == OPCP_EXP){
		do{
			if(brotherEkind != OPCPOR_EXP){
				brother = root->brother;
				nextBrother = root->brother = root->child->brother;
				root->eKind = root->child->eKind;
				root->reference = root->child->reference;
				root->referenceLength = root->child->referenceLength;
				child = root->child;
				root->child = root->child->child;
				if(nextBrother != NULL){
					while(nextBrother->brother != NULL)
						nextBrother = nextBrother->brother;
					nextBrother->brother = brother;
				}
				else
					root->brother = brother;
				delete_ExprNode(child);
				child = NULL;
			}
		}while(root->eKind == OPCP_EXP && brotherEkind != OPCPOR_EXP);
		if(root->eKind == NOT_EXP || root->eKind == LAZY_EXP)
	  	  root->absAddress = --instrCount;
	}

	if(root->eKind == OPCPOR_EXP){
		if(root->child->eKind == OPCPOR_EXP){
			brother = root->child->brother;
			while(brother->eKind != OPCP_EXP)
				brother = brother->brother;
			if(brother->brother == NULL){
				brother->brother = root->brother;
				brother->eKind = OPCPOR_EXP;
				root->brother = root->child->brother;
				child = root->child;
				root->child = root->child->child;
				delete_ExprNode(child);
				child = NULL;
			}
		}
	}
	else{
		if(root->eKind == LAZY_EXP && root->child->brother == NULL && root->child->eKind == QUANTIFIER_EXP){
			child = root->child;
			if(!(refTmp = (char *)malloc(sizeof(char)*child->referenceLength)))
				return -1;
			memcpy(refTmp, child->reference, child->referenceLength);
			content = strtok(refTmp, s);
			content = strtok(NULL, s);
			root->child = root->child->child;
			root->reference = child->reference;
			root->referenceLength = child->referenceLength;
			delete_ExprNode(child);
			child = NULL;
			root->absAddress = ++instrCount;
			if(content != NULL){
				root->eKind = QUANTIFIER_LAZY_EXP;
			} 
			else{
				root->eKind = QUANTIFIER_EXP;
			}
			free(refTmp);
			refTmp = NULL;
		}
	}
	if(isCallOp(root->eKind))
	     callOpCounterTmp = (root->brother == NULL) ? callOpCounter + 1 : 1;
	else
		callOpCounterTmp = 0;

	actualAdress = middleend(root->child, instrCount, NOOP_EXP, callOpCounterTmp, root->eKind);

	if(root->brother != NULL)
		actualAdress = middleend(root->brother, actualAdress, root->eKind, callOpCounter, parentEkind);
	else
		if(isCallOp(root->eKind) && isCallOp(parentEkind)){
			actualAdress++;
		}
	return actualAdress;
}

void backend(OptStruct *opt, AstNodePtr root, ExpKind parentEkind, int negate){
	int referenceWidth = opt->lineWidth;
	char *reference, *partialReference;
	char *op1;
	char *op2;
	const char s[2] = ",";
	char *min, *max;
	int max_value;

   if(!(reference = (char *)malloc(sizeof(char)*referenceWidth)) ||
       !(op1 = (char *)malloc(sizeof(char)*referenceWidth)) ||
       !(op2 = (char *)malloc(sizeof(char)*referenceWidth)))
    	return;
	for(int i = 0; i < referenceWidth; i++){
		*(reference + i) = '0';
		*(op1 + i) = '0';
		*(op2 + i) = '0';
	}

	if(root==NULL)
		return;
	callOptoStr(opt, op1, root->eKind);
	if(root->eKind == QUANTIFIER_EXP || root->eKind == QUANTIFIER_LAZY_EXP){
		partialReference = reference + (referenceWidth - 4*QUANTIZER_STACKCOUNTING_BITWIDTH);
		min = strtok(root->reference, s);
		max = strtok(NULL, s);
		max = (max != NULL) ? max : min;
		max_value = atoi(max);
		max_value = (max_value == 0 && max != NULL && strcmp(max, "0") != 0) ? COUNTER_INFINITE : max_value;
		integerToAscii(atoi(min), partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
		partialReference += QUANTIZER_STACKCOUNTING_BITWIDTH;
		integerToAscii(max_value, partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
		partialReference += QUANTIZER_STACKCOUNTING_BITWIDTH;
		if(root->eKind == QUANTIFIER_LAZY_EXP){
			integerToAscii(root->jump, partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
			partialReference += QUANTIZER_STACKCOUNTING_BITWIDTH;
			integerToAscii(0, partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
		}
		else{
			integerToAscii(0, partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
			partialReference += QUANTIZER_STACKCOUNTING_BITWIDTH;
			integerToAscii(root->jump, partialReference, QUANTIZER_STACKCOUNTING_BITWIDTH);
		}
		*(reference + 0) = '1';
		*(reference + 1) = '1';
		*(reference + 2) = '1';
		*(reference + 3) = '1';
		if(root->eKind == QUANTIFIER_LAZY_EXP)
			*(reference + 4) = '1';
		free(root->reference);
		root->reference = NULL;
	}
	else 
		if(root->reference != NULL){
   			stringToReference(opt, root->reference, root->referenceLength, reference);
			integerToAscii((1<<opt->clusterWidth) - (1<<(opt->clusterWidth - root->referenceLength)),
			 op1 + (referenceWidth - OPCODE_LEN - opt->clusterWidth), opt->clusterWidth);	
		}
   	else
			if(root->eKind == OPCPOR_EXP){
				partialReference = reference + (referenceWidth - ASCII_BITWIDTH);
				integerToAscii(root->brother->absAddress , partialReference, 4);
				partialReference += 4;
				integerToAscii(root->jump, partialReference, 4);
				*(reference + 2) = '1';
				*(reference + 3) = '1';
		}
	if(!isCallOp(root->eKind)){
		intOptoStr(opt, op1, root->eKind, negate);
		if(root->brother == NULL)
		  if(isCallOp(parentEkind))
		  	extOptoStr(opt, op1, parentEkind);
	}
	if(root->eKind == NOT_EXP || root->eKind == LAZY_EXP){
		if(root->child != NULL && root->child->reference != NULL && *(root->child->reference) == '\n'){
			free(root->child->reference);
			if(root->child->reference = (char *)malloc(sizeof(char)*(opt->clusterWidth))){
				for(int r = 0; r < (opt->clusterWidth); r++)
					*(root->child->reference + r) = '\n';
				root->child->referenceLength = opt->clusterWidth;
			}else
			 printf("fatal error");
		}
		backend(opt, root->child, root->eKind, 1);
	}
	else{
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, reference);
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, op1);
		backend(opt, root->child, root->eKind, negate);
	}
	backend(opt, root->brother, parentEkind, negate);
	if(isCallOp(root->eKind) && root->brother == NULL 
		&& isCallOp(parentEkind)){
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, op2);
		extOptoStr(opt, op2, parentEkind);
		fprintf(opt->fileOutPtr, "%.*s\n", referenceWidth, op2);
	}
	free(reference);
	free(op1);
	free(op2);
}

void integerToAscii(int character, char ascii[], int size){
	int i = 0;
	for(i = 0; i < size; i++){
		ascii[i] = (character & (1 << (size - i -1))) ? '1' : '0';
	}

}

void stringToReference(OptStruct *opt, char referenceStr[], int referenceLength, char reference[]){
	int length = referenceLength;
	int i, jump;
	if(opt->clusterWidth > length)
		jump = 0;	
	else
		jump = opt->lineWidth - opt->wordWidth*length; 
	for(i = 0; i < length; i++)
		integerToAscii(referenceStr[i], &(reference[i*(opt->wordWidth)+jump]), opt->wordWidth);
}

void intOptoStr(OptStruct *opt, char v[], ExpKind ekind, int negate){
	switch(ekind){
		case OR_EXP:
			strncpy(&(v[opt->lineWidth-6]), "001", 3);
			break;
		case AND_EXP:
			strncpy(&(v[opt->lineWidth-6]), "010", 3);
			break;
		case RANGE_EXP:
			strncpy(&(v[opt->lineWidth-6]), "011", 3);
			break;
		}
	if(negate == 1)
		v[opt->lineWidth-6] = '1';
}

void extOptoStr(OptStruct *opt, char v[], ExpKind ekind){
	switch(ekind){
		case OPCP_EXP:
			strncpy(&(v[opt->lineWidth-3]), "100", 3);
			break;
		case QUANTIFIER_EXP:
		case OPCPSTAR_EXP:
		case OPCPLUS_EXP:
			strncpy(&(v[opt->lineWidth-3]), "010", 3);
			break;
		case QUANTIFIER_LAZY_EXP:
			strncpy(&(v[opt->lineWidth-3]), "001", 3);
			break;
		case OPCPOR_EXP:
			strncpy(&(v[opt->lineWidth-3]), "011", 3);
			break;
		}
}

void callOptoStr(OptStruct *opt, char v[], ExpKind ekind){
	switch(ekind){
		case OPCPLUS_EXP:	
		case OPCP_EXP:
		case OPCPSTAR_EXP:
		case OPCPOR_EXP:
		case QUANTIFIER_EXP:
		case QUANTIFIER_LAZY_EXP:
			strncpy(&(v[opt->lineWidth-7]), "1", 1);
		}
}

int isCallOp(ExpKind kind){
	int response = 0;
	switch(kind){
		case OPCP_EXP:
		case OPCPSTAR_EXP:
		case OPCPLUS_EXP:
		case OPCPOR_EXP:
		case QUANTIFIER_EXP:
		case QUANTIFIER_LAZY_EXP:
			response = 1;
		}
	return response;
}

void OptStructInit(OptStruct *opt){
	opt->automata=0;
	opt->outputFile = OUTPUTFILE;
	opt->inputFile = INPUTFILE;
	opt->wordWidth = WORDWIDTH;
	opt->lineWidth = LINEWIDTH;
	opt->clusterWidth = CLUSTERWIDTH;
	opt->nopFill = NOPFILL;
	opt->fileInPtr = stdin;
	opt->fileOutPtr = stdout;
	opt->numInstr = (1 << MEMWIDTH);
}

int isInternalOp(ExpKind kind){
	int response = 0;
	switch(kind){
		case OR_EXP:
		case AND_EXP:
		case RANGE_EXP:
			response = 1;
		}
	return response;
}

void printOp(ExpKind kind){
	switch(kind){
	    case OR_EXP:
			printf("OR: ");
			break;
		case AND_EXP:
			printf("AND: ");
			break;
		case RANGE_EXP:
			printf("RANGE: ");
			break;
		case NOT_EXP:
			printf("NOT: ");
			break;
		case OPCP_EXP:
			printf("OPCP");
			break;
		case OPCPSTAR_EXP:
			printf("OPCPSTAR");
			break;
		case OPCPLUS_EXP:
			printf("OPCPLUS");
			break;
		case OPCPOR_EXP:
			printf("OPCPOR");
			break;
		case CMPLX_EXP:
			printf("CMPLX");
			break;
		case QUANTIFIER_EXP:
			printf("QUANTIFIER: ");
			break;
		case QUANTIFIER_LAZY_EXP:
			printf("QUANTIFIER_LAZY_EXP: ");
			break;
		case LAZY_EXP:
			printf("LAZY");
			break;
		case NOT_OR_EXP:
			printf("NOT_OR_EXP");
			break;
		case ANCHOR_SOL_NODE:
			printf("ANCHOR_SOL_NODE");
			break;
		case ANCHOR_EOL_NODE:
			printf("ANCHOR_EOL_NODE");
			break;
		}
}