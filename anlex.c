/*
 *	Analizador Léxico	
 *	Curso: Compiladores y Lenguajes de Bajo de Nivel
 *	Práctica de Programación Nro. 1
 *	
 *	Descripcion:
 *	Implementa un analizador léxico que reconoce números, identificadores, 
 * 	palabras reservadas, operadores y signos de puntuación para un lenguaje
 * 	con sintaxis tipo Pascal.
 *	
 */

/*********** Inclusión de cabecera **************/
#include "tablaSimbolos.c"


/************* Variables globales **************/

int consumir;			/* 1 indica al analizador lexico que debe devolver
						el sgte componente lexico, 0 debe devolver el actual */

char cad[5*TAMLEX];		// string utilizado para cargar mensajes de error
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

/**************** Funciones **********************/


// Rutinas del analizador lexico

void error(const char* mensaje)
{
	fprintf(output,"\n Lin %d: Error Lexico. %s.\n",numLinea,mensaje);	
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
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMLEX)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			
			t.pe=buscar(id);
			
			if (t.pe->compLex==-1) {
			  error("ERROR");
			  t.pe=buscar("ERROR");
			  t.compLex=ERROR;
			  while(c!=' ' || c!=EOF || c!=','){
				c=fgetc(archivo);
				if (c == ',') {
				  ungetc(c,archivo);
				  break;		
				}
				if (c==EOF)
				error("No se esperaba el fin del 									archivo");								
			}
		       }
			else
			  t.compLex=t.pe->compLex;			
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
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else {
							error(msg);
							id[++i]='\0';
							t.pe=buscar("ERROR");
							t.compLex=ERROR;
							while(c!=' ' || c!=EOF || c!=','){
							   c=fgetc(archivo);
							   if (c == ',') {
								ungetc(c,archivo);
								break;		
							    }
							   if (c==EOF)
								error("No se esperaba el fin del 									archivo");								
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
			break;
		}
		else if (c==',')
		{
			t.compLex=COMA;
			t.pe=buscar(",");
			break;
		}
		
		else if (c=='[')
		{
			t.compLex=L_CORCHETE;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.compLex=R_CORCHETE;
			t.pe=buscar("]");
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
			break;
		}
		else if (c=='{')
		{
			t.compLex=L_LLAVE;
			t.pe=buscar("{");
			break;
		}
		else if (c=='}')
		{
			t.compLex=R_LLAVE;
			t.pe=buscar("}");
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

int main(int argc,char* args[])
{
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();

	output = fopen ("output.txt", "w");
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
			//printf("Lin %d: %s -> %d\n",numLinea,t.pe->lexema,t.compLex);
		if(con > -1){
                	int j = 0;
                for(j=0; j<=con;j++)
                    fprintf(output,"%c",cont_esp[j]);
            	}
		switch(t.compLex)
			{
                case L_CORCHETE:
                    fprintf(output,"%s"," L_CORCHETE");
                    break;
                case R_CORCHETE:
                    fprintf(output,"%s"," R_CORCHETE");
                    break;
                case L_LLAVE:
                    fprintf(output,"%s"," L_LLAVE");
                    break;
                case R_LLAVE:
                    fprintf(output,"%s"," R_LLAVE");
                    break;
                case COMA:
                    fprintf(output,"%s"," COMA");
                    break;
                case DOS_PUNTOS:
                    fprintf(output,"%s"," DOS_PUNTOS");
                    break;
                case LITERAL_CADENA:
                    fprintf(output,"%s"," LITERAL_CADENA");
                    break;
                case LITERAL_NUM:
                    fprintf(output,"%s"," LITERAL_NUM");
                    break;
                case PR_TRUE:
                    fprintf(output,"%s"," PR_TRUE");
                    break;
                case PR_FALSE:
                    fprintf(output,"%s"," PR_FALSE");
                    break;
                case PR_NULL:
                    fprintf(output,"%s"," PR_NULL");
                    break;
		case ERROR:
		    break;
                case EOF:
                    fprintf(output,"%s"," EOF");
                    break;
            }

		}
		fclose(archivo);
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	fclose(output);
	return 0;
}
