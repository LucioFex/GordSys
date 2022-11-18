#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRECIO_SALCHICHA 57.5
#define PRECIO_HAMBURGUESA 130

#define ID_SALCHICHA 101
#define ID_HAMBURGUESA 102

typedef struct {
    int id;
    char nombre[30];
    float precio;
    int cantidad;
} producto;

typedef struct {
    int id; // Comenzando desde 0
    char cliente[30]; // Nombre
    producto salchicha;
    producto hamburguesa;
} Pedido;

typedef struct Elemento { // Lista enlazada
    Pedido pedido;
    struct Elemento* siguiente;
} Nodo;

// Definición de funciones --------------------------- NOTA 💀: No todas tienen que ser Void, eso cambienlo ustedes muchachos
void crearNodo(Pedido pedido);
void ingresarPedido(Nodo **head, int *ptrCantPedidos, int *ptrPreparandose,int *cantSalchichas, int *cantHamburguesas); // NOTA 💀: No es sintácticamente posible en C hacer las estructuras genéricas con pre-valores (como meterle los productos pre armasdos a 'pedido'). Así que Agus, cagaste, vas a tener que añadir los 'precios', 'nombres' e 'IDs' de los productos al momento de declarar 'Pedido' al iniciar esta función
void finalizarPedido(Nodo **head, int *ptrPreparandose);
void emitirReporte(Nodo *head, int *ptrPreparandose);
void encontrarRegistro(Nodo *head);
void guardarDatosVentas(char *archivo, int *cantHamburguesas, int *cantSalchichas);
void insertarFinal(Nodo **head, Pedido datos);
void salir();

int main() {
    Nodo *head = NULL;
    int op = 0, cantPedidos = 0, preparandose = 0, cantSalchichas = 0, cantHamburguesas = 0;
    int *ptrCantPedidos = &cantPedidos, *ptrPreparandose = &preparandose;
    int *ptrCantSalchichas = &cantSalchichas, *ptrCantHamburguesas = &cantHamburguesas;

    do {
        system("cls");
        printf("Menu de Opciones: (Pedidos en preparacion: %d)", preparandose);
        printf("\n\t1) Ingresar pedido");
        printf("\n\t2) Finalizar pedido");
        printf("\n\t3) Emitir reporte");
        printf("\n\t4) Salir");

        printf("\nSeleccione alguna de las opciones: ");
        scanf("%d", &op);

        system("cls");
        switch (op) {
            case 1:
                /* ingresarPedido(&head, &cantPedidos, &preparandose, &cantSalchichas, &cantHamburguesas); */ // Che pa, haciendo esto nos ahorramos 4 variables 😎
                ingresarPedido(&head, ptrCantPedidos, ptrPreparandose, ptrCantSalchichas, ptrCantHamburguesas);
                break;
            case 2:
                finalizarPedido(&head, ptrPreparandose);
                break;
            case 3:
                emitirReporte(head, ptrPreparandose);
                break;
            case 4:
                salir(); // Función hardcore
                break;
            default:
                printf("Opcion no reconocida, vuelva a intentarlo...\n");
                break;
        }
        system("pause");
    } while (op != 4);

    return EXIT_SUCCESS;
}

void crearNodo(Pedido pedido) {}
void ingresarPedido(Nodo **head, int *ptrCantPedidos, int *ptrPreparandose, int *cantSalchichas, int *cantHamburguesas) {}
void finalizarPedido(Nodo **head, int *ptrPreparandose) {}
void emitirReporte(Nodo *head, int *ptrPreparandose) {}
void encontrarRegistro(Nodo *head) {}
void guardarDatosVentas(char *archivo, int *cantHamburguesas, int *cantSalchichas) {}
void insertarFinal(Nodo **head, Pedido datos) {}
void salir() {}
