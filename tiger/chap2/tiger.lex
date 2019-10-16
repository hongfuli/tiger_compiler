%{
#include <string.h>
#include <stdio.h>

#include "util.h"
#include "tokens.h"
#include "errormsg.h"

int charPos=1;

int yywrap(void)
{
 charPos=1;
 return 1;
}


void adjust(void)
{
 EM_tokPos=charPos;
 charPos+=yyleng;
}

char *quoteChar = "\"";

%}

%%

  /* 规则书写原则：
   1. 优先匹配较长的规则 
   2. 如果两条规则长度相同，则书写顺序很重要，优先匹配靠前的规则 
  */

[\t ]	 {adjust(); continue;}
\n	 {adjust(); EM_newline(); continue;}
\/\*.*?\*\/   {
    adjust();
    printf("==comment: %s\n", yytext);
}
  /* 双引号包含的字符串，因为要考虑转义字符，所以使用 yymore, input 等lex高级特性 */
\"[^"]*  {
        if (yytext[yyleng - 1] == '\\') {
            yymore();
        } else {
            adjust();
            char * strV;
            strV = malloc(sizeof(char) * (yyleng + 1));
            strcpy(strV, yytext);
            strcat(strV, quoteChar);
            yylval.sval = strV;
            input();
            charPos++;
            return STRING;
        }
    }

[0-9]+	 {adjust(); yylval.ival=atoi(yytext); return INT;}

","	 {adjust(); return COMMA;}
":"	 {adjust(); return COLON;}
";"	 {adjust(); return SEMICOLON;}
"("	 {adjust(); return LPAREN;}
")"	 {adjust(); return RPAREN;}
"["	 {adjust(); return LBRACK;}
"]"	 {adjust(); return RBRACK;}
"{"	 {adjust(); return LBRACE;}
"}"	 {adjust(); return RBRACE;}
"."	 {adjust(); return DOT;}
"+"	 {adjust(); return PLUS;}
"-"	 {adjust(); return MINUS;}
"*"	 {adjust(); return TIMES;}
"/"	 {adjust(); return DIVIDE;}
"="	 {adjust(); return EQ;}
"<>"	 {adjust(); return NEQ;}
"<"	 {adjust(); return LT;}
"<="	 {adjust(); return LE;}
">"	 {adjust(); return GT;}
">="	 {adjust(); return GE;}
"&"	 {adjust(); return AND;}
"|"	 {adjust(); return OR;}
":="	 {adjust(); return ASSIGN;}

"array of"	 {adjust(); return ARRAY;}
"if"	 {adjust(); return IF;}
"then"	 {adjust(); return THEN;}
"else"	 {adjust(); return ELSE;}
"while"	 {adjust(); return WHILE;}
"for"  	 {adjust(); return FOR;}
"to"  	 {adjust(); return TO;}
"do"  	 {adjust(); return DO;}
"let"  	 {adjust(); return LET;}
"in"  	 {adjust(); return IN;}
"end"  	 {adjust(); return END;}
"of"  	 {adjust(); return OF;}
"break"  	 {adjust(); return BREAK;}
"nil"  	 {adjust(); return NIL;}
"function"  	 {adjust(); return FUNCTION;}
"var"  	 {adjust(); return VAR;}
"type"  	 {adjust(); return TYPE;}
[a-zA-Z][a-zA-Z0-9]*  {adjust(); yylval.sval=yytext; return ID;}
.	 {adjust(); EM_error(EM_tokPos,"illegal token");}


