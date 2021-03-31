#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

// Constantes
#define LINES 12
#define COLUMNS 12
#define NUMBER_OF_THREADS 6
#define divisor_linhas  2
#define divisor_colunas  3

// Indexes



// Declaração de Variáveis Globais

int matrix[LINES][COLUMNS];

int contador_sequencial = 0;
int contador_paralelo = 0;


// Declaração de Métodos
int isNumberPrime(int number);
void createMatrix();
void defineSubMatrix();
void teste();
void contagemSequencial();
void contagemParalela(void *ptr);
int isMatrixDivisibleByNumberOfThreads();
void *_thread_loop(void *p);
struct subMatriz
{
    int i_start_index;
    int i_stop_index;
    int j_start_index;
    int j_stop_index;

};

struct subMatriz *subMatrizes;

void printHelloWorld(void *ptr){

    int contador_local = 0;
    int n = (int)ptr;


    int i,j;

    for(i = subMatrizes[n].i_start_index; i < subMatrizes[n].i_stop_index; i++ ){
        printf("\n");

        for(j = subMatrizes[n].j_start_index; j < subMatrizes[n].j_stop_index; j++ ){

            if(isNumberPrime(matrix[i][j])){
                contador_local++;
            }
        }
    }
    printf("\nThread %d: %d números primos encontrados",n, contador_local);
    pthread_exit(NULL);


}



int isMatrixDivisibleByNumberOfThreads(){
    div_t verifica_linhas, verifica_colunas;
    verifica_linhas = div(LINES, divisor_linhas);
    verifica_colunas = div(COLUMNS, divisor_colunas);
    printf("\n  Verificando se a matrix (%d x %d) pode ser dividida em %d partes iguais",LINES, COLUMNS, NUMBER_OF_THREADS);

    printf("\n      Linhas: resto da divisão entre %d e %d = %d", LINES, divisor_linhas, verifica_linhas.rem);
    printf("\n      Colunas: resto da divisão entre %d e %d = %d", COLUMNS, divisor_colunas, verifica_colunas.rem);


    return (verifica_linhas.rem == 0 && verifica_colunas.rem == 0);

}



int main(int argc, char **argv) {


    printf("\n=============== Iniciando o Experimento ===============");
    printf("\nTamanho da matrix: (%d x %d);", LINES, COLUMNS);
//    todo() verificar se o lines + columns = numero de threads
    createMatrix();
    printf("\n ----------------------- Iniciando Contagem Sequencial -------------------------");
    contagemSequencial();
    printf("\nResultado: %d números primos encontrados.", contador_sequencial);
    printf("\n ----------------------- Finalizando Contagem Sequencial -------------------------\n");



    printf("\n----------------------- Iniciando Contagem Paralela -------------------------");

    printf("\nNúmero de threads: %d;\n", NUMBER_OF_THREADS);

    if (!isMatrixDivisibleByNumberOfThreads()){
        printf("\n  A Matrix (%d x %d) não pode ser dividida igualmente em %d partes.", LINES, COLUMNS, NUMBER_OF_THREADS);
        printf("\n----------------------- Finalizando Contagem Paralela -------------------------");

        return 0;
    }else{
        printf("\n  A Matrix (%d x %d) pode ser dividida igualmente em %d partes.", LINES, COLUMNS, NUMBER_OF_THREADS);
        printf("\n  Iniciando Divisão da Matrix");
    }


    int bloco_linhas = LINES/ divisor_linhas;
    int bloco_colunas = COLUMNS/ divisor_colunas;

//    remover
//    printf("\ndivisor linha: %d e bloco coliuna: %d -> ", bloco_linhas, bloco_colunas);


//    remover
//    printf("\nObjetivo:\n");
//    printf("\nBloco 1 -> linhas: 0 a 10500; coluna: 0 a 7000");
//    printf("\nBloco 2 -> linhas: 0 a 10500; coluna: 7000 a 14000");
//    printf("\nBloco 3 -> linhas: 0 a 10500; coluna: 14000 a 21000");
//
//    printf("\nBloco 4 -> linhas: 10500 a 21000; coluna: 0 a 7000");
//    printf("\nBloco 5 -> linhas: 10500 a 21000; coluna: 7000 a 14000");
//    printf("\nBloco 6 -> linhas: 10500 a 21000; coluna: 14000 a 21000");
//    printf("\nObjetivo:\n");

    subMatrizes = (struct subMatriz*)malloc(NUMBER_OF_THREADS*sizeof(struct subMatriz));

    int i,j,counter=0;
    int aux1,aux2;
    aux2 = 1;

    for (i=0; i < divisor_linhas; i++){
        aux1 = 1;
        for(j=0; j < divisor_colunas; j++){
            printf("\nSubmatriz %d : linhas: %d a %d, colunas: %d a %d ",counter, i*bloco_linhas, aux2* bloco_linhas, j* bloco_colunas, aux1* bloco_colunas);

            subMatrizes[counter].i_start_index = i * bloco_linhas;
            subMatrizes[counter].i_stop_index  = aux2 * bloco_linhas;
            subMatrizes[counter].j_start_index = j * bloco_colunas;
            subMatrizes[counter].j_stop_index  = aux1 * bloco_colunas;
            aux1++;
            counter++;
        }
        aux2++;

    }

//    for (int p=0; p<NUMBER_OF_THREADS; p++){ DEBUG
//        printf("\n----");
//        printf("\ni start = %d", subMatrizes[p].i_start_index);
//        printf("\ni stop = %d", subMatrizes[p].i_stop_index );
//        printf("\nj start = %d", subMatrizes[p].j_start_index);
//        printf("\nj stop = %d",subMatrizes[p].j_stop_index );
//        printf("\n----");
//    }

//    printf("\n------------------\n"); DEBUG
//    int k,y;
//    for(k=0; k<LINES; k++) {
//        printf("\n");
//        for(y=0;y<COLUMNS;y++) {
//            printf("%d ", matrix[k][y]);
//        }
//        printf("\n");
//    }
//    printf("\n------------------\n");



    pthread_t threads[NUMBER_OF_THREADS];

    int  status;
//    status = pthread_create( &threads[0], NULL, printHelloWorld, (void *) 0);
//    if(status != 0){
//        printf("error");
//    }
//    pthread_join(threads[0], NULL);


    for (i=0; i < NUMBER_OF_THREADS; i++){

        status = pthread_create( &threads[i], NULL, printHelloWorld, (void *) i);
        if(status != 0){
            printf("error");
        }
    }
//
    for (i=0; i < NUMBER_OF_THREADS; i++){

        pthread_join(threads[i], NULL);

    }
//
//    sleep(5);


//    int i,j;
//    createMatrix();

//    i = 10;
    #define x i


//    if(result.rem == 0){
//        printf("Divisão inteira %d", x);
//
//
//    }


//


    return 0;
}

void createMatrix(){
    int a;
    int i,j;


    for(i=0; i<LINES; i++) {
        for(j=0;j<COLUMNS;j++) {
            a = rand() % 29999;

            matrix[i][j] = a;
        }
    }
}

int isNumberPrime(int number){
    int n=number, i, flag = 1;
    // Iterate from 2 to sqrt(n)
    for (i = 2; i <= sqrt(n); i++) {
        // If n is divisible by any number between
        // 2 and n/2, it is not prime
        if (n % i == 0) {
            flag = 0;
            break;
        }
    }
    if (n <= 1)
        flag = 0;

    if (flag == 1) {
        return 1;
    }
    else {
        return 0;
    }
}

void contagemSequencial(){
    int i,j,number;

    for(i=0; i<LINES; i++) {
        for(j=0;j<COLUMNS;j++) {

            number = matrix[i][j];

            if(isNumberPrime(number)){
                contador_sequencial++;
            }

        }
    }
}

void *_thread_loop(void *p)
{
    printf("hello thread");
    int contador_local = 0, number;
    uint64_t n = (uint64_t)p;

    int i, j;
    for(i = subMatrizes[n].i_start_index; i<subMatrizes[n].i_stop_index; i++ ) {
        for (j = subMatrizes[n].j_start_index; i < subMatrizes[n].j_stop_index; j++) {
            number = matrix[i][j];
            if (isNumberPrime(number)) {
                contador_local++;
            }

        }

    }
    return NULL;
}

void contagemParalela(void *ptr){


}

void teste(){
    printf("%d %d ", LINES, COLUMNS);
}

