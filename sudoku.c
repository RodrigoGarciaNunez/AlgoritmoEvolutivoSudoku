#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

typedef struct
{
    double fitness;
    double **matriz; // este es la matriz/solución
} individuo;

void mutacion(individuo *poblacion, int tamMatriz, float probMutacion, int **instancia, int m);
void cruza(individuo *poblacion, individuo *NuevaPoblacion, int tamanioPoblacion, int tamMatriz, int indicep1, int indicep2, int posicion, int m);
void calculaFitness(individuo *individuo, int tamMatriz, int m);
void generaPoblacion(individuo *poblacion, int tamPoblacion, int tamMatriz, int **instancia, int m);
void llenaMatrizInicial(individuo *individuo, int tamMatriz, int **instancia);
int seleccion(individuo *pobalcion, int sumafitness);
void imprimirMejorSolucion(individuo *poblacion, int tamMatriz);
void leerArchivo(int **instancia, char *nombreArchivo, int tamMatriz);
int fitnessColumna(individuo *individuo, int num, int j, int TamMatriz);
int fitnessFila(individuo *individuo, int num, int i, int tamMatriz);
int fitnessSubcuadro(individuo *individuo, int num, int m, int i, int j);

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        printf("Para ejecutar (./ejecutable tamPoblacion tamMatriz numGeneraciones probMutacion archivoInstancia)\n");
        return 0;
    }

    srand(time(NULL));

    int tamPoblacion = atoi(argv[1]);
    int tamMatriz = atoi(argv[2]); // tam matriz sirve para saber el tamaño de la matriz y el número de subcuadros
    int numGeneraciones = atoi(argv[3]);
    double probMutacion = atof(argv[4]);
    char *archivo = argv[5];

    int m = sqrt(tamMatriz); // m sirve para saber el tamaño de los subcuadros

    int **casillasOcupadas = (int **)malloc(tamMatriz * sizeof(int *)); // filas

    for (int i = 0; i < tamMatriz; i++)
    {
        casillasOcupadas[i] = (int *)malloc(tamMatriz * sizeof(int)); // columnas
        for (int j = 0; j < tamMatriz; j++)
        {
            casillasOcupadas[i][j] = 0; // Inicializa todas las casillas como no ocupadas
        }
    }

    leerArchivo(casillasOcupadas, archivo, tamMatriz);

    // primero se hace la poblacion inicial

    individuo *poblacion = (individuo *)malloc(tamPoblacion * sizeof(individuo));
    individuo *Nuevapoblacion = (individuo *)malloc(tamPoblacion * sizeof(individuo));

    generaPoblacion(poblacion, tamPoblacion, tamMatriz, casillasOcupadas, m);

    // ciclo de las generaciones
    int generacionActual = 0;
    individuo mejorsito = poblacion[0];

    while (generacionActual < numGeneraciones && mejorsito.fitness > 0)
    {
        int sumaFitness = 0;
        printf("-------Genereacion %d---------\n", generacionActual);
        fflush(stdout);
        // aquí vemos cual individuo fue el mejor de la población inicial
        for (int i = 1; i < tamPoblacion; i++)
        {
            if (mejorsito.fitness > poblacion[i].fitness)
            {                             // si el mejoristo tiene más penalizaciones que poblacion[i], entonces, este ultimo
                mejorsito = poblacion[i]; // pasa a ser el mejorsito
            }
        }

        // aqui iría la selección, la mutación y la cruza (creo que, dependiendo de la estrategia evolutiva, el orden de la mutacion y cruza puede cambiar)
        // printf("\nse empieza la cruza\n");
        // fflush(stdout);

        for (int i = 0; i < tamPoblacion; i++)
        {
            // printf("%d",);
            for (int j = 0; j < tamMatriz; j++)
            {
                for (int y = 0; y < tamMatriz; y++)
                {
                    printf("  %0.lf", poblacion[i].matriz[j][y]);
                }
                printf("\n");
            }
            printf("\nMi fitness es: %0.lf\n\n", poblacion[i].fitness);
            sumaFitness += poblacion[i].fitness;
        }

        // individuo *Nuevapoblacion = (individuo *)malloc(tamPoblacion * sizeof(individuo));
        Nuevapoblacion[0] = mejorsito; // El más chido pasa a directo a la nueva poblacion

        for (int i = 1; i < tamPoblacion; i+=2) // en la cruza se obtendrán dos hijos que pasasrán a la siguiente generacion
        {
            int indicep1 = seleccion(poblacion, sumaFitness);
            int indicep2 = seleccion(poblacion, sumaFitness);

            cruza(poblacion, Nuevapoblacion, tamPoblacion, tamMatriz, indicep1, indicep2, i, m); // el resultado de la cruza se coloca en la posicion i de la Nueva Poblacion
            mutacion(&(Nuevapoblacion[i]), tamMatriz, probMutacion, casillasOcupadas, m);
            if((i+1) < tamPoblacion){
                mutacion(&(Nuevapoblacion[i+1]), tamMatriz, probMutacion, casillasOcupadas, m);
            }
            
        }
        printf("sali");
        fflush(stdout);

        for (int i = 0; i < tamPoblacion; i++)
        {
            poblacion[i] = Nuevapoblacion[i];
        }

        printf("\nel mejor fitness de la generacion %d fue: %0.lf\n", generacionActual, mejorsito.fitness);

        generacionActual++;
    }
    imprimirMejorSolucion(&mejorsito, tamMatriz);

    free(poblacion);
    free(casillasOcupadas);
    free(Nuevapoblacion);

    return 0;
}

void generaPoblacion(individuo *poblacion, int tamPoblacion, int tamMatriz, int **instancia, int m)
{

    for (int i = 0; i < tamPoblacion; i++)
    {
        poblacion[i].matriz = (double **)malloc(tamMatriz * sizeof(double)); // filas
        poblacion[i].fitness = 0;
        for (int j = 0; j < tamMatriz; j++)
        {
            poblacion[i].matriz[j] = (double *)malloc(tamMatriz * sizeof(double)); // columnas
        }

        llenaMatrizInicial(&(poblacion[i]), tamMatriz, instancia);
        calculaFitness(&(poblacion[i]), tamMatriz, m);
    }
}

void llenaMatrizInicial(individuo *individuo, int tamMatriz, int **instancia)
{
    int contador = 0;
    for (int i = 0; i < tamMatriz; i++)
    {
        for (int j = 0; j < tamMatriz; j++)
        {
            if (instancia[i][j] == 0)
            {
                individuo->matriz[i][j] = (rand() % tamMatriz) + 1;
            }
            else
            {
                individuo->matriz[i][j] = instancia[i][j];
            }
        }
    }
}

int fitnessColumna(individuo *individuo, int num, int j, int tamMatriz)
{
    int penalizaciones = 0;
    int banderai = 0;
    for (int i = 0; i < tamMatriz; i++)
    {
        //printf(" columnas: i,j = %d, %d", i, j);
        //fflush(stdout);
        if ((individuo->matriz[i][j] == num) && (banderai == 0))
        {
            banderai = 1;
        }
        else if ((individuo->matriz[i][j] == num) && (banderai == 1))
        {
            penalizaciones++;
        }
    }
    //printf(" %d Columnas: voy a retornar %d penalizaciones ", num,penalizaciones);
    //fflush(stdout);
    return penalizaciones;
}

int fitnessFila(individuo *individuo, int num, int i, int tamMatriz)
{

    int penalizaciones = 0;
    int banderai = 0;
    for (int j = 0; j < tamMatriz; j++)
    {
        // printf(" Filas: i,j = %d, %d", i, j);
        // fflush(stdout);
        if ((individuo->matriz[i][j] == num) && (banderai == 0))
        {
            banderai = 1;
        }
        else if ((individuo->matriz[i][j] == num) && (banderai == 1))
        {
            penalizaciones++;
        }
    }
    //printf("%d Filas: voy a retornar %d penalizaciones ", num,penalizaciones);
    //fflush(stdout);
    return penalizaciones;
}

int fitnessSubcuadro(individuo *individuo, int num, int m, int i, int j)
{
    int penalizaciones = 0;
    int banderai = 0;
    int limitei = i + m;
    int limitej = j + m;
    int auxi = i;
    int auxj = j;
    for (i = auxi; i < limitei; i++)
    {
        for (j = auxj; j < limitej; j++)
        {
            // printf(" Subcuadro: i,j = %d, %d", i, j);
            // fflush(stdout);
            if (individuo->matriz[i][j] == num && banderai == 0)
            {
                banderai = 1;
            }
            else if (individuo->matriz[i][j] == num && banderai == 1)
            {
                penalizaciones++;
            }
        }
    }
    //printf(" %d Subcuadro: voy a retornar %d penalizaciones ", num,penalizaciones);
    //fflush(stdout);
    return penalizaciones;
}

void calculaFitness(individuo *individuo, int tamMatriz, int m)
{
    int penalizaciones = 0;
    int banderai = 0; // es para indicar que un número ya está
    int i = 0, j = 0;
    for (int num = 1; num < tamMatriz+1; num++)
    {
        // for (int i = 0, j = 0; j < tamMatriz; i++)
        while (1) // aqui se checa las columnas
        {

            if (j == tamMatriz)
            {
                j = 0;
                break;
            }
            penalizaciones += fitnessColumna(individuo, num, j, tamMatriz);
            j++;

            // printf(" i, j = %d,%d", i, j);
            // fflush(stdout);
        }
        // for (int j = 0, i = 0; i < tamMatriz; j++)
        while (1) // aqui se checan las filas
        {
            if (i == tamMatriz)
            {
                i = 0;
                break;
            }
            penalizaciones += fitnessFila(individuo, num, i, tamMatriz);
            i++;
            // printf(" i2 = %d,", i);
            // fflush(stdout);
        }

        for (int i = 0; i < tamMatriz; i += m)
        {
            for (int j = 0; j < tamMatriz; j += m)
            {
                penalizaciones += fitnessSubcuadro(individuo, num, m, i, j);
                // printf(" va el siguiente ");
                // fflush(stdout);
            }
        }
    }
    individuo->fitness = penalizaciones;
    //printf("\nfitnesss: %0.lf !!!!!!!", individuo->fitness);
    //fflush(stdout);
}

void mutacion(individuo *individuo, int tamMatriz, float probMutacion, int **instancia, int m)
{
    // for (int i = 0; i < tamanioPoblacion; i++)
    //{
    for (int j = 0; j < tamMatriz; j++)
    {
        for (int k = 0; k < tamMatriz; k++)
        {
            if (instancia[j][k] == 0)
            { // si la casilla se puede mutar, entonces se pocede.
                float probabilidad = (float)rand() / RAND_MAX;
                if (probabilidad < probMutacion)
                {
                    float valor;
                    valor = (rand() % tamMatriz) + 1; // aqui generamos un valor entre 1 y el tamaño de la matriz
                    individuo->matriz[j][k] = valor;
                }
            }
        }
    }
    calculaFitness(individuo, tamMatriz, m);
    //}
}

void cruza(individuo *poblacion, individuo *NuevaPoblacion, int tamanioPoblacion, int tamMatriz, int indicep1, int indicep2, int posicion, int m)
{
    // individuo * hijo1 = (individuo *)malloc(sizeof(individuo));
    individuo hijo1;
    individuo hijo2;
    int puntoCruza = rand() % (tamMatriz * tamMatriz);
    hijo1.matriz = (double **)malloc(tamMatriz * sizeof(double)); // filas
    hijo2.matriz = (double **)malloc(tamMatriz * sizeof(double));

    for (int j = 0; j < tamMatriz; j++)
    {
        hijo1.matriz[j] = (double *)malloc(tamMatriz * sizeof(double)); // columnas
        hijo2.matriz[j] = (double *)malloc(tamMatriz * sizeof(double));
    }

    int i = 0, j = 0;
    for (int num = 1; num < tamMatriz; num++)
    {
        // for (int i = 0, j = 0; j < tamMatriz; i++)
        while (1) // aqui se checa las columnas
        {
            if (j == tamMatriz)
            {
                j = 0;
                break;
            }

            if (fitnessColumna(&(poblacion[indicep1]), num, j, tamMatriz) < fitnessColumna(&(poblacion[indicep2]), num, j, tamMatriz))
            {// si la columna de p1 es mejor, se copia en hijo1
                for (int i = 0; i < tamMatriz; i++)
                {
                    hijo1.matriz[i][j] = poblacion[indicep1].matriz[i][j];
                }
            }
            else
            {//si la columna de p2 es mejor o son iguales, la de p2 se copia en hijo1
                for (int i = 0; i < tamMatriz; i++)
                {
                    hijo1.matriz[i][j] = poblacion[indicep2].matriz[i][j];
                }
            }
            j++;

            // printf(" i, j = %d,%d", i, j);
            // fflush(stdout);
        }
        // for (int j = 0, i = 0; i < tamMatriz; j++)
        for (int i = 0; i < tamMatriz; i += m)
        {
            for (int j = 0; j < tamMatriz; j += m)
            {
                int limitei = i + m;
                int limitej = j + m;
                if (fitnessSubcuadro(&(poblacion[indicep1]), num, m, i, j) < fitnessSubcuadro(&(poblacion[indicep2]), num, m, i, j))
                { //si el subcuadro de p1 es mejor, se copia en hijo2
                    for (int x = i; x < limitei; x++)
                    {
                        for (int y = j; y < limitej; y++)
                        {
                            hijo2.matriz[x][y] = poblacion[indicep1].matriz[x][y];
                        }
                    }
                }
                else
                {//si el subcuadro de p2 es mejor o son iguales, la de p2 se copia en hijo2
                    for (int x = i; x < limitei; x++)
                    {
                        for (int y = j; y < limitej; y++)
                        {
                            hijo2.matriz[x][y] = poblacion[indicep2].matriz[x][y];
                        }
                    }
                }
            }
        }
    }

    //  aqui se hace una cruza por puntoCruza
    /*int casillas = 0;
    for (int i = 0; i < tamMatriz; i++)
    {
        for (int j = 0; j < tamMatriz; j++)
        {
            if (casillas < puntoCruza)
            {
                hijo1.matriz[i][j] = poblacion[indicep1].matriz[i][j];
            }
            else
            {
                hijo1.matriz[i][j] = poblacion[indicep2].matriz[i][j];
            }
            casillas++;
            // hijo1.matriz[i][j] = trunc((poblacion[indicep1].matriz[i][j] + poblacion[indicep2].matriz[i][j]) / 2);
        }
    }*/

    //  evaluamos a los hijos
    calculaFitness(&hijo1, tamMatriz, m);
    calculaFitness(&hijo2, tamMatriz, m);
    NuevaPoblacion[posicion] = hijo1;
    if(posicion+1 < tamanioPoblacion){
        NuevaPoblacion[posicion + 1] = hijo2;
    }    
}

int seleccion(individuo *poblacion, int sumafitness)
{
    // necesitamos regresar una posicion
    int ruleta = rand() % sumafitness; // este valor va a estar entre 0 y la suma del fitness -1
    int acumula = 0;
    int Posicion = 0;
    while (acumula <= ruleta)   
    {
        acumula += (sumafitness - poblacion[Posicion].fitness);
        Posicion++;
    }
    return Posicion - 1;
}

void imprimirMejorSolucion(individuo *mejor, int tamMatriz)
{
    printf("\n--------Mejor Solución encontrada del Sudoku ---------\n");
    for (int j = 0; j < tamMatriz; j++)
    {
        for (int k = 0; k < tamMatriz; k++)
        {
            printf(" %0.lf ", mejor->matriz[j][k]);
        }
        printf("\n");
    }
    printf("Fitness: %0.lf\n", mejor->fitness);
}

void leerArchivo(int **instancia, char *nombreArchivo, int tamMatriz)
{
    FILE *instanciaTXT;
    instanciaTXT = fopen(nombreArchivo, "r");

    for (int i = 0; i < tamMatriz; i++)
    {
        for (int j = 0; j < tamMatriz; j++)
        {
            fscanf(instanciaTXT, "%d", &(instancia[i][j]));
        }
    }
    fclose(instanciaTXT);
}
