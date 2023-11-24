%{
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include "ast.hpp"
#include "../re2automata/automata.hpp"
#include "compiler.tab.hh"
#define REPS 15

extern int yylex();
extern FILE *yyin;
AstNodePtr fixOR(AstNodePtr root);
void  yyerror(FILE *fp, const char *s);
AstNodePtr regular;
std::vector<AstNodePtr> *mrg = new std::vector<AstNodePtr>;
std::vector<int> fe;

%}
%parse-param {FILE *fp}
%union
{
	AstNodePtr nodePtr;
	char character;
  char *string;
}

%type <nodePtr> Expressions Expression OrExpression Range RangeExpression QuantifiedExpression OpCpExpression AndExpression
%token <character> CHAR
%token DOT
%token OR_OP RANGE_OP STAR_OP PLUS_OP NOT_OP
%token OP CP
%token <string> QUANTIFIER 
%token OR CR LAZY_OP
%token ANCHOR_SOL
%token END

%left CHAR DOT
%left CP CR RANGE_OP
%right OP OR OR_OP

%start Input
%%

Input:
     | Input Line {
     }
;

Line:
     END
     |  OrExpression END { regular = fixOR($1); 
        // printAST(regular, 0);
        mrg->push_back(regular);
        // printf("size is now %d\n", mrg->size());
     }
;

Expressions:
	Expression {
     	  $$ = $1; 
     	}
  | ANCHOR_SOL Expression {
    $$ = new_ExprNode(ANCHOR_SOL_NODE, NULL, 0);
    $$->brother = $2;
  }

  | Expression LAZY_OP{
        if($1->eKind == QUANTIFIER_EXP || $1->eKind == OPCPSTAR_EXP || 
              $1->eKind == OPCPLUS_EXP){
          $$ = new_ExprNode(LAZY_EXP, NULL, 0);
          $$->child = $1;
        }
        else 
          // printf("type %d\n", $1->eKind);
          if($1->eKind == AND_EXP || $1->eKind == RANGE_EXP 
            || $1->eKind == NOT_EXP){
            $$ = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
            $$->child = $1;
          }
          else
            if($1->eKind == OPCP_EXP){
              $$ = $1;
              $$->reference = strdup("0,1");
              $$->referenceLength = 3;
              $$->eKind = QUANTIFIER_EXP;
           }
           else{
              yyerror(stderr, "syntax error");
              YYERROR;
           }
  }

  |Expression LAZY_OP LAZY_OP{
        $$ = new_ExprNode(LAZY_EXP, NULL, 0);
        if($1->eKind == AND_EXP || $1->eKind == RANGE_EXP 
          || $1->eKind == NOT_EXP){
          $$->child = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
          $$->child->child = $1;
        }
        else
          if($1->eKind == OPCP_EXP){
            $$->child = $1;
            $$->child->reference = strdup("0,1");
            $$->child->referenceLength = 3;
            $$->child->eKind = QUANTIFIER_EXP;
         }
         else{
            yyerror(stderr, "syntax error");
            YYERROR;
         }
  }

  |Expressions Expression LAZY_OP LAZY_OP{
    AstNodePtr last = new_ExprNode(LAZY_EXP, NULL, 0);
        if($2->eKind == AND_EXP || $2->eKind == RANGE_EXP 
          || $2->eKind == NOT_EXP){
          last->child = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
          last->child->child = $2;
        }
        else
          if($2->eKind == OPCP_EXP){
            last->child = $2;
            last->child->reference = strdup("0,1");
            last->child->referenceLength = 3;
            last->child->eKind = QUANTIFIER_EXP;
         }
         else{
            yyerror(stderr, "syntax error");
            YYERROR;
         }
      AstNodePtr next = $1;
          while(next->brother != NULL)
            next = next->brother;
          next->brother = last;     

      $$ = $1;
  } 

  |Expressions Expression LAZY_OP{
    AstNodePtr last;
        if($2->eKind == QUANTIFIER_EXP || $2->eKind == OPCPSTAR_EXP || 
              $2->eKind == OPCPLUS_EXP){
          last = new_ExprNode(LAZY_EXP, NULL, 0);
          last->child = $2;
        }
        else 
          if($2->eKind == AND_EXP || $2->eKind == RANGE_EXP 
            || $2->eKind == NOT_EXP){
            last = new_ExprNode(QUANTIFIER_EXP, strdup("0,1"), 3);
            last->child = $2;
          }
          else
            if($2->eKind == OPCP_EXP){
              last = $2;
              last->reference = strdup("0,1");
              last->referenceLength = 3;
              last->eKind = QUANTIFIER_EXP;
           }
           else{
              yyerror(stderr, "syntax error");
              YYERROR;
           }
      AstNodePtr next = $1;
          while(next->brother != NULL)
            next = next->brother;
          next->brother = last;     

      $$ = $1;
  }

	|Expressions Expression {
		char *ref;
   		int length;
   		AstNodePtr prev, last;
   		prev = $1;
   		do{
   			last = prev;
   			prev = prev->brother;
   		}while(prev != NULL);
		if(last->eKind == AND_EXP && $2->eKind == AND_EXP){
			length = last->referenceLength;
	    	if(ref = (char *)malloc(sizeof(char)*(length+1))){
	    	  memcpy(ref, last->reference, last->referenceLength);
	    	  *(ref + last->referenceLength) = *($2->reference);
	    	  free($2->reference);
	    	  free($2); 
	    	  free(last->reference);
	    	  last->reference = ref;
          last->referenceLength = last->referenceLength + 1;
	     	}
	     	else
	     		printf("fatal error");
        }
        else{
			AstNodePtr next = $1;
	    	  while(next->brother != NULL)
	    	  	next = next->brother;
	     	  next->brother = $2;			
        }

	     	  $$ = $1;
        }
;

Expression:
     AndExpression  { $$ = $1;}

   | AndExpression PLUS_OP{
      $$ = new_ExprNode(OPCPLUS_EXP, NULL, 0);
      $$->child = $1;
    }

   | AndExpression STAR_OP{
      $$ = new_ExprNode(OPCPSTAR_EXP, NULL, 0);
      $$->child = $1;
    }
   | RangeExpression { $$ = $1;}

   | RangeExpression PLUS_OP{
      $$ = new_ExprNode(OPCPLUS_EXP, NULL, 0);
      $$->child = $1;
    }

   | RangeExpression STAR_OP{
      $$ = new_ExprNode(OPCPSTAR_EXP, NULL, 0);
      $$->child = $1;
    }
   | OpCpExpression { 
      $$ = $1;
    }

   | OpCpExpression PLUS_OP{ 
      $$ = $1;
      $$->eKind = OPCPLUS_EXP;
    }
 
   | OpCpExpression STAR_OP{ 
      $$ = $1;
      $$->eKind = OPCPSTAR_EXP;
    }

   | QuantifiedExpression { 
      $$ = $1;
    }
   ;

QuantifiedExpression:  
     AndExpression QUANTIFIER { 
      $$ = new_ExprNode(QUANTIFIER_EXP, $2, strlen($2));
      $$->child = $1;
    }

     | OpCpExpression QUANTIFIER {     
      $$ = new_ExprNode(QUANTIFIER_EXP, $2, strlen($2));
      $$->child = $1;
    }

     | RangeExpression QUANTIFIER {
      $$ = new_ExprNode(QUANTIFIER_EXP, $2, strlen($2));
      $$->child = $1;
    }
;

AndExpression: 
    CHAR {
      char *ref;
    	if(ref = (char *)malloc(sizeof(char)*1)){
      	  *(ref) = $1;
       	  $$ = new_ExprNode(AND_EXP, ref, 1);
     	}
     	else
     		printf("fatal error");
    }

    | DOT { 
          char *ref;
          if(ref = (char *)malloc(sizeof(char)*1)){
            *(ref) = '\n';
            $$ = new_ExprNode(NOT_EXP, NULL, 0);
            $$->child = new_ExprNode(OR_EXP, "", 0);
            $$->child->child = new_ExprNode(AND_EXP, ref, 1);

          }
          else
            printf("fatal error");

    }
;

OpCpExpression:
	OP OrExpression CP {  
	 	  $$ = new_ExprNode(OPCP_EXP, NULL, 0);
     	  $$->child = fixOR($2);
     	}
;

OrExpression:
      Expressions{ 
      if($1->brother != NULL){
      		$$ = new_ExprNode(CMPLX_EXP, NULL, 0);
     	    $$->child = $1;
        }
        else{
        	$$ = $1;
        }
    }
    |OrExpression OR_OP Expressions{
    	char *ref;
    	int length;
    	AstNodePtr tmp,next;
    	if($3->brother != NULL){
      		tmp = new_ExprNode(CMPLX_EXP, NULL, 0);
     	    tmp->child = $3;
     	    $3 = tmp;
     	 }
      if($1->eKind != OR_EXP || ($1->eKind == OR_EXP && $1->child == NULL)){
    		tmp = new_ExprNode(OR_EXP, NULL, 0);
     	  tmp->child = $1;
      }
    	else{
    		tmp = $1;
      }
		next = tmp->child;
		while(next->brother != NULL){
			next = next->brother;
		}

		if(next->reference != NULL)
			length = next->referenceLength;
    	if($3->eKind == AND_EXP && $3->referenceLength == 0 && 
    		((next->eKind == AND_EXP && length == 1 ) || next->eKind == OR_EXP)) {
    		next->eKind = OR_EXP;
	    	if(ref = (char *)malloc(sizeof(char)*(length+1))){
          memcpy(ref, next->reference, next->referenceLength);
          *(ref + next->referenceLength) = *($3->reference);
	    	  free(next->reference);
	    	  free($3);
	    	  next->reference = ref;
          next->referenceLength = next->referenceLength + 1;
	     	}
	     	else
	     		printf("fatal error");
    	}
    	else
			  next->brother = $3;
	    $$ = tmp;
    }

;

RangeExpression:
            OR Range CR { 
              AstNodePtr next, tmp, new_OR, brother;
              next = $2;
              if(next->brother != NULL){
                brother = NULL;
                new_OR = new_ExprNode(OR_EXP, NULL, 0);
                while(next != NULL){
                  tmp = new_ExprNode(OPCP_EXP, NULL, 0);
                  tmp->child = next;
                  next = next->brother;
                  tmp->child->brother = NULL;
                  if(brother != NULL)
                    brother->brother = tmp;
                  else
                    new_OR->child = tmp;
                  brother = tmp;
                }
                next = new_OR;
              }
              tmp = new_ExprNode(OPCP_EXP, NULL, 0);
              tmp->child = next;
              $$ = tmp;
            }
            | OR NOT_OP Range CR{
                AstNodePtr next, tmp, new_OR, brother;
                next = $3;
                if(next->brother != NULL){
                  brother = NULL;
                  new_OR = new_ExprNode(OR_EXP, NULL, 0);
                  while(next != NULL){
                    tmp = new_ExprNode(OPCP_EXP, NULL, 0);
                    tmp->child = next;
                    next = next->brother;
                    tmp->child->brother = NULL;
                    if(brother != NULL)
                      brother->brother = tmp;
                    else
                      new_OR->child = tmp;
                    brother = tmp;
                  }
                  next = new_OR;
                }
                tmp = new_ExprNode(NOT_EXP, NULL, 0);
                tmp->child = next;
                $$ = tmp;
            }
;

Range: 
      CHAR RANGE_OP CHAR {
  			char *ref;
	    	if(ref = (char *)malloc(sizeof(char)*2)){
	    	  *(ref) = $1;
	    	  *(ref + 1) = $3;
	     	  $$ = new_ExprNode(RANGE_EXP, ref, 2);
	     	}
	     	else
	     		printf("fatal error");
	     }

       | Range CHAR RANGE_OP CHAR {	
		   		char *ref;
		   		int length;
          AstNodePtr next;
          next = $1;
          while(next->brother != NULL){
            next = next->brother;
          }
          if(next->eKind == RANGE_EXP){
            length = next->referenceLength;
  		    	if(ref = (char *)malloc(sizeof(char)*(length+2))){
    		    	  memcpy(ref, next->reference, next->referenceLength);
                *(ref + length) = $2;
    		    	  *(ref + length + 1) = $4;
    		    	  free(next->reference);
                next->reference = ref;
                next->referenceLength = next->referenceLength + 2; 
  		     	}
  		     	else
  		     		printf("fatal error");
         }
         else
            if(ref = (char *)malloc(sizeof(char)*2)){
              *(ref) = $2;
              *(ref + 1) = $4;
              next->brother = new_ExprNode(RANGE_EXP, ref, 2);
            }
            else
              printf("fatal error");
         $$ = $1;
       }
      
      | CHAR {      
            char *ref;
            if(ref = (char *)malloc(sizeof(char)*1)){
              *(ref) = $1;
              $$ = new_ExprNode(OR_EXP, ref, 1);
            }
            else
              printf("fatal error");
        }
      | Range CHAR {
         char *ref;
          int length;
          AstNodePtr next;
          next = $1;
          while(next->brother != NULL){
            next = next->brother;
          }
          if(next->eKind == OR_EXP){
            length = next->referenceLength;
            if(ref = (char *)malloc(sizeof(char)*(length+1))){
                memcpy(ref, next->reference, next->referenceLength);
                *(ref + length) = $2;
                free(next->reference);
                next->reference = ref;
                next->referenceLength = next->referenceLength + 1; 
            }
            else
              printf("fatal error");
         }
         else
            if(ref = (char *)malloc(sizeof(char)*1)){
              *(ref) = $2; 
              next->brother = new_ExprNode(OR_EXP, ref, 1);
            }
            else
              printf("fatal error");
         $$ = $1;
       }
;

%%

AstNodePtr fixOR(AstNodePtr root){
	AstNodePtr tmp = root;
	if(root->eKind == OR_EXP && root->child != NULL &&
		root->child->eKind == OR_EXP && root->child->brother == NULL){
		tmp = root->child;
		free(root);
	}
	return tmp;
}

void yyerror(FILE *fp, const char *s) {
  fprintf(fp, "%s\n", s);		
}

int main(int argc, char *argv[]) {
  OptStruct opt;
  int index;
  int c, b=0;
  opterr = 0;
  OptStructInit(&opt);
  char *time_dir = NULL;

  while ((c = getopt (argc, argv, "m:o:a:b:i:w:t:l:f")) != -1)
    switch (c)
      {
      case 'm':
        opt.numInstr = (1 << (atoi(optarg)));
        break;
      case 'w':
        opt.clusterWidth = atoi(optarg);
        break;
      case 'l':
        opt.lineWidth = atoi(optarg);
        break;
      case 'i':
        opt.inputFile = optarg;
        opt.fileInPtr = fopen(optarg, "r");
        if(opt.fileInPtr == NULL){
          opt.fileInPtr = stdin;
          printf("ciao");
        }
        break;
      case 't': 
        time_dir = optarg;
        break;
      case 'o':
        opt.outputFile = optarg;
        if(!opt.automata)
          opt.fileOutPtr = fopen(optarg, "w");
        if(opt.fileOutPtr == NULL)
          opt.fileOutPtr = stdout;
        break;
      case 'b':
        b = atoi(optarg);
        opt.automata=1;
        break;
      case 'f':
        opt.nopFill = 1;
        break;
      case '?':
        if (optopt == 'l' || optopt == 'w' || optopt == 'i' || optopt == 'o' || optopt == 'a')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }


  printf ("line_width = %d\ncpu_width = %d\ninput_file = %s\noutput_file = %s\n",
          opt.lineWidth, opt.clusterWidth, opt.inputFile, opt.outputFile);

  for (index = optind; index < argc; index++)
    printf ("Non-option argument %s\n", argv[index]);
  yyin = opt.fileInPtr;
  int parser = 1;
  auto start_f = std::chrono::high_resolution_clock::now();
  parser = yyparse(opt.fileInPtr);
  auto end_f = std::chrono::high_resolution_clock::now();
  auto duration_f = std::chrono::duration_cast<std::chrono::microseconds>(end_f - start_f);
  int time_fe = duration_f.count();
  
  
  int output = 0;
  if (!parser){
    if(opt.automata){
      /* opt.fileOutPtr="tmp"; */
      if(b==-1)
        b=mrg->size();
      if(opt.outputFile==""){
        fprintf(stderr, "output file is mandatory\n");
        abort();
      }
      if (mrg->size()<b){
        fprintf(stderr, "not enough REs\n");
      }
      // profiler(mrg);
      float avg_fe=avg(fe);
      output = merge_automata(mrg, b, opt.outputFile, time_dir, time_fe);
      if(output)
        fprintf(stderr, "Could not parse, missing arguments.\n");
    } else {
      clock_t start, end;
      double execution_time;
      start = clock();
  	  translateAST(&opt, regular);
      end = clock();
      execution_time = ((double)(end - start))/CLOCKS_PER_SEC;
      printf("automata single comp. time %f\n", execution_time);
    } 
    if(!output)
      fprintf(stderr, "Successful parsing.\n");
  }
  else
     fprintf(stderr, "error found.\n");
}