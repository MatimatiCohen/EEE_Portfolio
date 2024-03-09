#ifndef FLAG_OF_ACTION_H
#define FLAG_OF_ACTION_H
#include <stdbool.h>


typedef struct Flag_Of_Action{
    bool is_on;
}flag_of_action;


void initialize_flag(flag_of_action *flag);
void activate(flag_of_action *flag);

#endif
