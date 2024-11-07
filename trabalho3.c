#include "header.h"

int criarArvore()
{
    char chave;
    indice = fopen("indice.bin", "w+b");

    fseek(indice, 0, SEEK_SET);
    int header = -1;
    fwrite(&header, sizeof(int), 1, indice);

    return header;
}

int criarRaiz(CHAVE_PAGINA chave, int esquerda, int direita)
{
    BTPAGE page;
    int rrn = buscarQuantidadePagina();
    iniciarPagina(&page);
    page.chaves[0] = chave;
    page.filhos[0] = esquerda;
    page.filhos[1] = direita;
    page.quantidadeNos = 1;
    escrevePagina(rrn, &page);
    atualizarIndiceComNovaRaiz(rrn);
    return (rrn);
}

void iniciarPagina(BTPAGE *pagina)
{
    int j;

    for (j = 0; j < MAXKEYS; j++)
    {
        pagina->chaves[j] = criaNoKey();
        pagina->filhos[j] = NIL;
    }

    pagina->filhos[MAXKEYS] = NIL;
}

int insereRegistro(REGISTRO novoRegistro)
{
    char registro[256];
    sprintf(registro, "%.3s#%.3s#%.50s#%.50s#%.2f#%.2f#\n",
            novoRegistro.ID_aluno,
            novoRegistro.sigla_disc,
            novoRegistro.nome_aluno,
            novoRegistro.nome_disc,
            novoRegistro.media,
            novoRegistro.frequencia);

    int tamRegistro = strlen(registro);

    fseek(resultado, 0, SEEK_END);
    int offSet = ftell(resultado);

    fwrite(&tamRegistro, sizeof(int), 1, resultado);
    fwrite(registro, sizeof(char), tamRegistro, resultado);
    return offSet;
}

void inserirNaPagina(CHAVE_PAGINA chave, int rrnPromovido, BTPAGE *pagina)
{
    int j;

    for (j = pagina->quantidadeNos; compararChaves(chave, pagina->chaves[j - 1]) < 0 && j > 0; j--)
    {
        pagina->chaves[j] = pagina->chaves[j - 1];
        pagina->filhos[j + 1] = pagina->filhos[j];
    }

    pagina->quantidadeNos++;
    pagina->chaves[j] = chave;
    pagina->filhos[j + 1] = rrnPromovido;
}

bool inserirNaArvore(int rrn, CHAVE_PAGINA proximaChave, int *rrnPromovido, CHAVE_PAGINA *chavePromovida)
{
    BTPAGE paginaAtual, novaPagina;
    bool encontrado, promovido;
    int posicao, rrnPromovidoDeBaixo;
    CHAVE_PAGINA chavePromovidaDeBaixo;

    if (rrn == NIL)
    {
        *chavePromovida = proximaChave;
        *rrnPromovido = NIL;
        printf("+----- Chave Inserida: <%s%s> -----+\n", proximaChave.id.ID_aluno, proximaChave.id.sigla_disc);
        return true;
    }

    lerPagina(rrn, &paginaAtual);
    encontrado = buscarNo(proximaChave, &paginaAtual, &posicao);
    if (encontrado)
    {

        return false;
    }

    promovido = inserirNaArvore(paginaAtual.filhos[posicao], proximaChave, &rrnPromovidoDeBaixo, &chavePromovidaDeBaixo);

    if (!promovido)
    {
        return false;
    }

    if (paginaAtual.quantidadeNos < MAXKEYS)
    {
        inserirNaPagina(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual);
        escrevePagina(rrn, &paginaAtual);
        return false;
    }
    else
    {
        split(chavePromovidaDeBaixo, rrnPromovidoDeBaixo, &paginaAtual, chavePromovida, rrnPromovido, &novaPagina);
        escrevePagina(rrn, &paginaAtual);
        escrevePagina(*rrnPromovido, &novaPagina);
        return true;
    }
}

int buscaRegistroNaArvore(int rrn, CHAVE_PAGINA chave)
{
    BTPAGE paginaAtual;
    bool encontrado;
    int posicao;

    if (rrn == NIL)
    {
        return -1;
    }

    lerPagina(rrn, &paginaAtual);
    encontrado = buscarNo(chave, &paginaAtual, &posicao);

    if (encontrado)
    {
        return paginaAtual.chaves[posicao].rrn;
    }

    return buscaRegistroNaArvore(paginaAtual.filhos[posicao], chave);
}

void buscaRegistroRRN(int rrnArquivoResultado, FILE *arquivo)
{
    int tamanhoRegistro;
    char registroBuscado[130];
    fseek(arquivo, rrnArquivoResultado, SEEK_SET);
    fread(&tamanhoRegistro, sizeof(int), 1, arquivo);
    fread(registroBuscado, sizeof(char), tamanhoRegistro, arquivo);
    registroBuscado[tamanhoRegistro] = '\0';
    puts(registroBuscado);
}

bool buscarNo(CHAVE_PAGINA chave, BTPAGE *pagina, int *posicao)
{
    int i;
    for (i = 0; i < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[i]) > 0; i++)
        ;
    *posicao = i;
    return (*posicao < pagina->quantidadeNos && compararChaves(chave, pagina->chaves[*posicao]) == 0);
}

int recuperarRrnRaiz()
{
    int rrnRaiz;
    fseek(indice, 0, SEEK_SET);
    fread(&rrnRaiz, sizeof(int), 1, indice);
    return rrnRaiz;
}

int compararChaves(CHAVE_PAGINA chave1, CHAVE_PAGINA chave2)
{
    char id1[20], id2[20];
    sprintf(id1, "%s%s", chave1.id.ID_aluno, chave1.id.sigla_disc);
    sprintf(id2, "%s%s", chave2.id.ID_aluno, chave2.id.sigla_disc);

    return strcmp(id1, id2);
}

void split(CHAVE_PAGINA chave, int filhoDireita, BTPAGE *paginaDividida, CHAVE_PAGINA *chavePromovida, int *filhoDireitaChavePromovida, BTPAGE *novaPagina)
{
    printf("+----- Divisão de Nó -----+\n");

    int j;
    CHAVE_PAGINA auxChaves[MAXKEYS + 1];
    int auxFilhos[MAXKEYS + 2];

    for (j = 0; j < MAXKEYS; j++)
    {
        auxChaves[j] = paginaDividida->chaves[j];
        auxFilhos[j] = paginaDividida->filhos[j];
    }
    auxFilhos[MAXKEYS] = paginaDividida->filhos[MAXKEYS];

    for (j = MAXKEYS; (compararChaves(chave, auxChaves[j - 1]) < 0) && j > 0; j--)
    {
        auxChaves[j] = auxChaves[j - 1];
        auxFilhos[j + 1] = auxFilhos[j];
    }
    auxChaves[j] = chave;
    auxFilhos[j + 1] = filhoDireita;

    *filhoDireitaChavePromovida = buscarQuantidadePagina();
    iniciarPagina(novaPagina);

    for (j = 0; j < MINKEYS; j++)
    {
        paginaDividida->chaves[j] = auxChaves[j]; // posições: 0 e 1
        paginaDividida->filhos[j] = auxFilhos[j]; // posições: 0 e 1

        novaPagina->chaves[j] = auxChaves[j + MINKEYS]; // posições: 2 e 3
        novaPagina->filhos[j] = auxFilhos[j + MINKEYS]; // posições: 2 e 3

        if ((j + MINKEYS) < MAXKEYS)
        {
            paginaDividida->chaves[j + MINKEYS] = criaNoKey(); // posições: 2 e 3
        }
        paginaDividida->filhos[j + MINKEYS] = NIL; // posições: 2 e 3
    }
    novaPagina->filhos[MINKEYS] = auxFilhos[j + MINKEYS]; // posição: 4

    paginaDividida->chaves[1] = criaNoKey();

    novaPagina->quantidadeNos = (MAXKEYS + 1) - MINKEYS;
    paginaDividida->quantidadeNos = MINKEYS - 1;

    *chavePromovida = auxChaves[1];

    printf("+----- Chave Promovida: <%s%s> -----+\n", chavePromovida->id.ID_aluno, chavePromovida->id.sigla_disc);
}

void imprimeArvoreEmOrdem(int rrn)
{
    BTPAGE paginaAtual;

    lerPagina(rrn, &paginaAtual);

    // Se a página for uma folha
    if (paginaAtual.filhos[0] == NIL)
    {
        // Percorre as chaves da página e imprime os registros associados a essas chaves
        for (int i = 0; i < paginaAtual.quantidadeNos; i++)
        {
            buscaRegistroRRN(paginaAtual.chaves[i].rrn, resultado);
        }
        // Retorna da função, encerrando a execução para esse nó
        return;
    }
    else
    {
        // Se a página não for uma folha, chama recursivamente a função para o primeiro filho
        imprimeArvoreEmOrdem(paginaAtual.filhos[0]);
    }

    // Percorre as chaves da página e chama recursivamente a função para os filhos
    for (int i = 0; i < paginaAtual.quantidadeNos; i++)
    {
        // Imprime o registro associado à chave na posição i
        buscaRegistroRRN(paginaAtual.chaves[i].rrn, resultado);
        // Chama a função recursivamente para o filho da direita da chave i
        imprimeArvoreEmOrdem(paginaAtual.filhos[i + 1]);
    }
}

FILE *verificaArquivo(char *arquivo)
{
    FILE *fp = fopen(arquivo, "r+b");

    if (fp == NULL)
    {
        printf("O arquivo %s não existe.", arquivo);
        exit(0);
    }
    return fp;
}

int buscarQuantidadePagina()
{
    long addr;
    fseek(indice, 0, SEEK_END);
    addr = ftell(indice) - sizeof(int);

    return ((int)addr / PAGESIZE);
}

// Lê a página e procura pela chave a ser inserida
void lerPagina(int rrn, BTPAGE *pagina)
{
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    fread(pagina, sizeof(BTPAGE), 1, indice);
}

void escrevePagina(int rrn, BTPAGE *pagina)
{
    long addr;
    addr = (long)rrn * (long)PAGESIZE + sizeof(int);
    fseek(indice, addr, SEEK_SET);
    fwrite(pagina, sizeof(BTPAGE), 1, indice);
}

void atualizarIndiceComNovaRaiz(int rrnRaiz)
{
    fseek(indice, 0, SEEK_SET);
    fwrite(&rrnRaiz, sizeof(int), 1, indice);
}

CHAVE_PAGINA criaNoKey()
{
    CHAVE_PAGINA noKey;
    CHAVE_PRIMARIA nullKey;
    char nullableKey[12] = "@@@@@@@@@@@";
    char nullableKey2[8] = "#######";
    strcpy(nullKey.ID_aluno, nullableKey);
    strcpy(nullKey.sigla_disc, nullableKey2);

    noKey.id = nullKey;
    noKey.rrn = -1;

    return noKey;
}

int main()
{
    FILE *arquivo;
    arquivo = verificaArquivo("insere.bin");
    REGISTRO registrosInseridos[10];
    fread(registrosInseridos, sizeof(REGISTRO), 10, arquivo);
    fclose(arquivo);

    arquivo = verificaArquivo("busca.bin");
    CHAVE_PRIMARIA buscaIndice[10];
    fread(buscaIndice, sizeof(CHAVE_PRIMARIA), 10, arquivo);
    fclose(arquivo);

    resultado = fopen("resultado.bin", "a+b");
    BTPAGE pagina;
    int rrnRaiz;

    fclose(resultado);

    if ((indice = fopen("indice.bin", "r+b")) > 0)
    {
        rrnRaiz = recuperarRrnRaiz();
    }
    else
    {
        rrnRaiz = criarArvore();
    }
    fclose(indice);

    int opcao, i;

    do
    {
        printf("\nSelecione uma das opções abaixo:\n\n");
        printf("1- Inserir um registro.\n");
        printf("2- Buscar por um registro.\n");
        printf("3- Listar todos os registros.\n");
        printf("0- Sair.\n");
        printf("Opção: ");
        scanf("%d", &opcao);

        switch (opcao)
        {
        case 1:
            while (1)
            {
                printf("\nDigite 0 para retornar ao menu principal.");
                printf("\nInforme um número de 1 a 10: ");
                scanf("%d", &i);

                if (i == 0)
                    break;
                if (i > 10)
                {
                    printf("Opção inválida!");
                }
                else
                {
                    bool foiPromovido;
                    int rrnPromovido;
                    CHAVE_PRIMARIA chavePrimaria;
                    strcpy(chavePrimaria.ID_aluno, registrosInseridos[i - 1].ID_aluno);
                    strcpy(chavePrimaria.sigla_disc, registrosInseridos[i - 1].sigla_disc);
                    CHAVE_PAGINA chavePromovida, chave;
                    chave.id = chavePrimaria;

                    indice = verificaArquivo("indice.bin");
                    int offSet = buscaRegistroNaArvore(rrnRaiz, chave);

                    if (offSet == (-1))
                    {
                        resultado = verificaArquivo("resultado.bin");
                        int offSet = insereRegistro(registrosInseridos[i - 1]);
                        fclose(resultado);
                        chave.rrn = offSet;

                        foiPromovido = inserirNaArvore(rrnRaiz, chave, &rrnPromovido, &chavePromovida);
                        if (foiPromovido)
                            rrnRaiz = criarRaiz(chavePromovida, rrnRaiz, rrnPromovido);
                    }
                    else
                    {
                        printf("+----- Chave duplicada: <%s%s> -----+\n", chave.id.ID_aluno, chave.id.sigla_disc);
                    }
                    fclose(indice);
                }
            }
            break;

        /* Busca */
        case 2:
            while (1)
            {
                printf("\nDigite 0 para retornar ao menu principal.");
                printf("\nInforme um número de 1 a 4: ");
                scanf("%d", &i);

                if (i == 0)
                    break;
                if (i < 1 || i > 4)
                {
                    printf("Opção inválida!");
                }
                else
                {
                    printf("\nAcessando o arquivo com chaves para busca...\n");

                    CHAVE_PRIMARIA chavePrimaria;
                    strcpy(chavePrimaria.ID_aluno, buscaIndice[i - 1].ID_aluno);
                    strcpy(chavePrimaria.sigla_disc, buscaIndice[i - 1].sigla_disc);
                    CHAVE_PAGINA chavePagina;
                    chavePagina.id = chavePrimaria;

                    indice = verificaArquivo("indice.bin");

                    int offSet = buscaRegistroNaArvore(rrnRaiz, chavePagina);
                    fclose(indice);

                    if (offSet != (-1))
                    {
                        resultado = verificaArquivo("resultado.bin");
                        buscaRegistroRRN(offSet, resultado);
                        fclose(resultado);
                    }
                    else
                    {
                        printf("Chave não encontrada no índice!\n");
                    }
                }
            }
            break;

        case 3:
            printf("\nListando todos os alunos...\n\n");
            indice = verificaArquivo("indice.bin");
            resultado = verificaArquivo("resultado.bin");
            imprimeArvoreEmOrdem(recuperarRrnRaiz());
            printf("\n");
            fclose(resultado);
            fclose(indice);
            break;

        case 0:
            printf("\nSaindo do programa...\n\n");
            break;

        default:
            break;
        }
    } while (opcao != 0);
}
