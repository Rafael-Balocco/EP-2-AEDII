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

Arv* criaArvore() {
    Arv* arvore = (Arv*)malloc(sizeof(Arv));
    NO* p = (NO*)malloc(sizeof(NO));
    p->folha = true;
    p->numChaves = 0;
    p->prox = NULL;
    arvore->raiz = p;
    return arvore;
}

void inserirPrimDir(NO* n, int chave) {
    int i = n->numChaves;

    int j = 0;

    if (n->folha) {
        for (j; j < i; j++) {
            if (n->chave[j] == chave)
                return;
        }

        while (i > 0 && chave < n->chave[i - 1]) {
            n->chave[i] = n->chave[i - 1];
            i--;
        }

        n->chave[i] = chave;
        n->numChaves++;
    } else {
        while (i > 0 && chave < n->chave[i - 1])
            i--;

        inserirPrimDir(n->ponteiros[i], chave);
    }
}

void splitFilho(NO* n1, int i, NO* n2) {
    NO* p = (NO*)malloc(sizeof(NO));
    NO* aux = NULL;
    p->folha = n2->folha;
    p->numChaves = t - 1;
    int j;

    for (j = 0; j < t - 1; j++) {
        p->chave[j] = n2->chave[j + t];
    }

    if (!(n2->folha)) {
        for (j = 0; j < t; j++) {
            p->ponteiros[j] = n2->ponteiros[j + t];
        }
    }

    if (n2->prox != NULL) {
        aux = n2->prox;
    }
    n2->prox = p;
    p->prox = aux;

    n2->numChaves = t - 1;

    for (j = n1->numChaves; j > i; j--) {
        n1->ponteiros[j + 1] = n1->ponteiros[j];
    }

    n1->ponteiros[i] = n2;
    n1->ponteiros[i + 1] = p;

    for (j = n1->numChaves - 1; j >= i; j--) {
        n1->chave[j + 1] = n1->chave[j];
    }

    n1->chave[i] = n2->chave[t - 1];
    n1->numChaves++;

    inserirPrimDir(p, n2->chave[t - 1]);
}

void inserirDifCheio(NO* n, int chave) {
    int i = n->numChaves;

    if (n->folha) {
        while (i > 0 && chave < n->chave[i - 1]) {
            n->chave[i] = n->chave[i - 1];
            i--;
        }

        n->chave[i] = chave;
        n->numChaves++;
    } else {
        while (i > 0 && chave < n->chave[i - 1])
            i--;

        if (n->ponteiros[i]->numChaves == (2 * t - 1)) {
            splitFilho(n, i, n->ponteiros[i]);
            n->ponteiros[i]->prox = n->ponteiros[i + 1];
            if (chave > n->chave[i])
                i++;
        }

        inserirDifCheio(n->ponteiros[i], chave);
    }
}

void inserir(Arv* arv, int chave) {
    NO* raiz = arv->raiz;

    if (raiz->numChaves == (2 * t - 1)) {
        NO* p = (NO*)malloc(sizeof(NO));
        arv->raiz = p;
        p->folha = false;
        p->numChaves = 0;
        p->prox = NULL;
        p->ponteiros[0] = raiz;
        splitFilho(p, 0, raiz);
        inserirDifCheio(p, chave);
    } else
        inserirDifCheio(raiz, chave);
}

void inOrder(NO* no, FILE* saida) {
    int i;

    if (no->numChaves == 0) {
        fprintf(saida, "vazia");
        return;
    }

    for (i = 0; i < no->numChaves; i++) {
        if (no == NULL)
            return;

        if (!(no->folha) && i == 0) {
            fprintf(saida, "(");
            inOrder(no->ponteiros[i], saida);
        } else if (no->folha && i == 0)
            fprintf(saida, "(");

        if (!(no->folha))
            fprintf(saida, "%c", ' ');

        fprintf(saida, "%d", no->chave[i]);

        if (!(no->folha))
            fprintf(saida, "%c", ' ');

        if (i < no->numChaves - 1 && no->folha)
            fprintf(saida, "%c", ' ');

        if (!(no->folha))
            inOrder(no->ponteiros[i]->prox, saida);
    }

    if (no->folha)
        fprintf(saida, ")");

    if (!(no->folha))
        fprintf(saida, ")");
}

void deleteFromLeaf(NO* leaf, int key) {
    int i = 0;
    int j;
    while (i < leaf->numChaves && leaf->chave[i] != key) {
        i++;
    }

    if (i == leaf->numChaves) {
        return;  // Key not found
    }

    for (j = i; j < leaf->numChaves - 1; j++) {
        leaf->chave[j] = leaf->chave[j + 1];
    }

    leaf->numChaves--;
}

void fix(Arv* arv, NO* parent, int i) {
    NO* node = parent->ponteiros[i];

    if (i > 0 && parent->ponteiros[i - 1]->numChaves >= t) {
        rotateRight(parent, i);
    } else if (i < parent->numChaves && parent->ponteiros[i + 1]->numChaves >= t) {
        rotateLeft(parent, i);
    } else {
        if (i == parent->numChaves) {
            i--;
        }
        merge(parent, i, parent->ponteiros[i], parent->ponteiros[i + 1]);
    }
}

void rotateRight(NO* parent, int i) {
    NO* node = parent->ponteiros[i];
    NO* prev = parent->ponteiros[i - 1];
    node->chave[0] = parent->chave[i - 1];
    parent->chave[i - 1] = prev->chave[prev->numChaves - 1];
    prev->numChaves--;
    int j;

    if (!(node->folha)) {
        node->ponteiros[0] = prev->ponteiros[prev->numChaves];
        prev->ponteiros[prev->numChaves] = NULL;
    }

    for (j = node->numChaves; j > 0; j--) {
        node->chave[j] = node->chave[j - 1];
    }

    if (!(node->folha)) {
        for (j = node->numChaves + 1; j > 0; j--) {
            node->ponteiros[j] = node->ponteiros[j - 1];
        }
    }

    node->numChaves++;
}

void rotateLeft(NO* parent, int i) {
    NO* node = parent->ponteiros[i];
    NO* next = parent->ponteiros[i + 1];
    node->chave[node->numChaves] = parent->chave[i];
    parent->chave[i] = next->chave[0];
    next->numChaves--;
    int j;

    for (j = 0; j < next->numChaves; j++) {
        next->chave[j] = next->chave[j + 1];
    }

    if (!(next->folha)) {
        for (j = 0; j <= next->numChaves; j++) {
            next->ponteiros[j] = next->ponteiros[j + 1];
        }
    }

    node->numChaves++;
}

void merge(NO* parent, int i, NO* pred, NO* succ) {
    pred->chave[pred->numChaves] = parent->chave[i];
    pred->numChaves++;
    int j;

    for (j = 0; j < succ->numChaves; j++) {
        pred->chave[pred->numChaves + j] = succ->chave[j];
    }

    if (!(pred->folha)) {
        for (j = 0; j <= succ->numChaves; j++) {
            pred->ponteiros[pred->numChaves + j] = succ->ponteiros[j];
        }
    }

    pred->numChaves += succ->numChaves;

    for (j = i; j < parent->numChaves - 1; j++) {
        parent->chave[j] = parent->chave[j + 1];
        parent->ponteiros[j + 1] = parent->ponteiros[j + 2];
    }

    parent->numChaves--;
    free(succ);
}

void delete(Arv* arv, int key) {
    NO* curr = arv->raiz;
    int found = false;
    int i = 0;

    while (i < curr->numChaves) {
        if (key == curr->chave[i]) {
            found = true;
            break;
        } else if (key < curr->chave[i]) {
            break;
        }
        i++;
    }

    if (found) {
        if (curr->folha) {
            deleteFromLeaf(curr, key);
        } else {
            NO* pred = curr->ponteiros[i];

            if (pred->numChaves >= t) {
                int predKey = pred->chave[pred->numChaves - 1];
                curr->chave[i] = predKey;
                delete(arv, predKey);
            } else {
                NO* succ = curr->ponteiros[i + 1];

                if (succ->numChaves >= t) {
                    int succKey = succ->chave[0];
                    curr->chave[i] = succKey;
                    delete(arv, succKey);
                } else {
                    merge(curr, i, pred, succ);
                    delete(arv, key);
                }
            }
        }

        if (curr == arv->raiz && curr->numChaves == 0) {
            arv->raiz = curr->ponteiros[0];
            free(curr);
        }
    } else {
        if (curr->folha) {
            return;  // Key not found
        } else {
            if (curr->ponteiros[i]->numChaves < t) {
                if (i != 0 && curr->ponteiros[i - 1]->numChaves >= t) {
                    rotateRight(curr, i);
                } else if (i != curr->numChaves && curr->ponteiros[i + 1]->numChaves >= t) {
                    rotateLeft(curr, i);
                } else {
                    if (i == curr->numChaves) {
                        i--;
                    }
                    merge(curr, i, curr->ponteiros[i], curr->ponteiros[i + 1]);
                }
            }
            delete(arv, key);
        }
    }
}

int main(int argc, char* argv[]) {
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
        } else if (comando == 'r') {
            fscanf(entrada, "%d", &chave);
            fscanf(entrada, "\n");
            delete(arv, chave);
        } else if (comando == 'p') {
            inOrder(arv->raiz, saida);
            fprintf(saida, "\n");
            fscanf(entrada, "\n");
        } else if (comando == 'a') {
            int z = 0;
        }

        fscanf(entrada, "%c", &comando);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}
