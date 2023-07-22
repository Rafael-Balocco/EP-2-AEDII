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

void liberarArvore(NO* no);

NO* buscaFolha(NO* n, int chave);

void removeChave(NO* p, int chave);

void fundirFilhos(NO* pai, int indexFilhoEsquerdo);

void emprestarChave(NO* pai, int indexFilhoEmprestador);

void ajustarArvore(Arv* arvore);

void deletar(Arv* arvore, int chave);

Arv* criaArvore () {

    Arv* arvore = (Arv*) malloc(sizeof(Arv));
    NO* p = (NO*) malloc(sizeof(NO));
    p -> folha = true;
    p -> numChaves = 0;
    p -> prox = NULL;
    arvore -> raiz = p;
    return arvore;
}


void inserePrimDir (NO* n, int chave) {

    int i = n -> numChaves;

    int j; 

    if (n -> folha) {

        for ( j  = 0 ; j < i ; j++){
            
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

        inserePrimDir(n -> ponteiros[i], chave);
    }
}

void split (NO* n1, int i, NO* n2) {

    int j;

    NO* p = (NO*) malloc(sizeof(NO));

    NO* aux = NULL;
    
    p -> numChaves = t - 1;
    p -> folha = n2 -> folha;
   

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

    inserePrimDir(p, n2 -> chave[t - 1]);

}


void insereDifCheio (NO* n, int chave) {

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

            split(n, i, n -> ponteiros[i]);

            n->ponteiros[i]->prox = n->ponteiros[i+1];


            if (chave > n -> chave[i]) i++;
        }

        insereDifCheio(n -> ponteiros[i], chave);
    }
}


void insere (Arv* arv, int chave) {

    NO* raiz = arv ->raiz;

    if (raiz -> numChaves == (2 * t - 1)) {

        NO* p = (NO*) malloc(sizeof(NO));

        p -> numChaves = 0;
        p -> prox = NULL;
        p -> ponteiros[0] = raiz;
        arv -> raiz = p;
        p -> folha = false;

        split(p, 0, raiz);

        insereDifCheio(p, chave);
    
    }
    
    else insereDifCheio(raiz, chave);
}


NO* buscaFolha(NO* n, int chave) {
    if (n->folha)
        return n;
    else
        return buscaFolha(n->ponteiros[0], chave);
}


void liberarArvore(NO* no) {
    int i;
    
    if (no) {
        if (!no->folha) {
            for (i = 0; i <= no->numChaves; i++) {
                liberarArvore(no->ponteiros[i]);
            }
        }
        free(no);
    }
}

// Função para remover uma chave em uma árvore B+
void removeChave(NO* p, int chave) {
    int i = 0;
    int j;
    while (i < p->numChaves && chave > p->chave[i])
        i++;

    if (p->folha) {
        if (i < p->numChaves && chave == p->chave[i]) {
            // Remover chave da folha
            for (j = i; j < p->numChaves - 1; j++) {
                p->chave[j] = p->chave[j + 1];
            }
            p->numChaves--;
            return;
        }
    } else {
        NO* filhoEsquerdo = p->ponteiros[i];
        NO* filhoDireito = p->ponteiros[i + 1];

        if (i < p->numChaves && chave == p->chave[i]) {
            if (filhoEsquerdo->numChaves >= t) {
                // Caso 2a: Emprestar do filho esquerdo
                NO* predecessor = buscaFolha(filhoEsquerdo, chave);
                p->chave[i] = predecessor->chave[predecessor->numChaves - 1];
                removeChave(filhoEsquerdo, predecessor->chave[predecessor->numChaves - 1]);
            } else if (filhoDireito->numChaves >= t) {
                // Caso 2b: Emprestar do filho direito
                NO* sucessor = buscaFolha(filhoDireito, chave);
                p->chave[i] = sucessor->chave[0];
                removeChave(filhoDireito, sucessor->chave[0]);
            } else {
                // Caso 2c: Fundir com um dos filhos
                fundirFilhos(p, i);
                removeChave(p->ponteiros[i], chave);
            }
        } else {
            if (filhoEsquerdo->numChaves == t - 1 && i > 0 && p->ponteiros[i - 1]->numChaves >= t) {
                // Caso 3a: Emprestar do filho esquerdo
                emprestarChave(p, i - 1);
            } else if (filhoEsquerdo->numChaves == t - 1 && i < p->numChaves && p->ponteiros[i + 1]->numChaves >= t) {
                // Caso 3b: Emprestar do filho direito
                emprestarChave(p, i);
            } else if (filhoEsquerdo->numChaves == t - 1) {
                // Caso 3c: Fundir com o filho esquerdo
                fundirFilhos(p, i - 1);
            } else {
                // Caso 3c: Fundir com o filho direito
                fundirFilhos(p, i);
            }
            removeChave(p->ponteiros[i], chave);
        }
    }
}

// Função para fundir dois nós filhos de um nó pai
void fundirFilhos(NO* pai, int indexFilhoEsquerdo) {
    NO* filhoEsquerdo = pai->ponteiros[indexFilhoEsquerdo];
    NO* filhoDireito = pai->ponteiros[indexFilhoEsquerdo + 1];
    int i;

    // Transferir chave do pai para o filho esquerdo
    filhoEsquerdo->chave[t - 1] = pai->chave[indexFilhoEsquerdo];
    filhoEsquerdo->numChaves++;

    // Transferir chaves do filho direito para o filho esquerdo
    for (i = 0; i < filhoDireito->numChaves; i++) {
        filhoEsquerdo->chave[t + i] = filhoDireito->chave[i];
    }

    // Transferir ponteiros do filho direito para o filho esquerdo, se não for folha
    if (!filhoDireito->folha) {
        for (i = 0; i <= filhoDireito->numChaves; i++) {
            filhoEsquerdo->ponteiros[t + i] = filhoDireito->ponteiros[i];
        }
    }

    // Atualizar número de chaves do filho esquerdo
    filhoEsquerdo->numChaves += filhoDireito->numChaves;

    // Remover chave do pai e ajustar ponteiros
    for (i = indexFilhoEsquerdo; i < pai->numChaves - 1; i++) {
        pai->chave[i] = pai->chave[i + 1];
        pai->ponteiros[i + 1] = pai->ponteiros[i + 2];
    }
    pai->numChaves--;

    free(filhoDireito);
}

// Função para emprestar uma chave de um nó irmão
void emprestarChave(NO* pai, int indexFilhoEmprestador) {
    
    NO* filhoEmprestador = pai->ponteiros[indexFilhoEmprestador];
    NO* filhoReceptor = pai->ponteiros[indexFilhoEmprestador + 1];
    int i;
    // Mover chave do pai para o filho emprestador
    filhoEmprestador->chave[filhoEmprestador->numChaves] = pai->chave[indexFilhoEmprestador];
    filhoEmprestador->numChaves++;

    // Transferir chave do filho receptor para o pai
    pai->chave[indexFilhoEmprestador] = filhoReceptor->chave[0];

    // Ajustar chaves do filho receptor
    for (i = 0; i < filhoReceptor->numChaves - 1; i++) {
        filhoReceptor->chave[i] = filhoReceptor->chave[i + 1];
    }
    filhoReceptor->numChaves--;

    // Ajustar ponteiros do filho receptor, se não for folha
    if (!filhoReceptor->folha) {
        filhoReceptor->ponteiros[0] = filhoReceptor->ponteiros[1];
    }
}

// Função para ajustar a árvore após a remoção da chave
void ajustarArvore(Arv* arvore) {
    NO* raiz = arvore->raiz;
    if (raiz->numChaves == 0 && !raiz->folha) {
        // Caso a raiz fique vazia após a remoção
        arvore->raiz = raiz->ponteiros[0];
        free(raiz);
    }
}

// Função para deletar uma chave da árvore B+
void deletar(Arv* arvore, int chave) {
    NO* raiz = arvore->raiz;
    if (raiz == NULL || raiz->numChaves == 0) {
        return; // Árvore vazia ou raiz sem chaves
    }

    NO* folha = buscaFolha(raiz, chave);
    if (folha == NULL) {
        return; // Chave não encontrada na árvore
    }

    removeChave(folha, chave);
    ajustarArvore(arvore);
}

void printEmOrdem (NO* no, FILE* saida){

    int i;

    if(no -> numChaves == 0) {

        fprintf(saida, "vazia");
        return;
    }

    for (i = 0; i < no -> numChaves; i++){

        if (no == NULL) return;

        if (!(no -> folha) && i == 0) {

            fprintf(saida, "(");     
            printEmOrdem(no -> ponteiros[i], saida);

        }

        else if (no -> folha && i==0){

            fprintf(saida, "(");
        } 

        if (!(no -> folha)) {
            
            fprintf(saida, "%c", ' ');
        }

        fprintf(saida, "%d", no -> chave[i]);

        if (!(no -> folha)){
            
            fprintf(saida, "%c", ' ');
        } 

        if (i < no -> numChaves - 1 && no -> folha){
            
            fprintf(saida, "%c", ' ');
        } 

        if (!(no -> folha)){
            
            printEmOrdem(no -> ponteiros[i] -> prox, saida);
        }
    }

    if (no -> folha){
        
        fprintf(saida, ")");
    }

    if (!(no -> folha)){
        
        fprintf(saida, ")");
    }
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
            insere(arv, chave);
        }

        else if (comando == 'r') {

            fscanf(entrada, "%d", &chave);
            fscanf(entrada, "\n");
            deletar(arv, chave);
        }

        else if (comando == 'p') {

            printEmOrdem(arv -> raiz, saida);
            fprintf(saida, "\n");
            fscanf(entrada, "\n");
        }

        fscanf(entrada, "%c", &comando);
    }

    fclose(entrada);
    fclose(saida);

    return 1;
}