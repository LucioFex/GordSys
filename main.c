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
    struct Elemento * siguiente;
} Nodo;

// Definición de funciones --------------------------- NOTA 💀: No todas tienen que ser Void, eso cambienlo ustedes muchachos
void ingresarPedido(Nodo **head, int *ptrCantPedidos, int *ptrPreparandose,int *cantSalchichas, int *cantHamburguesas); // NOTA 💀: No es sintácticamente posible en C hacer las estructuras genéricas con pre-valores (como meterle los productos pre armasdos a 'pedido'). Así que Agus, cagaste, vas a tener que añadir los 'precios', 'nombres' e 'IDs' de los productos al momento de declarar 'Pedido' al iniciar esta función
void finalizarPedido(Nodo **head, int *ptrPreparandose);
void emitirReporte(Nodo *head, int *ptrPreparandose);
void guardarDatosVentas(char *archivo, int *cantHamburguesas, int *cantSalchichas);
//guardarPedido
void salir();

Nodo * crearNodo(Pedido pedido);
int insertarFinal(Nodo **head, Pedido datos);
int eliminarPedido(Nodo ** head, int id);


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



void ingresarPedido(Nodo **head, int *ptrCantPedidos, int *ptrPreparandose, int *cantSalchichas, int *cantHamburguesas) {
    char opcion;
    int menuopt=0, flag=0;

    Pedido * nuevoPedido = NULL, *aux = NULL;
    aux = (Pedido *) malloc(sizeof(Pedido));

     if(aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['ingresarPedido']\n");
        printf("El pedido no se ha podido cargar\n");
        system("pause");
        return;
    }

    nuevoPedido = aux;

    //Def datos productos
    nuevoPedido->salchicha.id = ID_SALCHICHA;
    strcpy(nuevoPedido->salchicha.nombre, "Salchicha");
    nuevoPedido->salchicha.precio = 629.50;

    nuevoPedido->hamburguesa.id = ID_HAMBURGUESA;
    strcpy(nuevoPedido->hamburguesa.nombre, "Hamburguesa XXL");
    nuevoPedido->hamburguesa.precio = 1450;

    nuevoPedido->id = ptrCantPedidos + 1;

    printf("\nIngrese a nombre de quien es el pedido: ");
    fflush(stdin);
    gets(nuevoPedido->cliente);

    do {
        printf("1) Agregar salchicha/s al pedido\n2) Agregar hamburguesa/s al pedido\n");
        printf("Ingrese una opcion: ");
        scanf("%d", menuopt);
        flag = 0;

        switch (menuopt) {
            case 1:
                flag=1;
                printf("Ingrese la cantidad de salchichas");
                scanf("%d", nuevoPedido->salchicha.cantidad);
                break;
            
            case 2:
                flag=1;
                printf("Ingrese la cantidad de hamburguesas");
                scanf("%d", nuevoPedido->hamburguesa.cantidad);
                break;
            
            default:
                printf("\nLa opcion ingresada no es valida...");
                strcpy(opcion, "s");
                break;
        }
        
        if(flag == 1) {
            printf("Desea ingresar mas productos al pedido? S/N");
            fflush(stdin);
            gets(tolower(opcion));
        }
        

    } while (strcmp(opcion, "s") == 0);
    
    
    //guardarDatosVentas(file, nuevoPedido->salchicha.cantidad, nuevoPedido->hamburguesa.cantidad)
    //guardaPedido

    if(insertarFinal(head, *nuevoPedido) == 1) {
        printf("ERROR: No se pudo crear el pedido. funcion:['ingresarPedido']\n");
        system("pause");
        return;
    }

    cantSalchichas += nuevoPedido->salchicha.cantidad;
    cantHamburguesas += nuevoPedido->hamburguesa.cantidad;
    ++(ptrCantPedidos);
    ++(ptrPreparandose);
    
    printf("Pedido cargado correctamente!\n");
    return;
}

void finalizarPedido(Nodo **head, int *ptrPreparandose) {}
void emitirReporte(Nodo *head, int *ptrPreparandose) {}
void guardarDatosVentas(char *archivo, int *cantHamburguesas, int *cantSalchichas) {}

Nodo * crearNodo(Pedido pedido) {
    Nodo * nuevoNodo = NULL;
    nuevoNodo = (Nodo *) malloc(sizeof(Nodo));

    if(nuevoNodo == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['crearNodo']\n");
        system("pause");
        return NULL;
    }

    nuevoNodo->pedido = pedido;
    nuevoNodo->siguiente = NULL; 

    return nuevoNodo;
}

int insertarFinal(Nodo **head, Pedido nuevoPedido) {
    Nodo *ultimo=NULL, *aux=NULL;

    ultimo = *head;
    if (ultimo == NULL) {
        aux = crearNodo(nuevoPedido);
        if (aux == NULL) {
            printf("ERROR: No se pudo insertar el nuevo nodo(pedido). funcion:['insertarFinal']\n");
            system("pause");
            return 1;
        }

        ultimo = aux;
        return 0;
    } 

    for (;ultimo->siguiente;) { //Recorre la lista el ultimo nodo, osea, hasta el final, para poder colocar el nuevo nodo
        ultimo=ultimo->siguiente;
    }
    
    aux = crearNodo(nuevoPedido);
    if (aux == NULL) {
        printf("ERROR: No se pudo insertar el nuevo nodo(pedido). funcion:['insertarFinal']\n");
        system("pause");
        return 1;
    }

    ultimo->siguiente = crearNodo(nuevoPedido); 
    return 0;
}

int eliminarPedido(Nodo ** head, int id) {
    Nodo * actual=NULL;
    int flag=0;

    actual = *head;
    if (actual == NULL) {
        printf("No se puede eliminar el pedido porque lista esta vacias. funcion:['eliminarNodo']\n");
        return;
    } 

    if (actual->pedido.id == id) { //Si el que deseo eliminar es cabeza, reasigno cabeza al siguiente.
        *head = actual->siguiente;
        free(actual);

    } else {
        for(;actual->siguiente;) {
            if (actual->siguiente->pedido.id == id) { //ya parto buscando desde la 2da pos, la 1ra ya fue revisada.
                actual->siguiente = actual->siguiente->siguiente; //Si el nodo sucesor tiene el id que busco, hago que el nodo actual apunte al siguiente del siguiente, osea lo saltea, elimininandolo del enlace.
                free(actual->siguiente);
                flag=1;
                break;
            }
            actual = actual->siguiente;
        }
    }

    return flag;
}



void salir() {}
