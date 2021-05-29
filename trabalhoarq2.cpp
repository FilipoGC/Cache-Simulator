#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deque>
#include <vector>
#include <math.h>

using namespace std;

struct cedulamem{
	char endereco[11];	//endereco da memoria
	
	char conteudo[6];	// conteudo da memoria
};typedef struct cedulamem CedulaMemoria;

struct cedulacache{
	char endereco[6];	// endereco da memoria cache	
	char tag[11];		// endereco da memoria principal que ela representa
	char conteudo[6];	// conteudo da memoria principal guardado na memoria cache
	 
};typedef struct cedulacache CedulaCache;

CedulaMemoria* criaCedula(CedulaMemoria* a){		// aloca uma cedula da memoria principal
	a = (CedulaMemoria*) malloc (sizeof(CedulaMemoria));
	return a;
}

CedulaCache* criaCedulaCache(CedulaCache* a){		// aloca uma cedula da memoria cache
	a = (CedulaCache*) malloc (sizeof(CedulaCache));
	return a;
}

CedulaMemoria** criaMemoria(CedulaMemoria **p){	// funcão que cria uma memoria principal zerada
	int control = 1, cont;
	char bin = '0';
	CedulaMemoria *q;
	p =(CedulaMemoria**) malloc (1024*sizeof(CedulaMemoria*));	// aloca memoria para uma memoria de 32 posicoes
	
	for(int i = 0; i < 1024; i++){			//cria as 32 cedulas de memoria  coloca no vetor da memoria principal
		q = criaCedula(q);
		p[i] = q;
	}
	
	for(int i = 9, cont = 0; i >= 0; i--){	// monta os enderecos da memoria de 00000 a 11111
		cont = 0;
		bin = '0';
		for(int j = 0; j < 1024; j++){
			if(cont < control){
				p[j]->endereco[i] = bin;
				cont++;
			}
			else{
				if(bin == '1')
					bin = '0';
				else bin = '1';
				cont = 1;
				p[j]->endereco[i] = bin;
			}
		}
		control *= 2;
	}
	return p;
}

CedulaCache** criaCache(CedulaCache **p){	//funcao que cria uma memoria cache
	int control = 1, cont;
	char bin = '0';
	CedulaCache *q;
	p = (CedulaCache**) malloc (32*sizeof(CedulaCache*));	//aloca memoria para uma memoria cache de 8 posicoes
	
	for(int i = 0; i < 32; i++){
		q = criaCedulaCache(q);
		p[i] = q;
	}
	
	for(int i = 4, cont = 0; i >= 0; i--){	//preenche os enderecos
		cont = 0;
		bin = '0';
		for(int j = 0; j < 32; j++){
			if(cont < control){
				p[j]->endereco[i] = bin;
				cont++;
			}
			else{
				if(bin == '1')
					bin = '0';
				else bin = '1';
				cont = 1;
				p[j]->endereco[i] = bin;
			}
		}
		control *= 2;
	}
	return p;		//retorna a memoria cache vazia
}

void printaCache(CedulaCache **p){	//FUNCAO QUE PRTINTA MEMORIA PARA MELHOR VISUALIZACAO
	printf("------------MemoriaCache------------\n");
	printf("IndiceCache  IndiceMemoria  Conteudo\n");
	for(int i = 0; i < 32; i++)
		printf("   %s      %s     %s \n", p[i]->endereco, p[i]->tag, p[i]->conteudo);
	
}

void printaMem(CedulaMemoria **p){	//funcao que printa a memoria principal
	printf("Memoria principal\n");
	for(int i = 0; i < 1024; i++)
		printf("%s %s\n", p[i]->endereco, p[i]->conteudo);

}

int buscaNaCache(CedulaCache **p, char aux[], int inicioCache, int finalCache){	//funcao que busca um endereco na memoria cache e retorna 1 caso encontre e 0 caso nao

	for(int i = inicioCache; i < finalCache; i++)
		if(strncmp(p[i]->tag, aux, 10)==0)
			return 1;
						
	return 0;
}

//funcao de busca na memoria para MAPEAMENTO DIRETO
int buscaNaMemoriaPrincipalMapeamentoDireto(CedulaMemoria **mem, CedulaCache **cache, char aux[]){
	int result = 0, indc;
	char newN[5];
	 
	for(int j = 9; j >= 5; j--){    //pegando os 5 ultimos numeros do index da memoria principal que sera a posicao na cache, mesma coisa que modulo
		newN[j-5] = aux[j]; 
	}
	for(int k = 0; k < 5; k++){		//convertendo binario para decimal
		result += (int)((newN[k]-'0') * pow(2,4-k));
	}
	for(int i = 0; i < 1024; i++){
		if(strncmp(mem[i]->endereco, aux, 10)==0){
			strcpy(cache[result]->tag, mem[i]->endereco);
			strcpy(cache[result]->conteudo, mem[i]->conteudo);
			//printaCache(cache);
			
			break;				
		}
	}
	return result;	// a funcao retorna o local a ser subistituido
			
}

//Funcao que busca na memoria com LFU
int buscaNaMemoriaPrincipalLFU(CedulaMemoria **mem, CedulaCache **cache, char aux[], int freq[], int tamBloco, int indice, int** mat){
	int menor = 99999, indc;
	if(mat == NULL) {                            //if para caso a LFU esetja sendo usada para Mapeamento completamente associativo
		for(int i = 0; i < tamBloco; i++)		//funcao que busca na memoria principal o dado, e coloca ele na cache, colocando na posicao de menor freq
			if(freq[i] < menor){
				menor = freq[i];	
				indc = i;
			}
	}		
	else {
		for(int i = 0; i < tamBloco; i++)	{ 
				if(mat[indice][i] < menor){
					menor = mat[indice][i];	
					indc = i;
				}
		}
	}			
	for(int i = 0; i < 1024; i++){
		if(strncmp(mem[i]->endereco, aux, 10)==0){
			strcpy(cache[indc + (indice * 8)]->tag, mem[i]->endereco);
			strcpy(cache[indc + (indice * 8)]->conteudo, mem[i]->conteudo);
			//printaCache(cache);
			
			break;				
		}
	}
	return indc;	// a funcao retorna o indice que foi substituido para se atualizar a nova frequencia
			
}

//funcao que busca na memoria com FIFO
int buscaNaMemoriaPrincipalFIFO(CedulaMemoria **mem, CedulaCache **cache, char aux[], int freq[], int tamBloco, int indice, int** mat){
	int indc;
	if(mat == NULL) {                            //if para caso a FIFO esteja sendo usada para mapeamento completamente associativo
		for(int i = 0;i < tamBloco; i++) {		//funcao que busca na memoria prncipal o dado na primeira posicao
			if(freq[i] < 0) {
				indc = i;
				break;
			}
			if(freq[i] == 1){
				indc = i;
			}
		}
	}else {
		for(int i = 0;i < tamBloco; i++) {	
			if(mat[indice][i] < 0) {
				indc = i;
				break;
			}
			if(mat[indice][i] == 1){
				indc = i;
			}
		}
	}
	for(int i = 0; i < 1024; i++){
		if(strncmp(mem[i]->endereco, aux, 10)==0){
			strcpy(cache[indc + (indice * 8)]->tag, mem[i]->endereco);
			strcpy(cache[indc + (indice * 8)]->conteudo, mem[i]->conteudo);
			//printaCache(cache);
			
			break;				
		}
	}
	return indc;	// a funcao retorna o indice que foi substituido pelo primeiro da fila 
			
}

int qFrequencia(CedulaCache** p, char aux[]){	//funcao que retorna em qual posicao da cache esta o dado, para que se possa aumentar a freq
	for(int i = 0;i < 32; i++)
		if(strncmp(p[i]->tag, aux, 10)==0)
			return i;
}


int main(){

	CedulaMemoria **memoriaPrincipal;
	CedulaCache **memoriaCache;
	char aux[10];
	int frequencia[32];
	int FIFO[32];
	int control;
	int contHit = 0, contMiss = 0, contHitFIFO = 0, contMissFIFO = 0, contHitMapDir = 0, contMissMapDir = 0, contMissAssFIFO = 0, contHitAssFIFO = 0, contMissAssLFU = 0, contHitAssLFU = 0;
	
	for(int i = 0; i < 32 ; i++)
		frequencia[i] = 0;
	
	memoriaPrincipal = criaMemoria(memoriaPrincipal);	// cria a memoria  principal e a memoria cache
	memoriaCache = criaCache(memoriaCache);
	
	
	for(int i = 0; i < 1024; i++)
		strcpy(memoriaPrincipal[i]->conteudo, "dados");
	
	FILE *q = fopen("Leitura1024.txt","r");
	int cont = 0;
	
	//----------Mapeamento Completamente Associativo LFU------------------------------------------------------------------------
	while(true){					//faz as chamadas referentes as requisições de memoria
		
		cont++;
		fscanf(q, "%s", aux);		// le a chamada
		if(feof(q))					// testa se o arquivo nao acabou
			break;		
		if(buscaNaCache(memoriaCache, aux, 0, 32)){				//testa se o endereço esta na cache
			//printf("Cache Hit para o endereço %s\n", aux);	// se estiver printa o cache hit
			if(cont > 32)									//aquecimento de cache
				contHit++;										//aumenta o numero de hits
			frequencia[qFrequencia(memoriaCache, aux)]++;	// e aumenta a frequencia que aquele endereco aparece
		}
		else{
			//printf("Cache Miss para o endereco %s\n", aux);	//caso nao estiver na cache printa o cache miss
			if(cont > 32);									//aquecimento de cache
				contMiss++;										//aumenta o numero de miss
			control = buscaNaMemoriaPrincipalLFU(memoriaPrincipal, memoriaCache, aux, frequencia, 32, 0, NULL); // busca na memoria principal o dado 
			frequencia[control] = 1;	// coloca 1 na frequencia do item que foi substituido
		}
	}
	
	memoriaCache = criaCache(memoriaCache); //cria outra cache vazia
	for(int i = 0; i < 32 ; i++)
		FIFO[i] = -1;
		
	//----------Mapeamento Completamente Associativo - FIFO --------------------------------------------------------------------------------
	q = fopen("Leitura1024.txt","r");
	cont = 0;
	while(true){					//faz as chamadas referentes as requisições de memoria
		
		cont++;
		fscanf(q, "%s", aux);		// le a chamada
		if(feof(q))					// testa se o arquivo nao acabou
			break;		
		if(buscaNaCache(memoriaCache, aux, 0, 32)){				//testa se o endereço esta na cache
			//printf("Cache Hit para o endereço %s\n", aux);	// se estiver printa o cache hit
			if(cont > 32)									//aquecimento de cache
				contHitFIFO++;										//aumenta o numero de hits
		}
		else{
			//printf("Cache Miss para o endereco %s\n", aux);	//caso nao estiver na cache printa o cache miss
			if(cont > 32);									//aquecimento de cache
				contMissFIFO++;										//aumenta o numero de miss
			control = buscaNaMemoriaPrincipalFIFO(memoriaPrincipal, memoriaCache, aux, FIFO, 32, 0, NULL); // busca na memoria principal o dado
			for(int k = 0; k < 32; k++)
				FIFO[k]--;
			FIFO[control] = 32;	// coloca o dado na fila
		}
		
	}
	
	memoriaCache = criaCache(memoriaCache); //cria outra cache vazia
	
	//----------Mapeamento Direto ----------------------------------------------------------------------------------------------------
	q = fopen("Leitura1024.txt","r");
	cont = 0;
	while(true){					//faz as chamadas referentes as requisições de memoria
		
		cont++;
		fscanf(q, "%s", aux);		// le a chamada
		if(feof(q))					// testa se o arquivo nao acabou
			break;		
		if(buscaNaCache(memoriaCache, aux, 0, 32)){				//testa se o endereço esta na cache
			//printf("Cache Hit para o endereço %s\n", aux);	// se estiver printa o cache hit
			if(cont > 32)									//aquecimento de cache
				contHitMapDir++;										//aumenta o numero de hits
		}
		else{
			//printf("Cache Miss para o endereco %s\n", aux);	//caso nao estiver na cache printa o cache miss
			if(cont > 32);									//aquecimento de cache
				contMissMapDir++;										//aumenta o numero de miss
			control = buscaNaMemoriaPrincipalMapeamentoDireto(memoriaPrincipal, memoriaCache, aux); // busca na memoria principal o dado
		}
		
	}
	
	memoriaCache = criaCache(memoriaCache); //cria outra cache vazia
	//----------Mapeamento Associativo por Conjunto 4-way - FIFO --------------------------------------------------------------------------
	q = fopen("Leitura1024.txt","r");
	cont = 0;
	
	//criando matriz para a FIFO
	int** matrizFIFO = (int**) malloc(4 * sizeof(int*));
	for(int i = 0; i < 4; i++)
		matrizFIFO[i] = (int*) malloc(8 * sizeof(int));
		
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 8; j++)
			matrizFIFO[i][j] = -1;		
	
	while(true){						//faz as chamadas referentes as requisições de memoria
		
		cont++;
		fscanf(q, "%s", aux);			// le a chamada
		char newN[2];
		for(int j= 9; j >= 8; j--)    	//pegando os 5 ultimos numeros do index da memoria principal que sera a posicao na cache, mesma coisa que modulo
			newN[j-8] = aux[j];
			
		int indice = 0;
		for(int k = 0; k < 2;k++)		//convertendo binario para decimal
			indice += (int)((newN[k]-'0') * pow(2,1-k));
		
		if(feof(q))						// testa se o arquivo nao acabou
			break;		
		if(buscaNaCache(memoriaCache, aux, 0 + (indice * 8), 8 + (indice * 8))){				//testa se o endereço esta na cache
			//printf("Cache Hit para o endereço %s\n", aux);									// se estiver printa o cache hit
			if(cont > 32)																		//aquecimento de cache
				contHitAssFIFO++;																//aumenta o numero de hits
		}
		else{
			//printf("Cache Miss para o endereco %s\n", aux);	//caso nao estiver na cache printa o cache miss
			if(cont > 32);									//aquecimento de cache
				contMissAssFIFO++;										//aumenta o numero de miss
			control = buscaNaMemoriaPrincipalFIFO(memoriaPrincipal, memoriaCache, aux, NULL, 8, indice, matrizFIFO); // busca na memoria principal o dado
			for(int k = 0; k < 8; k++)
				matrizFIFO[indice][k]--;
			matrizFIFO[indice][control] = 8;
		}
		
	}
	
	memoriaCache = criaCache(memoriaCache); //cria outra cache vazia
	//----------Mapeamento Associativo por Conjunto 4-way - LFU ---------------------------------------------------------------------------------------------------
	q = fopen("Leitura1024.txt","r");
	cont = 0;
	
	//criando matriz para a LFU
	int** matrizLFU = (int**) malloc(4 * sizeof(int*));
	for(int i = 0; i < 4; i++)
		matrizLFU[i] = (int*) malloc(8 * sizeof(int));
		
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 8; j++)
			matrizLFU[i][j] = 0;		
	
	while(true){						//faz as chamadas referentes as requisições de memoria
		
		cont++;
		fscanf(q, "%s", aux);			// le a chamada
		char newN[2];
		for(int j= 9; j >= 8; j--)    	//pegando os 5 ultimos numeros do index da memoria principal que sera a posicao na cache, mesma coisa que modulo
			newN[j-8] = aux[j];
			
		int indice = 0;
		for(int k = 0; k < 2;k++)		//convertendo binario para decimal
			indice += (int)((newN[k]-'0') * pow(2,1-k));
		
		if(feof(q))						// testa se o arquivo nao acabou
			break;		
		if(buscaNaCache(memoriaCache, aux, 0 + (indice * 8), 8 + (indice * 8))){				//testa se o endereço esta na cache
			//printf("Cache Hit para o endereço %s\n", aux);									// se estiver printa o cache hit
			if(cont > 32)																		//aquecimento de cache
				contHitAssLFU++;
			matrizLFU[indice][qFrequencia(memoriaCache, aux)]++;	
		}
		else{
			//printf("Cache Miss para o endereco %s\n", aux);									//caso nao estiver na cache printa o cache miss
			if(cont > 32);																		//aquecimento de cache
				contMissAssLFU++;																//aumenta o numero de miss
			control = buscaNaMemoriaPrincipalLFU(memoriaPrincipal, memoriaCache, aux, NULL, 8, indice, matrizLFU); // busca na memoria principal o dado
			matrizLFU[indice][control] = 1;
		}
		
	}
	
	printf("Resultados LFU com Mapeamento Associativo por Conjunto 4-way com\n");
	printf("a taxa de hit eh: %.2f (%d)\n", (float)contHitAssLFU/(contHitAssLFU + contMissAssLFU), contHitAssLFU);	//imprime a taxa de hit
	printf("a taxa de miss eh %.2f (%d)\n", (float)contMissAssLFU/(contHitAssLFU + contMissAssLFU), contMissAssLFU);//imprime a taxa de miss
	
	printf("Resultados FIFO com Mapeamento Associativo por Conjunto 4-way com\n");
	printf("a taxa de hit eh: %.2f (%d)\n", (float)contHitAssFIFO/(contHitAssFIFO + contMissAssFIFO), contHitAssFIFO);
	printf("a taxa de miss eh %.2f (%d)\n", (float)contMissAssFIFO/(contHitAssFIFO + contMissAssFIFO), contMissAssFIFO);
	
	printf("Resultados Mapeamento Direto\n");
	printf("a taxa de hit eh: %.2f (%d)\n", (float)contHitMapDir/(contHitMapDir + contMissMapDir), contHitMapDir);
	printf("a taxa de miss eh %.2f (%d)\n", (float)contMissMapDir/(contHitMapDir + contMissMapDir), contMissMapDir);
	
	printf("Resultados LFU com Mapeamento Completamente Associativo\n");
	printf("a taxa de hit eh: %.2f (%d)\n", (float)contHit/(contHit + contMiss), contHit);	
	printf("a taxa de miss eh %.2f (%d)\n", (float)contMiss/(contHit + contMiss), contMiss);	
	
	
	printf("Resultados FIFO com Mapeamento Completamente Associativo\n");
	printf("a taxa de hit eh: %.2f (%d)\n", (float)contHitFIFO/(contHitFIFO + contMissFIFO), contHitFIFO);	
	printf("a taxa de miss eh %.2f (%d)\n", (float)contMissFIFO/(contHitFIFO + contMissFIFO), contMissFIFO);	
	
}


