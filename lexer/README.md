compiladores: Analizador Lexico
============
Alumno: Porfirio Ramon Perez Gonzalez.

Compiladores FPUNA

Para compilar: gcc anlex.c tablaSimbolos.c -o lexer

Para correr: lexer.exe fuente.txt

Con muestra de errores: lexer.exe prueba.txt


Para Compilar parser

Para compilar: gcc anlex.c tablaSimbolos.c anpar.c -o parser
Para correr: parser.exe fuente.txt