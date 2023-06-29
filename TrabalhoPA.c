#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

typedef struct Node {
    int speed;
    struct Node* next;
} Node;

Node* head = NULL;
int count = 0;

void insertNode(int speed) {
    if (count == 3) {
        // Remove o nó mais antigo
        Node* current = head;
        head = head->next;
        free(current);
        count--;
    }

    // Insere o novo nó no início da lista
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->speed = speed;
    newNode->next = head;
    head = newNode;
    count++;
}

void printList() {
    Node* current = head;
    printf("Lista de velocidades:\n");
    while (current != NULL) {
        printf("%d\n", current->speed);
        current = current->next;
    }
}

void freeList() {
    Node* current = head;
    Node* next;
    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
    count = 0;
}

int main() {
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD bytesRead, bytesWritten;
    char command[6];

    // Abre a porta serial COM3
    hSerial = CreateFile("COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Falha ao abrir a porta serial COM3.\n");
        return 1;
    }

    // Configura os parâmetros da porta serial
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Falha ao obter os parametros da porta serial.\n");
        CloseHandle(hSerial);
        return 1;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Falha ao configurar os parametros da porta serial.\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Configura os timeouts da porta serial
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Falha ao configurar os timeouts da porta serial.\n");
        CloseHandle(hSerial);
        return 1;
    }

    // Loop principal
    while (1) {
        // Leitura do comando
        printf("Digite um comando ('v' para velocidade, 'b1' para botao 1, 'b2' para botao 2, 'lista' para imprimir a lista): ");
        scanf("%s", command);

        // Envio do comando pela porta serial
        if (!WriteFile(hSerial, command, strlen(command), &bytesWritten, NULL)) {
            printf("Falha ao enviar o comando pela porta serial.\n");
            CloseHandle(hSerial);
            freeList();
            return 1;
        }

        // Leitura da resposta pela porta serial
        char response[256];
        if (!ReadFile(hSerial, response, sizeof(response), &bytesRead, NULL)) {
            printf("Falha ao ler a resposta pela porta serial.\n");
            CloseHandle(hSerial);
            freeList();
            return 1;
        }

        // Manipulação da resposta
        response[bytesRead] = '\0';
        if (strcmp(command, "v") == 0) {
            // Comando 'v' - Velocidade
            int speed = atoi(response);
            insertNode(speed);
            printf("Velocidade do motor: %d\n", speed);
        } else if (strcmp(command, "b1") == 0) {
            // Comando 'b1' - Botão 1
            printf("Botao 1: %s\n", response);
        } else if (strcmp(command, "b2") == 0) {
            // Comando 'b2' - Botão 2
            printf("Botao 2: %s\n", response);
        } else if (strcmp(command, "lista") == 0) {
            // Comando 'lista' - Imprimir a lista
            printList();
        } else {
            // Comando inválido
            printf("Comando invalido.\n");
        }
    }

    // Fechamento da porta serial e liberação de memória
    CloseHandle(hSerial);
    freeList();

    return 0;
}
