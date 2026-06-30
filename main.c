#include <stdio.h>
#include <stdlib.h>
#define ordem 5

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef struct pagina{
	int nchaves;
	int chaves[ordem-1];
	struct pagina *filhos[ordem];
}pg;


int buscaVal(pg *pagina,int val){
	if(pagina == NULL)
		return 0;
	int i;
	for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);
	if(pagina->chaves[i] == val && i < pagina->nchaves)
		return pagina->chaves[i];
	return buscaVal(pagina->filhos[i],val);
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
	
	for(i=0;i<meio;i++){
		pai->chaves[i] = auxChave[i];
		pai->filhos[i] = auxFilho[i];
	}
	pai->filhos[i] = auxFilho[i];
	
	pg *nova = criarPag();
	
	int pos;
	for(i++;i<ordem;i++){
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
	for(i = ordem; i>pos ;i--)
		pai->filhos[i] = pai->filhos[i-1];
	pai->filhos[i] = filho;
}

pg *inserir(pg *pagina,int val){
	if(!pagina)
		return pagina;
	int p,i,x=ordem-1;
	for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);
	if(pagina->chaves[i] == val && i < pagina->nchaves){
		printf("Valor já inserido");
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
	return nova;
}

int main(int argc, char *argv[]) {
	return 0;
}

