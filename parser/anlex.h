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
#define TAMCONJ            10
#define TAMARRAY          500


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
	int linea; //nuevo
	entrada *pe;
} token;

/************* Prototipos ********************/
void insertar(entrada e);
entrada* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
/*** Prototipos del Parser ***/
void json(int setSync[]);
void element();
void array(int setSync[]);
void arrayp(int setSync[]);
void element_list();
void element_listp(int setSync[]);
void object(int setSync[]);
void objectp(int setSync[]);
void atribute_list();
void atribute_listp(int setSync[]);
void atribute();
void atribute_name();
void atribute_value(int setSync[]);
void getToken();
void check_input(int con_primero[], int con_siguiente[]);
void match(int t);
void setsync(int []);
void parser();
void scanto(int setSync1[], int setSync2[]);
