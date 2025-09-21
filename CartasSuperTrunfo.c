// supertrunfo.c
// Um arquivo só.
// Salva ./cartas.json e mostra carta ASCII após cada cadastro.

// Bibliotecas:
// <stdio.h> para printf/puts/scanf/fopen
// <stdlib.h> para exit (não usamos malloc)
// <string.h> para strlen, snprintf
// <ctype.h> para toupper (aceitar 'a' e 'A')

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR     128
#define MAX_CARTAS  1000

static const char *REGIAO[] = {
    "Sudeste","Sudeste","Sul","Sul","Nordeste","Nordeste","Centro-Oeste","Centro-Oeste"
};
static const char *UF[] = {
    "SP","MG","SC","PR","BA","CE","MT","GO"
};
static const char *ESTADO[] = {
    "São Paulo","Minas Gerais","Santa Catarina","Paraná","Bahia","Ceará","Mato Grosso","Goiás"
};
static const char LETRA[] = { 'A','B','C','D','E','F','G','H' };
static const int N_ESTADOS = 8;

static char  V_letra[MAX_CARTAS];
static char  V_codigo[MAX_CARTAS][9];
static char  V_cidade[MAX_CARTAS][MAX_STR];
static long long V_populacao[MAX_CARTAS];
static double V_area[MAX_CARTAS];
static double V_pib[MAX_CARTAS];
static int    V_pontos[MAX_CARTAS];
static int CONT_POR_ESTADO[256] = {0};

static void mostrar_tabela_estados(void) {
    puts("\n=== Tabela de Estados (Letra → Região / UF / Estado) ===");
    puts("Letra | Região        | UF | Estado");
    puts("------+---------------+----+-------------------");
    for (int i = 0; i < N_ESTADOS; i++) {
        printf("  %c   | %-13s | %-2s | %s\n",
               LETRA[i], REGIAO[i], UF[i], ESTADO[i]);
    }
    puts("---------------------------------------------------------\n");
}

static int idx_por_letra(char L) {
    for (int i = 0; i < N_ESTADOS; i++)
        if (LETRA[i] == L) return i;
    return -1;
}

static void salvar_json(int n) {
    FILE *f = fopen("./cartas.json", "w");
    if (!f) { perror("Erro abrindo ./cartas.json"); return; }

    fprintf(f, "{\n  \"cartas\": [\n");
    for (int i = 0; i < n; i++) {
        fprintf(f,
          "    {\n"
          "      \"estado_letra\": \"%c\",\n"
          "      \"codigo\": \"%s\",\n"
          "      \"cidade\": \"%s\",\n"
          "      \"populacao\": %lld,\n"
          "      \"area_km2\": %.2f,\n"
          "      \"pib_bilhoes\": %.2f,\n"
          "      \"pontos_turisticos\": %d\n"
          "    }%s\n",
          V_letra[i], V_codigo[i], V_cidade[i],
          V_populacao[i], V_area[i], V_pib[i], V_pontos[i],
          (i == n - 1) ? "" : ",");
    }
    fprintf(f, "  ]\n}\n");
    fclose(f);
}

static void imprimir_carta_ascii(int i) {
    int idx = idx_por_letra(V_letra[i]);
    const char *uf = (idx >= 0) ? UF[idx] : "--";
    const char *estado = (idx >= 0) ? ESTADO[idx] : "Desconhecido";
    const char *regiao = (idx >= 0) ? REGIAO[idx] : "--";

    puts("+------------------------------------------+");
    printf("| SUPER TRUNFO %-26s |\n", "");
    puts("|------------------------------------------|");
    printf("| Código: %-8s  Estado: %-14s |\n", V_codigo[i], estado);
    printf("| UF: %-3s  Região: %-20s |\n", uf, regiao);
    printf("| Cidade: %-30s |\n", V_cidade[i]);
    puts("|------------------------------------------|");
    printf("| População: %10lld hab            |\n", V_populacao[i]);
    printf("| Área:       %10.2f km^2           |\n", V_area[i]);
    printf("| PIB:        %10.2f R$ bi          |\n", V_pib[i]);
    printf("| Pontos Tur.: %6d                   |\n", V_pontos[i]);
    puts("+------------------------------------------+\n");
}

int main(void) {
    int n = 0;

    puts("=== Super Trunfo — KISS com validador ===");
    puts("Use ponto para decimais (ex.: 1200.25).");
    puts("Digite Q na letra do estado para sair.\n");

    for (;;) {
        if (n >= MAX_CARTAS) {
            puts("Limite máximo atingido. Salvando e saindo.");
            break;
        }

        mostrar_tabela_estados();
        // Estado (letra)
        char entrada[64];
        char letra;
        int idxEstado;
        while (1) {
            printf("Estado (letra A-H, ou Q para sair): ");
            if (!fgets(entrada, sizeof(entrada), stdin)) return 0;
            letra = entrada[0];
            if (letra == 'q' || letra == 'Q') goto fim;
            letra = (char)toupper((unsigned char)letra);
            idxEstado = idx_por_letra(letra);
            if (idxEstado < 0) {
                puts("Letra inválida. Use A-H ou Q para sair.\n");
                continue;
            }
            break;
        }

        // Cidade
        printf("Nome da cidade: ");
        if (!fgets(V_cidade[n], sizeof(V_cidade[n]), stdin)) break;
        size_t k = strlen(V_cidade[n]);
        if (k && V_cidade[n][k-1] == '\n') V_cidade[n][k-1] = '\0';

        // População
        printf("População (inteiro): ");
        scanf("%lld", &V_populacao[n]);

        // Área
        printf("Área (km^2): ");
        scanf("%lf", &V_area[n]);

        // PIB
        printf("PIB (R$ bilhões): ");
        scanf("%lf", &V_pib[n]);

        // Pontos turísticos
        printf("Número de Pontos Turísticos: ");
        scanf("%d", &V_pontos[n]);

        // limpa \n que sobrou do scanf
        int ch; while ((ch = getchar()) != '\n' && ch != EOF) {}

        // Gera código <LETRA>-<NN>
        V_letra[n] = letra;
        CONT_POR_ESTADO[(unsigned char)letra]++;
        int seq = CONT_POR_ESTADO[(unsigned char)letra];
        snprintf(V_codigo[n], sizeof(V_codigo[n]), "%c-%02d", letra, seq);

        printf("Cadastro da carta (%s) feito.\n", V_codigo[n]);

        // Carta em ASCII
        imprimir_carta_ascii(n);

        // Salva ./cartas.json
        salvar_json(n + 1);
        puts("Arquivo atualizado: ./cartas.json\n");

        n++;
    }

fim:
    salvar_json(n);
    printf("Total de cartas: %d. Saindo.\n", n);
    return 0;
}
