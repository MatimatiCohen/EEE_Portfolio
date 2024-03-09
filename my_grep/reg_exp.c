#include "reg_exp.h"
#include <assert.h>
#include <string.h>
#include <stdbool.h>


#define JUMP_TO_END_PARENTHESIS 2
#define JUMP_TO_LOW_RANGE 1
#define JUMP_TO_HIGH_RANGE 3
#define JUMP_TO_END_BRACKET 4
#define CHAR_LENGTH 1
#define BRACKETS_LENGTH 5
#define PARENTHESIS_LENGTH 3

enum reg_exp_options check_type(char p_sub_str );
void parse_parenthesis(char *sub_string, char **left, int *left_len, char ** right, int *right_len);
int reg_exp_type_size(Reg_Exp *regex_string);

void set_reg_expression(Reg_Exp *regex_string,char *sub_string){
    char* left=NULL, *right=NULL, *high=NULL, *low=NULL;
    int left_len=0,right_len=0;
    while (*sub_string!='\0'){
        switch (check_type(*sub_string)) {
            case CHAR:
                if (*sub_string=='\\'){
                    sub_string++;
                }
                setChar(regex_string,*sub_string);
                break;
            case DOT:
                setDot(regex_string);
                break;
            case BRACKETS:
                low=sub_string+JUMP_TO_LOW_RANGE;
                high=sub_string+JUMP_TO_HIGH_RANGE;
                setBrackets(regex_string,*low,*high);
                sub_string+=JUMP_TO_END_BRACKET;
                break;
            case PARENTHESIS:
                parse_parenthesis(sub_string,&left,&left_len,&right,&right_len);
                setParenthesis(regex_string,left,left_len,right,right_len);
                sub_string+=left_len+right_len+JUMP_TO_END_PARENTHESIS;
                break;
            default:
                assert(false);
        }
        sub_string++;
        regex_string++;
    }
    setChar(regex_string,'\0');
}


void setChar(Reg_Exp *regex_string,char single_char){
    regex_string->type=CHAR;
    regex_string->data.single_char=single_char;
}

void setDot(Reg_Exp *regex_string){
    regex_string->type=DOT;
    regex_string->data.dot=true;
}


void setBrackets(Reg_Exp *regex_string, char low, char high){
    regex_string->type=BRACKETS;
    regex_string->data.bra.low_str=low;
    regex_string->data.bra.high_str=high;
}

void setParenthesis(Reg_Exp *regex_string, char *left,int left_len, char * right, int right_len){
    regex_string->type= PARENTHESIS;
    regex_string->data.par.left_len=left_len;
    regex_string->data.par.left_str=left;
    regex_string->data.par.right_len=right_len;
    regex_string->data.par.right_str=right;
    regex_string->data.par.x_on_right_len=false;
    regex_string->data.par.x_on_left_len=false;
}

void parse_parenthesis(char *sub_string, char **left, int *left_len, char ** right, int *right_len){
    if (*sub_string!='('){
        return;
    }
    *left_len=0;
    sub_string++;
    *left=sub_string;
    while (*sub_string!='|'){
        assert(*sub_string!='\0');
        *left_len+=1;
        sub_string++;
    }
    *right_len=0;
    sub_string++;
    *right=sub_string;
    while (*sub_string!=')'){
        assert(*sub_string!='\0');
        *right_len+=1;
        sub_string++;
    }
}

enum reg_exp_options check_type(char p_sub_str ){
    switch(p_sub_str){
        case '.':
            return DOT;
        case '[':
            return BRACKETS;
        case '(':
            return PARENTHESIS;
        default:
            return CHAR;
    }
}


bool end_of_reg_exp(Reg_Exp *regex_string){
    bool check=(regex_string->type==CHAR) && (regex_string->data.single_char=='\0');
    return check;
}


int reg_exp_len(Reg_Exp *regex_string){
    int len=0;
    while (!end_of_reg_exp(regex_string)){
        len=len+reg_exp_type_size(regex_string);
        regex_string++;
    }
    return len;
}


int reg_exp_type_size(Reg_Exp *regex_string){
    switch (regex_string->type) {
        case CHAR:
            return CHAR_LENGTH;
        case DOT:
            return CHAR_LENGTH;
        case PARENTHESIS:
         if(regex_string->data.par.x_on_left_len){
            return regex_string->data.par.left_len;
          }
          if(regex_string->data.par.x_on_right_len){
             return regex_string->data.par.right_len;
           }
        case BRACKETS:
            return CHAR_LENGTH;
        default:
            assert(false);
    }
}
