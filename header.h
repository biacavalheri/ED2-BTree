//-------------------- Libraries ----------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//-------------------- Constants ----------------------------

#define MAXKEYS 3
#define MINKEYS 2
#define NIL (-1)
#define PAGESIZE sizeof(BTPAGE)

//--------------------  Structs  ----------------------------

typedef struct
{
    char ID_aluno[4];
    char sigla_disc[4];
    char nome_aluno[50];
    char nome_disc[50];
    float media;
    float frequencia;
} REGISTRO;

typedef struct
{
    char ID_aluno[4];
    char sigla_disc[4];
} CHAVE_PRIMARIA;

typedef struct 
{
    CHAVE_PRIMARIA id;
    int rrn;
} CHAVE_PAGINA;

typedef struct {
    int quantidadeNos;
    CHAVE_PAGINA chaves[MAXKEYS];
    int filhos[MAXKEYS+1];
} BTPAGE;

//--------------------  Creation Functions  ----------------------------

int criarArvore();
int criarRaiz(CHAVE_PAGINA chave, int esquerda, int direita);
void iniciarPagina(BTPAGE *pagina);

//--------------------  Insertion Function  ----------------------------

int insereRegistro(REGISTRO novoRegistro);
void inserirNaPagina(CHAVE_PAGINA chave, int rrnPromovido, BTPAGE *pagina);
bool inserirNaArvore (int rrn, CHAVE_PAGINA proximaChave, int *rrnPromovido, CHAVE_PAGINA *chavePromovida);

//--------------------   Search Functions   ----------------------------

int buscaRegistroNaArvore(int rrn, CHAVE_PAGINA chave);
void buscaRegistroRRN(int buscaRegistroRRN, FILE* arquivo);
bool buscarNo (CHAVE_PAGINA chave, BTPAGE *pagina, int *posicao);
int recuperarRrnRaiz();

//--------------------    Aux. Functions    ----------------------------

int compararChaves(CHAVE_PAGINA chave1, CHAVE_PAGINA chave2);
void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina);
void imprimeArvoreEmOrdem(int rrn);
FILE* verificaArquivo(char *arquivo);
int buscarQuantidadePagina ();
void lerPagina (int rrn, BTPAGE *pagina);
void escrevePagina (int rrn, BTPAGE *pagina);
void atualizarIndiceComNovaRaiz(int rrnRaiz);
CHAVE_PAGINA criaNoKey();

//--------------------    Files    ----------------------------

FILE* indice;
FILE* resultado;