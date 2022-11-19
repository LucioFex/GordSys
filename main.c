#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRECIO_SALCHICHA 629.5
#define PRECIO_HAMBURGUESA 1450

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
void ingresarPedido(char *nombreArchivo, Nodo **head, int *ptrCantPedidos, int *ptrPreparandose,int *cantSalchichas, int *cantHamburguesas); // NOTA 💀: No es sintácticamente posible en C hacer las estructuras genéricas con pre-valores (como meterle los productos pre armasdos a 'pedido'). Así que Agus, cagaste, vas a tener que añadir los 'precios', 'nombres' e 'IDs' de los productos al momento de declarar 'Pedido' al iniciar esta función
void finalizarPedido(Nodo **head, int *ptrPreparandose);
void emitirReporte(char *nombreArchivo, Nodo *head, int *ptrPreparandose);
void guardarDatosVentas(char *nombreArchivo, int *cantHamburguesas, int *cantSalchichas);
//guardarPedido
void salir();

Nodo * crearNodo(Pedido pedido);
int insertarFinal(Nodo **head, Pedido datos);
int eliminarPedido(Nodo ** head, int id);


int main() {
    Nodo *head = NULL;
    char nombreArchivo[10] = "log.txt";
    int op = 0, cantPedidos = 0, preparandose = 0, cantSalchichas = 0, cantHamburguesas = 0;

    int *ptrCantPedidos = &cantPedidos, *ptrPreparandose = &preparandose;
    int *ptrCantSalchichas = &cantSalchichas, *ptrCantHamburguesas = &cantHamburguesas;

    // Creación/limpieza del archivo 'log.txt'
    FILE * archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        return EXIT_FAILURE;
    }
    fclose(archivo);

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
                ingresarPedido(nombreArchivo, &head, ptrCantPedidos, ptrPreparandose, ptrCantSalchichas, ptrCantHamburguesas);
                break;
            case 2:
                finalizarPedido(&head, ptrPreparandose);
                break;
            case 3:
                emitirReporte(nombreArchivo, head, ptrPreparandose);
                break;
            case 4:
                salir();
                break;
            default:
                printf("Opcion no reconocida, vuelva a intentarlo...\n");
                break;
        }
        printf("\n");
        system("pause");
    } while (op != 4);

    return EXIT_SUCCESS;
}

void ingresarPedido(char *nombreArchivo, Nodo **head, int *ptrCantPedidos, int *ptrPreparandose, int *cantSalchichas, int *cantHamburguesas) {
    char opcion[2];
    int menuopt=0, flag=0, bufferSalchichas = 0, bufferHamburguesas = 0;

    Pedido * nuevoPedido = NULL, *aux = NULL;
    aux = (Pedido *) malloc(sizeof(Pedido));

    if (aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['ingresarPedido']\n");
        printf("El pedido no se ha podido cargar\n");
        system("pause");
        return;
    }

    nuevoPedido = aux;

    // Def datos productos
    nuevoPedido->salchicha.id = ID_SALCHICHA;
    strcpy(nuevoPedido->salchicha.nombre, "Salchicha");
    nuevoPedido->salchicha.precio = PRECIO_SALCHICHA;
    nuevoPedido->salchicha.cantidad = 0;

    nuevoPedido->hamburguesa.id = ID_HAMBURGUESA;
    strcpy(nuevoPedido->hamburguesa.nombre, "Hamburguesa XXL");
    nuevoPedido->hamburguesa.precio = PRECIO_HAMBURGUESA;
    nuevoPedido->hamburguesa.cantidad = 0;

    nuevoPedido->id = ptrCantPedidos + 1;

    printf("Ingrese el nombre de quien es el pedido: ");
    fflush(stdin);
    gets(nuevoPedido->cliente);

    do {
        printf("1) Agregar salchicha/s al pedido\n2) Agregar hamburguesa/s al pedido\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &menuopt);
        flag = 0;

        switch (menuopt) {
            case 1:
                flag=1;
                printf("Ingrese la cantidad de salchichas: ");
                scanf("%d", &bufferSalchichas);
                nuevoPedido->salchicha.cantidad += bufferSalchichas;
                break;
            
            case 2:
                flag=1;
                printf("Ingrese la cantidad de hamburguesas: ");
                scanf("%d", &bufferHamburguesas);
                nuevoPedido->hamburguesa.cantidad += bufferHamburguesas;
                break;
            
            default:
                printf("\nLa opcion ingresada no es valida...\n");
                strcpy(opcion, "s");
                break;
        }

        if(flag == 1) {
            printf("Desea ingresar mas productos al pedido? S/N: ");
            fflush(stdin);
            gets(opcion);
        }
    } while (strcasecmp(opcion, "s") == 0);

    if(insertarFinal(head, *nuevoPedido) == 1) {
        printf("ERROR: No se pudo crear el pedido. funcion:['ingresarPedido']\n");
        system("pause");
        return;
    }

    guardarDatosVentas(nombreArchivo, &nuevoPedido->hamburguesa.cantidad, &nuevoPedido->salchicha.cantidad);

    *cantSalchichas += nuevoPedido->salchicha.cantidad;
    *cantHamburguesas += nuevoPedido->hamburguesa.cantidad;
    ++(*ptrCantPedidos);
    ++(*ptrPreparandose);
    
    printf("\nPedido cargado correctamente!");
    return;
}

void finalizarPedido(Nodo **head, int *ptrPreparandose) {}

void emitirReporte(char *nombreArchivo, Nodo *head, int *ptrPreparandose) { // Incompleta
    FILE * archivo = fopen(nombreArchivo, "r");
    char texto[100];
    
    printf("Reporte del dia\n");

    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        return;
    }

    fscanf(archivo, " %[^\n]", texto);
    while (!feof(archivo)) {
        printf("%s\n", texto);
        fscanf(archivo, " %[^\n]", texto);
    }

    fclose(archivo);
}

void guardarDatosVentas(char *nombreArchivo, int *cantHamburguesas, int *cantSalchichas) {
    FILE * archivo = fopen(nombreArchivo, "w");
    
    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        return;
    }

    fprintf(archivo, "Cantidad de Salchichas vendidas: %d\n", *cantSalchichas);
    fprintf(archivo, "Cantidad de Hamburguesas vendidas: %d\n", *cantHamburguesas);

    fprintf(archivo, "SubTotal facturado en Salchichas: %.2f\n", (float)(*cantSalchichas * PRECIO_SALCHICHA));
    fprintf(archivo, "SubTotal facturado en Hamburguesas: %.2f\n", (float)(*cantHamburguesas * PRECIO_HAMBURGUESA));

    // TODO CAMBIO DE IMPRESIÓN

    fclose(archivo);
}

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

        *head = aux;
        return 0;
    } 

    // Recorre la lista el ultimo nodo, osea, hasta el final, para poder colocar el nuevo nodo
    for (;ultimo->siguiente;) { 
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
