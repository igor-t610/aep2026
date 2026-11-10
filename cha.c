#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ACOES 100
#define TAM_TEXTO 80
#define TAM_CODIGO 20

typedef struct {
    char codigo[TAM_CODIGO];
    char escola[TAM_TEXTO];
    char tema[TAM_TEXTO];
    char data[11];
    char publico[TAM_TEXTO];
    char responsavel[TAM_TEXTO];
    int participantesPrevistos;
    int participantesEfetivos;
    int situacao; /* 1 = planejada, 2 = realizada, 3 = cancelada */
} Acao;

void removerNovaLinha(char texto[]) {
    texto[strcspn(texto, "\n")] = '\0';
}

void lerTextoObrigatorio(const char *mensagem, char destino[], int tamanho) {
    do {
        printf("%s", mensagem);
        fgets(destino, tamanho, stdin);
        removerNovaLinha(destino);
        if (strlen(destino) == 0)
            printf("Erro: este campo e obrigatorio.\n");
    } while (strlen(destino) == 0);
}

int lerInteiroMinimo(const char *mensagem, int minimo) {
    char entrada[50], *fim;
    long valor;
    do {
        printf("%s", mensagem);
        fgets(entrada, sizeof(entrada), stdin);
        valor = strtol(entrada, &fim, 10);
        while (isspace((unsigned char)*fim)) fim++;
        if (*fim != '\0' || valor < minimo || valor > 1000000)
            printf("Erro: informe um numero inteiro maior ou igual a %d.\n", minimo);
        else
            return (int)valor;
    } while (1);
}

const char *nomeSituacao(int situacao) {
    if (situacao == 1) return "Planejada";
    if (situacao == 2) return "Realizada";
    return "Cancelada";
}

int buscarPorCodigo(Acao acoes[], int total, const char codigo[]) {
    int i;
    for (i = 0; i < total; i++)
        if (strcmp(acoes[i].codigo, codigo) == 0) return i;
    return -1;
}

void exibirAcao(const Acao *acao) {
    printf("\nCodigo: %s\nEscola: %s\nTema: %s\nData prevista: %s\n",
           acao->codigo, acao->escola, acao->tema, acao->data);
    printf("Publico-alvo: %s\nResponsavel: %s\nPrevistos: %d\n",
           acao->publico, acao->responsavel, acao->participantesPrevistos);
    printf("Situacao: %s\n", nomeSituacao(acao->situacao));
    if (acao->situacao == 2)
        printf("Participantes efetivos: %d\n", acao->participantesEfetivos);
}

void cadastrarAcao(Acao acoes[], int *total) {
    Acao nova;
    if (*total == MAX_ACOES) {
        printf("Limite de %d acoes atingido.\n", MAX_ACOES);
        return;
    }
    do {
        lerTextoObrigatorio("Codigo: ", nova.codigo, TAM_CODIGO);
        if (buscarPorCodigo(acoes, *total, nova.codigo) != -1)
            printf("Erro: ja existe uma acao com esse codigo.\n");
    } while (buscarPorCodigo(acoes, *total, nova.codigo) != -1);

    lerTextoObrigatorio("Escola: ", nova.escola, TAM_TEXTO);
    lerTextoObrigatorio("Tema: ", nova.tema, TAM_TEXTO);
    lerTextoObrigatorio("Data prevista (dd/mm/aaaa): ", nova.data, sizeof(nova.data));
    lerTextoObrigatorio("Publico-alvo: ", nova.publico, TAM_TEXTO);
    lerTextoObrigatorio("Responsavel: ", nova.responsavel, TAM_TEXTO);
    nova.participantesPrevistos = lerInteiroMinimo("Quantidade prevista de participantes: ", 0);
    nova.participantesEfetivos = 0;
    nova.situacao = 1;
    acoes[*total] = nova;
    (*total)++;
    printf("Acao cadastrada com sucesso. Situacao inicial: Planejada.\n");
}

void listarAcoes(Acao acoes[], int total) {
    int i;
    if (total == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }
    printf("\n========== ACOES CADASTRADAS ==========" "\n");
    for (i = 0; i < total; i++) {
        printf("\n--- Acao %d ---", i + 1);
        exibirAcao(&acoes[i]);
    }
}

int contemIgnoreCase(const char texto[], const char termo[]) {
    char a[TAM_TEXTO], b[TAM_TEXTO];
    int i;
    strncpy(a, texto, TAM_TEXTO - 1); a[TAM_TEXTO - 1] = '\0';
    strncpy(b, termo, TAM_TEXTO - 1); b[TAM_TEXTO - 1] = '\0';
    for (i = 0; a[i]; i++) a[i] = (char)tolower((unsigned char)a[i]);
    for (i = 0; b[i]; i++) b[i] = (char)tolower((unsigned char)b[i]);
    return strstr(a, b) != NULL;
}

void pesquisarAcoes(Acao acoes[], int total) {
    int opcao, i, encontrou = 0;
    char termo[TAM_TEXTO];
    if (total == 0) { printf("Nenhuma acao cadastrada.\n"); return; }
    printf("1 - Codigo\n2 - Escola\n3 - Tema\n");
    opcao = lerInteiroMinimo("Pesquisar por: ", 1);
    if (opcao < 1 || opcao > 3) { printf("Erro: opcao inexistente.\n"); return; }
    lerTextoObrigatorio("Digite o termo: ", termo, TAM_TEXTO);
    for (i = 0; i < total; i++) {
        if ((opcao == 1 && contemIgnoreCase(acoes[i].codigo, termo)) ||
            (opcao == 2 && contemIgnoreCase(acoes[i].escola, termo)) ||
            (opcao == 3 && contemIgnoreCase(acoes[i].tema, termo))) {
            exibirAcao(&acoes[i]);
            encontrou = 1;
        }
    }
    if (!encontrou) printf("Nenhuma acao encontrada.\n");
}

void atualizarSituacao(Acao acoes[], int total) {
    char codigo[TAM_CODIGO];
    int indice, situacao;
    if (total == 0) { printf("Nenhuma acao cadastrada.\n"); return; }
    lerTextoObrigatorio("Informe o codigo da acao: ", codigo, TAM_CODIGO);
    indice = buscarPorCodigo(acoes, total, codigo);
    if (indice == -1) { printf("Erro: codigo nao encontrado.\n"); return; }
    printf("1 - Planejada\n2 - Realizada\n3 - Cancelada\n");
    situacao = lerInteiroMinimo("Nova situacao: ", 1);
    if (situacao < 1 || situacao > 3) { printf("Erro: opcao inexistente.\n"); return; }
    acoes[indice].situacao = situacao;
    if (situacao == 2)
        acoes[indice].participantesEfetivos = lerInteiroMinimo("Quantidade efetiva de participantes: ", 0);
    else
        acoes[indice].participantesEfetivos = 0;
    printf("Situacao atualizada com sucesso.\n");
}

void gerarResumo(Acao acoes[], int total) {
    int i, planejadas = 0, realizadas = 0, canceladas = 0;
    int previstos = 0, efetivos = 0;
    float percentual = 0;
    for (i = 0; i < total; i++) {
        if (acoes[i].situacao == 1) planejadas++;
        else if (acoes[i].situacao == 2) { realizadas++; efetivos += acoes[i].participantesEfetivos; }
        else canceladas++;
        previstos += acoes[i].participantesPrevistos;
    }
    if (previstos > 0) percentual = (efetivos * 100.0f) / previstos;
    printf("\n========== RESUMO GERAL ==========" "\n");
    printf("Acoes planejadas: %d\nAcoes realizadas: %d\nAcoes canceladas: %d\n", planejadas, realizadas, canceladas);
    printf("Participantes previstos: %d\nTotal de participantes efetivos: %d\n", previstos, efetivos);
    printf("Percentual de participacao: %.2f%%\n", percentual);
}

int main(void) {
    Acao acoes[MAX_ACOES];
    int total = 0, opcao;
    do {
        printf("\n===== SISTEMA DO PSE =====\n");
        printf("1 - Cadastrar acao\n2 - Listar acoes\n3 - Pesquisar acoes\n");
        printf("4 - Atualizar situacao\n5 - Gerar resumo geral\n0 - Encerrar\n");
        opcao = lerInteiroMinimo("Escolha uma opcao: ", 0);
        switch (opcao) {
            case 1: cadastrarAcao(acoes, &total); break;
            case 2: listarAcoes(acoes, total); break;
            case 3: pesquisarAcoes(acoes, total); break;
            case 4: atualizarSituacao(acoes, total); break;
            case 5: gerarResumo(acoes, total); break;
            case 0: printf("Sistema encerrado.\n"); break;
            default: printf("Erro: opcao inexistente.\n");
        }
    } while (opcao != 0);
    return 0;
}
