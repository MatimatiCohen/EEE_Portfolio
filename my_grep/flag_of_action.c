#include "flag_of_action.h"

void activate(flag_of_action *flag){
    flag->is_on=true;
}

void initialize_flag(flag_of_action *flag){
    flag->is_on=false;
}
