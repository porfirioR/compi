#include "anlex.h"

FILE *archivo;          // Fuente json
FILE *traducidoXML;
FILE *errorSintactico;
token t;
char msg[200];           // Mensaje de error.
short entro_error = 0;
int profundidad = 0;
char* tabulador = "";
int arrayIdentacion = 0;

// Función que concatena tabulaciones.
char* tabular(int valor) {
    char* elementoTab = "";
    switch (valor)
    {
    case 0:
        elementoTab = "";
        break;
    case 1:
        elementoTab = "\t";
        break;
    case 2:
        elementoTab = "\t\t";
        break;
    case 3:
        elementoTab = "\t\t\t";
        break;
    case 4:
        elementoTab = "\t\t\t\t";
        break;
    case 5:
        elementoTab = "\t\t\t\t\t";
        break;
    case 6:
        elementoTab = "\t\t\t\t\t\t";
        break;
    case 7:
        elementoTab = "\t\t\t\t\t\t\t";
        break;
    case 8:
        elementoTab = "\t\t\t\t\t\t\t\t";
        break;
    case 9:
        elementoTab = "\t\t\t\t\t\t\t\t\t";
        break;
    default:
        elementoTab = "\t\t\t\t\t\t\t\t\t\t";
        break;
    }
    return elementoTab;
}

void cerrarBorrarArchivo() {
    fclose(traducidoXML);
    fclose(errorSintactico);
    if(entro_error != 0) {
        traducidoXML = fopen( "outputXML.txt", "w" );
        fclose(traducidoXML);
        remove("outputXML.txt");
    } else {
        remove("error.txt");
    }
}

void inicio_anasintactico() {
    if(strcmp(t.compLex, "L_LLAVE") == 0 || strcmp(t.compLex, "L_CORCHETE") == 0) {
        json();
    } else {
        sprintf(msg, "Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        fprintf(errorSintactico, "Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
    }

    if (strcmp(t.compLex, "EOF") != 0) {
        error_msg("No se esperaba fin del archivo.");
        fprintf(errorSintactico, "No se esperaba fin del archivo.\n");
        cerrarBorrarArchivo();
        exit(1);
    }
}

void json() {
    element();
    match("EOF");
}

void element() {
    if(strcmp(t.compLex, "L_LLAVE") == 0) {
        object();
    } else if(strcmp(t.compLex, "L_CORCHETE") == 0) {
        array();
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"{\" no \"%s\"", t.pe->lexema);
        fprintf(errorSintactico, msg);
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
        fprintf(errorSintactico, "%s\n", msg);
        error_msg(msg);
    }
}

void aPrima() {
    if(strcmp(t.compLex, "R_CORCHETE") != 0) {
        element_list();
        match("R_CORCHETE");
        arrayIdentacion++;
        arrayIdentacion++;
    } else if(strcmp(t.compLex, "R_CORCHETE") == 0) {
        match("R_CORCHETE");
    } else {
        sprintf(msg,"Se esperaba un \"[\" o \"]\" no \"%s\"", t.pe->lexema);
        fprintf(errorSintactico, "%s\n", msg);
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
        fprintf(errorSintactico, "%s\n", msg);
        error_msg(msg);
        getToken();
    }
}

void ePrima() {
    profundidad--;
    fprintf(traducidoXML, "%s</item>\n", tabular(profundidad));
    profundidad--;
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
        //match("R_LLAVE");
    }
    if(strcmp(t.compLex, "R_LLAVE") == 0) {
        match("R_LLAVE");
    } else {
        sprintf(msg,"Se esperaba una \"Cadena literal\" o un \"], no \"%s\"", t.pe->lexema);
        error_msg(msg);
        fprintf(errorSintactico, "%s\n", msg);
        //prueba
        getToken();
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
        if(arrayIdentacion <= 0) {
            fprintf(traducidoXML, "</%s>\n", nombre);
            --profundidad;
        } else if(arrayIdentacion > 0) {
            fprintf(traducidoXML, "%s</%s>\n", tabular(arrayIdentacion), nombre);
            arrayIdentacion = arrayIdentacion - 4;
        }
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
        fprintf(errorSintactico, "%s\n", msg);
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
        sprintf(msg, "Se esperaba una \"Cadena literal\",  \"{\", \"[\", \"true\", \"false\", \"null\", \"numero\" no: \"%s\"", t.pe->lexema);
        fprintf(errorSintactico, "%s\n", msg);
        error_msg(msg);
        getToken();
    }
}

void match(char* n) {
    
	if(strcmp(t.compLex, n) == 0) {
        getToken();
    } else {
        printf("intentando machear: %s con %s", t.compLex, n);
        error_msg("Error en el match");
    }
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
            errorSintactico = fopen( "error.txt", "w" );
            parser();
            if(traducidoXML == NULL || errorSintactico == NULL) {
                printf("%s", "Error al abrir el traducctor o registro de errores.");
                exit(1);
            }
        }
        if (entro_error == 0) {
            printf("Sintacticamente correcto.\n");
        }
        cerrarBorrarArchivo();
        fclose(archivo);
    } else {
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }
    return 0;
}