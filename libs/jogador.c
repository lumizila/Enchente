#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "jogador.h"
#include "lista.h"
#include "grafo.h"

Lista Joga(Grafo g, Lista grupo){
    Lista jogadas = constroiLista();
    //TODO: A Logica toda do jogo vai ficar aqui
    int counter = 1;
    while(tamanhoLista(grupo) < tamanhoLista(g->vertices)) {
        // Pega os filhos do grupo
        Lista filhos = filhosGrupo(grupo);
        // Monta a árvore de busca:
        //      - RAIZ: grupo
        //      - FILHOS: Cores alcancáveis a partir da raiz
        //      - NETOS: Cores alcançáveis a partir dos filhos que NÃO são alcançáveis a partir da raiz
        //          Só é necessário para calcular o bônus de cada filho
        Lista coresFilhos = agrupaCores(filhos, g);
        // printf("\tJOGADA %d\n", counter);
        // printf("\tTamanho coresFilhos %d\n", tamanhoLista(coresFilhos));
        // for(No n = primeiroNoLista(coresFilhos); n; n = getSucessorNo(n)) {
        //     Vertice v = (Vertice) getConteudo(n);
        //     printf("\t\tVértice - cor: %d, peso: %d, bonus: %d\n", v->cor, v->peso, v->bonus);
        // }
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

Lista agrupaCores(Lista filhos, Grafo g) {
    Lista agrupa = constroiLista();
    for(No n = primeiroNoLista(filhos); n; n = getSucessorNo(n)) {
        Vertice v = (Vertice) getConteudo(n);
        // Verifica se a cor já está na lista
        bool estaNaLista = false;
        for(No m = primeiroNoLista(agrupa); m; m = getSucessorNo(m)) {
            Vertice w = (Vertice) getConteudo(m);
            // Se está, soma o peso do vértice
            if(w->cor == v->cor) {
                w->peso += v->peso;
                w->bonus += calculaBonus(v, filhos);
                estaNaLista = true;
            }
        }
        // Se não está, cria um vértice para a cor
        if(!estaNaLista) {
            Vertice w = criaVertice();
            w->cor = v->cor;
            w->peso = v->peso;
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
            v->bonus += 100; // Mais bonus para que essa cor seja a escolhida
        }
    }

    return agrupa;
}

int calculaBonus(Vertice v, Lista filhos) {
    int bonus = 0;
    for(No n = primeiroNoLista(v->filhos); n; n = getSucessorNo(n)) {
        Vertice filho = getConteudo(n);
        // Se o filho não está na lista filhos e não está no grupo de vértices já consumidos
        if(!filho->grupo && !pertenceLista(filho, filhos)) {
            bonus += filho->peso;
        }
    }
    return bonus;
}
