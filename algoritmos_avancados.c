#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// ============================================================================
///                       ESTRUTURAS USADAS NO JOGO
/// ============================================================================

/// ----------------------------
/// Árvore Binária (Salas)
/// ----------------------------
typedef struct Sala {
    char nome[50];
    struct Sala *esq;
    struct Sala *dir;
} Sala;

/// ----------------------------
/// Árvore de Busca (Pistas)
/// ----------------------------
typedef struct Pista {
    char texto[50];
    struct Pista *esq;
    struct Pista *dir;
} Pista;

/// ----------------------------
/// Tabela Hash (Suspeitos)
/// ----------------------------
typedef struct NoSuspeito {
    char suspeito[50];
    char pista[50];
    struct NoSuspeito *prox;
} NoSuspeito;

#define TAM_HASH 10
NoSuspeito *tabelaHash[TAM_HASH];


/// ============================================================================
///                       FUNÇÕES DO NÍVEL NOVATO
///             (MAPA DA MANSÃO — ÁRVORE BINÁRIA FIXA)
/// ============================================================================

Sala* criarSala(char nome[]) {
    Sala *n = (Sala*) malloc(sizeof(Sala));
    strcpy(n->nome, nome);
    n->esq = NULL;
    n->dir = NULL;
    return n;
}

void explorarSalas(Sala *atual) {
    char opc;

    while (1) {
        if (atual == NULL) {
            printf("Você chegou ao fim do caminho.\n");
            return;
        }

        printf("\nVocê está na sala: %s\n", atual->nome);
        printf("Escolha o caminho: (e) esquerda | (d) direita | (s) sair\n> ");
        scanf(" %c", &opc);

        if (opc == 'e') {
            atual = atual->esq;
        } else if (opc == 'd') {
            atual = atual->dir;
        } else if (opc == 's') {
            printf("Saindo da exploração...\n");
            return;
        } else {
            printf("Opção inválida.\n");
        }
    }
}


/// ============================================================================
///                     FUNÇÕES DO NÍVEL AVENTUREIRO
///                (PISTAS EM ÁRVORE DE BUSCA — BST)
/// ============================================================================

Pista* criarPista(char texto[]) {
    Pista *p = (Pista*) malloc(sizeof(Pista));
    strcpy(p->texto, texto);
    p->esq = NULL;
    p->dir = NULL;
    return p;
}

Pista* inserirPista(Pista *raiz, char texto[]) {
    if (raiz == NULL) return criarPista(texto);

    if (strcmp(texto, raiz->texto) < 0) {
        raiz->esq = inserirPista(raiz->esq, texto);
    } else if (strcmp(texto, raiz->texto) > 0) {
        raiz->dir = inserirPista(raiz->dir, texto);
    }
    return raiz;
}

void mostrarPistasEmOrdem(Pista *raiz) {
    if (raiz == NULL) return;

    mostrarPistasEmOrdem(raiz->esq);
    printf("- %s\n", raiz->texto);
    mostrarPistasEmOrdem(raiz->dir);
}


/// ============================================================================
///                     FUNÇÕES DO NÍVEL MESTRE (TABELA HASH)
/// ============================================================================

int hashFunc(char nome[]) {
    int soma = 0;
    for (int i = 0; nome[i] != '\0'; i++)
        soma += nome[i];
    return soma % TAM_HASH;
}

void inserirHash(char suspeito[], char pista[]) {
    int h = hashFunc(suspeito);

    NoSuspeito *novo = (NoSuspeito*) malloc(sizeof(NoSuspeito));
    strcpy(novo->suspeito, suspeito);
    strcpy(novo->pista, pista);
    novo->prox = tabelaHash[h];
    tabelaHash[h] = novo;
}

void listarHash() {
    printf("\n=== Relação de suspeitos e pistas ===\n");

    for (int i = 0; i < TAM_HASH; i++) {
        NoSuspeito *aux = tabelaHash[i];
        if (aux != NULL)
            printf("\nBucket %d:\n", i);

        while (aux != NULL) {
            printf("Suspeito: %s | Pista: %s\n", aux->suspeito, aux->pista);
            aux = aux->prox;
        }
    }
}

void suspeitoMaisCitado() {
    int contadores[50] = {0};
    char nomes[50][50];
    int usados = 0;

    for (int i = 0; i < TAM_HASH; i++) {
        NoSuspeito *aux = tabelaHash[i];
        while (aux != NULL) {

            int found = -1;
            for (int j = 0; j < usados; j++) {
                if (strcmp(nomes[j], aux->suspeito) == 0) {
                    found = j;
                    break;
                }
            }

            if (found == -1) {
                strcpy(nomes[usados], aux->suspeito);
                contadores[usados]++;
                usados++;
            } else {
                contadores[found]++;
            }

            aux = aux->prox;
        }
    }

    if (usados == 0) {
        printf("\nNenhum suspeito registrado ainda.\n");
        return;
    }

    int maior = 0;
    int id = 0;

    for (int i = 0; i < usados; i++) {
        if (contadores[i] > maior) {
            maior = contadores[i];
            id = i;
        }
    }

    printf("\nSuspeito mais citado: %s (%d pistas)\n",
           nomes[id], contadores[id]);
}


/// ============================================================================
///                           FUNÇÃO PRINCIPAL DO JOGO
/// ============================================================================

int main() {

    /// --------------------------
    /// 1) Criar mapa fixo da mansão (ÁRVORE BINÁRIA)
    /// --------------------------
    Sala *hall = criarSala("Hall de Entrada");
    hall->esq = criarSala("Biblioteca");
    hall->dir = criarSala("Cozinha");

    hall->esq->esq = criarSala("Sala de Mapas");
    hall->esq->dir = criarSala("Jardim Interno");

    hall->dir->esq = criarSala("Despensa");
    hall->dir->dir = criarSala("Sótão");

    /// --------------------------
    /// 2) Criar BST de pistas
    /// --------------------------
    Pista *raizPistas = NULL;

    /// --------------------------
    /// 3) Inicializar tabela hash
    /// --------------------------
    for (int i = 0; i < TAM_HASH; i++) tabelaHash[i] = NULL;


    int opc;
    char pistaTemp[50];
    char suspeitoTemp[50];

    while (1) {
        printf("\n=========== MENU DETECTIVE QUEST ===========\n");
        printf("1 - Explorar a mansão\n");
        printf("2 - Adicionar pista manualmente\n");
        printf("3 - Listar pistas coletadas (ordem alfabética)\n");
        printf("4 - Relacionar pista com suspeito\n");
        printf("5 - Ver todas relações pista → suspeito\n");
        printf("6 - Mostrar suspeito mais citado\n");
        printf("0 - Sair\n> ");
        scanf("%d", &opc);

        if (opc == 0) {
            printf("Saindo...\n");
            break;
        }

        switch(opc) {
            case 1:
                explorarSalas(hall);
                break;

            case 2:
                printf("Digite o nome da pista: ");
                scanf(" %[^\n]", pistaTemp);
                raizPistas = inserirPista(raizPistas, pistaTemp);
                break;

            case 3:
                printf("\n=== Pistas coletadas ===\n");
                mostrarPistasEmOrdem(raizPistas);
                break;

            case 4:
                printf("Nome da pista: ");
                scanf(" %[^\n]", pistaTemp);
                printf("Nome do suspeito: ");
                scanf(" %[^\n]", suspeitoTemp);
                inserirHash(suspeitoTemp, pistaTemp);
                break;

            case 5:
                listarHash();
                break;

            case 6:
                suspeitoMaisCitado();
                break;

            default:
                printf("Opção inválida.\n");
        }
    }

    return 0;
}
