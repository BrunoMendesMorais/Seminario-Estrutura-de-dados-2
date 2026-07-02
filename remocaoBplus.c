#include <stdio.h>
#include <stdlib.h>

#define ORDEM 3

typedef struct Pagina {
    int chaves[ORDEM];
    struct Pagina *filhos[ORDEM + 1];
    struct Pagina *prox;
    struct Pagina *pai;
    int numChaves;
    int folha;
    int raiz;
} PG;

int encontrarIndicePai(PG *pai, PG *no)
{
    int i = 0;
    while (pai->filhos[i] != no) i++;
    return i;
}

void atualizarChavePai(PG *no, int chaveAntiga, int chaveNova)
{
    PG *pai = no->pai;
    int i;
    while (pai != NULL) {
        for (i = 0; i < pai->numChaves; i++) {
            if (pai->chaves[i] == chaveAntiga){
                pai->chaves[i] = chaveNova;
                return;
            }
        }
        pai = pai->pai;
    }
}

void removerDaPagina(PG *pagina, int idx)
{
    int i;
    for (i = idx; i < pagina->numChaves - 1; i++)
        pagina->chaves[i] = pagina->chaves[i + 1];

    if (pagina->folha == 0) {
        for (i = idx + 1; i < pagina->numChaves; i++)
            pagina->filhos[i] = pagina->filhos[i + 1];
        pagina->filhos[pagina->numChaves] = NULL;
    }
    pagina->numChaves--;
}

void corrigeUnderflow(PG **raiz, PG *pagina);

void corrigeUnderflow(PG **raiz, PG *pagina)
{
    int i;

    if (pagina->pai == NULL) {
        if (pagina->numChaves == 0 && pagina->folha == 0) {
            PG *novaRaiz = pagina->filhos[0];
            novaRaiz->pai  = NULL;
            novaRaiz->raiz = 1;
            *raiz = novaRaiz;
            free(pagina);
        }
        return;
    }

    int minimo = (pagina->folha) ? ORDEM / 2 : (ORDEM + 1) / 2 - 1;
    if (pagina->numChaves >= minimo) return;

    PG *pai      = pagina->pai;
    int idx      = encontrarIndicePai(pai, pagina);
    PG *irmaoEsq = (idx > 0)          ? pai->filhos[idx - 1] : NULL;
    PG *irmaoDir = (idx < pai->numChaves) ? pai->filhos[idx + 1] : NULL;

    if (irmaoEsq && irmaoEsq->numChaves > ORDEM / 2) {
        if (pagina->folha) {
            for (i = pagina->numChaves; i > 0; i--)
                pagina->chaves[i] = pagina->chaves[i - 1];
            pagina->chaves[0] = irmaoEsq->chaves[irmaoEsq->numChaves - 1];
            pagina->numChaves++;
            irmaoEsq->numChaves--;
            pai->chaves[idx - 1] = pagina->chaves[0];
        } else{
            for (i = pagina->numChaves; i > 0; i--)
                pagina->chaves[i] = pagina->chaves[i - 1];
            for (i = pagina->numChaves + 1; i > 0; i--)
                pagina->filhos[i] = pagina->filhos[i - 1];
            pagina->chaves[0]    = pai->chaves[idx - 1];
            pagina->filhos[0]    = irmaoEsq->filhos[irmaoEsq->numChaves];
            pagina->filhos[0]->pai = pagina;
            pagina->numChaves++;
            pai->chaves[idx - 1] = irmaoEsq->chaves[irmaoEsq->numChaves - 1];
            irmaoEsq->filhos[irmaoEsq->numChaves] = NULL;
            irmaoEsq->numChaves--;
        }
        return;
    }

    if (irmaoDir && irmaoDir->numChaves > ORDEM / 2) {
        if (pagina->folha) {
            pagina->chaves[pagina->numChaves] = irmaoDir->chaves[0];
            pagina->numChaves++;
            for (i = 0; i < irmaoDir->numChaves - 1; i++)
                irmaoDir->chaves[i] = irmaoDir->chaves[i + 1];
            irmaoDir->numChaves--;
            pai->chaves[idx] = irmaoDir->chaves[0];
        } else {
            pagina->chaves[pagina->numChaves]      = pai->chaves[idx];
            pagina->filhos[pagina->numChaves + 1]  = irmaoDir->filhos[0];
            pagina->filhos[pagina->numChaves + 1]->pai = pagina;
            pagina->numChaves++;
            pai->chaves[idx] = irmaoDir->chaves[0];
            for (i = 0; i < irmaoDir->numChaves - 1; i++)
                irmaoDir->chaves[i] = irmaoDir->chaves[i + 1];
            for (i = 0; i < irmaoDir->numChaves; i++)
                irmaoDir->filhos[i] = irmaoDir->filhos[i + 1];
            irmaoDir->filhos[irmaoDir->numChaves] = NULL;
            irmaoDir->numChaves--;
        }
        return;
    }

    if (irmaoEsq){
        if (pagina->folha) {
            irmaoEsq->prox = pagina->prox;

            for (i = 0; i < pagina->numChaves; i++)
                irmaoEsq->chaves[irmaoEsq->numChaves + i] = pagina->chaves[i];
            irmaoEsq->numChaves += pagina->numChaves;
        } else{
            irmaoEsq->chaves[irmaoEsq->numChaves] = pai->chaves[idx - 1];
            irmaoEsq->numChaves++;
            for (i = 0; i < pagina->numChaves; i++)
                irmaoEsq->chaves[irmaoEsq->numChaves + i] = pagina->chaves[i];
            for (i = 0; i <= pagina->numChaves; i++) {
                irmaoEsq->filhos[irmaoEsq->numChaves + i] = pagina->filhos[i];
                irmaoEsq->filhos[irmaoEsq->numChaves + i]->pai = irmaoEsq;
            }
            irmaoEsq->numChaves += pagina->numChaves;
        }
        removerDaPagina(pai, idx - 1);
        free(pagina);
    } else{
        if (pagina->folha) {
            pagina->prox = irmaoDir->prox;

            for (i = 0; i < irmaoDir->numChaves; i++)
                pagina->chaves[pagina->numChaves + i] = irmaoDir->chaves[i];
            pagina->numChaves += irmaoDir->numChaves;
        } else{
            pagina->chaves[pagina->numChaves] = pai->chaves[idx];
            pagina->numChaves++;
            for (i = 0; i < irmaoDir->numChaves; i++)
                pagina->chaves[pagina->numChaves + i] = irmaoDir->chaves[i];
            for (i = 0; i <= irmaoDir->numChaves; i++) {
                pagina->filhos[pagina->numChaves + i] = irmaoDir->filhos[i];
                pagina->filhos[pagina->numChaves + i]->pai = pagina;
            }
            pagina->numChaves += irmaoDir->numChaves;
        }
        removerDaPagina(pai, idx);
        free(irmaoDir);
    }

    corrigeUnderflow(raiz, pai);
}

void remover(PG **raiz, int valor)
{
    if (*raiz == NULL) {
        printf("Arvore vazia.\n");
        return;
    }

    PG *folha = encontrarFolha(*raiz, valor);

    int idx = -1, i;
    for (i = 0; i < folha->numChaves; i++) {
        if (folha->chaves[i] == valor) { idx = i; break; }
    }
    if (idx == -1) {
        printf("Valor %d nao encontrado na arvore.\n", valor);
        return;
    }

    int chaveAntiga = folha->chaves[0];

    removerDaPagina(folha, idx);

    if (folha->numChaves > 0 && idx == 0)
        atualizarChavePai(folha, chaveAntiga, folha->chaves[0]);

    if (folha->pai == NULL) return;

    corrigeUnderflow(raiz, folha);
}