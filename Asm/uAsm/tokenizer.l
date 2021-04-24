
%%

    /* Key Characters */

    /* Numeric Literals */
"0x"[0-9a-fA-F]+            {printf("INT %ld\n", strtol(yytext, NULL, 16));}
"0"[0-7]+                   {printf("INT %ld\n", strtol(yytext, NULL, 8));}
[0-9]+                      {printf("INT %ld\n", strtol(yytext, NULL, 10));}

    /* Alphanum Literals */
[a-zA-Z_][a-zA-Z0-9_]*      {printf("WORD %s\n", yytext);}

    /* Whitespace Handling */
[ \t]                       {}
","                         {}
";"                         {}

\/\/[ \tA-Za-z0-9_:\<\>\-\+=,\(\).`\']*\n        {printf("EOL\n");}
#[ \tA-Za-z0-9_:\<\>\-\+=,\(\).`\']*\n         {printf("EOL\n");}
\n                          {printf("EOL\n");}

    /* Misc */
.                           {printf("UNKNOWN %s\n", yytext);}

%%

