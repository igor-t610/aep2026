/*
 * Sistema do PSE - Programa Saude na Escola (AEP Unicesumar)
 * Dados ficticios e coletivos. Nenhum dado individual de estudantes.
 *
 * Atores (conforme diagrama de casos de uso):
 *  - Coordenacao pedagogica: cadastrar, pesquisar, listar, gerar resumo
 *  - Professores: pesquisar, listar, atualizar situacoes
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_ACOES 100
#define TAM 50

#define PLANEJADA 1
#define REALIZADA 2
#define CANCELADA 3

typedef struct {
    char codigo[TAM];
    char escola[TAM];
    char tema[TAM];
    char data[TAM];
    char publico[TAM];
    char responsavel[TAM];
    int previstos;
    int efetivos;
    int situacao;
} Acao;

Acao acoes[MAX_ACOES];
int total = 0;

/* ---------- utilitarios de entrada ---------- */

void removerEnter(char *s) {
    s[strcspn(s, "\n")] = '\0';
}

int vazio(const char *s) {
    int i;
    for (i = 0; s[i] != '\0'; i++)
        if (!isspace((unsigned char)s[i])) return 0;
    return 1;
}

/* le um texto obrigatorio (nao aceita vazio) */
void lerTexto(const char *msg, char *dest) {
    do {
        printf("%s", msg);
        fgets(dest, TAM, stdin);
        removerEnter(dest);
        if (vazio(dest))
            printf("  [ERRO] Campo obrigatorio. Tente novamente.\n");
    } while (vazio(dest));
}

/* le um inteiro >= 0 (nao aceita negativo nem texto) */
int lerInteiro(const char *msg) {
    char linha[TAM];
    int n;
    while (1) {
        printf("%s", msg);
        fgets(linha, TAM, stdin);
        if (sscanf(linha, "%d", &n) != 1)
            printf("  [ERRO] Digite um numero valido.\n");
        else if (n < 0)
            printf("  [ERRO] A quantidade nao pode ser negativa.\n");
        else
            return n;
    }
}

/* le um inteiro entre min e max (para menus) */
int lerOpcao(const char *msg, int min, int max) {
    char linha[TAM];
    int n;
    while (1) {
        printf("%s", msg);
        fgets(linha, TAM, stdin);
        if (sscanf(linha, "%d", &n) == 1 && n >= min && n <= max)
            return n;
        printf("  [ERRO] Opcao inexistente. Escolha entre %d e %d.\n", min, max);
    }
}

/* ---------- utilitarios de texto ---------- */

void paraMinusculo(const char *origem, char *dest) {
    int i;
    for (i = 0; origem[i] != '\0'; i++)
        dest[i] = (char)tolower((unsigned char)origem[i]);
    dest[i] = '\0';
}

/* compara ignorando maiusculas/minusculas */
int iguais(const char *a, const char *b) {
    char x[TAM], y[TAM];
    paraMinusculo(a, x);
    paraMinusculo(b, y);
    return strcmp(x, y) == 0;
}

/* verifica se 'parte' aparece dentro de 'texto' */
int contem(const char *texto, const char *parte) {
    char x[TAM], y[TAM];
    paraMinusculo(texto, x);
    paraMinusculo(parte, y);
    return strstr(x, y) != NULL;
}

const char *nomeSituacao(int s) {
    if (s == PLANEJADA) return "Planejada";
    if (s == REALIZADA) return "Realizada";
    return "Cancelada";
}

/* ---------- funcoes do sistema ---------- */

int buscarPorCodigo(const char *codigo) {
    int i;
    for (i = 0; i < total; i++)
        if (iguais(acoes[i].codigo, codigo)) return i;
    return -1;
}

void exibirAcao(const Acao *a) {
    printf("------------------------------------------\n");
    printf("Codigo................: %s\n", a->codigo);
    printf("Escola................: %s\n", a->escola);
    printf("Tema..................: %s\n", a->tema);
    printf("Data prevista.........: %s\n", a->data);
    printf("Publico-alvo..........: %s\n", a->publico);
    printf("Responsavel...........: %s\n", a->responsavel);
    printf("Participantes previstos: %d\n", a->previstos);
    if (a->situacao == REALIZADA)
        printf("Participantes efetivos.: %d\n", a->efetivos);
    printf("Situacao..............: %s\n", nomeSituacao(a->situacao));
}

void cadastrarAcao(void) {
    Acao nova;
    char codigo[TAM];

    printf("\n=== CADASTRAR ACAO DO PSE ===\n");
    if (total >= MAX_ACOES) {
        printf("[ERRO] Limite de %d acoes atingido.\n", MAX_ACOES);
        return;
    }

    lerTexto("Codigo: ", codigo);
    if (buscarPorCodigo(codigo) != -1) {
        printf("[ERRO] Ja existe uma acao com o codigo '%s'.\n", codigo);
        return;
    }
    strcpy(nova.codigo, codigo);

    lerTexto("Escola: ", nova.escola);
    lerTexto("Tema: ", nova.tema);
    lerTexto("Data prevista (dd/mm/aaaa): ", nova.data);
    lerTexto("Publico-alvo (ex.: 6o ano): ", nova.publico);
    lerTexto("Responsavel: ", nova.responsavel);
    nova.previstos = lerInteiro("Quantidade prevista de participantes: ");

    printf("Situacao inicial:\n 1 - Planejada\n 2 - Realizada\n 3 - Cancelada\n");
    nova.situacao = lerOpcao("Escolha: ", 1, 3);

    nova.efetivos = 0;
    if (nova.situacao == REALIZADA)
        nova.efetivos = lerInteiro("Quantidade efetiva de participantes: ");

    acoes[total] = nova;
    total++;
    printf("\n[OK] Acao cadastrada com sucesso!\n");
}

void listarAcoes(void) {
    int i;
    printf("\n=== ACOES CADASTRADAS ===\n");
    if (total == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }
    for (i = 0; i < total; i++)
        exibirAcao(&acoes[i]);
    printf("------------------------------------------\n");
    printf("Total de acoes: %d\n", total);
}

void pesquisarAcoes(void) {
    int opcao, i, achou = 0;
    char termo[TAM];

    printf("\n=== PESQUISAR ACOES ===\n");
    printf(" 1 - Por codigo\n 2 - Por escola\n 3 - Por tema\n");
    opcao = lerOpcao("Escolha: ", 1, 3);
    lerTexto("Digite o termo de busca: ", termo);

    for (i = 0; i < total; i++) {
        int encontrou = 0;
        if (opcao == 1) encontrou = contem(acoes[i].codigo, termo);
        if (opcao == 2) encontrou = contem(acoes[i].escola, termo);
        if (opcao == 3) encontrou = contem(acoes[i].tema, termo);
        if (encontrou) {
            exibirAcao(&acoes[i]);
            achou++;
        }
    }
    if (achou == 0)
        printf("Nenhuma acao encontrada para '%s'.\n", termo);
    else
        printf("------------------------------------------\n%d acao(oes) encontrada(s).\n", achou);
}

void atualizarSituacao(void) {
    char codigo[TAM];
    int pos, nova;

    printf("\n=== ATUALIZAR SITUACAO ===\n");
    if (total == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }
    lerTexto("Codigo da acao: ", codigo);
    pos = buscarPorCodigo(codigo);
    if (pos == -1) {
        printf("[ERRO] Acao '%s' nao encontrada.\n", codigo);
        return;
    }

    printf("Situacao atual: %s\n", nomeSituacao(acoes[pos].situacao));
    printf("Nova situacao:\n 1 - Planejada\n 2 - Realizada\n 3 - Cancelada\n");
    nova = lerOpcao("Escolha: ", 1, 3);

    acoes[pos].situacao = nova;
    if (nova == REALIZADA)
        acoes[pos].efetivos = lerInteiro("Quantidade efetiva de participantes: ");
    else
        acoes[pos].efetivos = 0;

    printf("\n[OK] Situacao atualizada para %s.\n", nomeSituacao(nova));
}

void gerarResumo(void) {
    int i, planejadas = 0, realizadas = 0, canceladas = 0;
    int totalEfetivos = 0, previstosRealizadas = 0;
    float percentual = 0.0f;

    printf("\n=== RESUMO GERAL ===\n");
    for (i = 0; i < total; i++) {
        if (acoes[i].situacao == PLANEJADA) planejadas++;
        else if (acoes[i].situacao == REALIZADA) {
            realizadas++;
            totalEfetivos += acoes[i].efetivos;
            previstosRealizadas += acoes[i].previstos;
        } else canceladas++;
    }
    if (previstosRealizadas > 0)
        percentual = (float)totalEfetivos * 100.0f / previstosRealizadas;

    printf("Acoes planejadas.................: %d\n", planejadas);
    printf("Acoes realizadas.................: %d\n", realizadas);
    printf("Acoes canceladas.................: %d\n", canceladas);
    printf("Total de participantes (efetivos): %d\n", totalEfetivos);
    printf("Participacao em relacao ao previsto (acoes realizadas): %.2f%%\n", percentual);
}

/* ---------- menus por perfil ---------- */

int escolherPerfil(void) {
    printf("\n=== SISTEMA DO PSE ===\n");
    printf("Quem esta usando o sistema?\n");
    printf(" 1 - Coordenacao pedagogica\n");
    printf(" 2 - Professor(a)\n");
    printf(" 0 - Encerrar programa\n");
    return lerOpcao("Escolha: ", 0, 2);
}

void menuCoordenacao(void) {
    int op;
    do {
        printf("\n--- MENU: COORDENACAO PEDAGOGICA ---\n");
        printf(" 1 - Cadastrar acao do PSE\n");
        printf(" 2 - Pesquisar acoes (codigo/escola/tema)\n");
        printf(" 3 - Listar acoes cadastradas\n");
        printf(" 4 - Gerar resumo geral\n");
        printf(" 0 - Voltar\n");
        op = lerOpcao("Escolha: ", 0, 4);
        switch (op) {
            case 1: cadastrarAcao(); break;
            case 2: pesquisarAcoes(); break;
            case 3: listarAcoes(); break;
            case 4: gerarResumo(); break;
        }
    } while (op != 0);
}

void menuProfessor(void) {
    int op;
    do {
        printf("\n--- MENU: PROFESSORES ---\n");
        printf(" 1 - Pesquisar acoes (codigo/escola/tema)\n");
        printf(" 2 - Listar acoes cadastradas\n");
        printf(" 3 - Atualizar situacao de uma acao\n");
        printf(" 0 - Voltar\n");
        op = lerOpcao("Escolha: ", 0, 3);
        switch (op) {
            case 1: pesquisarAcoes(); break;
            case 2: listarAcoes(); break;
            case 3: atualizarSituacao(); break;
        }
    } while (op != 0);
}

int main(void) {
    int perfil;
    do {
        perfil = escolherPerfil();
        if (perfil == 1) menuCoordenacao();
        else if (perfil == 2) menuProfessor();
    } while (perfil != 0);

    printf("\nPrograma encerrado. Ate logo!\n");
    return 0;
}
