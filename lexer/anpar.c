#include "anlex.h"

FILE *archivo;          // Fuente json

token t;
char msg[41];           // Mensaje de error.
short entro_error = 0;

void inicio_anasintactico() {
    json();
    if (strcmp(t.compLex, "EOF") != 0) {
        error_msg("No se esperaba fin del archivo.");
        exit(1);
    }
}

void json() {
    element();
    match("EOF");
}

void element() {
    if(strcmp(t.compLex, "L_LLAVE") == 0)
        object();
    else if(strcmp(t.compLex, "L_CORCHETE") == 0) {
        array();
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
        getToken();
    }
}

void array() {
    if(strcmp(t.compLex, "L_CORCHETE") == 0) {
        match("L_CORCHETE");
        aPrima();
    } else {
        sprintf(msg,"Se esperaba un \"[\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void aPrima() {
    //printf("\nLlego a': %s\n", t.compLex);
    if(strcmp(t.compLex, "R_CORCHETE") != 0) {
        element_list();
        match("R_CORCHETE");
    } else if(strcmp(t.compLex, "R_CORCHETE") == 0) {
        match("R_CORCHETE");
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"]\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void element_list() {
    if (strcmp(t.compLex, "L_CORCHETE") == 0 || strcmp(t.compLex, "L_LLAVE") == 0) {
        element();
        ePrima();
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void ePrima() {
    if(strcmp(t.compLex, "COMA") == 0) {
        match("COMA");
        element();
        ePrima();
    }
}

void object() {
    match("L_LLAVE");
    oPrima();
}

void oPrima() {
    if(strcmp(t.compLex, "R_LLAVE") != 0) {
        attribute_list();
        match("R_LLAVE");
    } else if(strcmp(t.compLex, "R_LLAVE") == 0)
        match("R_LLAVE");
    else {
        sprintf(msg,"Se esperaba una \"Cadena literal\" o un \"], no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void attribute_list() {
    attribute();
    lPrima();
}

void lPrima() {
    if(strcmp(t.compLex, "COMA") == 0) {
        match("COMA");
        attribute();
        lPrima();
    }
}

void attribute() {
    attribute_name();
    if(strcmp(t.compLex, "DOS_PUNTOS") == 0) {//Manejador de error
        match("DOS_PUNTOS");
        attribute_value();
    }
}

void attribute_name() {
    if(strcmp(t.compLex, "LITERAL_CADENA") == 0) {
        match("LITERAL_CADENA");
    } else {
        sprintf(msg,"Se esperaba una \"Cadena literal\", no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void attribute_value() {
    if (strcmp(t.compLex, "L_CORCHETE") == 0 || strcmp(t.compLex, "L_LLAVE") == 0) {
        element();
    } else if(strcmp(t.compLex, "LITERAL_CADENA") == 0) {
        match("LITERAL_CADENA");
    } else if(strcmp(t.compLex, "LITERAL_NUM") == 0) {
        match("LITERAL_NUM");
    } else if(strcmp(t.compLex, "PR_TRUE") == 0) {
        match("PR_TRUE");
    } else if(strcmp(t.compLex, "PR_FALSE") == 0) {
        match("PR_FALSE");
    } else if(strcmp(t.compLex,  "PR_NULL") == 0) {
        match("PR_NULL");
    } else {
        getToken();
    }
}

void match(char* n) {
    //printf("\nLlego aca: %s,---Componente: %s\n", n, t.compLex);
	if(strcmp(t.compLex, n) == 0) {
        getToken();
//    else if(strcmp(t.compLex, "EOF") == 0 && strcmp(n, "EOF") != 0) {
//        printf("No se esperaba fin del archivo.\n");
//        entro_error = 1;
        //exit(1);
    } else
        error_msg("Error en el match");
}

void getToken(void) {
	sigLex();
}

void error_msg(char* mensaje) {
    entro_error = 1;
    printf("Error Sintactico. %s.\n", mensaje);
}

void parser() {
    while (strcmp(t.compLex, "EOF") != 0) {
        getToken();
        inicio_anasintactico();
    }
}

int main(int argc,char* args[]) {
    // inicializar analizador lexico
    initTabla();
    initTablaSimbolos();

    if(argc > 1) {
        if (!(archivo = fopen(args[1], "rt"))) {
            printf("Archivo no encontrado.\n");
            exit(1);
        }
        while (strcmp(t.compLex, "EOF") != 0) {
            parser();
            if (entro_error == 0)
                printf("%s\n", t.compLex);
        }
        if (entro_error == 0)
            printf("Sintacticamente correcto.\n");

        fclose(archivo);
    } else {
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;

}