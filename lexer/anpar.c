#include "anlex.h"
token t;
char msg[41]; // Mensaje de error.
short error_flag = 0;

void inicio_anasintactico() {
    json();
    if (t.compLex != EOF)
        error_msg("No se esperaba fin del archivo.");
}

void json() {
    element();
    match(EOF);
}

void element() {
    if(t.compLex == 'L_LLAVE')
        object();
    else if(t.compLex == 'L_CORCHETE')
        array();
    else if (t.compLex == 'R_CORCHETE' || t.compLex == 'R_LLAVE' || t.compLex == 'COMA') {
        sprintf(msg, "Se esperaba un \"{\" o \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    } else
        getToken();
}

void array() {
    if('L_CORCHETE') {
        match("[");
        aPrima();
    } else
        error();
}

void aPrima() {
    if(t.compLex != 'R_CORCHETE') {
        element_list();
        match("R_CORCHETE");
    } else if(t.compLex == 'R_CORCHETE') {
        match("R_CORCHETE");
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"]\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void element_list() {
    if (t.compLex == 'L_CORCHETE' || t.compLex == 'L_LLAVE') {
        element();
        ePrima();
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void ePrima() {
    if(t.compLex == 'COMA') {
        match("COMA");
        element();
        ePrima();
    } else {
        getToken();
    }
}

void object() {
    match('L_LLAVE');
    oPrima();
}

void oPrima() {
    if(t.compLex == 'R_LLAVE')
        match('R_LLAVE');
    else {
        attribute_list();
        match('R_LLAVE');
    }
}

void attribute_list() {
    attribute();
    lPrima();
}

void lPrima() {
    //,
    match(',');
    attribute();
}

void attribute() {
    attribute_name();
    match('DOS_PUNTOS');
    attribute_value();
}

void attribute_name() {
    if(t.compLex == 'LITERAL_CADENA') {
        match('string');
    } else {
        sprintf(msg,"Se esperaba una \"Cadena literal\", no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void attribute_value() {
    if (t.compLex == 'L_CORCHETE' || t.compLex == 'L_LLAVE') {
        element();
    } else if(t.compLex == 'LITERAL_CADENA') {
        match('LITERAL_CADENA');
    } else if(t.compLex == 'LITERAL_NUM') {
        match('LITERAL_NUM');
    } else if(t.compLex == 'PR_TRUE') {
        match('PR_TRUE');
    } else if(t.compLex == 'PR_FALSE') {
        match('PR_FALSE');
    } else if(t.compLex == 'PR_NULL') {
        match('PR_NULL');
    } else if (t.compLex == 'R_LLAVE' || t.compLex == 'COMA' || t.compLex == 'DOS_PUNTOS') {
        sprintf(msg,"Se esperaba un \"{\" o \"[\" o \"string\" o \"number\" o \"true\" o \"false\" o \"null\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    } else {
        getToken();
    }
}

void match(int n) {
	if(t.compLex == n)
        getToken();
	else
        error_msg("Error en el match");
}

void getToken(void) {
	sigLex();
}

void error_msg(char* mensaje) {
    error_flag = 1;
    printf("Lin %d: Error Sintáctico. %s.\n", linea(), mensaje);	
}