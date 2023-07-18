#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define t 3


typedef struct str_no {

    int chave[2 * t - 1];
    struct str_no* ponteiros[2 * t];
    int numChaves;
    bool folha;
    struct str_no* prox;
} NO;

typedef struct {

    NO* raiz;
} Arv;



Arv* criaArvore () {

    Arv* arvore = (Arv*) malloc(sizeof(Arv));
    NO* p = (NO*) malloc(sizeof(NO));
    p -> folha = true;
    p -> numChaves = 0;
    p -> prox = NULL;
    arvore -> raiz = p;
    return arvore;
}

void inserirPrimDir (NO* n, int chave) {

    int i = n -> numChaves;

    int j = 0 ; 

    if (n -> folha) {

        for ( j ; j < i ; j++){
            
            if(n->chave[j] == chave ) return;
        
        }

        while (i > 0 && chave < n -> chave[i - 1]) {

            n -> chave[i] = n -> chave[i - 1];
            i--;
        }

        n -> chave[i] = chave;
        n -> numChaves++;
    } 
    
    else {

        while (i > 0 && chave < n -> chave[i - 1]) i--;

        inserirPrimDir(n -> ponteiros[i], chave);
    }
}

void splitFilho (NO* n1, int i, NO* n2) {

    NO* p = (NO*) malloc(sizeof(NO));

    NO* aux = NULL;

    p -> folha = n2 -> folha;
    p -> numChaves = t - 1;
    int j;


    for (j = 0; j < t - 1; j++) {

        p -> chave[j] = n2 -> chave[j + t];
    }

    if (!(n2 -> folha)) {

        for (j = 0; j < t; j++) {

            p -> ponteiros[j] = n2 -> ponteiros[j + t];
        }
    }

    if (n2 -> prox != NULL) {

            aux = n2 -> prox;
    }
    n2 -> prox = p;
    p -> prox = aux;
    
    n2 -> numChaves = t - 1;


    for (j = n1 -> numChaves; j > i; j--) {

        n1 -> ponteiros[j + 1] = n1 -> ponteiros[j];
    }

    n1 -> ponteiros[i] = n2;
    n1 -> ponteiros[i+1] = p;

    for (j = n1 -> numChaves - 1; j >= i; j--) {

        n1 -> chave[j + 1] = n1 -> chave[j];
    }


    n1 -> chave[i] = n2 -> chave[t - 1];
    n1 -> numChaves++;

    inserirPrimDir(p, n2 -> chave[t - 1]);

}


void inserirDifCheio (NO* n, int chave) {

    int i = n -> numChaves;

    if (n -> folha) {

        while (i > 0 && chave < n -> chave[i - 1]) {

            n -> chave[i] = n -> chave[i - 1];
            i--;
        }

        n -> chave[i] = chave;
        n -> numChaves++;
    } 
    
    else {

        while (i > 0 && chave < n -> chave[i - 1]) i--;
        

        if (n -> ponteiros[i] -> numChaves == (2 * t - 1)) {

            splitFilho(n, i, n -> ponteiros[i]);

            n->ponteiros[i]->prox = n->ponteiros[i+1];


            if (chave > n -> chave[i]) i++;
        }

        inserirDifCheio(n -> ponteiros[i], chave);
    }
}


void inserir (Arv* arv, int chave) {

    NO* raiz = arv ->raiz;

    if (raiz -> numChaves == (2 * t - 1)) {

        NO* p = (NO*) malloc(sizeof(NO));

        arv -> raiz = p;
        p -> folha = false;
        p -> numChaves = 0;
        p -> prox = NULL;
        p -> ponteiros[0] = raiz;


        splitFilho(p, 0, raiz);
        inserirDifCheio(p, chave);
    }
    
    else inserirDifCheio(raiz, chave);
}


NO* encontrar (NO* n, int chave) {

    int i = 0;

    while (i < n -> numChaves && chave > n -> chave[i]) i++;


    if (i < n -> numChaves && chave == n -> chave[i]) return n;

    else if (n -> folha) return NULL;

    else return encontrar(n -> ponteiros[i], chave);
}


NO* acharFolha (NO* n) {

    if (n -> folha) return n;

    else return acharFolha (n -> ponteiros[0]);
}


void removerChave (NO* p, int chave) {

    int i = 0;
    int j;

    while (i < p -> numChaves && chave > p -> chave[i]) i++;
    

    if (p -> folha) {

        if (i < p -> numChaves && chave == p -> chave[i]) {

            for (j = i; j < p -> numChaves - 1; j++) p -> chave[j] = p -> chave[j + 1];
            
            p -> numChaves--;
        }
    }
    
    else {

        NO* n1 = p -> ponteiros[i];

        if (i < p -> numChaves && chave == p -> chave[i]) {

            if (n1 -> numChaves >= t) {

                NO* pred = acharFolha(n1 -> ponteiros[n1 -> numChaves]);

                p -> chave[i] = pred -> chave[pred -> numChaves - 1];

                removerChave(n1, pred -> chave[pred -> numChaves - 1]);
            }
            
            else {

                NO* n2 = p -> ponteiros[i + 1];

                if (n2 -> numChaves >= t) {

                    NO* succ = acharFolha(n2);

                    p -> chave[i] = succ -> chave[0];

                    removerChave(n2, succ -> chave[0]);
                }
                
                else {

                    n1 -> chave[n1 -> numChaves] = p -> chave[i];

                    for (j = 0; j < n2 -> numChaves; j++)
                        n1 -> chave[n1 -> numChaves + 1 + j] = n2 -> chave[j];
                    

                    for (j = 0; j <= n2 -> numChaves; j++) 
                        n1 -> ponteiros[n1 -> numChaves + 1 + j] = n2 -> ponteiros[j];
                    

                    n1 -> numChaves += n2 -> numChaves + 1;


                    for (j = i; j < p -> numChaves - 1; j++) 
                        p -> chave[j] = p -> chave[j + 1];
                    

                    for (j = i + 1; j < p -> numChaves; j++) 
                        p -> ponteiros[j] = p -> ponteiros[j + 1];
                    

                    p -> numChaves--;

                    removerChave(n1, chave);
                }
            }
        }
        
        else {

            if (n1 -> numChaves == t - 1) {

                NO* n2 = NULL;

                if (i < p -> numChaves && p -> ponteiros[i + 1] -> numChaves >= t) {

                    n2 = p -> ponteiros[i + 1];

                    for (j = n2 -> numChaves; j > 0; j--) 
                        n2 -> chave[j] = n2 ->chave[j - 1];
                    

                    for (j = n2 -> numChaves + 1; j > 0; j--) 
                        n2 -> ponteiros[j] = n2 -> ponteiros[j - 1];
                    

                    n2 -> chave[0] = p -> chave[i];
                    n2 -> ponteiros[0] = n1 -> ponteiros[n1 -> numChaves];
                    n2 -> numChaves++;

                    n1 -> ponteiros[n1 -> numChaves] = NULL;
                    p -> chave[i] = n1 -> chave[n1 -> numChaves - 1];
                    n1 -> chave[n1 -> numChaves - 1] = 0;
                    n1 -> numChaves--;
                }
                
                else if (i > 0 && p -> ponteiros[i - 1] -> numChaves >= t) {

                    n2 = p -> ponteiros[i - 1];
                    n2 -> chave[n2 -> numChaves] = p -> chave[i - 1];
                    n2 -> ponteiros[n2 -> numChaves + 1] = n1 -> ponteiros[0];
                    n2 -> numChaves++;

                    n1 -> ponteiros[0] = NULL;
                    p -> chave[i - 1] = n1 -> chave[0];

                    for (j = 0; j < n1 -> numChaves - 1; j++)
                        n1 -> chave[j] = n1 -> chave[j + 1];
                    

                    for (j = 0; j < n1 -> numChaves; j++) 
                        n1 -> ponteiros[j] = n1 -> ponteiros[j + 1];
                    

                    n1 -> numChaves--;
                }


                if (n2 == NULL) {

                    if (i < p -> numChaves) {

                        n2 = p -> ponteiros[i + 1];

                        n1 -> chave[t - 1] = p -> chave[i];
                        n1 -> ponteiros[t] = n2 -> ponteiros[0];
                        p -> chave[i] = n2 -> chave[0];

                        for (j = 0; j < n2 -> numChaves - 1; j++) 
                            n2 -> chave[j] = n2 -> chave[j + 1];
                        

                        for (j = 0; j < n2 -> numChaves; j++) 
                            n2 -> ponteiros[j] = n2 -> ponteiros[j + 1];
                        

                        n2 -> numChaves--;
                    }
                    
                    else {
                        
                        n2 = p -> ponteiros[i - 1];

                        for (j = n1 -> numChaves; j > 0; j--) 
                            n1 -> chave[j] = n1 -> chave[j - 1];
                        

                        for (j = n1 -> numChaves + 1; j > 0; j--) 
                            n1 -> ponteiros[j] = n1 -> ponteiros[j - 1];
                        

                        n1 -> chave[0] = p -> chave[i - 1];
                        n1 -> ponteiros[0] = n2 -> ponteiros[n2 -> numChaves];

                        p -> chave[i - 1] = n2 -> chave[n2 -> numChaves - 1];

                        n2 -> chave[n2 -> numChaves - 1] = 0;
                        n2 -> ponteiros[n2 -> numChaves] = NULL;
                        n2 -> numChaves--;
                    }
                }
                n1 -> numChaves++;
                removerChave(n1, chave);
            }
            
            else removerChave(n1, chave);
        }
    }
}


void remover (Arv* arv, int chave) {

    NO* raiz = arv -> raiz;

    if (raiz -> numChaves == 0) return;
    

    removerChave(raiz, chave);

    if (raiz -> numChaves == 0 && !(raiz -> folha)) {

        arv -> raiz = raiz -> ponteiros[0];
        free(raiz);
    }
}


void inOrder (NO* no, FILE* saida){

    int i;

    if(no -> numChaves == 0) {

        fprintf(saida, "vazia");
        return;
    }

    for (i = 0; i < no -> numChaves; i++){

        if (no == NULL) return;

        if (!(no -> folha) && i == 0) {

            fprintf(saida, "(");     
            inOrder(no -> ponteiros[i], saida);
        }

        else if (no -> folha && i==0)fprintf(saida, "(");

        if (!(no -> folha)) fprintf(saida, "%c", ' ');

        fprintf(saida, "%d", no -> chave[i]);

        if (!(no -> folha)) fprintf(saida, "%c", ' ');

        if (i < no -> numChaves - 1 && no -> folha) fprintf(saida, "%c", ' ');

        if (!(no -> folha)) inOrder(no -> ponteiros[i] -> prox, saida);
    }

    if (no -> folha)fprintf(saida, ")");

    if (!(no -> folha))fprintf(saida, ")");
}



int main (int argc, char* argv[]) {

    FILE* entrada;
    entrada = fopen(argv[1], "r");
    FILE* saida;
    saida = fopen("saida.txt", "w");

    Arv* arv = criaArvore();

    char comando;
    int chave;

    fscanf(entrada, "%c", &comando);

    while (comando != 'f') {

        if (comando == 'i') {

            fscanf(entrada, "%d", &chave);
            fscanf(entrada, "\n");
            inserir(arv, chave);
        }

        else if (comando == 'r') {

            fscanf(entrada, "%d", &chave);
            fscanf(entrada, "\n");
            remover(arv, chave);
        }

        else if (comando == 'p') {

            inOrder(arv -> raiz, saida);
            fprintf(saida, "\n");
            fscanf(entrada, "\n");
        }

        else if (comando == 'a'){
            int z = 0;
        }

        fscanf(entrada, "%c", &comando);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}
