/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define WRITELN		281
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAMCOMPLEX 	20
/************* Estructuras ********************/

typedef struct entrada {
    //definir los campos de 1 entrada de la tabla de simbolos
    char compLex[TAMCOMPLEX];
    char lexema[TAMLEX];
    struct entrada *tipoDato; // null puede representar variable no declarada	
    // aqui irian mas atributos para funciones y procedimientos...
} entrada;

typedef struct {
    char compLex[TAMCOMPLEX];
    entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
