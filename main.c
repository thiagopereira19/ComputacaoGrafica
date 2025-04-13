#include <stdio.h>
#include <stdlib.h>

// Função para remover a extensão do nome do arquivo
void remove_extensao(char *destino, const char *origem) {
    int i = 0;
    while (origem[i] != '\0' && origem[i] != '.') {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

// Função para comparar duas strings
int compara_strings(const char *a, const char *b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i])
            return 0; // São diferentes
        i++;
    }
    return a[i] == b[i]; // Iguais se ambas terminam
}

// Função para ignorar comentários no cabeçalho
void ignora_comentarios(FILE *file) {
    char c = fgetc(file);
    while (c == '#') { // Ignora linhas começando com #
        while (c != '\n') c = fgetc(file);
        c = fgetc(file);
    }
    ungetc(c, file); // Retorna o último caractere lido para o stream
}

int main() {
    char nome_arquivo[100], nome_base[100];
    FILE *input, *saida1, *saida2, *saida3;
    int largura, altura, max_valor;

    // Entrada do nome do arquivo
    printf("Digite o nome da imagem PPM (formato P3): ");
    scanf("%s", nome_arquivo);

    remove_extensao(nome_base, nome_arquivo);

    input = fopen(nome_arquivo, "r");
    if (!input) {
        printf("Erro ao abrir %s\n", nome_arquivo);
        return 1;
    }

    // Lê o cabeçalho e ignora comentários
    char formato[3];
    fscanf(input, "%s", formato);
    if (!compara_strings(formato, "P3")) {
        printf("Formato inválido. Esperado P3.\n");
        fclose(input);
        return 1;
    }

    ignora_comentarios(input);
    fscanf(input, "%d %d", &largura, &altura);

    ignora_comentarios(input);
    fscanf(input, "%d", &max_valor);

    // Aloca dinamicamente as matrizes
    int **r = malloc(altura * sizeof(int *));
    int **g = malloc(altura * sizeof(int *));
    int **b = malloc(altura * sizeof(int *));
    for (int i = 0; i < altura; i++) {
        r[i] = malloc(largura * sizeof(int));
        g[i] = malloc(largura * sizeof(int));
        b[i] = malloc(largura * sizeof(int));
    }

    // Lê os pixels e armazena nas matrizes
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            fscanf(input, "%d %d %d", &r[i][j], &g[i][j], &b[i][j]);
        }
    }
    fclose(input);

    // Gera nomes dos arquivos de saída
    char nome1[110], nome2[110], nome3[110];
    snprintf(nome1, sizeof(nome1), "%s_1.pgm", nome_base);
    snprintf(nome2, sizeof(nome2), "%s_2.pgm", nome_base);
    snprintf(nome3, sizeof(nome3), "%s_3.ppm", nome_base);

    // Abre arquivos de saída
    saida1 = fopen(nome1, "w");
    saida2 = fopen(nome2, "w");
    saida3 = fopen(nome3, "w");

    if (!saida1 || !saida2 || !saida3) {
        printf("Erro ao criar arquivos de saída.\n");
        return 1;
    }

    // Cabeçalhos
    fprintf(saida1, "P2\n%d %d\n%d\n", largura, altura, max_valor);
    fprintf(saida2, "P2\n%d %d\n%d\n", largura, altura, max_valor);
    fprintf(saida3, "P3\n%d %d\n%d\n", largura, altura, max_valor);

    // Processa os pixels e escreve nos arquivos de saída
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            int media_simples = (r[i][j] + g[i][j] + b[i][j]) / 3;
            int media_ponderada = (int)(0.299 * r[i][j] + 0.587 * g[i][j] + 0.114 * b[i][j]);

            int neg_r = max_valor - r[i][j];
            int neg_g = max_valor - g[i][j];
            int neg_b = max_valor - b[i][j];

            fprintf(saida1, "%d\n", media_simples);
            fprintf(saida2, "%d\n", media_ponderada);
            fprintf(saida3, "%d %d %d\n", neg_r, neg_g, neg_b);
        }
    }

    fclose(saida1);
    fclose(saida2);
    fclose(saida3);

    // Libera memória alocada
    for (int i = 0; i < altura; i++) {
        free(r[i]);
        free(g[i]);
        free(b[i]);
    }
    free(r);
    free(g);
    free(b);

    printf("Conversão concluída com sucesso!\n");
    return 0;
}
