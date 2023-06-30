#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_COMMAND_LENGTH 20
#define MAX_LIST_SIZE 3

typedef struct Node {
    int speed;
    struct Node* next;
} Node;

// Função para enviar um comando para o Arduino pela porta serial
void sendCommand(HANDLE serialHandle, const char* command) {
    DWORD bytesWritten;
    WriteFile(serialHandle, command, strlen(command), &bytesWritten, NULL);
}

// Função para receber uma resposta do Arduino pela porta serial
void receiveResponse(HANDLE serialHandle, char* response) {
    DWORD bytesRead;
    ReadFile(serialHandle, response, MAX_COMMAND_LENGTH, &bytesRead, NULL);
    response[bytesRead] = '\0';
    printf("%s\n", response);
}

// Função para imprimir a lista de velocidades
void printSpeedList(Node* head) {
    printf("Lista de velocidades:\n");
    Node* current = head;
    while (current != NULL) {
        printf("%d\n", current->speed);
        current = current->next;
    }
}

// Função para inserir um novo nó na lista
Node* insertNode(Node* head, int speed) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->speed = speed;
    newNode->next = NULL;

    if (head == NULL) {
        return newNode;
    } else {
        Node* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
        return head;
    }
}

// Função para liberar a memória ocupada pela lista
void freeList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    HANDLE serialHandle;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    char command[MAX_COMMAND_LENGTH];
    char response[MAX_COMMAND_LENGTH];
    Node* speedList = NULL;
    int speedListSize = 0;

    // Abre a porta serial COM3
    serialHandle = CreateFile("COM3", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (serialHandle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Erro ao abrir a porta serial.\n");
        return 1;
    }

    // Configura os parâmetros da porta serial
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(serialHandle, &dcbSerialParams)) {
        fprintf(stderr, "Erro ao obter os parâmetros da porta serial.\n");
        CloseHandle(serialHandle);
        return 1;
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (!SetCommState(serialHandle, &dcbSerialParams)) {
        fprintf(stderr, "Erro ao configurar os parâmetros da porta serial.\n");
        CloseHandle(serialHandle);
        return 1;
    }

    // Configura os timeouts da porta serial
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;
    if (!SetCommTimeouts(serialHandle, &timeouts)) {
        fprintf(stderr, "Erro ao configurar os timeouts da porta serial.\n");
        CloseHandle(serialHandle);
        return 1;
    }

    printf("Comandos disponíveis:\n");
    printf("b1 - Liga/desliga motor e LED 1\n");
    printf("b2 - Liga/desliga LED 2 e move o servo motor\n");
    printf("v  - Obtem a velocidade do motor\n");
    printf("status - Obtem informações do estado dos componentes\n");
    printf("lista - Imprime a lista de velocidades\n");
    printf("exit - Sai do programa\n");

    while (1) {
        printf("\nDigite um comando: ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // Remove o caractere de nova linha

        if (strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "lista") == 0) {
            printSpeedList(speedList);
        } else {
            sendCommand(serialHandle, command);
            receiveResponse(serialHandle, response);

            if (strcmp(command, "v") == 0) {
                int speed = atoi(response);
                speedList = insertNode(speedList, speed);
                speedListSize++;
                while (speedListSize > MAX_LIST_SIZE) {
                    Node* temp = speedList;
                    speedList = speedList->next;
                    free(temp);
                    speedListSize--;
                }
            }
        }
    }

    // Libera a memória ocupada pela lista
    freeList(speedList);

    // Fecha a porta serial
    CloseHandle(serialHandle);

    return 0;
}
