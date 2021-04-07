#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

// Constantes
#define LINES 4000
#define COLUMNS 4000
#define NUMBER_OF_THREADS 2
#define divisor_linhas  2
#define divisor_colunas  2

// Peterson's Solution

#define FALSE 0
#define TRUE 0
#define N NUMBER_OF_THREADS


int turn;
int interested[N];

void init_interested_vector(){
    int i;
    for (i = 0; i < N; i++){
        interested[i] = 0;
    }

}

void enter_region(int process){

    //flag[self]=1 show that process self want to enter in critical section
    if(process < N-1 && process >=0){
        interested[process] = 1;
    }
    turn = N-process;
    if(N-1-process != process){
        while(interested[N-1-process]==1 && turn==N-process);

    }else if (N-2-process >= 0){
        while(interested[N-2-process]==1 && turn==N-process);
    }

}

void leave_region(int process){
    interested[process] = FALSE;
    if(process <= N && process >= 0){
        interested[process] = 0;
    }
}


// Declaração de Variáveis Globais

int matrix[LINES][COLUMNS];
int contador_sequencial = 0;
int contador_paralelo = 0;


// Declaração de Métodos
int isNumberPrime(int number);
void createMatrix();
void contagemSequencial();
void *contagemParalela(void *ptr);
int isMatrixDivisibleByNumberOfThreads();
pthread_mutex_t lock;


struct subMatriz
{
    int i_start_index;
    int i_stop_index;
    int j_start_index;
    int j_stop_index;

};

struct subMatriz *subMatrizes;

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
void *contagemParalela(void *ptr){

    int contador_local = 0;
    int n = (intptr_t) ptr;


    int i,j;

    for(i = subMatrizes[n].i_start_index; i < subMatrizes[n].i_stop_index; i++ ){

        for(j = subMatrizes[n].j_start_index; j < subMatrizes[n].j_stop_index; j++ ){

            if(isNumberPrime(matrix[i][j])){
                contador_local++;



            }
        }
    }
//    printf("\nContador Local da Thread %d encontrou %d números primos",n, contador_local);

    enter_region(n);
    printf("\nThread %d incrementando contador global %d vezes",n, contador_local);
    for(i = 0; i<contador_local; i++){
        contador_paralelo++;
    }
    printf("\nThread %d liberando o contador global!",n);
    leave_region(n);

    pthread_exit(NULL);


}

int isMatrixDivisibleByNumberOfThreads(){
    div_t verifica_linhas, verifica_colunas;
    verifica_linhas = div(LINES, divisor_linhas);
    verifica_colunas = div(COLUMNS, divisor_colunas);
    printf("\nVerificando se a matrix (%d x %d) pode ser dividida em %d partes iguais",LINES, COLUMNS, NUMBER_OF_THREADS);

    printf("\n  %d %% %d = %d", LINES, divisor_linhas, verifica_linhas.rem);
    printf("\n  %d %% %d = %d", COLUMNS, divisor_colunas, verifica_colunas.rem);


    return (verifica_linhas.rem == 0 && verifica_colunas.rem == 0);

}
void debugMatrix(){
    int i,j;

    createMatrix();
    for(i=0; i<LINES; i++) {
        printf("\n", matrix[i][j]);

        for(j=0;j<COLUMNS;j++) {

            printf(" %d", matrix[i][j]);



        }
    }
}



int main() {


    printf("\n=============== Iniciando o Experimento ===============");
    printf("\nTamanho da matrix: (%d x %d);", LINES, COLUMNS);
    printf("\nNúmero de threads: %d;", NUMBER_OF_THREADS);

    if(NUMBER_OF_THREADS != divisor_colunas * divisor_linhas){
        printf("\nDefina corretamente o número de threads e os numeros divisores, a seguinte condição deve ser satisfeita: NUMBER_OF_THREADS == divisor_linhas * divisor colunas");
        printf("\n----------------------- Finalizando Experimento -------------------------");
        return 0;
    }

    if (!isMatrixDivisibleByNumberOfThreads()){
        printf("\nA Matrix (%d x %d) não pode ser dividida igualmente em %d partes.", LINES, COLUMNS, NUMBER_OF_THREADS);
        printf("\n----------------------- Finalizando Experimento -------------------------");

        return 0;
    }else{
        printf("\nA Matrix (%d x %d) pode ser dividida igualmente em %d partes.", LINES, COLUMNS, NUMBER_OF_THREADS);

    }


    init_interested_vector();
    createMatrix();
    printf("\nMatriz Criada!");
    printf("\n ----------------------- Iniciando Contagem Sequencial -------------------------");
    contagemSequencial();
    printf("\n ----------------------- Finalizando Contagem Sequencial -------------------------\n");



    printf("\n----------------------- Iniciando Contagem Paralela -------------------------");




    printf("\nIniciando Divisão da Matrix");
    int bloco_linhas = LINES/ divisor_linhas;
    int bloco_colunas = COLUMNS/ divisor_colunas;


    subMatrizes = (struct subMatriz*)malloc(NUMBER_OF_THREADS*sizeof(struct subMatriz));

    int i,j,counter=0;
    int aux1,aux2;
    aux2 = 1;

    for (i=0; i < divisor_linhas; i++){
        aux1 = 1;
        for(j=0; j < divisor_colunas; j++){
            subMatrizes[counter].i_start_index = i * bloco_linhas;
            subMatrizes[counter].i_stop_index  = aux2 * bloco_linhas;
            subMatrizes[counter].j_start_index = j * bloco_colunas;
            subMatrizes[counter].j_stop_index  = aux1 * bloco_colunas;
            aux1++;
            counter++;
        }
        aux2++;

    }


    pthread_t threads[NUMBER_OF_THREADS];

    int  status;

    for (i=0; i < NUMBER_OF_THREADS; i++){

        status = pthread_create( &threads[i], NULL, contagemParalela, (void *)(intptr_t)i);
        if(status != 0){
            printf("error");
        }
    }
    for (i=0; i < NUMBER_OF_THREADS; i++){

        pthread_join(threads[i], NULL);

    }
    printf("\n----------------------- Finalizando Contagem Paralela -------------------------");

    if(contador_paralelo != contador_sequencial){
        printf("Erro na contagem!");
        printf("\n\nContador Sequencial --> %d números primos", contador_sequencial);
        printf("\nContador Global --> %d números primos", contador_paralelo);
    }else{
        printf("\nResultado:");
        printf("\nContador Sequencial --> %d números primos", contador_sequencial);
        printf("\nContador Global --> %d números primos", contador_paralelo);
    }


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





