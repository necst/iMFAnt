%option noyywrap nodefault yylineno
%{
#include<iostream>
#include<cstdlib>
#include<cstdio>
#include "ast.hpp"
#include "compiler.tab.hh"
int flag;
%}
%s RANGE
quantifier \{[0-9]+,?[0-9]*\}
character [a-zA-Z0-9{},]
backslash \\.
%%

<INITIAL>"/"$ {}

<INITIAL>^"/" {}

<INITIAL>\\w { /*:word:*/
  char* wordRE = "[A-Za-z0-9_]";
  int i;
  for(i=strlen(wordRE)-1; i>=0; i--){
    /*must be done back to front, unput puts
    the given char back at the beginning of the stream*/
    unput(wordRE[i]);
  }
}

<INITIAL>\\d { /*:digit:*/
  char* digitRE = "[0-9]";
  int i;
  for(i=strlen(digitRE)-1; i>=0; i--){
    unput(digitRE[i]);
  }
}

<INITIAL>\\h { /*:blank:*/
  char* blankRE = "[ \\t]";
  int i;
  for(i=strlen(blankRE)-1; i>=0; i--){
    unput(blankRE[i]);
  }
}

<INITIAL>\\s { /*:blank:*/
  char* blankRE = "[\\r\\n\\t\\f\\v ]";
  int i;
  for(i=strlen(blankRE)-1; i>=0; i--){
    unput(blankRE[i]);
  }
}

<INITIAL>"/" {return yytext[0];}

<RANGE>"]" {
     BEGIN(INITIAL);
     return CR;
     }

<INITIAL>[^\\]\$ {
  yylval.character='\n';
  return CHAR;
}

<INITIAL>\^ {
	return ANCHOR_SOL;
}

<RANGE>"^" {
	if(flag == 1){
		flag = 0;
		return NOT_OP;
	}
	yylval.character	= yytext[0]; 
	return CHAR;
}

<INITIAL,RANGE>\\x([0-9]|[A-F]|[a-f]){2} {
	yylval.character = (char) strtol(&yytext[2], NULL, 16);
	return CHAR;
 }

<INITIAL,RANGE>\\n {
	yylval.character = '\n'; 
	flag = 0;
	return CHAR;
 }

<INITIAL,RANGE>\\t {
	flag = 0;
	yylval.character = '\t'; 
	return CHAR;
 }

<INITIAL,RANGE>\\r {
	flag = 0;
	yylval.character = '\r';
	return CHAR;
} 
 
<INITIAL>\. {
	flag = 0;
	return DOT;
}

<INITIAL>{character} { 
 flag = 0;
 yylval.character	= yytext[0]; 
 return CHAR;
}

<INITIAL>{quantifier} {
 yytext[strlen(yytext) - 1] = '\0';
 yylval.string	= strdup((yytext + 1)); 
 return QUANTIFIER;
}

<RANGE>\\w { /*:word:*/
  char* wordRE = "A-Za-z0-9_";
  int i;
  flag = 0;
  for(i=strlen(wordRE)-1; i>=0; i--){
    /*must be done back to front, unput puts
    the given char back at the beginning of the stream*/
    unput(wordRE[i]);
  }
}

<RANGE>\\d { /*:digit:*/
  char* digitRE = "0-9";
  int i;
  flag = 0;
  for(i=strlen(digitRE)-1; i>=0; i--){
    unput(digitRE[i]);
  }
}

<RANGE>\\h { /*:blank:*/
  char* blankRE = " \\t";
  int i;
  flag = 0;
  for(i=strlen(blankRE)-1; i>=0; i--){
    unput(blankRE[i]);
  }
}

<RANGE>\\s { /*:space:*/
  char* spaceRE = "\\r\\n\\t\\f\\v ";

  printf("%d\n", strlen(spaceRE));
  int i;
  for(i=strlen(spaceRE)-1; i>=0; i--){
    unput(spaceRE[i]);
  }
}

<INITIAL,RANGE>{backslash} {
	flag = 0;
	yylval.character	= yytext[1];
	return CHAR;
}

<INITIAL>"*" return STAR_OP;

<INITIAL>"+" return PLUS_OP;

<RANGE>"-" {
 	if(flag == 1){
 		flag = 0;
 		yylval.character = yytext[0]; 
 		return CHAR;
 	}
 	return RANGE_OP;	
 }

 <RANGE>"-]" {
 	yylval.character = yytext[0]; 
 	unput(yytext[1]);
 	return CHAR;	
 }

<INITIAL>"|" return OR_OP;

<INITIAL>"[" {
	 flag = 1;
     BEGIN(RANGE);
     return OR;
    }

<INITIAL>"(" return OP;

<INITIAL>")" return CP;

<INITIAL>"?" return LAZY_OP;

<INITIAL>"\n" return END;

<INITIAL,RANGE>. {
	flag = 0;
  yylval.character = yytext[0]; 
 	return CHAR;
}



.|\n	printf("bad input character '%s' at line %d\n", yytext, yylineno);
