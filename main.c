#include <stdio.h>
#include <stdlib.h>
#define m 5

//inserir na mesma pagina
//buscaVal
//criar pagina

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

typedef struct pagina{
	int nchaves;
	int chaves[m-1];
	struct pagina *filhos[m];
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

pg *criarPag(int val){
	int i;
	pg *novaPag = (pg *) malloc(sizeof(pg));
	novaPag->nchaves = 1;
	novaPag->chaves[0] = val;
	for(i=0;i<m;i++){
		novaPag->filhos[i] = NULL;
	}
}

pg *split(pg *pagina,int val, pg *filho){
	int i,p,over[m];
	int me = (m-1)/2;
	
	for(i = 0; pagina->chaves[i] < val; i++){
		over[i] = pagina->chaves[i];
	}
	
	over[i] = val;
	
	for(i+1; i < m-1; i++){
		over[i] = pagina->chaves[i-1];
	}
	
	pagina->nchaves = me;
	
	pg *novaPag = criarPag(over[me+1]);
	
	for(i=me+2;me<m;me++){
		novaPag->chaves[i -me-1] = over[i];
	}
	return novaPag;
}

pg *inserir(pg *pagina,int val){
	if(!pagina)
		return pagina;
	int p,i,x=m-1;
	for(i = 0; i < pagina->nchaves && pagina->chaves[i] < val; i++);
	if(pagina->chaves[i] == val && i < pagina->nchaves){
		printf("Valor já inserido");
		return pagina;
	}
	pg *temp = inserir(pagina->filhos[i],val);
	
	p = pagina->nchaves;
	if(temp != pagina->filhos[i]){
		pg *aux;
		for(x;x>i;x--){
			aux = pagina->filhos[x];
			pagina->filhos[x] = pagina->filhos[x-1];
		}
		pagina->filhos[i] = temp;
		while(p>i){
			pagina->chaves[p] = pagina->chaves[p-1];
			p--;
		}
		pagina->chaves[p] = pagina->filhos[i]->chaves[(m-1)/2];
	}
	if(pagina->nchaves < m && !temp){
		while(p>i){
			pagina->chaves[p] = pagina->chaves[p-1];
			p--;
		}
		pagina->chaves[p] = val;
	} 
	if(pagina->nchaves > m && !temp){
		return split(pagina,val);
	}
	return pagina;
}


int main(int argc, char *argv[]) {
	return 0;
}
