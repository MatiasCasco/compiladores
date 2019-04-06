/*********** Librerias utilizadas **************/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

/************* Definiciones ********************/

//Codigos
#define PR_TRUE		256	// MODIFICADO
#define PR_FALSE	257	// MODIFICADO
#define LITERAL_NUM	258 // CAMBIADO
#define LITERAL_CADENA	259 // CAMBIADO
#define ERROR		260
// ASCII
#define L_LLAVE		123 // { CODIGO ASCII
#define R_LLAVE		125 // } CODIGO ASCII
#define L_CORCHETE	91 // { CODIGO ASCII
#define R_CORCHETE	93 // } CODIGO ASCII
#define COMA 		44
#define PR_NULL		00
#define DOS_PUNTOS	58
			
// Fin Codigos
#define TAMBUFF 	5
#define TAMLEX 		50
#define TAMHASH 	101

/************* Estructuras ********************/

typedef struct entrada{
	//definir los campos de 1 entrada de la tabla de simbolos
	int compLex;
	char lexema[TAMLEX];	
	struct entrada *tipoDato; // null puede representar variable no declarada	
	// aqui irian mas atributos para funciones y procedimientos...
	
} entrada;

typedef struct {
	int compLex;
	entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
