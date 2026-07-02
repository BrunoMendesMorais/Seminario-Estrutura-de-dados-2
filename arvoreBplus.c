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

/* ===================== CRIACAO ===================== */

PG *criarPagina(int folha, PG *pai, int raiz)
{
    int i;
    PG *novo = (PG *) malloc(sizeof(PG));
    novo->folha   = folha;
    novo->pai     = pai;
    novo->raiz    = raiz;
    novo->numChaves = 0;
    novo->prox    = NULL;
    for (i = 0; i < ORDEM + 1; i++)
        novo->filhos[i] = NULL;
    return novo;
}

/* ===================== BUSCA ===================== */

/* Retorna o indice da primeira chave >= valor (usado em insercao e navegacao) */
int buscarPos(PG *pagina, int valor)
{
    int i = 0;
    while (i < pagina->numChaves && valor >= pagina->chaves[i])
        i++;
    return i;
}

/* Desce ate a folha onde valor deveria estar */
PG *encontrarFolha(PG *raiz, int valor)
{
    PG *pagina = raiz;
    int pos;
    while (pagina->folha == 0) {
        pos = buscarPos(pagina, valor);
        pagina = pagina->filhos[pos];
    }
    return pagina;
}

/* ===================== INSERCAO ===================== */

void inserirOrdenado(PG *pagina, int valor)
{
    int i;
    int pos = buscarPos(pagina, valor);
    for (i = pagina->numChaves; i > pos; i--)
        pagina->chaves[i] = pagina->chaves[i - 1];
    pagina->chaves[pos] = valor;
}

/* Declaracao antecipada (splitInterno chama inserirPai e vice-versa) */
void inserirPai(PG **raiz, PG *pai, int chave, PG *novo);

void splitInterno(PG **raiz, PG *indice)
{
    PG *novo = criarPagina(0, indice->pai, 0);
    int meio = ORDEM / 2;
    int i, j;

    for (i = meio + 1, j = 0; i < indice->numChaves; i++, j++) {
        novo->chaves[j] = indice->chaves[i];
        novo->numChaves++;
    }
    indice->numChaves = meio;

    for (i = meio + 1, j = 0; i < ORDEM + 1; i++, j++) {
        novo->filhos[j] = indice->filhos[i];
        indice->filhos[i] = NULL;
    }
    for (i = 0; novo->filhos[i] != NULL; i++)
        novo->filhos[i]->pai = novo;

    if (indice->raiz == 1) {
        PG *novoRaiz = criarPagina(0, NULL, 1);
        indice->raiz = 0;
        indice->pai  = novoRaiz;
        novo->pai    = novoRaiz;
        novoRaiz->filhos[0] = indice;
        novoRaiz->filhos[1] = novo;
        novoRaiz->chaves[0] = indice->chaves[meio];
        novoRaiz->numChaves = 1;
        *raiz = novoRaiz;
        return;
    }
    inserirPai(raiz, indice->pai, indice->chaves[meio], novo);
}

void inserirPai(PG **raiz, PG *pai, int chave, PG *novo)
{
    int pos, i;

    if (pai == NULL) return;

    pos = buscarPos(pai, chave);

    for (i = pai->numChaves; i > pos; i--)
        pai->chaves[i] = pai->chaves[i - 1];
    for (i = pai->numChaves + 1; i > pos + 1; i--)
        pai->filhos[i] = pai->filhos[i - 1];

    pai->chaves[pos]     = chave;
    pai->filhos[pos + 1] = novo;
    novo->pai            = pai;
    pai->numChaves++;

    if (pai->numChaves == ORDEM)
        splitInterno(raiz, pai);
}

void splitFolha(PG **raiz, PG *folha)
{
    PG *novo = criarPagina(1, folha->pai, 0);
    int meio = ORDEM / 2;
    int i, j;

    for (i = meio, j = 0; i < folha->numChaves; i++, j++) {
        novo->chaves[j] = folha->chaves[i];
        novo->numChaves++;
    }
    folha->numChaves = meio;

    /* Mantém a lista encadeada de folhas corretamente */
    novo->prox  = folha->prox;
    folha->prox = novo;

    if (folha->raiz == 1) {
        PG *novoRaiz = criarPagina(0, NULL, 1);
        folha->raiz = 0;
        folha->pai  = novoRaiz;
        novo->pai   = novoRaiz;
        novoRaiz->filhos[0] = folha;
        novoRaiz->filhos[1] = novo;
        novoRaiz->chaves[0] = novo->chaves[0];
        novoRaiz->numChaves = 1;
        *raiz = novoRaiz;
        return;
    }
    inserirPai(raiz, folha->pai, novo->chaves[0], novo);
}

void inserir(PG **raiz, int valor)
{
    if (*raiz == NULL) {
        *raiz = criarPagina(1, NULL, 1);
        (*raiz)->chaves[0]  = valor;
        (*raiz)->numChaves  = 1;
        return;
    }

    PG *folha = encontrarFolha(*raiz, valor);

    inserirOrdenado(folha, valor);
    folha->numChaves++;

    if (folha->numChaves == ORDEM)
        splitFolha(raiz, folha);
}

/* ===================== REMOCAO ===================== */

/* Retorna o indice de 'no' dentro do vetor filhos[] do pai */
int encontrarIndicePai(PG *pai, PG *no)
{
    int i = 0;
    while (pai->filhos[i] != no) i++;
    return i;
}

/*
Quando a primeira chave de uma folha muda apos remocao,
atualiza a chave-guia correspondente nos nos internos acima.
 */
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

/* Remove a chave (e filho, para nos internos) da posicao idx */
void removerDaPagina(PG *pagina, int idx)
{
    int i;
    for (i = idx; i < pagina->numChaves - 1; i++)
        pagina->chaves[i] = pagina->chaves[i + 1];

    if (pagina->folha == 0) {
        /* Remove o ponteiro de filho a direita da chave removida */
        for (i = idx + 1; i < pagina->numChaves; i++)
            pagina->filhos[i] = pagina->filhos[i + 1];
        pagina->filhos[pagina->numChaves] = NULL;
    }
    pagina->numChaves--;
}

/* Declaracao antecipada (corrigeUnderflow e recursiva) */
void corrigeUnderflow(PG **raiz, PG *pagina);

void corrigeUnderflow(PG **raiz, PG *pagina)
{
    int i;

    /* Caso especial: pagina e a raiz */
    if (pagina->pai == NULL) {
        if (pagina->numChaves == 0 && pagina->folha == 0) {
            /* Raiz interna ficou vazia; filho unico assume o papel de raiz */
            PG *novaRaiz = pagina->filhos[0];
            novaRaiz->pai  = NULL;
            novaRaiz->raiz = 1;
            *raiz = novaRaiz;
            free(pagina);
        }
        return;
    }

    int minimo = (pagina->folha) ? ORDEM / 2 : (ORDEM + 1) / 2 - 1;
    if (pagina->numChaves >= minimo) return; /* sem underflow */

    PG *pai      = pagina->pai;
    int idx      = encontrarIndicePai(pai, pagina);
    PG *irmaoEsq = (idx > 0)          ? pai->filhos[idx - 1] : NULL;
    PG *irmaoDir = (idx < pai->numChaves) ? pai->filhos[idx + 1] : NULL;

    /* --- Redistribuicao com irmao esquerdo --- */
    if (irmaoEsq && irmaoEsq->numChaves > ORDEM / 2) {
        if (pagina->folha) {
            /* Abre espaco no inicio da folha */
            for (i = pagina->numChaves; i > 0; i--)
                pagina->chaves[i] = pagina->chaves[i - 1];
            pagina->chaves[0] = irmaoEsq->chaves[irmaoEsq->numChaves - 1];
            pagina->numChaves++;
            irmaoEsq->numChaves--;
            pai->chaves[idx - 1] = pagina->chaves[0]; /* atualiza chave-guia */
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

    /* --- Redistribuicao com irmao direito --- */
    if (irmaoDir && irmaoDir->numChaves > ORDEM / 2) {
        if (pagina->folha) {
            pagina->chaves[pagina->numChaves] = irmaoDir->chaves[0];
            pagina->numChaves++;
            for (i = 0; i < irmaoDir->numChaves - 1; i++)
                irmaoDir->chaves[i] = irmaoDir->chaves[i + 1];
            irmaoDir->numChaves--;
            pai->chaves[idx] = irmaoDir->chaves[0]; /* atualiza chave-guia */
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

    /* --- Fusao (nenhum irmao tem chaves sobrando) --- */
    if (irmaoEsq){
        /* Funde 'pagina' dentro de 'irmaoEsq' */
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
        /* Funde 'irmaoDir' dentro de 'pagina' */
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

    /* A fusao removeu uma chave do pai: verifica underflow em cascata */
    corrigeUnderflow(raiz, pai);
}

void remover(PG **raiz, int valor)
{
    if (*raiz == NULL) {
        printf("Arvore vazia.\n");
        return;
    }

    PG *folha = encontrarFolha(*raiz, valor);

    /* Procura o valor na folha */
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

    /* Se a chave removida era a primeira da folha, atualiza chave-guia no pai */
    if (folha->numChaves > 0 && idx == 0)
        atualizarChavePai(folha, chaveAntiga, folha->chaves[0]);

    /* Folha e tambem a raiz: sem minimo a respeitar */
    if (folha->pai == NULL) return;

    corrigeUnderflow(raiz, folha);
}

/* ===================== IMPRESSAO ===================== */

void printar(PG *raiz)
{
    if (raiz == NULL) return;

    printf("[");
    for (int i = 0; i < raiz->numChaves; i++) {
        printf("%d", raiz->chaves[i]);
        if (i < raiz->numChaves - 1) printf(" | ");
    }
    printf("]\n");

    if (raiz->folha == 0) {
        for (int i = 0; i < ORDEM + 1; i++) {
            if (raiz->filhos[i] != NULL)
                printar(raiz->filhos[i]);
        }
    }
}

/* Imprime a lista encadeada de folhas (util para verificar o prox) */
void printarFolhas(PG *raiz)
{
    if (raiz == NULL) return;
    PG *pagina = raiz;
    while (pagina->folha == 0)
        pagina = pagina->filhos[0];

    printf("Folhas: ");
    while (pagina != NULL) {
        printf("[");
        for (int i = 0; i < pagina->numChaves; i++) {
            printf("%d", pagina->chaves[i]);
            if (i < pagina->numChaves - 1) printf("|");
        }
        printf("] -> ");
        pagina = pagina->prox;
    }
    printf("NULL\n");
}

void escreverDot(PG *pagina, FILE *arq)
{
    int i;
    if (pagina == NULL) return;

    if (pagina->folha) {
        // folha: shape=box para evitar conflito de ports com as arestas dashed do prox
        fprintf(arq, "node%p [shape=box, style=filled, fillcolor=lightblue, label=\"", (void*)pagina);
        for (i = 0; i < pagina->numChaves; i++) {
            fprintf(arq, "%d", pagina->chaves[i]);
            if (i < pagina->numChaves - 1) fprintf(arq, " | ");
        }
        fprintf(arq, "\"];\n");

        // aresta pontilhada representando o ponteiro prox (lista ligada entre folhas)
        if (pagina->prox != NULL)
            fprintf(arq, "node%p -> node%p [style=dashed, constraint=false, color=gray, arrowsize=0.5];\n",
                    (void*)pagina, (void*)pagina->prox);
    } else {
        // no interno: shape=record com ports <f0>..<fN> para conectar cada filho
        fprintf(arq, "node%p [label=\"", (void*)pagina);
        for (i = 0; i < pagina->numChaves; i++)
            fprintf(arq, "<f%d> | %d | ", i, pagina->chaves[i]);
        fprintf(arq, "<f%d>\"];\n", pagina->numChaves);

        for (i = 0; i <= pagina->numChaves; i++) {
            if (pagina->filhos[i] != NULL) {
                fprintf(arq, "node%p:f%d -> node%p [arrowsize=0.5];\n",
                        (void*)pagina, i, (void*)pagina->filhos[i]);
                escreverDot(pagina->filhos[i], arq);
            }
        }
    }
}

void gerarGraphviz(PG *raiz)
{
    if (raiz == NULL) {
        printf("Arvore vazia.\n");
        return;
    }

    FILE *arq = fopen("arvore.dot", "w");
    if (arq == NULL) {
        printf("Erro ao criar arquivo.\n");
        return;
    }

    fprintf(arq, "digraph G {\n");
    fprintf(arq, "    node [shape=record];\n"); // default para nos internos
    fprintf(arq, "    rankdir=TB;\n\n");

    escreverDot(raiz, arq);

    fprintf(arq, "}\n");
    fclose(arq);

    int ret = system("dot -Tpng arvore.dot -o arvore.png");
    if (ret == 0)
        printf("Imagem gerada: arvore.png\n");
    else
        printf("Erro ao executar o Graphviz.\n");
}

/* ===================== MAIN ===================== */

int main(int argc, char *argv[])
{
    PG *raiz = NULL;
    int op, valor, escolha;

    do {
        op = 1;
        system("cls");
        printf("===== CONTROLE DE ARVORE B+ =====\n");
        printf(" [1] INSERIR  [2] REMOVER  [3] PRINTAR  [4] FOLHAS  [5] GRAPHVIZ\n");
        printf("R: ");
        scanf("%d", &escolha);

        switch (escolha) {
            case 1: {
                system("cls");
                printf("Valor a inserir: ");
                scanf("%d", &valor);
                inserir(&raiz, valor);
                printf("Inserido! Verifique com PRINTAR.\n");
                getchar(); getchar();
                break;
            }
            case 2: {
                system("cls");
                printf("Valor a remover: ");
                scanf("%d", &valor);
                remover(&raiz, valor);
                printf("Operacao concluida. Verifique com PRINTAR.\n");
                getchar(); getchar();
                break;
            }
            case 3: {
                system("cls");
                printar(raiz);
                getchar(); getchar();
                break;
            }
            case 4: {
                system("cls");
                printarFolhas(raiz);
                getchar(); getchar();
                break;
            }
            case 5: {
                system("cls");
                gerarGraphviz(raiz);
                getchar(); getchar();
                break;
            }
            default: {
                op = 0;
                break;
            }
        }
    } while (op == 1);

    return 0;
}