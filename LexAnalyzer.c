/*
    TAREA 1 – Análisis Léxico   
    Compiladores y Lenguajes de Bajo de Nivel
    Alumno: Marlon Miranda Centurion
    
    Descripcion:
    Implementar un analizador léxico para el lenguaje Json simplificado. 
    Recibe un archivo fuente Json, debe realizar un análisis léxico 
    reconociendo tokens, y luego por cada línea del fuente imprimir una 
    linea en un archivo de salida con la secuencia de componentes léxicos
    encontrados separados por espacios (se adjuntan ejemplos de un fuente 
    y su respectivo archivo de salida). 
    En caso de error léxico deberá imprimir un mensaje de error y continuar 
    con la siguiente línea del archivo fuente.
*/

//==================INCLUDES==================
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

//==================DEFINES==================
#define BUFFSIZE     5
#define LEXSIZE      50

//Codigos para los tokens validos
#define STRING      256
#define NUMBER      257
#define L_CORCHETE  258
#define R_CORCHETE  259
#define L_LLAVE     260
#define R_LLAVE     261
#define COMA        262
#define DOS_PUNTOS  263
#define PR_TRUE     264
#define PR_FALSE    265
#define PR_NULL     266

//==================PROTOTYPES==================
void sigLex();

//==================STRUCTS==================
typedef struct {
    int compLex;
    char *comp;
    char *lexema;
} token;

//==================VARIABLES==================
FILE *archivo;          //Archivo tipo JSON
char buff[2*BUFFSIZE];  //Buffer para la lectura del archivo
char id[LEXSIZE];       //Utilizado por el analizador lexico
int numLinea=1;         //Numero de Linea
int numAux=1;           //Numero de Linea auxiliar utilizada para impresion
token t;

//==================FUNCTIONS==================

// Funcion de error lexico
void error(const char* mensaje){
    printf("\n\nLin %d: Error Lexico. %s.\n",numLinea,mensaje); 
}

// Funciones del analizador lexico
void sigLex(){
    int i=0;
    char c=0;
    int acepto=0;
    int state=0;
    char msg[41];

    while((c=fgetc(archivo))!=EOF){
        if (c==' ' || c=='\t'){

            continue;//elimina los espacios
        }
        else if(c=='\n'){
            numLinea++; //incrementa el numero de linea
            continue;
        }
        else if (c==':'){
            t.compLex=DOS_PUNTOS;
            t.comp="DOS_PUNTOS";
            t.lexema=":";
            break;
        }
        else if (c=='{'){
            t.compLex=L_LLAVE;
            t.comp="L_LLAVE";
            t.lexema="{";
            break;
        }
        else if (c=='}'){
            t.compLex=R_LLAVE;
            t.comp="R_LLAVE";
            t.lexema="}";
            break;
        }
        else if (c=='['){
            t.compLex=L_CORCHETE;
            t.comp="L_CORCHETE";
            t.lexema="[";
            break;
        }
        else if (c==']'){
            t.compLex=R_CORCHETE;
            t.comp="R_CORCHETE";
            t.lexema="]";
            break;
        }
        else if (c==','){
            t.compLex=COMA;
            t.comp="COMA";
            t.lexema=",";
            break;
        }
        else if (c == '"'){
            //si es un STRING = SE VERIFICA QUE SE INGRESE UN STRING VALIDO
            c=fgetc(archivo);
            i = 0;
            id[i]= c; 
            while(c!=EOF){
                if(c == '"'){
                    id[i] = '\0';
                    t.compLex=STRING;
                    t.comp="STRING";
                    t.lexema = id;
                    break;
                }else if(i>=LEXSIZE){
                    ungetc(c, archivo);
                }
                else{
                    c=fgetc(archivo);
                    id[++i] = c;
                }
            }
            if (c==EOF)
                error("Se llego al fin sin cerrar el String");
            else if(i>=LEXSIZE){
                error("Longitud de Identificador excede tamaño de buffer");
            }
            break;
        }
        else if (isdigit(c)){
            //si es un numero:
            i=0;
            state=0;
            acepto=0;
            id[i]=c;

            while(!acepto){
                switch(state){
                    case 0: //es una secuencia netamente de digitos, puede ocurrir . o e
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            state=0;
                        }
                        else if(c=='.'){
                            id[++i]=c;
                            state=1;
                        }
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            state=3;
                        }
                        else{
                            state=6;
                        }
                        break;
                    case 1://es un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
                        c=fgetc(archivo);                       
                        if (isdigit(c)){
                            id[++i]=c;
                            state=2;
                        }
                        else if(c=='.'){
                            i--;
                            fseek(archivo,-1,SEEK_CUR);
                            state=6;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            state=-1;
                        }
                        break;
                    case 2://es la fraccion decimal, pueden seguir los digitos o e
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            state=2;
                        }
                        else if(tolower(c)=='e'){
                            id[++i]=c;
                            state=3;
                        }
                        else
                            state=6;
                        break;
                    case 3://es una e, puede seguir +, - o una secuencia de digitos
                        c=fgetc(archivo);
                        if (c=='+' || c=='-'){
                            id[++i]=c;
                            state=4;
                        }
                        else if(isdigit(c)){
                            id[++i]=c;
                            state=5;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            state=-1;
                        }
                        break;
                    case 4://necesariamente debe venir por lo menos un digito
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            state=5;
                        }
                        else{
                            sprintf(msg,"No se esperaba '%c'",c);
                            state=-1;
                        }
                        break;
                    case 5://una secuencia de digitos correspondiente al exponente
                        c=fgetc(archivo);
                        if (isdigit(c)){
                            id[++i]=c;
                            state=5;
                        }
                        else{
                            state=6;
                        }break;
                    case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
                        if (c!=EOF)
                            ungetc(c,archivo);
                        else
                            c=0;
                            id[++i]='\0';
                            acepto=1;
                            t.compLex=NUMBER;
                            t.comp="NUMBER";
                            t.lexema = id;
                        break;
                    case -1:
                        if (c==EOF)
                            error("No se esperaba el fin de archivo");
                        else
                            error(msg);
                    }
                }
            break;
        }
        else if (isalpha(c)){
            i = 0;
            do{
                id[i] = c;
                i++;
                c = fgetc(archivo);
            }while (isalpha(c));
            id[i]='\0';
            if (c!=EOF)
                ungetc(c,archivo);
            if(strcmp(id, "true") == 0 || strcmp(id, "TRUE") == 0){
                t.compLex=PR_TRUE;
                t.comp="PR_TRUE";
                t.lexema = "true";
                break;
            }
            else if (strcmp(id, "false") == 0 || strcmp(id, "FALSE") == 0){
                t.compLex=PR_FALSE;
                t.comp="PR_FALSE";
                t.lexema = "false";
                break;
            }
            else if(strcmp(id, "null") == 0 || strcmp(id, "NULL") == 0){
                t.compLex=PR_NULL;
                t.comp="PR_NULL";
                t.lexema = "null";
                break;
            }
            else{
                sprintf(msg,"No se esperaba '%s'",id);
                error(msg);             
            }
        }
        else {
            sprintf(msg,"No se esperaba '%c'",c);
            error(msg);
        }
    }
    if (c==EOF){
        t.compLex=EOF;
        t.comp="EOF";

    }
}

int main(int argc,char* args[]){
    // inicializar analizador lexico
    if(argc > 1){
        if (!(archivo=fopen(args[1],"rt"))){
            printf("Archivo JSON no encontrado.\n");
            exit(1);
        }
        while (t.comp!="EOF"){
            sigLex();
            if(numAux!=numLinea && t.comp!="EOF"){
                printf("\n%s ",t.comp);
                numAux = numLinea;
            } else if(t.comp!="EOF"){
                printf("%s ",t.comp);   
            }     
        }
        fclose(archivo);
    }else{
        printf("Ingrese el path como parametro.\n");
        exit(1);
    }
    printf("\n");

    return 0;
}