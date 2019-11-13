/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101
#define TAMCOMPLEX 	20
#define TAMTRUENULL     4
#define TAMFALSE        5

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

//parser
void json();
void element();
void array();
void aPrima();
void element_list();
void ePrima();
void object();
void oPrima();
void attribute_list();
void lPrima();
void attribute();
void attribute_name();
void attribute_value();
void match(char* n);
void error_msg(char* mensaje);
void inicio_anasintactico();
void getToken(void);