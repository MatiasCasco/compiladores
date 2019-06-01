/*
 *	Analizador L�xico
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Pr�ctica de Programaci�n Nro. 1
 *
 *	Descripcion:
 *	Implementa un analizador l�xico que reconoce n�meros, identificadores,
 * 	palabras reservadas, operadores y signos de puntuaci�n para un lenguaje
 * 	con sintaxis tipo Pascal.
 *
 */

/*********** Inclusi�n de cabecera **************/
#include "tablaSimbolos.c"
#include <ctype.h>

/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// LITERAL_CADENA utilizado para cargar mensajes de error
token t;				// token global para recibir componentes del Analizador Lexico

// variables para el analizador lexico

FILE *archivo, *output; 			// Fuente pascal
char buff[2*TAMBUFF];	// Buffer para lectura de archivo fuente
char id[TAMLEX];		// Utilizado por el analizador lexico
int delantero=-1;		// Utilizado por el analizador lexico
int fin=0;				// Utilizado por el analizador lexico
int numLinea=1;			// Numero de Linea
char cont_esp[260];      // Variable para contar la cantidad de espacio
int con=-1;                 // Variable que indica la cantidad de espacio que ya se ha cargado en cont_esp
//variables parser
char msg1[41];
token array_tokens[TAMARRAY];
int tamano_actual = 0;

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	printf("\n Lin %d: Error Lexico. %s.\n",numLinea,mensaje);
}

void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	entrada e;

	while((c=fgetc(archivo))!=EOF)
	{
	   	if(c=='\n') {
                numLinea++;
                fprintf(output,"\n");
		    	continue;
        }
	    else if (c==' ') {
		    fprintf(output," ");
		    continue;
	    }
		else if (c=='\t')
		{
			//si es un tabulador que guarde los 4 espacios correspondientes
			while(c=='\t')
			{
				fprintf(output,"%c", c);
				c=fgetc(archivo);
			}
		}
		else if (isalpha(c))
		{
			//es un identificador (o palabra reservada)
			i=0;
			do{
				id[i]=toupper(c);
				i++;
				c=toupper(fgetc(archivo));
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tama�o de buffer");
			}while(isalpha(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;

            t.pe=buscar(id);

			if (t.pe->compLex==-1) {
			  error(("ERROR token:%s no forma parte de la gramatica",id));
			  //t.pe=buscar("ERROR");
			  //t.compLex=ERROR;
			  /*consumir mas caracteres hasta que se encuentre una coma,espacio o EOF para tratar de reconocer mas tokens*/
			  while(c!=' ' || c!=EOF || c!=','){
                    c=fgetc(archivo);
                    if (c == ',') {
                      ungetc(c,archivo);
                      break;
                    }
                    if (c==EOF)
                    error("No se esperaba el fin del archivo");
               }
            }
			else
                t.compLex=t.pe->compLex;
                t.linea=numLinea;//NUEVO
                array_tokens[tamano_actual] = t;//NUEVO
                tamano_actual++;//
			break;
		}
		else if (isdigit(c))
		{
				//es un numero
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;

				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;

					case 1://un punto, debe seguir un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							//id=" ";
							//fprintf(output,"%s %s %c",msg,"No se esperaba '%c' despues del signo",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						t.pe=buscar(id);
						if (t.pe->compLex==-1)
						{
							strcpy(e.lexema,id);
							e.compLex=LITERAL_NUM;
							insertar(e);
							t.pe=buscar(id);
						}
						t.compLex=LITERAL_NUM;
						t.linea=numLinea;//NUEVO
						array_tokens[tamano_actual] = t;//NUEVO
                        tamano_actual++;//
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else {
							error(msg);
							id[++i]='\0';
							//t.pe=buscar("ERROR");
							//t.compLex=ERROR;
							while(c!=' ' || c!=EOF || c!=','){
							   c=fgetc(archivo);
							   if (c == ',') {
								ungetc(c,archivo);
								break;
							    }
							   if (c==EOF)
								error("No se esperaba el fin del archivo");
							}
							acepto=1;
							break;
						}
						//exit(1); no nos sirve
					}

				}
			break;
		}

		else if (c==':')
		{
			//puede ser un :
			c=fgetc(archivo);
			ungetc(c,archivo);
			t.compLex=DOS_PUNTOS;
			t.pe=buscar(":");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c==',')
		{
			t.compLex=COMA;
			t.pe=buscar(",");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}

		else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.pe=buscar("[");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.pe=buscar("]");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c=='\"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\"')
				{
					c=fgetc(archivo);
					id[i]='\"';
					i++;
					break;
				}
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					id[i]=c;
					i++;
				}
			}while(isascii(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			t.pe=buscar(id);
			t.compLex=t.pe->compLex;
			if (t.pe->compLex==-1)
			{
				strcpy(e.lexema,id);
				e.compLex=LITERAL_CADENA;
				insertar(e);
				t.pe=buscar(id);
				t.compLex=e.compLex;
			}
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c=='{')
		{
			t.compLex=L_LLAVE;
			t.pe=buscar("{");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c=='}')
		{
			t.compLex=R_LLAVE;
			t.pe=buscar("}");
			t.linea=numLinea;//NUEVO
            array_tokens[tamano_actual] = t;//NUEVO
            tamano_actual++;//
			break;
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.compLex=EOF;
		// strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}

}
/********************ANalisis Sintactico**********************/
token token1;
int setSync[TAMCONJ];
int posicion=-1;
int errores= 0;

void error_sintac(const char* mensaje)
{
    errores=1;
   printf(" Error sintactico. %s",  mensaje);
}

void match(int t)
{
    if(t == token1.compLex)
    {
        getToken();
    }else
    {
        sprintf(msg1, "En la linea %d No se esperaba  %s\n", token1.linea, (*token1.pe).lexema );
        error_sintac(msg1);
    }
}

void getToken()
{
    posicion++;
    if(posicion<=sizeof(array_tokens))
        token1 = array_tokens[posicion];

}

void parser()
{
    getToken();
    int con_sig []={EOF,0,0,0,0,0,0,0};
    json(con_sig);
}

void json(int setSync[])
{
    if(token1.compLex == L_LLAVE || token1.compLex == L_CORCHETE){
        element();
    }else
    {
        sprintf(msg1, "En la linea %d Se esperaba L_LLAVE o L_CORCHETE\n" , token1.linea);
        error_sintac(msg1);
    }
}

void element()
{
    int con_primero[] = {L_LLAVE, L_CORCHETE,0,0,0,0,0,0};
    int con_sig []={EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    check_input(con_primero, con_sig);
    switch(token1.compLex)
    {
        case L_LLAVE:
            object(con_sig);
            break;
        case L_CORCHETE:
            array(con_sig);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba L_LLAVE o L_CORCHETE\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
    check_input(con_sig,con_primero);
}

void object(int setSync[])
{
    int con_primero[]={L_LLAVE,0,0,0,0,0,0,0};
    int con_sig[]={EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    check_input(con_primero,con_sig);
    switch (token1.compLex)
    {
        case L_LLAVE:
            match(L_LLAVE);
            objectp(setSync);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba L_LLAVE\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
    check_input(con_sig,con_primero);
}

void array(int setSync[])
{
    int con_primero[]={L_CORCHETE,0,0,0,0,0,0,0};
    int con_si[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    check_input(con_primero,con_si);
    switch (token1.compLex)
    {
        case L_CORCHETE:
            match(L_CORCHETE);
            arrayp(setSync);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba L_CORCHETE\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
    check_input(con_si,con_primero);
}

void objectp(int setSync[])
{
    int con_primero[]={LITERAL_CADENA,R_LLAVE,0,0,0,0,0,0};
    int con_si[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    check_input(con_primero,con_si);
    switch (token1.compLex)
    {
        case LITERAL_CADENA:
            atribute_list();
            match(R_LLAVE);
            break;
        case R_LLAVE:
            match(R_LLAVE);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba LITERAL_CADENA o R_LLAVE\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
    check_input(con_si,con_primero);
}

void arrayp(int setSync[])
{
    int con_primero[]={L_LLAVE, L_CORCHETE, R_CORCHETE,0,0,0,0,0};
    int con_si[] = {EOF,COMA, R_CORCHETE, R_LLAVE,0,0,0,0};
    check_input(con_primero,con_si);
    switch (token1.compLex)
    {
        case L_LLAVE:
            element_list();
            match(R_CORCHETE);
            break;
        case L_CORCHETE:
            element_list();
            match(R_CORCHETE);
            break;
        case R_CORCHETE:
            match(R_CORCHETE);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba L_LLAVE, L_CORCHETE o R_CORCHETE\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
    check_input(con_si,con_primero);
}

void element_list()
{
    int con_primero[]={L_LLAVE,L_CORCHETE,0,0,0,0,0,0};
    int con_siguient[]={R_CORCHETE,0,0,0,0,0,0,0};
    check_input(con_primero,con_siguient);
    if(token1.compLex ==L_LLAVE || token1.compLex == L_CORCHETE){
        element();
        element_listp(con_siguient);
    }else{
        sprintf(msg1, "En la linea %d Se esperaba L_LLAVE, L_CORCHETE\n" , token1.linea);
        error_sintac(msg1);
        check_input(con_siguient,con_primero);
    }
}


void element_listp(int setSync[])
{
    int con_primero[]={COMA,0,0,0,0,0,0,0};
    int con_sig[]={R_CORCHETE};
    if(token1.compLex==R_CORCHETE){
        check_input(con_sig,con_primero);
    }else{
        check_input(con_primero,con_sig);
    }
    switch(token1.compLex)
    {
        case COMA:
            match(COMA);
            element();
            element_listp(con_sig);
            break;
    }
   check_input(con_sig,con_primero);
}

void atribute()
{
    int con_primero[]={LITERAL_CADENA,0,0,0,0,0,0,0};
    int con_siguien[]={COMA,R_LLAVE,0,0,0,0,0,0};
    check_input(con_primero,con_siguien);
    switch(token1.compLex)
    {
        case LITERAL_CADENA:
            atribute_name();
            match(DOS_PUNTOS);
            atribute_value(con_siguien);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba LITERAL_CADENA\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
}

void atribute_list()
{
    int con_primero[]={LITERAL_CADENA,0,0,0,0,0,0,0};
    int con_siguie[]={R_LLAVE,0,0,0,0,0,0,0};
    check_input(con_primero,con_siguie);
    if(token1.compLex==LITERAL_CADENA){
        atribute();
        atribute_listp(con_siguie);
    }else{
        sprintf(msg1, "En la linea %d Se esperaba LITERAL_CADENA\n" , token1.linea);
        error_sintac(msg1);
    }
    check_input(con_siguie,con_primero);
}

void atribute_listp(int setSync[])
{
    int con_primero[]={COMA,0,0,0,0,0,0,0};
    int con_s[]={R_LLAVE,0,0,0,0,0,0,0};

    if(token1.compLex == R_LLAVE){
        check_input(con_s,con_primero);
    }else{
        check_input(con_primero,con_s);
    }
    switch(token1.compLex)
    {
        case COMA:
            match(COMA);
            atribute();
            atribute_listp(con_s);
            break;
    }
    check_input(con_s,con_primero);
}

void atribute_name()
{
    int con_primero[]={LITERAL_CADENA,0,0,0,0,0,0,0};
    int con_sigui[]={DOS_PUNTOS,0,0,0,0,0,0,0};
    check_input(con_primero,con_sigui);
    switch(token1.compLex)
    {
        case LITERAL_CADENA:
            match(LITERAL_CADENA);
            break;
        default:
            sprintf(msg1, "En la linea %d Se esperaba LITERAL_CADENA\n" , token1.linea);
            error_sintac(msg1);
            break;
    }
}

void atribute_value(int setSync[])
{
    int con_primero[]={L_LLAVE, L_CORCHETE, LITERAL_CADENA, LITERAL_NUM, PR_TRUE, PR_FALSE,PR_NULL,0};
    int con_siv[]={COMA,R_LLAVE,0,0,0,0,0,0};
    check_input(con_primero,con_siv);
    switch(token1.compLex)
    {
        case L_LLAVE:
            element();
            break;
        case L_CORCHETE:
            element();
            break;
        case LITERAL_CADENA:
            match(LITERAL_CADENA);
            break;
        case LITERAL_NUM:
            match(LITERAL_NUM);
            break;
        case PR_TRUE:
            match(PR_TRUE);
            break;
        case PR_FALSE:
            match(PR_FALSE);
            break;
        case PR_NULL:
            match(PR_NULL);
            break;
        default:
            error_sintac("Se esperaba L_LLAVE, L_CORCHETE, LITERAL_CADENA, LITERAL_NUM, TRUE,FALSE O NULL\n");
            break;
    }
}

/*** P A N I C  M O D E***/

void scanto(int setSync1[], int setSync2[])
{
    setSync[16];
    int i=0;
    int ban=0;
    int tam= 8;
    int tam1 = 16;
    int j;
    /***volcado de los dos array de syncronizacion*/
    for(j=0; j<8;j++)
    {
        setSync[j] = setSync1[j];
    }
    int k;
    int y=0;
    for(k=8; k<16;k++){
        setSync[k] = setSync2[y];
        y++;
    }
    do{
        while(i<tam1)
        {
            if(token1.compLex != setSync[i] && token1.compLex != EOF)
            {
                // Poner bandera y si no esta en el set de sincronizaciones entonces hacer get y volver a empezar el for o do while.
                ban = 1;
                i++;
            }else{
                ban=0;
                i=16;
                break;
            }
       }
       if(ban==1)
       {
           getToken();
           i=0;
       }
    }while(i<tam1);
}

void check_input(int con_primero[], int con_siguiente[])
{
    int tam= 8;
    int i;
    int ban1= 0; // Si es cero si esta en el conjunto primero.
    for (i = 0; i < tam; i++)
    {
        if(token1.compLex == con_primero[i])
        {
            ban1=0;
            break;
        }else{
            ban1 = 1; // Si se prende es porque el token no esta en el conjuntoprimero y debe lanzar un error.
        }
    }
    if(ban1==1)
    {
        sprintf(msg1, "En la linea %d No se esperaba  %s\n", token1.linea, (*token1.pe).lexema);
        error_sintac(msg1);
        scanto(con_primero, con_siguiente);
    }
}


/**********************MAIN******************************/
int main(int argc,char* args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();

	//como imprimir un \n
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		while (t.compLex!=EOF){
			sigLex();
        }
		fclose(archivo);
        parser();
        if(errores == 0)
        {
            printf("Es Correctamente Sintactico\n");
        }else{
            printf("No es correctamente sintactico\n");
        }
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}



	return 0;
}
