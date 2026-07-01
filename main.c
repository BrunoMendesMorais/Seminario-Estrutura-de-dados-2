#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ordem 10

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef struct pagina{
	int nchaves;
	int chaves[ordem-1];
	struct pagina *filhos[ordem];
}pg;

int buscaVal(pg *pagina, int val){
    if(pagina == NULL)
        return 0;

    int i;
    for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);

    if(i < pagina->nchaves && pagina->chaves[i] == val)
        return pagina->chaves[i];

    return buscaVal(pagina->filhos[i], val);
}

pg *criarPag(){
	int i;
	pg *novaPag = (pg *) malloc(sizeof(pg));
	novaPag->nchaves = 0;
	for(i=0;i<ordem;i++){
		novaPag->filhos[i] = NULL;
	}
	return novaPag;
}

pg *split(pg *pai,int chave, pg *filho){
	int i, meio = (ordem-1)/2;
	
	int auxChave[ordem];
	pg *auxFilho[ordem+1];
	
	for(i=0;i<ordem-1 && pai->chaves[i] < chave;i++){
		auxChave[i]= pai->chaves[i];
		auxFilho[i] = pai->filhos[i];
	}
	
	auxFilho[i] = pai->filhos[i];
	auxFilho[i+1] = filho; 
	auxChave[i] = chave;
	
	i++;
	
	for(i;i<ordem-1;i++){
		auxChave[i+1] = pai->chaves[i];
		auxFilho[i+1] = pai->filhos[i];
	}
	
	for(i=0;i<=meio;i++){
		pai->chaves[i] = auxChave[i];
		pai->filhos[i] = auxFilho[i];
	}
	
	pg *nova = criarPag();
	
	int pos;
	for(i;i<ordem;i++){
		pos = i - (meio+1);
		nova->chaves[pos] = auxChave[i];
		nova->filhos[pos] = auxFilho[i];
		pai->filhos[i] = NULL;
	}
	
	nova->filhos[meio] =  auxFilho[ordem];
	
	pai->nchaves = meio;
	nova->nchaves = ordem - (meio +1);
	
	return nova;	
}

void inserirNaPag(pg *pai,int chave, pg *filho, int pos){
	int aux, i;
	for(i= ordem -2; i>pos;i--)
		pai->chaves[i] = pai->chaves[i-1];
	pai->chaves[i] = chave;
	for(i = pai->nchaves; i > pos + 1; i--)
	    pai->filhos[i] = pai->filhos[i-1];
	
	pai->filhos[pos + 1] = filho;
}

pg *inserir(pg *pagina,int val){
	if(!pagina)
		return pagina;
	int p,i,x=ordem-1;
	for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);
	if(pagina->chaves[i] == val && i < pagina->nchaves){
		printf("Valor ja inserido");
		return pagina;
	}
	pg *temp = inserir(pagina->filhos[i],val);
	if(!temp || temp != pagina->filhos[i])
		pagina->nchaves ++;
	if(!temp && pagina->nchaves == ordem)
		return split(pagina,val,NULL);
	if(temp != pagina->filhos[i] && pagina->nchaves == ordem)
		return split(pagina,pagina->filhos[i]->chaves[(ordem-1)/2],temp);
	if(!temp)
		inserirNaPag(pagina,val,NULL,i);
	if(temp != pagina->filhos[i])
		inserirNaPag(pagina,pagina->filhos[i]->chaves[(ordem-1)/2],temp,i);
	return pagina;
}

pg *novaRaiz(int chave, pg *esq,pg *dir){
	pg *nova = criarPag();
	nova->chaves[0] = chave;
	nova->filhos[0] = esq;
	nova->filhos[1] = dir;
	nova->nchaves = 1;
	return nova;
}

void listarArvore(pg *pagina, int nivel){
    if(pagina == NULL)
        return;

    int i;

    printf("Nivel %d: ", nivel);

    printf("[ ");
    for(i = 0; i < pagina->nchaves; i++)
        printf("%d ", pagina->chaves[i]);
    printf("]\n");

    for(i = 0; i <= pagina->nchaves; i++)
        listarArvore(pagina->filhos[i], nivel + 1);
}

pg *redistribuir(pg *pai, int *chave, int pos, int *indice){
	pg *filho = NULL;
	int i;
	if(pos-1 >=0 && pai->filhos[pos-1]->nchaves > ceil(ordem/2)){
		pai->filhos[pos-1]->nchaves --;
		*chave = pai->filhos[pos-1]->chaves[pai->filhos[pos-1]->nchaves];
		filho = pai->filhos[pos-1]->filhos[pai->filhos[pos-1]->nchaves];
		pai->filhos[pos-1]->filhos[pai->filhos[pos-1]->nchaves] = NULL;
		*indice = pos-1;
		return filho;
	}
	if(pos+1 <= pai->nchaves && pai->filhos[pos+1]->nchaves > ceil(ordem/2.0)){
		pai->filhos[pos+1]->nchaves --;
		*chave = pai->filhos[pos+1]->chaves[0];
		filho = pai->filhos[pos+1]->filhos[0];
		for(i=0;i<pai->filhos[pos+1]->nchaves;i++){
			pai->filhos[pos+1]->chaves[i] = pai->filhos[pos+1]->chaves[i+1];
			pai->filhos[pos+1]->filhos[i] = pai->filhos[pos+1]->filhos[i+1];
		}
		pai->filhos[pos+1]->filhos[pai->filhos[pos+1]->nchaves] = pai->filhos[pai->filhos[pos+1]->nchaves+1];
		pai->filhos[pos+1]->filhos[pai->filhos[pos+1]->nchaves+1] = NULL;
		*indice = pos;
	}
	return filho;
}

void concatenacao(pg *pai, int pos){
	int i;
	
	if(pos == pai->nchaves)
		pos--;

    pai->filhos[pos]->chaves[pai->filhos[pos]->nchaves] = pai->chaves[pos];


    for(i=0;i<pai->filhos[pos+1]->nchaves;i++)
        pai->filhos[pos]->chaves[pai->filhos[pos]->nchaves+1+i] = pai->filhos[pos+1]->chaves[i];

    for(i=0;i<=pai->filhos[pos+1]->nchaves;i++)
        pai->filhos[pos]->filhos[pai->filhos[pos]->nchaves+1+i] = pai->filhos[pos+1]->filhos[i];

    pai->filhos[pos]->nchaves = pai->filhos[pos]->nchaves + pai->filhos[pos+1]->nchaves +1;

    for(i=pos;i<pai->nchaves-1;i++)
        pai->chaves[i] = pai->chaves[i+1];

    for(i=pos+1;i<pai->nchaves;i++)
        pai->filhos[i] = pai->filhos[i+1];

    pai->filhos[pai->nchaves] = NULL;

    pai->nchaves--;

    free(pai->filhos[pos+1]);
    pai->filhos[pos+1] = NULL;
}

pg *remocao(pg **pai,int val){
	pg *pagina = *pai;
    if(pagina == NULL)
        return;
    int i;
    for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);

    if(i < pagina->nchaves && pagina->chaves[i] != val){
		remocao(pagina->filhos[i], val);
		if(pagina->nchaves > ceil(ordem/2.0))
			return NULL;

	}
		
	



    return remocao(pagina->filhos[i], val);
}


int main(){
	int i;
    pg *raiz = criarPag();
    for(i=0;i<100;i++){
    	pg *temp = inserir(raiz,i);
    	raiz = temp != raiz?novaRaiz(raiz->chaves[(ordem-1)/2],raiz,temp): temp;
	}
	
    listarArvore(raiz,0);

    return 0;
}
