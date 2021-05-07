
%%


    /* Comments */
\/\/[ \tA-Za-z0-9_:\<\>\-\+=,\(\).`\']*\n        {printf("EOL\n");}
#[ \tA-Za-z0-9_:\<\>\-\+=,\(\).`\']*\n         {printf("EOL\n");}

    /* Key Characters */
"%"                         {printf("REG_MARK\n");}
"$"                         {printf("IMM_MARK\n");}
"+"                         {printf("PLUS_SIGN\n");}
"-"                         {printf("MINUS_SIGN\n");}
","                         {printf("COMMA\n");}
"@"                         {printf("AT_SIGN\n");}

    /* Structure Characters */
"("                         {printf("OPEN_PAREN\n");}
")"                         {printf("CLOSE_PAREN\n");}
"["                         {printf("OPEN_BRACKET\n");}
"]"                         {printf("CLOSE_BRACKET\n");}

    /* Section Directives */
"."[cC][oO][dD][eE]         {printf("CODE_SEG\n");}
"."[dD][aA][tT][aA]         {printf("DATA_SEG\n");}

    /* Alignment Directives */
"."[wW][oO][rR][dD]         {printf("WORD_ALIGN\n");}
"."[hH][aA][lL][fF]         {printf("HALF_ALIGN\n");}
"."[sS][hH][oO][rR][tT]     {printf("HALF_ALIGN\n");}
"."[bB][yY][tT][eE]         {printf("BYTE_ALIGN\n");}

    /* Misc Dot Directives */
 /* "."[a-zA-Z0-9_]+            {printf("DOT_DIR %s\n", yytext);} */

    /* Numeric Literals */
"0x"[0-9a-fA-F]+            {printf("INT %ld\n", strtol(yytext, NULL, 16));}
"0"[0-7]+                   {printf("INT %ld\n", strtol(yytext, NULL, 8));}
[0-9]+                      {printf("INT %ld\n", strtol(yytext, NULL, 10));}

    /* Alphanum Literals */
[a-zA-Z_][a-zA-Z0-9_]*":"   {printf("LABEL %.*s\n", yyleng - 1, yytext);}
[a-zA-Z_][a-zA-Z0-9_]*"="   {printf("LABEL %.*s\n", yyleng - 1, yytext);}
[a-zA-Z_][a-zA-Z0-9_]*" ="  {printf("LABEL %.*s\n", yyleng - 2, yytext);}
[a-zA-Z_][a-zA-Z0-9_]*      {printf("WORD %s\n", yytext);}

    /* Whitespace Handling */
[ \t]                       {}
\n                          {printf("EOL\n");}

    /* Misc */
.                           {printf("UNKNOWN %s\n", yytext);}

%%

