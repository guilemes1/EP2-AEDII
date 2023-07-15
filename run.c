#include "headers.h"
// #include "implementation.c"
int main(int argc, char const *argv[])
{
    bTree* tree = createTree("tree.dat",false);
	FILE* biblioteca = fopen("data.dat", "wb+");

	char operacao[3];
	char linha[200];
	int posicao = 0;

	while(strcmp(operacao,"F")){

		printf("Digite a operação que deseja realizar:\n");
		fgets(linha, sizeof(linha), stdin);
		sscanf(linha, "%2s ", operacao);
		printf("\n");

		if(!strcmp(operacao,"I")){

			recordNode* record = malloc(sizeof(recordNode));

			int codigoLivro; // chave
			char titulo[30];
			char nomeCompletoPrimeiroAutor[30];
			int anoPublicacao;
			sscanf(linha, "%2s %d;%[^;];%[^;];%d", operacao, &codigoLivro, titulo, nomeCompletoPrimeiroAutor, &anoPublicacao);
			enterData(record, codigoLivro, titulo, nomeCompletoPrimeiroAutor, anoPublicacao);

			insert(tree, record, &posicao);   //Insere na arvore / estrutura de indices		
			insertBook(biblioteca, record);

			printf(", record->codigoLivro);
			
		}

		if(!strcmp(operacao,"R")){
			int codigoLivro;
			sscanf(linha, "%2s %d", operacao, &codigoLivro);

			bool res = removeFromTree(tree,codigoLivro);
			removeBook(biblioteca, codigoLivro);

			if(res)
			{

				printf("O livro com o codigo %d foi removido da biblioteca\n\n", codigoLivro);
			}
			else
			{
				printf("Deletion not successful.\n\n");
			}
		}


		if(!strcmp(operacao,"B")){

			int codigoLivro;
			sscanf(linha, "%2s %d", operacao, &codigoLivro);
		    //recordNode* res = search(tree,codigoLivro);

			recordNode* result = searchBook(biblioteca, codigoLivro);
			
			if(result != NULL) {
				// printf("codigo\ttitulo\tautor\tano\n");
				printf("%d\t",result->codigoLivro);
				printf("%s\t",result->titulo);
				printf("%s\t",result->nomeCompletoPrimeiroAutor);
				printf("%d\t\n\n",result->anoPublicacao);

				//free(res);
			} else
				printf("O livro com codigo %d nao existe na biblioteca\n\n", codigoLivro);
		}

		if(!strcmp(operacao,"P1")){
			traverse(tree, tree->root);
		}

		if(!strcmp(operacao,"P2")){
			printTree(tree);
		}

		if(!strcmp(operacao,"P3")){

			recordNode* record = malloc(sizeof(recordNode));

			fseek(biblioteca, 0, SEEK_SET); // MOVE O PONTEIRO PARA O INICIO DO ARQUIVO

			while (fread(record, sizeof(recordNode), 1, biblioteca) == 1) {

        		// printf("Valid: %d\n", record->valid);
        		// printf("Codigo livro: %d\n", record->codigoLivro);
				// printf("Titulo: %s\n", record->titulo);
				// printf("Nome Autor: %s\n", record->nomeCompletoPrimeiroAutor);
				// printf("Ano de publicacao: %d\n", record->anoPublicacao);

				printf("%d\t",record->valid);
				printf("%d\t",record->codigoLivro);
				printf("%s\t",record->titulo);
				printf("%s\t",record->nomeCompletoPrimeiroAutor);
				printf("%d\t",record->anoPublicacao);
        
        		printf("\n"); // Adicionar uma linha em branco entre os registros
    		}
			printf("\n");
		}

		if(!strcmp(operacao,"F")){
			printf("Execução encerrada\n");
			free(tree);
			free(biblioteca);
			//fclose(biblioteca);
		}

	}	       
	
}	
