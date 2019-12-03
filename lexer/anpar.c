#include "anlex.h"

FILE *archivo;          // Fuente json
FILE *traducidoXML;
token t;
char msg[41];           // Mensaje de error.
char* atriName[1000][41];
short entro_error = 0;
int profundidad = 0;
char* tabulador = "";

// Función que concatena tabulaciones.
char* tabular(int valor) {
    switch (valor)
    {
    case 0:
        tabulador = "";
        break;
    case 1:
        tabulador = "\t";
        break;
    case 2:
        tabulador = "\t\t";
        break;
    case 3:
        tabulador = "\t\t\t";
        break;
    case 4:
        tabulador = "\t\t\t\t";
        break;
    case 5:
        tabulador = "\t\t\t\t\t";
        break;
    case 6:
        tabulador = "\t\t\t\t\t\t";
        break;
    case 7:
        tabulador = "\t\t\t\t\t\t\t";
        break;
    case 8:
        tabulador = "\t\t\t\t\t\t\t\t";
        break;
    case 9:
        tabulador = "\t\t\t\t\t\t\t\t\t";
        break;
    default:
        tabulador = "\t\t\t\t\t\t\t\t\t\t";
        break;
    }
    return tabulador;
}

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
    if(strcmp(t.compLex, "R_CORCHETE") != 0) {
        element_list();
        match("R_CORCHETE");
        //puede ser aca
    } else if(strcmp(t.compLex, "R_CORCHETE") == 0) {
        match("R_CORCHETE");
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"]\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void element_list() {
    if (strcmp(t.compLex, "L_CORCHETE") == 0 || strcmp(t.compLex, "L_LLAVE") == 0) {
        fprintf(traducidoXML, "\n%s<item>\n", tabular(profundidad++));
        element();
        ePrima();
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void ePrima() {
    profundidad--;
    fprintf(traducidoXML, "%s</item>\n", tabular(profundidad--));
    if(strcmp(t.compLex, "COMA") == 0) {
        profundidad = profundidad + 1;
        fprintf(traducidoXML, "%s<item>\n", tabular(profundidad++));
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
    } else if(strcmp(t.compLex, "R_LLAVE") == 0) {
        match("R_LLAVE");
    } else {
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
    char* nombre = attribute_name();
    if(strcmp(t.compLex, "DOS_PUNTOS") == 0) {//Manejador de error
        match("DOS_PUNTOS");
        attribute_value();
        //este no tiene tabs
        fprintf(traducidoXML, "</%s>\n", nombre);
        --profundidad;
    }
}

char* attribute_name() {
    if(strcmp(t.compLex, "LITERAL_CADENA") == 0) {
        char* nombreAtributo = t.pe->lexema;
        nombreAtributo++;
        nombreAtributo[strlen(nombreAtributo)-1] = '\0';
        fprintf(traducidoXML, "%s<%s>",tabular(profundidad), nombreAtributo);
        profundidad++;
        match("LITERAL_CADENA");
        return nombreAtributo;
    } else {
        sprintf(msg, "Se esperaba una \"Cadena literal\", no \"%s\"", t.pe->lexema);
        error_msg(msg);
    }
}

void attribute_value() {
    if (strcmp(t.compLex, "L_CORCHETE") == 0 || strcmp(t.compLex, "L_LLAVE") == 0) {
        element();
    } else if(strcmp(t.compLex, "LITERAL_CADENA") == 0) {
        fprintf(traducidoXML, "%s", t.pe->lexema);
        match("LITERAL_CADENA");
    } else if(strcmp(t.compLex, "LITERAL_NUM") == 0) {
        fputs(t.pe->lexema, traducidoXML);
        match("LITERAL_NUM");
    } else if(strcmp(t.compLex, "PR_TRUE") == 0) {
        fputs(t.pe->lexema, traducidoXML);
        match("PR_TRUE");
    } else if(strcmp(t.compLex, "PR_FALSE") == 0) {
        fputs(t.pe->lexema, traducidoXML);
        match("PR_FALSE");
    } else if(strcmp(t.compLex, "PR_NULL") == 0) {
        fputs(t.pe->lexema, traducidoXML);
        match("PR_NULL");
    } else {
        getToken();
    }
}

void match(char* n) {
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
            traducidoXML = fopen( "outputXML.txt", "w" );
            parser();
            if(traducidoXML == NULL) {
                printf("%s", "Error al abrir el traducctor");
                exit(1);
            }
            if (entro_error == 0)
                printf("%s\n", t.compLex);

        }
        if (entro_error == 0)
            printf("Sintacticamente correcto.\n");

        fclose(archivo);
        fclose(traducidoXML);
    } else {
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }
    fclose(traducidoXML);
    return 0;
}