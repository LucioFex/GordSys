#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#define PRECIO_SALCHICHA 340
#define PRECIO_HAMBURGUESA 1349
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
    char cliente[30];
    producto salchicha;
    producto hamburguesa;
} Pedido;

typedef struct { // Struct para almacenar datos en archivo binario
    int cantPedidos;
    int pedidosPreparandose;
    int productosEntregados;
    int productosPreparandose;
    int cantSalchichas;
    int cantHamburguesas;
} EstadoProductos;

typedef struct Elemento {
    Pedido pedido;
    struct Elemento * siguiente;
} Nodo;

//TODO RENAME HEAD->CABEZA
void ingresarPedido(Nodo **head, Pedido *pedidoSemilla, EstadoProductos *estadoProductos);
void finalizarPedido(char *path, Nodo **head, EstadoProductos *estadoProductos);
void emitirReporte(EstadoProductos *estadoProductos);
int salir();

//HELPERS
Pedido * configurarCatalogo(Pedido * pedidoSemilla);
Nodo * crearNodo(Pedido pedido);
int insertarFinal(Nodo **head, Pedido datos);
void guardarDatosVentas(char *path, int *cantSalchichas, int *cantHamburguesas);
int mostrarPedidos(Nodo * head);
Nodo * buscarPedido(Nodo * head, int id) ;
void eliminarPedido(Nodo ** head, int id);
void cargarDatosIniciales(char *nombreArchivo, EstadoProductos *estadoProductos); // Datos del día en el binario
void actualizarDatosBin(char *nombreArchivo, EstadoProductos *estadoProductos);
void fechaActual(char *fecha);
void gestionDir(char *logDir, char *pedidosDir, char *estadoDir);

int main() {
    Nodo *head = NULL;
    int opcion=0, flag=0;
    EstadoProductos * estadoProductos = NULL;
    char logDir[60], pedidosDir[60], estadoDir[60];

    // Asignación de memoria dinámica
    estadoProductos = (EstadoProductos*)malloc(sizeof(*estadoProductos));
    if (estadoProductos == NULL) {
        printf("ERROR: Al asignar espacio en memoria. funcion:['main']\n");
        return EXIT_FAILURE;
    }

    gestionDir(logDir, pedidosDir, estadoDir);
    cargarDatosIniciales(estadoDir, estadoProductos);

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
        printf("Menu de Opciones: (Pedidos en preparacion: %d)", estadoProductos->pedidosPreparandose);
        printf("\n\t1) Ingresar pedido");
        printf("\n\t2) Finalizar pedido");
        printf("\n\t3) Emitir reporte");
        printf("\n\t4) Salir");

        printf("\n\nSeleccione alguna de las opciones: ");
        scanf("%d", &opcion);

        system("cls");
        switch (opcion) {
            case 1:
                ingresarPedido(&head, pedidoSemilla, estadoProductos);
                guardarDatosVentas(logDir, &estadoProductos->cantSalchichas, &estadoProductos->cantHamburguesas);
                actualizarDatosBin(estadoDir, estadoProductos);
                break;
            case 2:
                finalizarPedido(pedidosDir, &head, estadoProductos);
                actualizarDatosBin(estadoDir, estadoProductos);
                break;
            case 3:
                emitirReporte(estadoProductos);
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

void ingresarPedido(Nodo **head, Pedido * nuevoPedido, EstadoProductos *estadoProductos) {
    char opcion[2];
    int opcionMenu=0, flag=0, bufferSalchichas=0, bufferHamburguesas=0;

    printf("Ingrese el nombre de quien es el pedido: ");
    fflush(stdin);
    gets(nuevoPedido->cliente);
    system("cls");

    // TODO: Refactorizar a Pedido *nuevopedido para ser utilizado sin resetear valores
    nuevoPedido->id = estadoProductos->cantPedidos + 1;
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

    estadoProductos->cantSalchichas += nuevoPedido->salchicha.cantidad;
    estadoProductos->cantHamburguesas += nuevoPedido->hamburguesa.cantidad;
    estadoProductos->productosPreparandose += nuevoPedido->salchicha.cantidad + nuevoPedido->hamburguesa.cantidad;

    estadoProductos->cantPedidos += 1;
    estadoProductos->pedidosPreparandose += 1;

    printf("Pedido cargado correctamente!");
    return;
}

void finalizarPedido(char *path, Nodo **head, EstadoProductos *estadoProductos) {
    int flag=0, busquedaId=0, prodsVendidos = 0;
    Nodo * pedido = NULL;
    FILE * archivo = NULL;

    if (mostrarPedidos(*head) == 1) {
        printf("No hay pedidos realizandose en este momento.\n"); // Valido que la lista no este vacia
        return;
    }

    printf("\nIngrese el numero del pedido que desea finalizar: ");
    scanf("%d", &busquedaId);

    pedido = buscarPedido(*head, busquedaId);
    if (pedido == NULL) {
        printf("No se encontro el pedido especificado");
        system("pause");
        return;
    }
    
    // Si se encontro un pedido con ese ID, guardo los datos de la venta antes de eliminarlos de la lista
    archivo = fopen(path, "a");
    if (archivo == NULL) {
        printf("\nEl archivo pedidosEntregados.txt no se pudo abrir.");
        system("pause");
        return;
    }

    fprintf(archivo, "---Pedido %d---\n", pedido->pedido.id); //guardo el pedido a eliminar en el archivo pedidosEntregados.txt
    fprintf(archivo, "\nSalchichas: %d a $%.2f c/u", pedido->pedido.salchicha.cantidad, pedido->pedido.salchicha.precio);
    fprintf(archivo, "\nHamburguesas: %d a $%.2f c/u\n\n", pedido->pedido.hamburguesa.cantidad, pedido->pedido.hamburguesa.precio);
    fclose(archivo);

    prodsVendidos = pedido->pedido.salchicha.cantidad + pedido->pedido.hamburguesa.cantidad;

    // Luego de guardar el pedido en el registro, lo elimino de la lista.
    eliminarPedido(head, busquedaId); // TODO: Añadir validación para cuando no se puede eliminar

    estadoProductos->pedidosPreparandose -= 1;
    estadoProductos->productosPreparandose -= prodsVendidos;
    estadoProductos->productosEntregados += prodsVendidos;

    system("cls");
    printf("Pedido finalizado con exito!");
    return;
}

void emitirReporte(EstadoProductos *estadoProductos) {
    // TODO: Añadir validación para productos
    printf("Reporte del Dia:\n");
    printf("Cantidad de productos en preparacion: %d\n", estadoProductos->productosPreparandose);
    printf("Cantidad de productos entegados: %d", estadoProductos->productosEntregados);
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

void cargarDatosIniciales(char *nombreArchivo, EstadoProductos *estadoProductos) {
    FILE * bin = fopen(nombreArchivo, "rb");

    if (bin == NULL) { // Archivo no existe / primera vez que se abre negocio en el día
        fclose(bin);
        bin = fopen(nombreArchivo, "wb");

        estadoProductos->cantPedidos = 0;
        estadoProductos->pedidosPreparandose = 0;
        estadoProductos->productosEntregados = 0;
        estadoProductos->productosPreparandose = 0;
        estadoProductos->cantSalchichas = 0;
        estadoProductos->cantHamburguesas = 0;

        fwrite(estadoProductos, sizeof(*estadoProductos), 1, bin);
        fclose(bin);
        return;
    }

    // Si el archivo ya existe, se guardan todos sus datos en "estadoProductos"
    fread(estadoProductos, sizeof(*estadoProductos), 1, bin);
    fclose(bin);
} 

void actualizarDatosBin(char *nombreArchivo, EstadoProductos *estadoProductos) {
    /* Se guardan todos los datos de la estructura 'estadoProductos' en binario*/
    FILE * bin = fopen(nombreArchivo, "wb");

    if (bin == NULL) { // Validación
        printf("ERROR: No se han podido almacenar los datos en archivo BackUp. funcion:['actualizarDatosBin']");
        system("pause");
        return;
    }

    fwrite(estadoProductos, sizeof(*estadoProductos), 1, bin);
    fclose(bin);
}

void fechaActual(char *dir) {
    /* Formatea un String con la fecha actual */
    time_t tiempo = time(NULL);
    struct tm tm = *localtime(&tiempo);

    // Se actualiza la variable "dir" con los datos del día de hoy
    // %d-%02d-%02d
    sprintf(dir, "./registros/log-%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900); 
}

void gestionDir(char *logDir, char *pedidosDir, char *estadoDir) {
    /* Asigna una caperta en 'registro' para el día actual, para almacenar los logs*/
    // Creación de carpeta 'registros'
    char dir[30];
    mkdir("./registros", 0777);

    // Creación de carpeta para el día actual (donde el programa opera)
    fechaActual(dir);
    mkdir(dir, 0777);

    // Actualización del path del programa ( ./registros/log-20-11-2022 )
    strcpy(logDir, dir);
    strcpy(pedidosDir, dir);
    strcpy(estadoDir, dir);

    // Agregado de archivos en cada path
    strcat(logDir, "/log.txt");
    strcat(pedidosDir, "/pedidosEntregados.txt");
    strcat(estadoDir, "/estadoProductos.bin");
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

void guardarDatosVentas(char *path, int *cantSalchichas, int *cantHamburguesas) {
    FILE * archivo = fopen(path, "w");

    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        system("pause");
        return;
    }

    int productosVendidos = *cantSalchichas + *cantHamburguesas;
    float dineroFacturado = (float)(*cantSalchichas * PRECIO_SALCHICHA + *cantHamburguesas * PRECIO_HAMBURGUESA);

    fprintf(archivo, "Cantidad de productos vendidos: %d\n", productosVendidos);
    fprintf(archivo, "Cantidad de dinero facturado: %.2f", dineroFacturado);

    fclose(archivo);
    return;
}

int mostrarPedidos(Nodo * head) {
    Nodo * actual=NULL;
    actual = head;

    if (actual == NULL) {
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
    if (actual->pedido.id == id) { // Si el que deseo eliminar es cabeza, reasigno cabeza al siguiente.
        *head = actual->siguiente;
        free(actual);
    } else {
        for(;actual->siguiente;) {
            if (actual->siguiente->pedido.id == id) { // ya parto buscando desde la 2da pos, la 1ra ya fue revisada.
                actual->siguiente = actual->siguiente->siguiente; // Si el nodo sucesor tiene el id que busco, hago que el nodo actual apunte al siguiente del siguiente, osea lo saltea, elimininandolo del enlace.
                free(actual->siguiente);
                break;
            }
            actual = actual->siguiente;
        }
    }
}
