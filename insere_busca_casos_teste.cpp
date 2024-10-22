#include<stdio.h>
#include<conio.h>
#include<string.h>

int main() {
    FILE *fd;
    
    //////////////////////////////   
    struct hist {
        char id_aluno[4];
        char sigla_disc[4];
        char nome_aluno[50];
        char nome_disc[50];
        float media;
        float freq;
    } vet[11] = {{"001", "111", "Nome-1", "Disc-111", 1, 1},
                {"001", "222", "Nome-2", "Disc-222", 2, 2},
                {"001", "333", "Nome-3", "Disc-333", 3, 3},
                {"001", "444", "Nome-4", "Disc-444", 4, 4},
                {"001", "555", "Nome-5", "Disc-555", 5, 5},
                {"001", "666", "Nome-6", "Disc-666", 6, 6},
                {"001", "777", "Nome-7", "Disc-777", 7, 7},
                {"001", "888", "Nome-8", "Disc-888", 8, 8},
                {"001", "999", "Nome-9", "Disc-999", 9, 9},
                {"002", "111", "Nome-10", "Disc-10", 10, 10},
				{"002", "111", "Nome-10", "Disc-10", 10, 10}};//duplicada
       
    fd = fopen("insere.bin", "w+b");
    fwrite(vet, sizeof(vet), 1, fd);
    fclose(fd);
    
    //////////////////////////////
	struct busca {
        char id_aluno[4];
        char sigla_disc[4];
    } vet_b[5] = {{"002", "111"},
                  {"001", "888"},
                  {"001", "444"},
				  {"001", "333"},
				  {"001", "000"}};//não encontrada
    
    fd = fopen("busca.bin", "w+b");
    fwrite(vet_b, sizeof(vet_b), 1, fd);
    fclose(fd);    
}

