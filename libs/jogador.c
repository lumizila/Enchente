#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "jogador.h"
#include "lista.h"
#include "grafo.h"

// Grupo eh o grupo inicial de quadrados
Lista Joga(Grafo g, Lista grupo){
    // Inicializando a lista com as jogadas que faremos
    Lista jogadas = constroiLista();
    int counter = 1;
    // Enquanto a soma das posicoes do grupo atual for menor do que o total das posicoes do tabuleiro
    while(tamanhoLista(grupo) < tamanhoLista(g->vertices)) {
        // Pega os filhos do grupo
        Lista filhos = filhosGrupo(grupo);
        // Monta a árvore de busca:
        //      - RAIZ: grupo
        //      - FILHOS: Cores alcancáveis a partir da raiz
        //      - NETOS: Cores alcançáveis a partir dos filhos que NÃO são alcançáveis a partir da raiz
        //          Só é necessário para calcular o bônus de cada filho
        
	// Agrupa os filhos que tem a mesma cor e calcula a quantidade de quadrados que podem ser consumidos
	// para cada cor que sera o valor do bonus, o bonus tambem aumenta de acordo com a quantidade de netos
	// do grupo atual que podem ser alcancados ao se escolher cada cor.
	// TODO: Queremos aumentar esse bonus de acordo com os bisnetos tambem.
	Lista coresFilhos = agrupaCores(filhos, g);
        
/*	 printf("\tJOGADA %d\n", counter);
         printf("\tTamanho coresFilhos %d\n", tamanhoLista(coresFilhos));
         for(No n = primeiroNoLista(coresFilhos); n; n = getSucessorNo(n)) {
             Vertice v = (Vertice) getConteudo(n);
             printf("\t\tVértice - cor: %d, peso: %d, bonus: %d\n", v->cor, v->peso, v->bonus);
         }
*/

        // Seleciona o melhor filho baseado em peso(filho) + bônus(filho) // (filho com a maior soma de filho e peso)
        // O bônus é calculado da seguinte forma:
        //      - Soma o valor de cada neto (que não é alcançável pela raiz)
        //      - Em caso de empate da soma peso + bônus:
        //          - Escolher o filho que tem mais netos da mesma cor de um filho
        // Após escolher um filho, repete o algoritmo até não terem mais filhos fora do grupo
        Vertice maior = (Vertice) getConteudo(primeiroNoLista(coresFilhos));
        for(No n = primeiroNoLista(coresFilhos); n; n = getSucessorNo(n)) {
            Vertice v = (Vertice) getConteudo(n);
            // TODO: tratar empates!
            if((v->bonus) > (maior->bonus)) {
                maior = v;
            } else if((v->bonus) == (maior->bonus)) {
                if(v->peso > maior->peso) {
                    maior = v;
                }
            }
        }
        // printf("\t\tCOR ESCOLHIDA: %d\n", maior->cor);
        insereLista(maior->cor, jogadas);
        // "Pinta o tablueiro"
        for(No n = primeiroNoLista(filhos); n; n = getSucessorNo(n)) {
            Vertice v = (Vertice) getConteudo(n);
            if(v->cor == maior->cor && !v->grupo) {
                insereUnicoLista(v, grupo);
                v->grupo = true;
            }
        }

        // Limpa as coisas
        destroiLista(filhos, NULL);
        destroiLista(coresFilhos, destroiVertice);

        // PARA DEBUG!! Imprime as últimas 10 jogadas em um arquivo
        //  char str[32];
        //  sprintf(str, "./jogada%d.out", counter );
        //  FILE* debug = fopen(str, "w+");
        //  if(debug) {
        //      grafoParaDot(g, grupo, debug);
        //  }
        //  fclose(debug);
         ++counter;
    }

    return jogadas;
}

Lista filhosGrupo(Lista grupoPai) {
    Lista filhos = constroiLista();
    for(No n = primeiroNoLista(grupoPai); n; n = getSucessorNo(n)) {
        Vertice pai = (Vertice) getConteudo(n);
        for(No m = primeiroNoLista(pai->filhos); m; m = getSucessorNo(m)) {
            Vertice filho = (Vertice) getConteudo(m);
            if(!filho->grupo) {
                insereUnicoLista(filho, filhos);
            }
        }
    }
    return filhos;
}

Lista agrupaCores(Lista filhos, Grafo g) { // filhos eh uma lista de filhos contendo os nodos filhos do grupo atual
    Lista agrupa = constroiLista();
    // Para cada nodo na lista de filhos
    for(No n = primeiroNoLista(filhos); n; n = getSucessorNo(n)) {
//	printf("o filho visitado agora eh:");
	Vertice v = (Vertice) getConteudo(n);
//        printf("\t\tVértice - cor: %d, peso: %d, bonus: %d\n -- \n", v->cor, v->peso, v->bonus);

	// Verifica se a cor já está na lista
        bool estaNaLista = false;
        for(No m = primeiroNoLista(agrupa); m; m = getSucessorNo(m)) {
            Vertice w = (Vertice) getConteudo(m);
            // Se está, soma o peso do vértice
            if(w->cor == v->cor) {
//	    	printf("A cor %d desse filho ja esta na lista, eu somo o peso desse filho...\n", v->cor);
                w->peso += v->peso;
                w->bonus += calculaBonus(v, filhos);
                estaNaLista = true;
            }
        }
        // Se não está, cria um vértice para a cor
        if(!estaNaLista) {
//	    printf("A cor deste filho -%d- nao esta na lista... adiciono na lista 'agrupa' ...\n", v->cor);
            Vertice w = criaVertice();
            w->cor = v->cor;
            w->peso = v->peso;
	    //Calcula bonus aqui esta somando o bonus dos filhos do filho v do grupo corrente 
            w->bonus = calculaBonus(v, filhos);
	    insereLista(w, agrupa);
        }
    }

    // Depois de agrupar, verifica se alguma cor vai chegar ao fim nesta jogada
    for(No n = primeiroNoLista(agrupa); n; n = getSucessorNo(n)) {
        Vertice v = (Vertice) getConteudo(n);
        int somaCor = 0;
        for(No m = primeiroNoLista(g->vertices); m; m = getSucessorNo(m)) {
            Vertice w = (Vertice) getConteudo(m);
            if(!w->grupo && w->cor == v->cor) {
                somaCor += w->peso;
            }
        }
        // Se a soma de todos os vértices que não pertencem ao grupo for igual
        //      ao peso do vértice agrupado, esta é a última jogada com aquela cor
        if(v->peso == somaCor) {
            v->bonus += 1000; // Mais bonus para que essa cor seja a escolhida
        }
    }

    return agrupa;
}

//TODO: eh aqui que irei alterar para somar o bonus dos netos
int calculaBonus(Vertice v, Lista filhos) {
    int bonus = 0;
    for(No n = primeiroNoLista(v->filhos); n; n = getSucessorNo(n)) {
        Vertice filhoDeV = getConteudo(n);
        // Se o filho não está na lista filhos e não está no grupo de vértices já consumidos
        if(!filhoDeV->grupo && !pertenceLista(filhoDeV, filhos)) {
	    // Calcula o peso dos netos (filhos deste filho) e soma no bonus
	    int bonusNetos = 0;
	    for(No neto = primeiroNoLista(filhoDeV->filhos); neto; neto = getSucessorNo(neto)){
		Vertice netoDeV = getConteudo(neto);
		// Se o neto nao esta na lista de seus tios e nao esta no grupo de vertices ja consumidos e nao esta na lista de filhos do grupo atual
		if(!netoDeV->grupo && !pertenceLista(netoDeV, v->filhos) && !pertenceLista(netoDeV, filhos)){
			bonusNetos += netoDeV->peso;
		}
	    }
            bonus = bonus + filhoDeV->peso + bonusNetos;
        }
    }
    return bonus;
}
