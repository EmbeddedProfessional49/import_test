
#include <stdio.h>
#include <rfid_card.h>

/*
 * Variable naming standard
 * Global = VariableName
 * Local = variable_name
 * Global_static = VariableName__
 * Local_static = variable_name__
 */


int main (void) {

    RFID_MODULE_OPS *card_1;

    Get_RFID_Card (card_1, 0);

    card_1->init ();



    return 0;
}

