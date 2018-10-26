%{
 /*
 * scanner.l
 *
 *  Created on: NOV 15, 2017
 *      Author: bsl
 *        desc: word analysis
 */
    #include "grammer.tab.h"
    #include "grammer.h"
%}

 /**
 * create命令
 **/
create [cC][rR][eE][aA][tT][eE]
table [tT][aA][bB][lL][eE]
index [iI][nN][dD][eE][xX]
using [uU][sS][iI][nN][gG]
btree [bB][tT][rR][eE][eE]
hash [hH][aA][sS][hH]
if [iI][fF]
exists [eE][xX][iI][sS][tT][sS]
on [oO][nN]
true [tT][rR][uU][eE]
false [fF][aA][lL][sS][eE]
 /**
 * 数据类型
 **/
char [cC][hH][aA][rR]
long [lL][oO][nN][gG]
date [dD][aA][tT][eE]
float [fF][lL][oO][aA][tT]
double [dD][oO][uU][bB][lL][eE]
varchar [vV][aA][rR][cC][hH][aA][rR]
integer [iI][nN][tT][eE][gG][eE][rR]
 /**
 * 约束条件
 **/
not [nN][oO][tT]
key [kK][eE][yY]
null [nN][uU][lL][lL]
unique [uU][nN][iI][qQ][uU][eE]
commit [cC][oO][mM][mM][iI][tT]
default [dD][eE][fF][aA][uU][lL][tT]
primary [pP][rR][iI][mM][aA][rR][yY]
foreign [fF][oO][rR][eE][iI][gG][nN]
autoincrement [aA][uU][tT][oO][iI][nN][cC][rR][eE][mM][eE][nN][tT]
distinct [dD][iI][sS][tT][iI][nN][cC][tT]
 /**
 * insert命令
 **/
insert [iI][nN][sS][eE][rR][tT]
into [iI][nN][tT][oO]
values [vV][aA][lL][uU][eE][sS]
 /**
 * select命令
 **/
select [sS][eE][lL][eE][cC][tT]
from [fF][rR][oO][mM]
where [wW][hH][eE][rR][eE]
like [lL][iI][kK][eE]
and [aA][nN][dD]
or [oO][rR]
between [bB][eE][tT][wW][eE][eE][nN]
is [iI][sS]
order [oO][rR][dD][eE][rR]
by [bB][yY]
asc [aA][sS][cC]
desc [dD][eE][sS][cC]
limit [lL][iI][mM][iI][tT]


dot [.]
com [,]
sem [;]
add [+]
sub [-]
mul [*]
div [/]
mod [%]
less [<]
equal [=]
greater [>]
add_equal [+][=]
sub_equal [-][=]
mul_equal [*][=]
div_equal [/][=]
mod_equal [%][=]
not_equal [!][=]
less_equal [<][=]
greater_equal [>][=]
left_bra [(]
right_bra [)]

 /**
 * delete命令
 **/
delete [dD][eE][lL][eE][tT][eE]
 /**
 * drop命令
 **/
drop [dD][rR][oO][pP]
 /**
 * update命令
 **/
update [uU][pP][dD][aA][tT][eE]
set [sS][eE][tT]

 /**
 * 特殊字符
 **/
newline [\n\r]
space ([ \t\n\r\f])+

op_chars [\~\`\!\@\#\$\%\^\&\*\(\)\-\_\+\=\{\}\[\]\:\;\<\>\,\.\?\/\|]
digit [0-9]
chars [a-zA-Z]
number {digit}+
real {digit}+[.]{digit}+
identifier {chars}+({chars}|{digit}|[-_])*
over (([\\][\\])|([\\]({chars}|{digit}|{op_chars}|["'])))
svalue [']({chars}|{digit}|{op_chars}|{space}|["]|{over})*[']
dvalue ["]({chars}|{digit}|{op_chars}|{space}|[']|{over})*["]
%%

{space}
{update}            {
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return UPDATE;
                    }
{set}               {
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return SET;
                    }
{dot}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DOT;
                    }
{com}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return COM;
                    }
{sem}               {
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return SEM;
                    }
{add}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext);
                        return ADD;
                    }
{sub}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return SUB;
                    }
{mul}               {
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext);
                        return MUL;
                    }
{div}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext);
                        return DIV;
                    }
{mod}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return MOD; 
                    }
{less}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return LESS; 
                    }
{equal}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return EQUAL; 
                    }
{greater}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return GREATER; 
                    }
{left_bra}          { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return LEFT_BRA;
                    }
{right_bra}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return RIGHT_BRA;
                    }
{if}                { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return IF; 
                    }
{on}                { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return ON;
                    }
{true}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return TRUE;
                    }
{false}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return FALSE;
                        }
{index}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext); 
                        return INDEX; 
                    }
{hash}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return HASH; 
                    }
{btree}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return BTREE; 
                    }
{using}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return USING;
                    }
{exists}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext); 
                        return EXISTS; 
                    }
{create}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return CREATE; 
                    }
{table}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return TABLE; 
                    }
{char}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_CHAR; 
                    }
{long}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_LONG;
                    }
{date}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_DATE;
                    }
{float}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext);
                        return G_FLOAT;
                    }
{double}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_DOUBLE; 
                    }
{varchar}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_VARCHAR;
                    }
{integer}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return G_INTEGER;
                    }
{not}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return NOT; 
                    }
{key}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return KEY;
                    }
{null}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return NULLS; 
                    }
{unique}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return UNIQUE; 
                    }
{commit}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return COMMIT;
                    }
{default}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DEFAULT;
                    }
{primary}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return PRIMARY;
                    }
{foreign}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return FOREIGN;
                    }
{distinct}          { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DISTINCT;
                    }
{autoincrement}     { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return AUTOINCREMENT;
                    }
{insert}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return INSERT;
                    }
{into}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return INTO;
                    }
{values}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return VALUES;
                    }
{select}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return SELECT;
                    }
{from}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return FROM; 
                    }
{where}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return WHERE;
                    }
{like}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return LIKE;
                    }
{and}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return AND;
                    }
{or}                { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext);
                        yylval.t_char = vmalloc(strlen(yytext) + 1);
                        strcpy(yylval.t_char, yytext);
                        return OR;
                    }
{is}                { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return IS;
                    }
{between}           { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return BETWEEN;
                    }
{order}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return ORDER; 
                    }
{by}                { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return BY; 
                    }
{asc}               { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return ASC;
                    }
{desc}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DESC; 
                    }
{limit}             { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return LIMIT;
                    }
{delete}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DELETE;
                    }
{drop}              { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DROP; 
                    }
{number}            { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return NUMBER; 
                    }
{identifier}        { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return IDENTIFIER; 
                    }
{svalue}            { 
						vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
						yylval.t_char = vmalloc(strlen(yytext) + 1); 
						strcpy(yylval.t_char, yytext + 1); 
						yylval.t_char[strlen(yylval.t_char) - 1] = '\0'; // delete the \'
						return SVALUE; 
					}
{dvalue}            { 
						vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
						yylval.t_char = vmalloc(strlen(yytext) + 1); 
						strcpy(yylval.t_char, yytext + 1); 
						yylval.t_char[strlen(yylval.t_char) - 1] = '\0'; // delete the \"
						return DVALUE; 
					}
{add_equal}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return ADD_EQUAL;
                    }
{sub_equal}         {
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return SUB_EQUAL; 
                    }
{mul_equal}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return MUL_EQUAL;
                    }
{div_equal}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return DIV_EQUAL;
                    }
{mod_equal}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return MOD_EQUAL;
                    }
{not_equal}         { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return NOT_EQUAL;
                    }
{less_equal}        {   
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return LESS_EQUAL;
                    }
{greater_equal}     { 
                        vmlog(GRAMMER_LOG, "content=%s\n", yytext); 
                        yylval.t_char = vmalloc(strlen(yytext) + 1); 
                        strcpy(yylval.t_char, yytext); 
                        return GREATER_EQUAL;
                    }
%%

int yywrap()
{
    return 1;
}
