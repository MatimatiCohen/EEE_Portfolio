#ifndef REG_EXP_H
#define REG_EXP_H

#include <stdbool.h>

enum reg_exp_options {CHAR,DOT,PARENTHESIS,BRACKETS};

#define PARENTHESIS_LEN(par) (par.left_len+par.right_len +PARENTHESIS_LENGTH)

typedef struct Regular_Parenthesis_String{
    char *left_str;
    int left_len;
    char *right_str;
    int right_len;
    bool x_on_right_len;
    bool x_on_left_len;
} reg_par_str;

typedef struct Regular_Brackets_String{
    char low_str;
    char high_str;
} reg_bra_str;

typedef bool reg_dot;

typedef union Reg_Exp_Data {
    char single_char;
    reg_dot dot;
    reg_par_str par;
    reg_bra_str bra;
} reg_exp_data ;


typedef struct Regular_Expression{
    enum reg_exp_options type;
    reg_exp_data data;
}Reg_Exp;

void set_reg_expression(Reg_Exp *regex_string,char *sub_string);
bool end_of_reg_exp(Reg_Exp *regex_string);
int reg_exp_len(Reg_Exp *regex_string);

void setChar(Reg_Exp *regex_string,char single_char);
void setDot(Reg_Exp *regex_string);
void setBrackets(Reg_Exp *regex_string, char low, char high);
void setParenthesis(Reg_Exp *regex_string, char *left,int left_len, char * right, int right_len);


#endif
