#include <stdio.h>
#include <stdlib.h>

#define ORDEM 4

typedef struct Pagina{
	int chaves[ORDEM];
	struct Pagina *filhos[ORDEM+1];
	struct Pagina *prox;
	struct Pagina *pai;
	int numChaves;
	int folha;
	int raiz;
	
}PG;

PG *criarPagina(int folha,PG *pai, int raiz)
{
	int i;
	
	PG *novo = (PG *) malloc((sizeof(PG)));
	
	novo->folha = folha;
	novo->pai = pai;
	novo->raiz = raiz;
	novo->numChaves = 0;
	novo->prox = NULL;
	
	for(i=0;i<ORDEM+1;i++){
		novo->filhos[i] = NULL;
	}
	
	return novo;	
}

int buscarPos(PG *pagina, int valor)
{
	int i = 0;
	
	while(i < pagina->numChaves && valor >= pagina->chaves[i]){
        i++;
    }
	return i;
}

void inserirOrdenado(PG *pagina, int valor)
{
	int i;
	
	int pos = buscarPos(pagina,valor);
	
	for(i=pagina->numChaves;i>pos;i--){
		pagina->chaves[i] = pagina->chaves[i-1];
	}
	pagina->chaves[pos] = valor;
	return;
}

void splitInterno(PG **raiz, PG *indice)
{
	PG *novo = criarPagina(0,indice->pai,0);
	int meio = ORDEM/2;
	int i,j;
	
	for(i=meio+1,j=0; i<indice->numChaves; i++,j++){
		novo->chaves[j] = indice->chaves[i];
		novo->numChaves += 1;
	}
	indice->numChaves = meio;
	
	for(i=meio+1,j=0;i<ORDEM+1;i++,j++){
		novo->filhos[j] = indice->filhos[i];
		indice->filhos[i] = NULL;
	}
	for(i=0;novo->filhos[i] != NULL;i++){
		novo->filhos[i]->pai = novo;
	}
	
	if(indice->raiz == 1){
		PG *novoRaiz = criarPagina(0,NULL,1);

		indice->raiz = 0;
		indice->pai = novoRaiz;
		novo->pai = novoRaiz;
		
		novoRaiz->filhos[0] = indice;
		novoRaiz->filhos[1] = novo;
		novoRaiz->chaves[0] = indice->chaves[meio];
		novoRaiz->numChaves = 1;
		*raiz = novoRaiz;
		return;
	}
	inserirPai(raiz,indice->pai, indice->chaves[meio], novo);
}

void inserirPai(PG **raiz, PG *pai, int chave, PG *novo)
{
    int pos = buscarPos(pai, chave);
    int i;

	if(pai == NULL) return;

    for(i = pai->numChaves; i > pos; i--){
        pai->chaves[i] = pai->chaves[i-1];
    }
    
    for(i = pai->numChaves + 1; i > pos + 1; i--){
        pai->filhos[i] = pai->filhos[i-1];
    }
    
    pai->chaves[pos] = chave;
    pai->filhos[pos + 1] = novo;
    novo->pai = pai;
    pai->numChaves++;
    
    if(pai->numChaves == ORDEM)
    	splitInterno(raiz,pai);
    return;
}

void splitFolha(PG **raiz, PG *folha)
{
	PG *novo = criarPagina(1,folha->pai,0);
	int meio = ORDEM/2;
	int i,j;
	
	for(i=meio,j=0; i<folha->numChaves; i++,j++){
		novo->chaves[j] = folha->chaves[i];
		novo->numChaves += 1;
	}
	folha->numChaves = meio;
	novo->prox = folha->prox;
	folha->prox = novo;
	
	if(folha->raiz == 1){
		PG *novoRaiz = criarPagina(0,NULL,1);

		folha->raiz = 0;
		folha->pai = novoRaiz;
		novo->pai = novoRaiz;
		novoRaiz->filhos[0] = folha;
		novoRaiz->filhos[1] = novo;
		novoRaiz->chaves[0] = novo->chaves[0];
		novoRaiz->numChaves = 1;
		*raiz = novoRaiz;
		return;
	}
	inserirPai(raiz,folha->pai, novo->chaves[0], novo);
	return;
	
}
	
void inserir(PG **raiz, int valor)
{
	int pos;
	
	if(*raiz == NULL){
		*raiz = criarPagina(1,NULL, 1);
		(*raiz)->chaves[0] = valor;
		(*raiz)->numChaves = 1;
		return;
	}
	
	PG *pagina = *raiz;

	while(pagina->folha == 0){
		pos = buscarPos(pagina,valor);
		pagina = pagina->filhos[pos];
	}
	
	inserirOrdenado(pagina, valor);
	pagina->numChaves++;
	
	if(pagina->numChaves == ORDEM){
		splitFolha(raiz,pagina);
	}	
}

void printar(PG *raiz)
{
    if(raiz == NULL) return;

    printf("[");

    for(int i = 0; i < raiz->numChaves; i++){
        printf("%d", raiz->chaves[i]);
        if(i < raiz->numChaves - 1)
            printf(" | ");
    }

    printf("]\n");

    if(raiz->folha == 0){
        for(int i = 0; i < ORDEM + 1; i++){
            if(raiz->filhos[i] != NULL){
                printar(raiz->filhos[i]);
            }
        }
    }
}

int main(int argc, char *argv[]) {
	PG *raiz = NULL;
	int op;
	int valor;
	int escolha;
	
	do{
		op = 1;
		system("cls");
		printf("CONTROLE DE ARVORE B+");
		printf("Voce quer ver os valores na arvore ou inserir?");
		printf("\n [ 1 ] - INSERIR [ 2 ] - PRINTAR [ 3 ] - ARVORE");
		printf("\nR: \n");
		scanf("%d", &escolha);
		
		switch(escolha){
			case 1:{
				system("cls");
				printf("Qual valor quer adicionar?: ");
				scanf("%d", &valor);
				inserir(&raiz,valor);
				printf("Verifique a insercao no printar!!!");
				getchar();
				getchar();
				break;
			}
			case 2:{
				printar(raiz);
				break;
			}
			case 3:{
				printar(raiz);
				getchar();
				getchar();
				break;
			}
			default:{
				op = 0;
				break;
			}
		}
	}while(op == 1);
	
}