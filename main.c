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
    int id; // Cada producto tendra un id definido previamente
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

typedef struct { // Struct para almacenar varias variables comodamente
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

// Funciones principales
void ingresarPedido(Nodo **cabeza, Pedido pedidoSemilla, EstadoProductos *estadoProductos);
void finalizarPedido(char *path, Nodo **cabeza, EstadoProductos *estadoProductos);
void emitirReporte(EstadoProductos *estadoProductos);
int salir();

// HELPERS
Pedido * configurarMenu(Pedido * pedidoSemilla);
Nodo * crearNodo(Pedido pedido);
int insertarFinal(Nodo **cabeza, Pedido nuevoPedido);
void guardarDatosVentas(char *path, int *cantSalchichas, int *cantHamburguesas);
int mostrarPedidos(Nodo *cabeza);
Nodo * buscarPedido(Nodo *cabeza, int id) ;
int eliminarPedido(Nodo **cabeza, int id);
void cargarBackup(char *archivoEstados, char *archivoPedidos, EstadoProductos *estadoProductos, Nodo **cabeza);
void actualizarBackup(char *archivoEstados, char *archivoPedidos, EstadoProductos *estadoProductos, Nodo **cabeza);
void dirFecha(char *fecha);
void gestionDir(char *logDir, char *pedidosDir, char *estadoDir, char *listaPedidosDir);

int main() {
    Nodo *cabeza = NULL;
    int opcion=0, flag=0;
    EstadoProductos * estadoProductos = NULL;
    char logDir[60], pedidosDir[60], estadoDir[60], listaPedidosDir[65];

    // Creacion de la estructura local de estados
    estadoProductos = (EstadoProductos*)malloc(sizeof(EstadoProductos));
    if (estadoProductos == NULL) {
        printf("ERROR: Al asignar espacio en memoria. funcion:['main']\n");
        system("pause");
        return EXIT_FAILURE;
    }

    gestionDir(logDir, pedidosDir, estadoDir, listaPedidosDir); //Creacion del directorio para guardar los reportes del dia
    cargarBackup(estadoDir, listaPedidosDir, estadoProductos, &cabeza); // Cargo el backup de los estados y la lista de pedidos

    Pedido * pedidoSemilla = NULL, * aux = NULL;
    aux = configurarMenu(pedidoSemilla);
    // Retorna el Nodo Pedido pre-setteado con datos del catalogo,
    // para utilizarlo como 'molde' en cada pedido nuevo que cree

    if (aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['main']\n");
        printf("La semilla del catalogo no se ha podido crear.\n");
        system("pause");
        return EXIT_FAILURE; // Si la creacion de la semilla fallo, el programa no puede seguir ejecutandose
    }

    // Luego del check, actualizo la variable con los nuevos valores
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
                ingresarPedido(&cabeza, *pedidoSemilla, estadoProductos);
                guardarDatosVentas(logDir, &estadoProductos->cantSalchichas, &estadoProductos->cantHamburguesas);
                actualizarBackup(estadoDir, listaPedidosDir, estadoProductos, &cabeza);
                break;
            case 2:
                finalizarPedido(pedidosDir, &cabeza, estadoProductos);
                actualizarBackup(estadoDir, listaPedidosDir, estadoProductos, &cabeza);
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

void ingresarPedido(Nodo **cabeza, Pedido nuevoPedido, EstadoProductos *estadoProductos) {
    char opcion[2];
    int opcionMenu=0, flag=0, bufferSalchichas=0, bufferHamburguesas=0;

    // El nodo nuevoPedido ya viene pre-setteado con el menu cargado,
    // partiendo de eso le creo un id, el cliente y le agrego las cantidades

    printf("Ingrese el nombre de quien es el pedido: ");
    fflush(stdin);
    gets(nuevoPedido.cliente);
    system("cls");

    nuevoPedido.id = estadoProductos->cantPedidos + 1;

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
                nuevoPedido.salchicha.cantidad += bufferSalchichas;
                flag=1;
                break;

            case 2:
                system("cls");
                printf("Ingrese la cantidad de hamburguesas: ");
                scanf("%d", &bufferHamburguesas);
                nuevoPedido.hamburguesa.cantidad += bufferHamburguesas;
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

    // Una vez que el usuario cargo los datos procedo a aniadir el pedido a la lista enlazada
    if (insertarFinal(cabeza, nuevoPedido) == 1) {
        printf("ERROR: No se pudo crear el pedido. funcion:['ingresarPedido']\n");
        system("pause");
        // Si el intento de insertar el pedido en la lista falla la funcion retorna aqui,
        // evitando que se modifiquen los contadores que hay a continuacion
        return;
    }


    // Como la creación del pedido fue exitosa incremento la estructura estados
    // que cuenta datos importantes de las ventas del dia
    estadoProductos->cantSalchichas += nuevoPedido.salchicha.cantidad;
    estadoProductos->cantHamburguesas += nuevoPedido.hamburguesa.cantidad;
    estadoProductos->productosPreparandose += nuevoPedido.salchicha.cantidad + nuevoPedido.hamburguesa.cantidad;

    estadoProductos->cantPedidos += 1;
    estadoProductos->pedidosPreparandose += 1;

    printf("Pedido cargado correctamente!");
    return;
}

void finalizarPedido(char *path, Nodo **cabeza, EstadoProductos *estadoProductos) {
    int flag=0, busquedaId=0, prodsVendidos = 0;
    Nodo * pedido = NULL;
    FILE * archivo = NULL;

    if (mostrarPedidos(*cabeza) == 1) {
        printf("No hay pedidos realizandose en este momento.\n"); // Valido que la lista no este vacia
        return;
    }

    printf("\nIngrese el numero del pedido que desea finalizar: ");
    scanf("%d", &busquedaId);

    // Si no encontro ningun pedido con ese id retorna NULL, sino, retorna el
    // Nodo correspondiente y lo guarda en la var 'pedido'
    pedido = buscarPedido(*cabeza, busquedaId);
    if (pedido == NULL) {
        printf("No se encontro el pedido especificado.\n");
        return;
    }

    // Si se encontro un pedido con ese ID, guardo los datos de la venta
    // en el archivo Entregados.txt antes de eliminarlos de la lista
    archivo = fopen(path, "a");
    if (archivo == NULL) {
        printf("\nEl archivo pedidosEntregados.txt no se pudo abrir.");
        system("pause");
        return;
    }

    fprintf(archivo, "---Pedido %d---\n", pedido->pedido.id);
    fprintf(archivo, "\nSalchichas: %d a $%.2f c/u", pedido->pedido.salchicha.cantidad, pedido->pedido.salchicha.precio);
    fprintf(archivo, "\nHamburguesas: %d a $%.2f c/u\n\n", pedido->pedido.hamburguesa.cantidad, pedido->pedido.hamburguesa.precio);
    fclose(archivo);

    // Obtengo este dato antes de eliminar el nodo
    prodsVendidos = pedido->pedido.salchicha.cantidad + pedido->pedido.hamburguesa.cantidad;

    // Luego de guardar el pedido en el registro, lo elimino de la lista.
    if (eliminarPedido(cabeza, busquedaId) == 1) {
        printf("No se ha encontrado el pedido que deseaba eliminar. funcion:['finalizarPedido]");
        return; //Si no se pudo eliminar, retorno para evitar modificar los estados mas adelante
    }

    // Actualizo los estados, luego de asegurarme que se elimino el pedido correctamente
    estadoProductos->pedidosPreparandose -= 1;
    estadoProductos->productosPreparandose -= prodsVendidos;
    estadoProductos->productosEntregados += prodsVendidos;

    system("cls");
    printf("Pedido finalizado con exito!");
    return;
}

void emitirReporte(EstadoProductos *estadoProductos) {
    if(estadoProductos->productosPreparandose + estadoProductos->productosEntregados > 0) {
        printf("Reporte del Dia:\n");
        printf("Cantidad de productos en preparacion: %d\n", estadoProductos->productosPreparandose);
        printf("Cantidad de productos entregados: %d", estadoProductos->productosEntregados);
    } else {
        printf("No hay productos en preparacion, ni entregas realizadas.");
    }
}

int salir() {
    char respuesta[2];

    printf("Esta seguro que desea salir? S/N: ");
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

void cargarBackup(char *archivoEstados, char *archivoPedidos, EstadoProductos *estadoProductos, Nodo **cabeza) {
    FILE * binEstados = fopen(archivoEstados, "rb");
    FILE * binPedidos = fopen(archivoPedidos, "rb");
    Pedido pedidoBuffer;

    // Revisión archivo binario pedidos
    //Si existe un archivo backup de la lista, leo cada pedido(struct) que tenga guardado y los voy insertando, recuperando de esta manera la lista
    if (binPedidos == NULL) {
        fclose(binPedidos);
    } else if (binPedidos != NULL) {
        while(fread(&pedidoBuffer, sizeof(Pedido), 1, binPedidos) == 1) {
            insertarFinal(cabeza, pedidoBuffer);
        };
        fclose(binPedidos);
    }

    // Revisión archivo binario estados
    if (binEstados == NULL) { // Archivo no existe/primera vez que se abre negocio en el día
        // Entonces no voy a leer los datos del archivo, cargo en 0 los estados en  mi struct local
        fclose(binEstados);
        binEstados = fopen(archivoEstados, "wb");

        estadoProductos->cantPedidos = 0;
        estadoProductos->pedidosPreparandose = 0;
        estadoProductos->productosEntregados = 0;
        estadoProductos->productosPreparandose = 0;
        estadoProductos->cantSalchichas = 0;
        estadoProductos->cantHamburguesas = 0;

        fwrite(estadoProductos, sizeof(*estadoProductos), 1, binEstados); // Las var creadas en 0 las cargo al file para luego poder modificarlas
        fclose(binEstados);
        return;
    }

    // Si el archivo ya existe, lo lee y se guardan todos sus datos en "estadoProductos"
    fread(estadoProductos, sizeof(*estadoProductos), 1, binEstados);
    fclose(binEstados);
}

void actualizarBackup(char *archivoEstados, char *archivoPedidos, EstadoProductos *estadoProductos, Nodo **cabeza) {
    FILE * binEstados = fopen(archivoEstados, "wb");
    FILE * binPedidos = fopen(archivoPedidos, "wb");

    //Archivo Estados
    if (binEstados == NULL) { // Valido que exista el archivo a attualizar
        printf("ERROR: No se han podido almacenar los datos en archivo BackUp estado de productos. funcion:['actualizarBinarios']");
        system("pause");
    } else if (binEstados != NULL) {
        fwrite(estadoProductos, sizeof(*estadoProductos), 1, binEstados); // Escribo la estructura Estados en el binario
    }

    // Archivo Pedidos
    if (binPedidos == NULL) {
        printf("ERROR: No se han podido almacenar los datos en archivo BackUp pedidos. funcion:['actualizarBinarios']");
        system("pause");
    } else if (binPedidos != NULL) {
        Nodo *aux = *cabeza;

        while(aux != NULL) { //Sobreescribo sobre el archivo toda la lista enlazada ya actualizada
            fwrite(&aux->pedido, sizeof(Pedido), 1, binPedidos);
            aux = aux->siguiente;
        }
    }

    fclose(binEstados);
    fclose(binPedidos);
}

void dirFecha(char *dir) {
    /* Formatea un String con la fecha actual */
    time_t tiempo = time(NULL);
    struct tm tm = *localtime(&tiempo); //Guardo la fecha local en la var ´tm´

    // Actualizo la variable "dir" con los datos del día de hoy
    // %d-%02d-%02d
    sprintf(dir, "./registros/log-%02d-%02d-%d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void gestionDir(char *logDir, char *pedidosDir, char *estadoDir, char *listaPedidosDir) {
    /* Asigna una carpeta en 'registros' para el día actual donde guardara los archivos de ese dia*/
    // Creación de carpeta 'registros'
    char dir[30];
    mkdir("./registros");

    // Creación de carpeta correspondiente al día actual (donde el programa crea los archivos)
    dirFecha(dir);
    mkdir(dir);

    // Agego la nueva raiz del programa ( ./registros/log-20-11-2022 )
    strcpy(logDir, dir);
    strcpy(pedidosDir, dir);
    strcpy(estadoDir, dir);
    strcpy(listaPedidosDir, dir);

    // Especifico la direccion de cada archivo
    strcat(logDir, "/log.txt");
    strcat(pedidosDir, "/pedidosEntregados.txt");
    strcat(estadoDir, "/estadoProductos.bin");
    strcat(listaPedidosDir, "/listaPedidos.bin");
}

Pedido * configurarMenu(Pedido * pedidoSemilla) {
    Pedido *aux = NULL;
    aux = (Pedido *) malloc(sizeof(Pedido));

    if (aux == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['configurarPrograma']\n");
        printf("La semilla del catalogo no se ha podido crear.\n");
        system("pause");
        return NULL;
    }

    // Crea un nodo Pedido y le pre-carga los datos del menu, para retornarlo y
    // luego usarlo de molde a la hora de crear nuevos pedidos
    pedidoSemilla = aux;

    // Definicion del catalogo de productos en el struct
    pedidoSemilla->salchicha.id = ID_SALCHICHA;
    strcpy(pedidoSemilla->salchicha.nombre, "Salchicha");
    pedidoSemilla->salchicha.precio = PRECIO_SALCHICHA;
    pedidoSemilla->salchicha.cantidad = 0;

    pedidoSemilla->hamburguesa.id = ID_HAMBURGUESA;
    strcpy(pedidoSemilla->hamburguesa.nombre, "Hamburguesa XXL");
    pedidoSemilla->hamburguesa.precio = PRECIO_HAMBURGUESA;
    pedidoSemilla->hamburguesa.cantidad = 0;

    return pedidoSemilla; // Retorno el struct pre-setteado

}

Nodo * crearNodo(Pedido pedido) {
    Nodo * nuevoNodo = NULL;
    nuevoNodo = (Nodo *) malloc(sizeof(Nodo));

    if(nuevoNodo == NULL) {
        printf("MEMORY ERROR: Error al asignar espacio en memoria. funcion:['crearNodo']\n");
        system("pause");
        return NULL; // Si el nodo no se pudo crear retorna NULL
    }

    nuevoNodo->pedido = pedido;
    nuevoNodo->siguiente = NULL;

    return nuevoNodo; // Retorna el nodo creado
}

int insertarFinal(Nodo **cabeza, Pedido nuevoPedido) {
    Nodo *ultimo=NULL, *aux=NULL;

    ultimo = *cabeza;
    if (ultimo == NULL) {
        // Si cabeza es NULL, es porque la lista esta vacia,
        // entonces en vez de insertar al final, lo inserto 1ro.
        aux = crearNodo(nuevoPedido);
        if (aux == NULL) {
            // Verifico que no haya fallado la funcion crearNodo,
            // si devolvio NULL fallo y termino aca la funcion, no podre insertar en la lista
            printf("ERROR: No se pudo insertar el nuevo nodo(pedido). funcion:['insertarFinal']\n");
            system("pause");
            // Si fallo la creación del nodo retorno 1, para avisar al 'mundo exterior' que la funcion no logro insertar el pedido
            return 1;
        }

        // Como acabo de crear el 1er nodo de la lista, apunto cabeza al mismo
        *cabeza = aux;
        return 0;
    }

    // Si llego a esta instancia es porque la lista tiene al menos 1 elemento
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
    return 0; // Si llego hasta aca no hubo fallas y el pedido se inserto
              // en la lista correctamente. Retorno 0 como flag.
}

void guardarDatosVentas(char *path, int *cantSalchichas, int *cantHamburguesas) {
    FILE * archivo = fopen(path, "w");

    if (archivo == NULL) {
        printf("ERROR: No se pudo abrir el archivo log.txt...");
        system("pause");
        return;
    }

    // Recibe la cantidad de cada producto que se vendieron y lo guarda en el
    // archivo log el cual almacen las ventas totales del dia
    int productosVendidos = *cantSalchichas + *cantHamburguesas;
    float dineroFacturado = (float)(*cantSalchichas * PRECIO_SALCHICHA + *cantHamburguesas * PRECIO_HAMBURGUESA);

    fprintf(archivo, "Cantidad de productos vendidos: %d\n", productosVendidos);
    fprintf(archivo, "Cantidad de dinero facturado: %.2f", dineroFacturado);

    fclose(archivo);
    return;
}

int mostrarPedidos(Nodo * cabeza) {
    /* recorre toda la lista de pedidos en proceso y la imprime */
    Nodo * actual=NULL;
    actual = cabeza;

    if (actual == NULL) {
        return 1; // Si la lista está vacia retorna y no muestra nada
    }

    system("cls");
    printf("------------------\nPedidos en proceso\n------------------");

    while(actual != NULL) {
        printf("\n\n------Pedido %d------", actual->pedido.id);
        printf("\nCliente: %s", actual->pedido.cliente);

        if (actual->pedido.salchicha.cantidad > 0) {
            printf("\nSalchichas: %d", actual->pedido.salchicha.cantidad);
        }

        if (actual->pedido.hamburguesa.cantidad > 0) {
            printf("\nHamburguesas: %d", actual->pedido.hamburguesa.cantidad);
        }

        actual = actual->siguiente;
    }

    printf("\n");
    system("pause");
    return 0;
}

Nodo * buscarPedido(Nodo * cabeza, int id) {
    /* Recorre la lista y devuelve el nodo con el id buscado, sino devuelve NULL */
    Nodo * busqueda = NULL, * actual = NULL;
    actual = cabeza;

    while (actual != NULL){
        if (actual->pedido.id == id) {
            return actual;
        }
        actual = actual->siguiente;
    }

    return NULL;
}

int eliminarPedido(Nodo ** cabeza, int id) {
    Nodo * actual = NULL;

    actual = *cabeza;
    if (actual->pedido.id == id) {
        // Si el que deseo eliminar es cabeza, reasigno cabeza al siguiente.
        *cabeza = actual->siguiente;
        free(actual);
        return 0;
    } else if (actual->pedido.id != id) {
        for(;actual->siguiente;) {
            if (actual->siguiente->pedido.id == id) { // ya parto buscando desde la 2da, la 1ra ya fue revisada, por eso estoy 1 pos desfazado. Si el nodo sucesor tiene el id que busco entonces:
                actual->siguiente = actual->siguiente->siguiente; // Hago que el nodo actual apunte al siguiente del siguiente, osea lo saltea, elimininandolo del enlace.
                free(actual->siguiente);
                return 0;
            }
            actual = actual->siguiente;
        }
    }

    printf("No se ha encontrado el pedido que deseaba eliminar. funcion:['eliminarPedido]");
    system("pause");
    return 1;
}