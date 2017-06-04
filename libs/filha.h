#ifndef _FILHA_
#define _FILHA_
#include <stdbool.h>
#include "no.h"

// Filha é uma estrutura que pode ser vista como Fila ou Pilha
typedef struct Filha *Filha;

// Nó cujo conteúdo é um void *
typedef struct No *No;

// Cria uma filha vazia e retorna
// Devolve NULL em caso de falha
Filha constroiFilha();

// Devolve o número de nós da filha f
unsigned int tamanhoFilha(Filha f);

// Remove o primeiro nó da filha f ou retorna NULL se f for vazia
No primeiroNoFilha(Filha f);

// Remove o último nó da filha f ou retorna NULL se f for vazia
No ultimoNoFilha(Filha f);

// Insere um novo nó no fim da Filha f
//
// Devolve o nó recém criado ou NULL em caso de falha
No insereFilha(void *conteudo, Filha f);

// Desaloca a filha f e todos os seus nós
//
// se destroi != NULL invoca
//      destroi(conteudo(n))
// para cada nó n da filha
//
// devolve true em caso de sucesso ou false em caso de falha
bool destroiFilha(Filha f, bool destroi(void *));

// Impreme todos os nós da filha f
//
// se imprime != NULL invoca
//      imprime(conteudo(n))
// para cada nó n da filha
void imprimeFilha(Filha f, void imprime(void *));

#endif
