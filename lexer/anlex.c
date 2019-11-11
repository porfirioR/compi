/*
 *	Analizador L�xico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Tarea Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador l�xico que reconoce la estructura JSON que
 *      incluye numeros, string, corchetes, llaves, comas, dos puntos, true,
 *      false, null y eof para fin del archivo.
 * 
 *      Alumno
 *      Porfirio Ramon Perez Gonzalez.
 */

/*********** Inclusi�n de cabecera **************/
#include "anlex.h"

/************* Variables globales **************/

int consumir;		/* 1 indica al analizador lexico que debe devolver
			el sgte componente lexico, 0 debe devolver el actual */
char impCompLex = 's';
char cad[5 * TAMLEX];	// string utilizado para cargar mensajes de error
token t;		// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo;			// Fuente pascal
char buff[2*TAMBUFF];           // Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero =-1;		// Utilizado por el analizador lexico
int fin=0;                      // Utilizado por el analizador lexico

/**************** Funciones **********************/
// Rutinas del analizador lexico
void error(const char* mensaje) {
    printf("Error Lexico %s.\n", mensaje);
}

void sigLex() {
    int i = 0;
    char c = 0;
    char msg[41];
    entrada e;
    while((c = fgetc(archivo)) != EOF) {
        if (c == ' ' || c=='\t' || c=='\n') {
            continue;	//eliminar espacios en blanco
        } else if (c == '\"') {//ver si es un string
            i = 0;
            do {
                c = fgetc(archivo);
                i++;
                if (i >= TAMLEX) {
                    error("Longitud del String excede tama�o de buffer");
                    exit(1);
                }
            } while(isalpha(c) || isdigit(c) || c != '\"');
            if (c != EOF && c != '\"')
                ungetc(c, archivo);
            else
                c = 0;

            strcpy(t.compLex, "LITERAL_CADENA");
            t.pe = buscar("string");
            break;
        }
        else if (isalpha(c)) {
            //ver si false, true, null (o palabra reservada)
            int i = 0;
            if(c == 'f') {
                char texto[TAMLEX];
                int identificador[TAMFALSE] = {'f','a','l','s','e'};
                char correcto = 's';
                texto[i] = c;
                for (int j = 1; j < TAMFALSE; j++) {
                    c = fgetc(archivo);
                    texto[j] = c;
                    if(c == identificador[j]) {
                        i++;
                        if (i >= TAMLEX) {
                            error("Longitud del String excede tama�o de buffer");
                            exit(1);
                        }
                    } else {
                        char errorMsg[i + 1];
                        for (int x = 0;  x < i+1 ; x++) {
                            errorMsg[x] = texto[x];
                        }
                        printf("Error Lexico %s no registrado\n", errorMsg);
                        correcto = 'n';
                        j = TAMFALSE;
                        impCompLex = 'n';
                    }
                }
                if(correcto == 's') {
                    strcpy(t.compLex, "PR_FALSE");
                    t.pe = buscar("false");
                }
                break;
            }
            else if(c == 't') {
                char texto[TAMLEX];
                int identificador[TAMTRUENULL] = {'t','r','u','e'};
                char correcto = 's';
                texto[i] = c;
                for (int j = 1; j < TAMTRUENULL; j++) {
                    c = fgetc(archivo);
                    texto[j] = c;
                    if(c == identificador[j]) {
                        i++;
                        if (i >= TAMLEX) {
                            error("Longitud del String excede tama�o de buffer");
                            exit(1);
                        }
                    } else {
                        char errorMsg[i + 1];
                        for (int x = 0;  x < i+1 ; x++) {
                            errorMsg[x] = texto[x];
                        }
                        printf("Error Lexico %s no registrado\n", errorMsg);
                        correcto = 'n';
                        j = TAMTRUENULL;
                        impCompLex = 'n';
                    }
                }
                if(correcto == 's') {
                    strcpy(t.compLex, "PR_TRUE");
                    t.pe = buscar("true");
                }
                break;
            } else if(c == 'n') {
                char texto[TAMLEX];
                int identificador[TAMTRUENULL] = {'n','u','l','l'};
                char correcto = 's';
                texto[i] = c;
                for (int j = 1; j < TAMTRUENULL; j++) {
                    c = fgetc(archivo);
                    texto[j] = c;
                    if(c == identificador[j]) {
                        i++;
                        if (i >= TAMLEX) {
                            error("Longitud del String excede tama�o de buffer");
                            exit(1);
                        }
                    } else {
                        char errorMsg[i + 1];
                        for (int x = 0;  x < i+1 ; x++) {
                            errorMsg[x] = texto[x];
                        }
                        printf("Error Lexico %s no registrado\n", errorMsg);
                        correcto = 'n';
                        j = TAMTRUENULL;
                        impCompLex = 'n';
                    }
                }
                if(correcto == 's') {
                    strcpy(t.compLex, "PR_NULL");
                    t.pe = buscar("null");
                }
                break;
            } else {
                impCompLex = 'n';
                sprintf(msg, "%c no registrado", c);
                error(msg);
            }
            break;
        }
        else if (isdigit(c)) {
            //es un numero
            i = 0;
            int estado = 0;
            int acepto = 0;
            id[i] = c;
            while(!acepto) {
                switch(estado) {
                    case 0: //una secuencia netamente de digitos, puede ocurrir . o e
                        c = fgetc(archivo);
                        if (isdigit(c)) {
                            id[++i] = c;
                            estado = 0;
                        } else if(c == '.') {
                            id[++i] = c;
                            estado = 1;
                        } else if(tolower(c) == 'e') {
                            id[++i] = c;
                            estado = 3;
                        } else {
                            estado = 6;
                        }
                        break;
                    case 1://un punto, debe seguir un digito
                        c = fgetc(archivo);
                        if (isdigit(c)) {
                            id[++i] = c;
                            estado = 2;
                        } else if (c == EOF) {
                            error("No se esperaba el fin de archivo");
                            exit(1);
                        } else {
                            sprintf(msg, "No se esperaba '%c' en %s", c, id);
                            estado = -1;
                        }
                        break;
                    case 2://la fraccion decimal, pueden seguir los digitos o e
                        c = fgetc(archivo);
                        if (isdigit(c)) {
                            id[++i] = c;
                            estado = 2;
                        }
                        else if(tolower(c) == 'e') {
                            id[++i] = c;
                            estado = 3;
                        } else
                            estado = 6;
                        break;
                    case 3://una e, puede seguir +, - o una secuencia de digitos
                        c = fgetc(archivo);
                        if (c == '+' || c == '-') {
                            id[++i] = c;
                            estado = 4;
                        } else if(isdigit(c)) {
                            id[++i] = c;
                            estado = 5;
                        } else {
                            sprintf(msg, "No se esperaba '%c' en %s", c, id);
                            estado = -1;
                        }
                        break;
                    case 4://necesariamente debe venir por lo menos un digito
                        c = fgetc(archivo);
                        if (isdigit(c)) {
                            id[++i] = c;
                            estado = 5;
                        } else{
                            sprintf(msg, "No se esperaba '%c' en '%s'", c, id);
                            estado = -1;
                        }
                        break;
                    case 5://una secuencia de digitos correspondiente al exponente
                        c = fgetc(archivo);
                        if (isdigit(c)) {
                            id[++i] = c;
                            estado = 5;
                        } else {
                            estado = 6;
                        }
                        break;
                    case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
                        if (c != EOF)
                            ungetc(c, archivo);
                        else
                            c = 0;

                        id[++i] = '\0';
                        acepto = 1;
                        strcpy(t.compLex, "LITERAL_NUM");
                        t.pe = buscar("number");
                        break;
                    case -1:
                        if (c == EOF) {
                            error("No se esperaba el fin de archivo");
                            exit(1);
                        } else {
                            ungetc(c, archivo);
                            error(msg);
                            acepto = 1;
                        }
                        impCompLex = 'n';
                        break;
                }
            }
            break;
        }
        else if (c==':') {
            strcpy(t.compLex, "DOS_PUNTOS");
            t.pe = buscar(":");
            break;
        }
        else if (c == ',') {
            strcpy(t.compLex, "COMA");
            t.pe = buscar(",");
            break;
        }
        else if (c=='[') {
            strcpy(t.compLex, "L_CORCHETE");
            t.pe = buscar("[");
            break;
        }
        else if (c==']') {
            strcpy(t.compLex, "R_CORCHETE");
            t.pe=buscar("]");
            break;
        }
        else if (c=='{') {
            strcpy(t.compLex, "L_LLAVE");
            t.pe = buscar("{");
            break;
        }
        else if (c=='}') {
            strcpy(t.compLex, "R_LLAVE");
            t.pe = buscar("{");
            break;
        }
        else if (c != EOF) {
            impCompLex = 'n';
            sprintf(msg, "%c no registrado", c);
            error(msg);
            break;
        }
    }
    if (c == EOF) {
        strcpy(t.compLex, "EOF");
        t.pe = &e;
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
            sigLex();
            if(impCompLex == 's')
                printf("%s\n", t.compLex);

            impCompLex = 's';
        }
        inicio_anasintactico();
        fclose(archivo);
    } else {
        printf("Debe pasar como parametro el path al archivo fuente.\n");
        exit(1);
    }

    return 0;
}
