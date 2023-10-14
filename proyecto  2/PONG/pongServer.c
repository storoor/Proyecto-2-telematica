#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define JUEGO_ANCHO 800
#define JUEGO_ALTO 600
#define PALETA_ANCHO 10
#define PALETA_ALTO 100
#define PELOTA_DIAMETRO 20
#define PUERTO 12345
#define MAX_CLIENTES 2



void *manejar_cliente(void *arg) {
    int cliente_socket = *((int *)arg);
    int jugador = num_clientes;
    num_clientes++;


// Definir las variables del juego
int pelota_x = JUEGO_ANCHO / 2;
int pelota_y = JUEGO_ALTO / 2;
int pelota_dx = 1; // Dirección de la pelota en el eje X
int pelota_dy = 1; // Dirección de la pelota en el eje Y

int paleta1_y = JUEGO_ALTO / 2 - PALETA_ALTO / 2;
int paleta2_y = JUEGO_ALTO / 2 - PALETA_ALTO / 2;

int puntuacion1 = 0;
int puntuacion2 = 0;

int clientes[MAX_CLIENTES];
int num_clientes = 0;
    while (1) {
         // Mover la pelota
    pelota_x += pelota_dx;
    pelota_y += pelota_dy;

    // Colisión de la pelota con las paletas
    if ((pelota_x < PALETA_ANCHO) && (pelota_y > paleta1_y) && (pelota_y < paleta1_y + PALETA_ALTO)) {
        pelota_dx = -pelota_dx;
    }

    if ((pelota_x > JUEGO_ANCHO - PALETA_ANCHO) && (pelota_y > paleta2_y) && (pelota_y < paleta2_y + PALETA_ALTO)) {
        pelota_dx = -pelota_dx;
    }

    // Colisión de la pelota con las paredes superior e inferior
    if (pelota_y < 0 || pelota_y > JUEGO_ALTO) {
        pelota_dy = -pelota_dy;
    }

    // Puntuación
    if (pelota_x < 0) {
        // Jugador 2 anota un punto
        puntuacion2++;
        // Restablecer la posición de la pelota
        pelota_x = JUEGO_ANCHO / 2;
        pelota_y = JUEGO_ALTO / 2;
        pelota_dx = -pelota_dx;
    } else if (pelota_x > JUEGO_ANCHO) {
        // Jugador 1 anota un punto
        puntuacion1++;
        // Restablecer la posición de la pelota
        pelota_x = JUEGO_ANCHO / 2;
        pelota_y = JUEGO_ALTO / 2;
        pelota_dx = -pelota_dx;
    }

    // Enviar datos de estado a los clientes
    // Definir una función para enviar datos de estado a un cliente específico
void enviar_datos_estado(int cliente_socket, int jugador) {
    char mensaje[1024];  // Tamaño del mensaje a enviar

    // Construir un mensaje que contenga la información del juego
    snprintf(mensaje, sizeof(mensaje), "Estado: Pelota(%d,%d) Paletas(%d,%d) Puntos(%d,%d)\n",
            pelota_x, pelota_y, paleta1_y, paleta2_y, puntuacion1, puntuacion2);

    // Enviar el mensaje al cliente específico
    if (send(cliente_socket, mensaje, strlen(mensaje), 0) < 0) {
        perror("Error al enviar datos de estado al cliente");
    }
}

// Dentro del bucle principal
while (1) {
    // ...

    // Enviar datos de estado a ambos clientes
    enviar_datos_estado(clientes[0], 1); // Jugador 1
    enviar_datos_estado(clientes[1], 2); // Jugador 2

    // ...
    // Definir una función para procesar los comandos de movimiento de las paletas
void procesar_comando_movimiento(int jugador, char comando) {
    // Actualizar la posición de la paleta correspondiente
    if (jugador == 1) {
        if (comando == 'U') {
            paleta1_y -= 10;  // Mover la paleta hacia arriba
        } else if (comando == 'D') {
            paleta1_y += 10;  // Mover la paleta hacia abajo
        }
    } else if (jugador == 2) {
        if (comando == 'U') {
            paleta2_y -= 10;  // Mover la paleta hacia arriba
        } else if (comando == 'D') {
            paleta2_y += 10;  // Mover la paleta hacia abajo
        }
    }

    // Asegurarse de que las paletas no se salgan de los límites del juego
    if (paleta1_y < 0) {
        paleta1_y = 0;
    }
    if (paleta1_y + PALETA_ALTO > JUEGO_ALTO) {
        paleta1_y = JUEGO_ALTO - PALETA_ALTO;
    }
    if (paleta2_y < 0) {
        paleta2_y = 0;
    }
    if (paleta2_y + PALETA_ALTO > JUEGO_ALTO) {
        paleta2_y = JUEGO_ALTO - PALETA_ALTO;
    }
}

// Dentro del bucle principal
while (1) {
    // ...

    // Recibir comandos de movimiento de las paletas desde los clientes y aplicarlos al juego
    char comando_jugador1, comando_jugador2;
    if (recv(clientes[0], &comando_jugador1, sizeof(comando_jugador1), 0) < 0) {
        perror("Error al recibir comando de movimiento de jugador 1");
    }
    if (recv(clientes[1], &comando_jugador2, sizeof(comando_jugador2), 0) < 0) {
        perror("Error al recibir comando de movimiento de jugador 2");
    }

    // Procesar los comandos de movimiento
    procesar_comando_movimiento(1, comando_jugador1);
    procesar_comando_movimiento(2, comando_jugador2);

    // ...

    // Enviar datos de estado a los clientes
    enviar_datos_estado(clientes[0], 1); // Jugador 1
    enviar_datos_estado(clientes[1], 2); // Jugador 2

    // ...
}

    // ...

    // Finalizar el juego si se alcanza un puntaje límite
    if (puntuacion1 >= PUNTAJE_LIMITE || puntuacion2 >= PUNTAJE_LIMITE) {
        // Notificar a los clientes sobre el resultado final
        enviar_datos_estado(clientes[0], 1);
        enviar_datos_estado(clientes[1], 2);
        break;
    }
}

    // Comprobar si se alcanzó un puntaje límite para terminar el juego
    if (puntuacion1 >= PUNTAJE_LIMITE || puntuacion2 >= PUNTAJE_LIMITE) {
        // Finalizar el juego y mostrar el ganador
        break;
    }
}
    }

    close(cliente_socket);
    return NULL;
}

int main() {
    int servidor_socket, cliente_socket;
    struct sockaddr_in servidor_addr, cliente_addr;
    socklen_t cliente_addr_len = sizeof(cliente_addr);
    pthread_t threads[MAX_CLIENTES];

    // Crear socket
    servidor_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (servidor_socket == -1) {
        perror("Error al crear el socket");
        exit(1);
    }

    // Configurar la dirección del servidor
    servidor_addr.sin_family = AF_INET;
    servidor_addr.sin_port = htons(PUERTO);
    servidor_addr.sin_addr.s_addr = INADDR_ANY;

    // Vincular el socket al puerto
    if (bind(servidor_socket, (struct sockaddr *)&servidor_addr, sizeof(servidor_addr) < 0)) {
        perror("Error al vincular el socket");
        exit(1);
    }

    // Escuchar por conexiones entrantes
    if (listen(servidor_socket, MAX_CLIENTES) < 0) {
        perror("Error al escuchar por conexiones entrantes");
        exit(1);
    }

    printf("Servidor Pong escuchando en el puerto %d...\n", PUERTO);

    while (num_clientes < MAX_CLIENTES) {
        // Aceptar conexiones entrantes
        cliente_socket = accept(servidor_socket, (struct sockaddr *)&cliente_addr, &cliente_addr_len);
        printf("Conexión entrante desde %s:%d\n", inet_ntoa(cliente_addr.sin_addr), ntohs(cliente_addr.sin_port));

        clientes[num_clientes] = cliente_socket;

        // Iniciar un hilo para manejar al cliente
        if (pthread_create(&threads[num_clientes], NULL, manejar_cliente, &clientes[num_clientes]) != 0) {
            perror("Error al crear el hilo del cliente");
            exit(1);
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < num_clientes; i++) {
        pthread_join(threads[i], NULL);
    }

    // Cerrar el servidor
    close(servidor_socket);

    return 0;
}
