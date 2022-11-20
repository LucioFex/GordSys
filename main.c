#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRECIO_SALCHICHA 340
#define PRECIO_HAMBURGUESA 1349
#define ID_SALCHICHA 101
#define ID_HAMBURGUESA 102

#define LOG_DIR "log.txt"
#define PEDIDOS_DIR "pedidosEntregados.txt"

typedef struct {
    int id;
    char nombre[30];
    float precio;
    int cantidad;
} producto;

typedef struct {
    int id; // Comenzando desde 0
    char cliente[30];
    producto salchicha;
    producto hamburguesa;
} Pedido;

typedef struct Elemento {
    Pedido pedido;
    struct Elemento * siguiente;
} Nodo;

//TODO RENAME HEAD->CABEZA
void ingresarPedido(Nodo **head, Pedido * pedidoSemilla, int *ptrCantPedidos, int *ptrPreparandose, int *cantSalchichas, int *cantHamburguesas);
void finalizarPedido(Nodo **head, int *ptrPreparandose);
void emitirReporte(Nodo *head, int *ptrPreparandose);
int salir();

//HELPERS
int resetearLog();
Pedido * configurarCatalogo(Pedido * pedidoSemilla);
Nodo * crearNodo(Pedido pedido);
int insertarFinal(Nodo **head, Pedido datos);
void guardarDatosVentas(int *cantSalchichas, int *cantHamburguesas);
int mostrarPedidos(Nodo * head);
Nodo * buscarPedido(Nodo * head, int id) ;
void eliminarPedido(Nodo ** head, int id);

int main() {
    Nodo *head = NULL;
    int opcion=0, cantPedidos=0, preparandose=0, cantSalchichas=0, cantHamburguesas=0, flag=0;

    int *ptrCantPedidos = &cantPedidos, *ptrPreparandose = &preparandose;
    int *ptrCantSalchichas = &cantSalchichas, *ptrCantHamburguesas = &cantHamburguesas;

    if(resetearLog() == 1) {
        return EXIT_FAILURE;
    }

    Pedido * pedidoSemilla = NULL, * aux = NULL;
    aux = configurarCatalogo(pedidoSemilla);
    if (aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['main']\n");
        printf("La semilla del catalogo no se ha podido crear.\n");
        return EXIT_FAILURE;
    }

    pedidoSemilla = aux;

    printf("----------------------------------------\n----------Bienvenido a GordSys----------\n----------------------------------------\n");
    system("pause");
    do {
        system("cls");
        printf("Menu de Opciones: (Pedidos en preparacion: %d)", preparandose);
        printf("\n\t1) Ingresar pedido");
        printf("\n\t2) Finalizar pedido");
        printf("\n\t3) Emitir reporte");
        printf("\n\t4) Salir");

        printf("\n\nSeleccione alguna de las opciones: ");
        scanf("%d", &opcion);

        system("cls");
        switch (opcion) {
            case 1:
                ingresarPedido(&head, pedidoSemilla, ptrCantPedidos, ptrPreparandose, ptrCantSalchichas, ptrCantHamburguesas);
                break;
            case 2:
                finalizarPedido(&head, ptrPreparandose);
                break;
            case 3:
                emitirReporte(head, ptrPreparandose);
                break;
            case 4:
                flag = salir();
                break;
            default:
                printf("Opcion no reconocida, vuelva a intentarlo...\n");
                break;
        }
        printf("\n");
        system("pause");
    } while (flag != 1);

    return EXIT_SUCCESS;
}


void ingresarPedido(Nodo **head, Pedido * nuevoPedido, int *ptrCantPedidos, int *ptrPreparandose, int *cantSalchichas, int *cantHamburguesas) {
    char opcion[2];
    int opcionMenu=0, flag=0, bufferSalchichas=0, bufferHamburguesas=0;

    printf("Ingrese el nombre de quien es el pedido: ");
    fflush(stdin);
    gets(nuevoPedido->cliente);
    system("cls");

    nuevoPedido->id = *ptrCantPedidos + 1;
    nuevoPedido->salchicha.cantidad = 0;
    nuevoPedido->hamburguesa.cantidad = 0;

    do {
        printf("1) Agregar salchicha/s al pedido\n2) Agregar hamburguesa/s al pedido\n");
        printf("Ingrese una opcion: ");
        scanf("%d", &opcionMenu);
        flag = 0;

        switch(opcionMenu) {
            case 1:
                system("cls");
                printf("Ingrese la cantidad de salchichas: ");
                scanf("%d", &bufferSalchichas);
                nuevoPedido->salchicha.cantidad += bufferSalchichas;
                flag=1;
                break;
            
            case 2:
                system("cls");
                printf("Ingrese la cantidad de hamburguesas: ");
                scanf("%d", &bufferHamburguesas);
                nuevoPedido->hamburguesa.cantidad += bufferHamburguesas;
                flag=1;
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
            system("cls");
        }
    } while (strcasecmp(opcion, "s") == 0);

    if (insertarFinal(head, *nuevoPedido) == 1) {
        printf("ERROR: No se pudo crear el pedido. funcion:['ingresarPedido']\n");
        system("pause");
        return;
    }

    *cantSalchichas += nuevoPedido->salchicha.cantidad;
    *cantHamburguesas += nuevoPedido->hamburguesa.cantidad;
    ++(*ptrCantPedidos);
    ++(*ptrPreparandose);

    guardarDatosVentas(cantSalchichas, cantHamburguesas);
    
    printf("Pedido cargado correctamente!");
    return;
}

void finalizarPedido(Nodo **head, int *ptrPreparandose) {
    int flag=0, busquedaId=0;
    Nodo * pedido = NULL;
    FILE * archivo = NULL;

    if(mostrarPedidos(*head) == 1) {
        printf("No hay pedidos realizandose en este momento.\n"); //Valido que la lista no este vacia
        return;
    }

    printf("\nIngrese el numero del pedido que desea finalizar: ");
    scanf("%d", &busquedaId);

    pedido = buscarPedido(*head, busquedaId);
    if(pedido == NULL) {
        printf("No se encontro el pedido especificado");
        system("pause");
        return;
    }
    
    //Si se encontro un pedido con ese ID, guardo los datos de la venta antes de eliminarlos de la lista
    archivo = fopen(PEDIDOS_DIR, "a");
    if(archivo == NULL) {
        printf("\nEl archivo pedidosEntregados.txt no se pudo abrir.");
        system("pause");
        return;
    }

    fprintf(archivo, "---Pedido %d---\n", pedido->pedido.id); //guardo el pedido a eliminar en el archivo pedidosEntregados.txt
    fprintf(archivo, "\nSalchichas: %d a $%.2f c/u", pedido->pedido.salchicha.cantidad, pedido->pedido.salchicha.precio);
    fprintf(archivo, "\nHamburguesas: %d a $%.2f c/u\n\n", pedido->pedido.hamburguesa.cantidad, pedido->pedido.hamburguesa.precio);
    fclose(archivo);

    //Luego de guardar el pedido en el registro, lo elimino de la lista.
    eliminarPedido(head, busquedaId);
    *ptrPreparandose = *ptrPreparandose - 1;
    system("cls");
    printf("Pedido finalizado con exito!");
    return;
}

void emitirReporte(Nodo *head, int *ptrPreparandose) { // Incompleta
    FILE * archivo = fopen(LOG_DIR, "r");
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


int salir() {
    char respuesta[2];

    printf("Esta seguro que desea salir? S/N");
    fflush(stdin);
    gets(respuesta);

    if(strcasecmp(respuesta, "s") == 0) {
        system("cls");
        printf("\n-------------------------------\nGracias por utilizar Gordsys...\n-------------------------------\n\n");
        return 1;
    }

    return 0;
}


//HELPERS - HELPERS -HELPERS - HELPERS

int resetearLog() {
    FILE * archivo = fopen(LOG_DIR, "w");
    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        system("pause");
        return EXIT_FAILURE;
    }

    fclose(archivo);
    return 0;
}

Pedido * configurarCatalogo(Pedido * pedidoSemilla) {
    
    Pedido *aux = NULL;
    aux = (Pedido *) malloc(sizeof(Pedido));

    if (aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['configurarPrograma']\n");
        printf("La semilla del catalogo no se ha podido crear.\n");
        system("pause");
        return NULL;
    }

    pedidoSemilla = aux;

    // Definicion del catalogo de productos
    pedidoSemilla->salchicha.id = ID_SALCHICHA;
    strcpy(pedidoSemilla->salchicha.nombre, "Salchicha");
    pedidoSemilla->salchicha.precio = PRECIO_SALCHICHA;
    pedidoSemilla->salchicha.cantidad = 0;

    pedidoSemilla->hamburguesa.id = ID_HAMBURGUESA;
    strcpy(pedidoSemilla->hamburguesa.nombre, "Hamburguesa XXL");
    pedidoSemilla->hamburguesa.precio = PRECIO_HAMBURGUESA;
    pedidoSemilla->hamburguesa.cantidad = 0;

    return pedidoSemilla;

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

void guardarDatosVentas(int *cantSalchichas, int *cantHamburguesas) {
    FILE * archivo = fopen(LOG_DIR, "w");
    
    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        system("pause");
        return;
    }

    fprintf(archivo, "Cantidad de Salchichas vendidas: %d\n", *cantSalchichas);
    fprintf(archivo, "Cantidad de Hamburguesas vendidas: %d\n", *cantHamburguesas);

    fprintf(archivo, "SubTotal facturado en Salchichas: %.2f\n", (float)(*cantSalchichas * PRECIO_SALCHICHA));
    fprintf(archivo, "SubTotal facturado en Hamburguesas: %.2f\n", (float)(*cantHamburguesas * PRECIO_HAMBURGUESA));

    // TODO CAMBIO DE IMPRESIÓN

    fclose(archivo);
    return;
}

int mostrarPedidos(Nodo * head) {
    Nodo * actual=NULL;
    actual = head;

    if(actual == NULL) {
        return 1;
    }

    system("cls");
    printf("------------------\nPedidos en proceso\n------------------");

    while(actual != NULL) {
        printf("\n\n------Pedido %d------", actual->pedido.id);
        printf("\nCliente: %s", actual->pedido.cliente);

        if (actual->pedido.salchicha.cantidad != 0) {
           printf("\nSalchichas: %d", actual->pedido.salchicha.cantidad);
        }

        if (actual->pedido.hamburguesa.cantidad != 0) {
           printf("\nHamburguesas: %d", actual->pedido.hamburguesa.cantidad);
        }
        
        actual = actual->siguiente;
    }

    printf("\n");
    system("pause");
    return 0;
}

Nodo * buscarPedido(Nodo * head, int id) {
    Nodo * busqueda = NULL, * actual = NULL;
    actual = head;

    while (actual != NULL){
        if (actual->pedido.id == id) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

void eliminarPedido(Nodo ** head, int id) {
    Nodo * actual=NULL;

    actual = *head;
    if (actual->pedido.id == id) { //Si el que deseo eliminar es cabeza, reasigno cabeza al siguiente.
        *head = actual->siguiente;
        free(actual);
    } else {
        for(;actual->siguiente;) {
            if (actual->siguiente->pedido.id == id) { //ya parto buscando desde la 2da pos, la 1ra ya fue revisada.
                actual->siguiente = actual->siguiente->siguiente; //Si el nodo sucesor tiene el id que busco, hago que el nodo actual apunte al siguiente del siguiente, osea lo saltea, elimininandolo del enlace.
                free(actual->siguiente);
                break;
            }
            actual = actual->siguiente;
        }
    }

    return;
}