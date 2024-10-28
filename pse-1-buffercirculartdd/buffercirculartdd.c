#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int total_testes = 0;
int total_falhas = 0;
int total_sucessos = 0;

// testes
#define verificar(mensagem, condicao) if (!(condicao)) return mensagem
#define rodar_teste(teste) { char *mensagem = teste(); total_testes++; if (mensagem) { \
    total_falhas++; printf("Falha no teste: %s\n", mensagem); } else { total_sucessos++; } }

// buffer estrutura
typedef struct {
    int *memoria;
    int tamanho_maximo;
    int elementos;
    int entrada;
    int saida;
    bool cheio;
    bool vazio;
} BufferCircular;

// inicialia buffer circular 
BufferCircular* inicializar_buffer(int capacidade) {
    BufferCircular *buffer = (BufferCircular *)malloc(sizeof(BufferCircular));
    buffer->memoria = (int *)calloc(capacidade, sizeof(int));
    buffer->tamanho_maximo = capacidade;
    buffer->elementos = 0;
    buffer->entrada = 0;
    buffer->saida = 0;
    buffer->cheio = false;
    buffer->vazio = true;
    return buffer;
}

// libera memória
void liberar_buffer(BufferCircular *buffer) {
    free(buffer->memoria);
    free(buffer);
}

// verifica cheio 
bool buffer_esta_cheio(BufferCircular *buffer) {
    return buffer->cheio;
}

// verifica vazio 
bool buffer_esta_vazio(BufferCircular *buffer) {
    return buffer->vazio;
}

// adiciona valor
void adicionar_ao_buffer(BufferCircular *buffer, int valor) {
    if (!buffer_esta_cheio(buffer)) {
        buffer->memoria[buffer->entrada] = valor;
        buffer->entrada = (buffer->entrada + 1) % buffer->tamanho_maximo;
        buffer->elementos++;
        buffer->vazio = false;

        if (buffer->entrada == buffer->saida) {
            buffer->cheio = true;
        }
    } else {
        printf("Buffer cheio! Não é possível adicionar.\n");
    }
}

// remove valor 
int remover_do_buffer(BufferCircular *buffer) {
    if (!buffer_esta_vazio(buffer)) {
        int valor = buffer->memoria[buffer->saida];
        buffer->saida = (buffer->saida + 1) % buffer->tamanho_maximo;
        buffer->elementos--;
        buffer->cheio = false;

        if (buffer->entrada == buffer->saida) {
            buffer->vazio = true;
        }
        return valor;
    } else {
        printf("Buffer vazio! Nada para remover.\n");
        return -999;
    }
}

// teste inicialização
char* teste_inicializacao() {
    BufferCircular *buffer = inicializar_buffer(5);
    verificar("Erro ao inicializar buffer", buffer != NULL);
    verificar("Erro: capacidade do buffer incorreta", buffer->tamanho_maximo == 5);
    verificar("Erro: buffer não está vazio após inicialização", buffer_esta_vazio(buffer));
    liberar_buffer(buffer);
    return 0;
}

// teste adicionar
char* teste_adicionar() {
    BufferCircular *buffer = inicializar_buffer(3);
    adicionar_ao_buffer(buffer, 10);
    verificar("Erro: valor não adicionado corretamente", buffer->memoria[0] == 10);
    adicionar_ao_buffer(buffer, 20);
    verificar("Erro: segundo valor não adicionado", buffer->memoria[1] == 20);
    adicionar_ao_buffer(buffer, 30);
    verificar("Erro: buffer não detectou estar cheio", buffer_esta_cheio(buffer));
    liberar_buffer(buffer);
    return 0;
}

// teste remover 
char* teste_remover() {
    BufferCircular *buffer = inicializar_buffer(3);
    adicionar_ao_buffer(buffer, 100);
    adicionar_ao_buffer(buffer, 200);
    int valor = remover_do_buffer(buffer);
    verificar("Erro: valor removido incorreto", valor == 100);
    verificar("Erro: segundo valor removido incorreto", remover_do_buffer(buffer) == 200);
    verificar("Erro: remoção de buffer vazio falhou", remover_do_buffer(buffer) == -999);
    liberar_buffer(buffer);
    return 0;
}

// teste verifica buffer cheio e vazio 
char* teste_cheio_vazio() {
    BufferCircular *buffer = inicializar_buffer(2);
    verificar("Erro: buffer deveria estar vazio", buffer_esta_vazio(buffer));
    adicionar_ao_buffer(buffer, 1);
    verificar("Erro: buffer não deveria estar vazio", !buffer_esta_vazio(buffer));
    adicionar_ao_buffer(buffer, 2);
    verificar("Erro: buffer deveria estar cheio", buffer_esta_cheio(buffer));
    remover_do_buffer(buffer);
    verificar("Erro: buffer não deveria estar cheio", !buffer_esta_cheio(buffer));
    liberar_buffer(buffer);
    return 0;
}

// todos os testes 
char* executar_testes() {
    rodar_teste(teste_inicializacao);
    rodar_teste(teste_adicionar);
    rodar_teste(teste_remover);
    rodar_teste(teste_cheio_vazio);
    return 0;
}


int main(void) {
    printf("Iniciando a execução dos testes...\n\n");

    char *resultado = executar_testes();

    if (resultado != 0) {
        printf("Houve falhas nos testes.\n");
    } else {
        printf("Todos os testes passaram com sucesso!\n");
    }

    printf("\nRelatório Final:\n");
    printf("Total de testes executados: %d\n", total_testes);
    printf("Total de testes com sucesso: %d\n", total_sucessos);
    printf("Total de falhas: %d\n", total_falhas);

    return (total_falhas > 0) ? 1 : 0;
}
