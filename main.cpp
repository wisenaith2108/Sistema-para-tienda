/* 
Nombre: Sistema de Compra y Venta para una tienda de abarrotes
Autores:
    * TRILLO MARAVI, JAIR GUILLERMO
    * MEDINA SUYO, JAVIER BENJAMÍN
    * CONTRERAS ESPINOZA, SEBASTIAN MATIAS
    * LOAYZA APONTE, JHORDY ARTURO
Descripción: 
Objetivo: Desarrollar una aplicación en modo consola en lenguaje C++ que involucre los aspectos centrales de la programación estructurada y gestión de archivos binarios.
*/

#include <iostream>
#include <locale.h>
#include <string>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <windows.h>
using namespace std;


void bienvenida_unica();
bool login_page();
bool key_exist();

bool login_state;

typedef long long int num_max;

typedef unsigned long long max_digito; //por default 

const int cad = 65;
typedef char cad_char[cad];

typedef struct
{
    max_digito cod; //codigo del producto
    cad_char nombre; // nombre del producto
    int cant; //cantidad en stock
    float p_unt; //precio unitario
}inventario;

typedef struct
{
    max_digito doc; //DNI o RUC
    bool tipo; // 0: persona 1: empresa
    cad_char nombre_cliente; //nombre del cliente
}clientes;

typedef struct{
        bool perfil; //0:admin 1:vendor
        unsigned long pin; // PIN de acceso
        cad_char nombre; //Nombre del admin o vendedor
    }llave;

/*MENUS*/
void menu_admin();
void menu_vendor();

/*ADMINISTRATIVO*/
void ver_inventario(); //admin (1)
void modificar_inventario(); //admin (2)
    void actualizar_stock();//admin (2.2)
    void agregar_producto(); //admin (2.3)
    void editar_producto(); // admin (2.4)
    void borrar_producto(); // admin (2.5)

/*GLOBAL: admin y vendor*/
void ver_stock();// (3)
void lista_clientes(); // (4)
void buscar_cliente(); // (5, 6.1)
void modificar_clientes(); //(6)
    void agregar_clientes(); // (6.2)
    void editar_cliente(); // (6.3)
    void borrar_cliente(); // (6.4)

/*PUNTO DE CAJA: global*/
void catalago_productos(); // (7)
void transaccion_venta(); // (8)

/*ESTADÍSTICAS*/
void ventas_por_dia(); //(9)
void ranking_prod(); // (10)

/*CONFIG*/
void cambiar_pin (); // (0)

int main(){
    //setlocale(LC_CTYPE,"Spanish");
    SetConsoleOutputCP(65001);
    while(true){
        if (key_exist()){
            if (login_page() == 0){
                menu_admin();
            }else
            {
                menu_vendor();
            }
            
        }
        else{
            bienvenida_unica();
        }
    }
    
    return 0;
}

void bienvenida_unica(){
    fstream key;
    ofstream inventario_file;
    ofstream clientes_file;
    bool val;
    
    { //BIENVENIDA
        cout << "[Solo se mostrará una vez esta ventana]\n\n";
        cout <<"Le damos la bienvenida a <name_program>!\n\n";
        cout << "Este es un sistema de gestión completa que le ayudará a:"<<endl;
        cout << "\t* Gestionar su inventario de productos\n\t* Guardar la relacián de clientes\n";
        cout << "\t* Gestionar las compras\n\t* Emitir Boletas o Facturas\n";
        cout << "\t* Crear tu catálogo de productos\nEntre otras muchas funciones más. (Revisar la guía de usuario incluido)."<<endl<<endl;
        cout << "Para comenzar, configuraremos los perfiles de\n* Administrador (acceso total) y,\n* Vendedor (acceso parcial):"<<endl<<endl;
        system ("pause");
        system ("cls");
    }

    key.open("key.dat", ios::app | ios::binary);

    if (key.is_open()) //PERFIL ADMINISTRADOR
    {
       {
        llave admin;
        admin.perfil = 0;
        cout << "Perfil: ADMINISTRADOR"<<endl;
        cout << "Ingresa tu nombre: ";
        cin.getline(admin.nombre,cad);
        cout << "Ingresa un PIN de 6 dígitos: ";
        cin >> admin.pin;
        val = (admin.pin >= 100000 and admin.pin <= 999999);
    
        do //PIN_admin
        {
            if (!val){
                system("cls");
                cout << "Perfil: ADMINISTRADOR"<<endl;
                cout << "Ingresa tu nombre: "<< admin.nombre<<endl;
                cout << "Ingrese un PIN de 6 dígitos correcto: ";
                cin >> admin.pin;
                val = (admin.pin >= 100000 and admin.pin <= 999999);
            }
        } while (!val);

        key.write((char *)&admin,sizeof(admin));
        }
    }
    cin.sync();

    system ("cls");

    if (key.is_open()) //PERFIL VENDEDOR
    {
        {
            llave vendor;
            vendor.perfil = 1;
            cout << "Perfil: VENDEDOR"<<endl;
            strcpy(vendor.nombre, "VENDEDOR");
            cout << "Ingresa un pin de 6 digitos: ";
            cin >> vendor.pin;
            val = (vendor.pin >= 100000 and vendor.pin <= 999999);

            do //PIN_vendor
            {
                if (!val){
                    system("cls");
                    cout << "Perfil: VENDEDOR"<<endl;
                    cout << "Ingrese un PIN de 6 digitos correcto: ";
                    cin >> vendor.pin;
                    val = (vendor.pin >= 100000 and vendor.pin <= 999999);
                }
            } while (!val);

            key.write((char *)&vendor,sizeof(vendor));
        }
    }
    cin.sync();

    //CREACIÓN DE ARCHIVOS DATA
    inventario_file.open("inventario.dat", ios::binary);
    if (inventario_file.is_open())
        inventario_file.close();

    clientes_file.open("clientes.dat", ios::binary);
    if (clientes_file.is_open())
        clientes_file.close();

}

bool login_page(){
    system("cls");
    unsigned short int opc;
    cout << "== INICIO DE SESIÓN DEL PROGRAMA =="<<endl << endl;
    cout << "Elige tu perfil:"<<endl;
    cout << "1. Administrador\n2. Vendedor"<<endl<<endl;
    cout << "Olvidaste tu PIN? Ingresa 3."<<endl<<endl;
    cout << "0. Cerrar Programa"<<endl<<endl;
    cout << "Ingresa una opción: ";
    cin >> opc;

    unsigned long pin;

    switch (opc)
    {
    case 0:
        system("cls");
        cout << "Cerrando programa...";
        exit(0);
        break;
    case 1:
        {
            system("cls");
            ifstream key; //ifstream
            key.open("key.dat", ios::binary);
            unsigned long pin_temp;
            bool val = (pin >= 100000 and pin <= 999999);
            llave acceso;
            if(key.is_open()){
                while (key.read((char*)&acceso, sizeof(acceso))){
                    if (acceso.perfil == 0){
                        pin_temp = acceso.pin;
                        break;
                    }
                }
            } 

            cout << "== ADMINISTRADOR =="<<endl<<endl;
            cout << "Ingresa tu PIN: ";
            cin >> pin;
            if (pin == pin_temp){
                login_state = 0;
            }
            else{
                if(!(pin == pin_temp) or  !val){
                    do //PIN_admin
                    {
                        if (!val){
                            do{
                                system("cls");
                                cout << "== ADMINISTRADOR =="<<endl<<endl;
                                cout << "Ingrese un PIN de 6 dígitos válido: ";
                                cin >> pin;
                                val = (pin >= 100000 and pin <= 999999);
                            } while (!(pin == pin_temp));
                        }
                        login_state = 0;
                    } while (!(pin == pin_temp) && !val);
                }
            }
            key.close();
        }
        break;
    case 2:
            {
                system("cls");
                ifstream key; //ifstream
                key.open("key.dat", ios::binary);
                unsigned long pin_temp;
                bool val = (pin >= 100000 and pin <= 999999);
                llave acceso;

                if(key.is_open()){
                    while (key.read((char*)&acceso, sizeof(acceso))){
                        if (acceso.perfil == 1){
                            pin_temp = acceso.pin;
                            break;
                        }
                    }
                cout << "== VENDEDOR =="<<endl<<endl;
                cout << "Ingresa tu PIN: ";
                cin >> pin;
                } 

                if (pin == pin_temp){
                    login_state = 1;
                }
                else{
                    if(!(pin == pin_temp) or !val){
                        do //PIN_admin
                        {
                            if (!val){
                                do{
                                    system("cls");
                                    cout << "== VENDEDOR =="<<endl<<endl;
                                    cout << "Ingrese un PIN de 6 dígitos válido: ";
                                    cin >> pin;
                                    val = (pin >= 100000 and pin <= 999999);
                                } while (!(pin == pin_temp));
                            }
                            login_state = 1;
                        } while (!(pin == pin_temp) && !val);
                    }
                }
                key.close();
            }
        break;
    }

    return login_state;
}

bool key_exist(){
    ifstream archivo("key.dat");
    return archivo.good();
}

void menu_admin(){
    llave llave;
    fstream key;
    key.open("key.dat", ios::in | ios::binary);
    string nombre;
    while (key.read((char *)&llave, sizeof(llave))){
        if (llave.perfil == 0){
            nombre = llave.nombre;
        }
    }
    key.close();
    int opc;
    do{
        system("cls");
        cout << "Bienvenido de vuelta, "<< nombre <<"."<<endl<<endl;
        cout << "== Gestionar Almacen ==\n1. Ver Inventario\n2. Modificar inventario...\n3. Stock de productos"<<endl<<endl;
        cout << "== Gestionar Clientes ==\n4. Lista de Clientes\n5. Buscar cliente...\n6. Modificar Clientes"<<endl<<endl;
        cout << "== Gestionar Punto de Caja ==\n7. Ver Catalogo de productos\n8. Generar transaccion de Venta..."<<endl<<endl;
        cout << "== Estadisticas ==\n9.  Ventas realizadas por dia\n10. Productos mas vendidos"<<endl<<endl;
        cout << "0. Cambiar PIN...\n11. Cerrar Sesion"<<endl<<endl;
        cout << "Seleccione una opcion: ";
        cin >> opc;
    
        switch (opc)
        {   
            case 0:
                cambiar_pin();
                break;
            case 1:
                ver_inventario();
                break;
            case 2:
                modificar_inventario();
                break;
            case 3:
                ver_stock();
                break;
            case 4:
                lista_clientes();
                break;
            case 5:
                buscar_cliente();
                break;
            case 6:
                modificar_clientes();
                break;
            case 11:
                login_page();
                break;
        }
    }while (opc >=0 && opc <=10);
}

void menu_vendor(){
    system("cls");
    cout << "En contruccion"<<endl;
    system("pause");
}

void ver_inventario(){
    fstream almacen;
    almacen.open("inventario.dat", ios::in | ios::binary);
    inventario ver;
    system("cls");
    cout << "== INVENTARIO =="<<endl<<endl;
    if (almacen.is_open()){
        almacen.read((char *)&ver, sizeof(ver));
        while (!almacen.eof())
        {
            cout << "Nombre: "<< ver.nombre<<endl;
            cout << "Cod.: "<<ver.cod<<endl;
            if (ver.cant == 1)
            {
                cout << "Stock: "<<ver.cant << " unidad"<<endl;
            }
            else
            {
                cout << "Stock: "<<ver.cant << " unidades"<<endl;
            }
            cout << "Precio Unit.: S/ "<<ver.p_unt<<endl;
            cout << endl;
            almacen.read((char *)&ver, sizeof(ver)); //repite
        }
        almacen.close();
    }
    system("pause");
}

void modificar_inventario(){
    system("cls");
    int opc1;
    cout << "== INVENTARIO =="<<endl<<endl;
    cout << "1. Ver Inventario\n2. Agregar Productos nuevos...\n3. Modificar Producto...\n4. Borrar Producto..."<<endl<<endl;
    cout << "Seleccione una opcion: ";
    cin >> opc1;

    switch (opc1)
    {
    case 1:
        ver_inventario();
        break;
    case 2:
        agregar_producto();
        break;
    case 3:
        editar_producto();
        break;
    case 4:
        borrar_producto();
        break;
    }
}

void agregar_producto(){
    unsigned short int num;
    inventario agregar;
    fstream almacen;
    almacen.open("inventario.dat", ios::app | ios::binary);
    if (almacen.is_open()){
        system("cls");
        cout << "Ingrese la cantidad de items a registrar: ";
        cin >> num;
        system("cls");
        for (int i = 0; i < num; i++)
        {
            system("cls");
            cin.sync();
            cout << i+1 << ". Producto"<<endl;
            cout << "Nombre del producto: ";
            cin.getline(agregar.nombre,cad);
            cin.sync();
            cout << "Codigo num. del Producto: ";
            cin >> agregar.cod;
            cout << "Stock inicial: ";
            cin >> agregar.cant;
            cout << "Precio unitario (con IGV): S/ ";
            cin >> agregar.p_unt;
            almacen.write((char *)&agregar, sizeof(agregar));
            cin.ignore();
        }
    almacen.close();
    }
}

void editar_producto(){
    system("cls");
    // Abrir el archivo en modo lectura y escritura binaria
    fstream archivo;

    archivo.open("inventario.dat", ios::in | ios::out | ios::binary);
    if (!(archivo.is_open())){ //comprobar si existe
        system("cls");
        cout << "Error al abrir el archivo" << endl;
        system("pause");
        return;
    }

    inventario producto;
    int codigo;
    
    cout << "Ingrese el código del producto a editar: ";
    cin >> codigo;
    cin.ignore();
    
    while (archivo.read((char *)&producto, sizeof(producto))){
        if (producto.cod == codigo){
            cout << "Producto Encontrado:"<<endl;
            cout << "Nombre: "<<producto.nombre<<"\n Precio Unitario: "<<producto.p_unt<<endl<<endl;
            cout << "Ingrese los Nuevo datos."<<endl;
            cout << "Nombre del producto: ";
            cin.getline(producto.nombre,cad);
            cout << "Ingrese el nuevo precio: ";
            cin >> producto.p_unt;
            cin.ignore();
            long pos = archivo.tellg();
            archivo.seekp(pos - sizeof(producto));
            archivo.write((char*)&producto, sizeof(producto));
            cout << "Producto actualizado correctamente." <<endl;
            system("pause");
            break;
        }else{
            system("cls");
            cout <<"No se ha encontrado un producto con ese código";
            system("pause");
            return;
        }
    }
}

void borrar_producto(){
    fstream archivo, temp;
    system("cls");
    archivo.open("inventario.dat", ios::in | ios::binary);
    temp.open("temp.dat", ios::out | ios::binary);

    if (!archivo.is_open()){
        cout << "Hubo un problema al abrir!\a";
        system("pause");
        return;
    }

    inventario producto;
    int codigo;
    cout << "Ingrese el código del producto que vas a eliminar:"<<endl;
    cout << "> ";
    cin >> codigo;
    system("pause");
    cin.ignore();

    bool encontrado = false;
    while (archivo.read((char*)&producto, sizeof(producto))){
        if ((producto.cod == codigo) /*and (producto.cant == 0)*/){
            encontrado = true;
            cout << "Estas a punto de eliminar este producto:"<<endl<<endl;
            cout << "Nombre: "<< producto.nombre<<endl;
            cout << "Cod.: "<<producto.cod<<endl;
            cout << "Precio Unit.: S/ "<<producto.p_unt<<endl<<endl;

            cout << "Para continuar, digite 1, de lo contrairo 0:\n";
            int confirmacion;
            cin >> confirmacion;
            cin.ignore();
            if (confirmacion == 1) {
                continue;
            }
        }/*else{
            cout << endl << "El producto no existe o todavía hay productos en stock";
            return;
        }*/
        temp.write((char*)&producto, sizeof(producto));
    }

    archivo.close();
    temp.close();

    remove("inventario.dat");
    rename("temp.dat", "inventario.dat");

    if (encontrado){
        cout << "Se eliminó exitosamente."<<endl;
    }else{
        cout << "Producto no encontrado."<<endl;
    }
    system("pause");
}

void ver_stock(){ 
    fstream almacen;
    almacen.open("inventario.dat", ios::in | ios::binary);
    
    system("cls");
    cout << "== STOCK =="<<endl<<endl;

    if (!almacen.is_open()){
        cout << "Error al abrir el archivo." << endl;
        system("pause");
        return;
    }

    inventario ver;
    int codigo;
    cout << "Ingrese el codigo en consulta: ";
    cin >> codigo;

    bool encontrado = false; // Bandera para saber si se encontró el producto

    while (almacen.read((char *)&ver, sizeof(ver))){
        if (ver.cod == codigo){
            encontrado = true; // Se encontró el producto
            cout << "Producto encontrado\a"<<endl<<endl;
            cout << "Nombre: "<< ver.nombre<<endl;
            if (ver.cant == 1)
            {
                cout << "Stock: "<<ver.cant << " unidad"<<endl;
            }
            else
            {
                cout << "Stock: "<<ver.cant << " unidades"<<endl;
            }
            cout << "Precio Unit.: S/ "<<ver.p_unt<<endl;
            cout << endl;
            system("pause");
            break; // Salir del bucle una vez que se encontró el producto
        }
    }

    if (encontrado == false) { // Si no se encontró el producto después de leer todo el archivo
        system("cls");
        cout << "Producto no encontrado"<<endl;
        system("pause");
    }

    almacen.close();
   
}


void lista_clientes(){
    fstream lista_clientes;
    lista_clientes.open("clientes.dat", ios::in | ios::binary);
    clientes ver;
    system("cls");

    cout << "== LISTADO DE CLIENTES =="<<endl<<endl;

    if (lista_clientes.is_open()){
        lista_clientes.read((char *)&ver, sizeof(ver));
        while (!lista_clientes.eof())
        {
            cout << "Nombre: "<< ver.nombre_cliente<<endl;
            if (ver.tipo == 0)
            {
                cout << "Tipo: Persona"<<endl;
                cout << "DNI: "<<ver.doc<<endl;
            }
            else
            {
                cout << "Tipo: Empresa"<<endl;
                cout << "RUC: "<<ver.doc<<endl;
            }
            cout << endl;

            lista_clientes.read((char *)&ver, sizeof(ver));
        }
        lista_clientes.close();
    }
    system("pause");
}

void buscar_cliente(){
    fstream lista_cliente;
    lista_cliente.open("clientes.dat", ios::in | ios::binary);

    system("cls");
    cout << "== BUSQUEDA DE CLIENTE =="<<endl<<endl;

    if (!lista_cliente.is_open()){
        cout << "No se encontro el archivo\a"<<endl;
        system("pause");
        return;
    }

    clientes cliente;
    long long int documento;
    cout << "Ingres el num. de documento en consulta: ";
    cin >> documento;

    bool encontrado = false;

    while (lista_cliente.read((char *)&cliente, sizeof(cliente)))
    {
        if (cliente.doc == documento){
            encontrado = true;
            cout << "Cliente encontrado"<<endl<<endl;
            cout << "Nombre: "<<cliente.nombre_cliente<<endl;
            if (cliente.tipo == 0){
                cout << "DNI: "<< cliente.doc;
            }else{
                cout << "RUC: "<<cliente.doc;
            }
            cout << endl;
            system("pause");
            break;
        }
    }

    if (encontrado == false){
        system("cls");
        cout << "Cliente no encontrado"<<endl;
        system("pause");
    }
    lista_cliente.close();
}

void modificar_clientes(){ //debe ser similar que modificar_inventario()
    system("cls");
    int opc;
    cout << "== CLIENTES =="<<endl<<endl;
    cout << "1. Buscar cliente...\n2. Agregar clientes...\n3. Actualizar clientes...\n4. Borrar cliente..."<<endl<<endl;
    cout << "Seleccione una opcion: ";
    cin >> opc;

    switch (opc)
    {
    case 1:
        buscar_cliente();
        break;
    case 2:
        agregar_clientes();
        break;
    case 3:
        editar_cliente();
        break;
    case 4:
        borrar_cliente();
    }

}

void agregar_clientes(){
    unsigned short int num;
    clientes agregar;
    fstream lista_clientes;
    lista_clientes.open("clientes.dat", ios::app | ios::binary);
    if (lista_clientes.is_open()){
        system("cls");
        cout << "Ingrese la cantidad de clientes a registrar: ";
        cin >> num;
        system("cls");
        for (int i = 0; i < num; i++)
        {
            system("cls");
            cin.sync();
            cout << "Nombre del Cliente: ";
            cin.getline(agregar.nombre_cliente,cad);
            cin.sync();
            cout << "Tipo:\n0 = Persona, 1 = Empresa\n> ";
            cin >> agregar.tipo;
            if (agregar.tipo == false)
            {
                cout << "Ingresa su DNI: ";
                cin >> agregar.doc;
            }
            else
            {
                cout << "Ingresa su RUC: ";
                cin >> agregar.doc;
            }
            lista_clientes.write((char *)&agregar, sizeof(agregar));
        }
    lista_clientes.close();
    }
}

void editar_cliente(){
    system("cls");
   fstream archivo; // Crea un objeto de archivo
    archivo.open("clientes.dat", ios::in | ios::out | ios::binary); // Abre el archivo clientes.dat en modo de lectura y escritura binaria
    if (!archivo.is_open()) { // Comprueba si el archivo se abrió correctamente
        cout << "Error al abrir el archivo." << endl; // Si no se abrió correctamente, muestra un mensaje de error
        system("pause");
        return; // Termina la función
    }

    clientes cliente; // Crea un objeto de cliente
    long int documento; // Crea una variable para el documento
    cout << "Ingrese el número de documento del cliente que desea editar: "; // Solicita al usuario que ingrese el número de documento del cliente
    cin >> documento; // Lee el número de documento del cliente
    cin.ignore(); // Ignora el siguiente carácter (generalmente un salto de línea)

    while (archivo.read((char*)&cliente, sizeof(clientes))) { // Lee los clientes del archivo uno por uno
        if (cliente.doc == documento) { // Comprueba si el nombre del cliente coincide con el nombre ingresado
            cout << "Cliente encontrado. Ingrese los nuevos datos." << endl; // Si coincide, muestra un mensaje

            cout << "Nombre del cliente: "; // Solicita al usuario que ingrese el nuevo nombre del cliente
            cin.getline(cliente.nombre_cliente, cad); // Lee el nuevo nombre del cliente

            long pos = archivo.tellg(); // Obtiene la posición actual en el archivo
            archivo.seekp(pos - sizeof(clientes)); // Mueve el puntero de escritura a la posición del cliente
            archivo.write((char*)&cliente, sizeof(clientes)); // Escribe los nuevos datos del cliente en el archivo

            cout << "Datos actualizados correctamente." << endl; // Muestra un mensaje indicando que los datos se actualizaron correctamente
            system("pause");
            break; // Termina el bucle
        }
    }
    archivo.close(); // Cierra el archivo
}

void borrar_cliente(){
    system("cls"); // Limpia la consola
    fstream archivo, temp; // Crea dos objetos de archivo
    archivo.open("clientes.dat", ios::in | ios::binary); // Abre el archivo clientes.dat en modo de lectura binaria
    temp.open("temp.dat", ios::out | ios::binary); // Abre un archivo temporal en modo de escritura binaria

    if (!archivo.is_open()) { // Comprueba si el archivo se abrió correctamente
        cout << "Error al abrir el archivo." << endl; // Si no se abrió correctamente, muestra un mensaje de error
        system("pause"); // Pausa la ejecución hasta que el usuario presione una tecla
        return; // Termina la función
    }

    clientes cliente; // Crea un objeto de cliente
    long int documento; // Crea una variable para el documento
    cout << "Ingrese el numero de documento del cliente que desea eliminar: "; // Solicita al usuario que ingrese el número de documento del cliente
    cin >> documento; // Lee el número de documento del cliente
    cin.ignore(); // Ignora el siguiente carácter (generalmente un salto de línea)

    bool encontrado = false; // Crea una variable para indicar si se encontró el cliente
    while (archivo.read((char*)&cliente, sizeof(clientes))) { // Lee los clientes del archivo uno por uno
        if (cliente.doc == documento) { // Comprueba si el documento del cliente coincide con el documento ingresado
            encontrado = true; // Indica que se encontró el cliente
            cout << "Deseas eliminar a:\n"; // Solicita confirmación al usuario
            cout << "Nombre: " << cliente.nombre_cliente << endl; // Muestra el nombre del cliente
            if(cliente.tipo == 0){
                cout << "DNI: " << cliente.doc << endl; // Muestra el DNI del cliente
            } else{
                cout << "RUC: " << cliente.doc << endl; // Muestra el RUC del cliente
            }
            cout << "Para continuar digite 1, de lo contrario 0:\n> "; // Solicita confirmación al usuario
            int confirmacion;
            cin >> confirmacion; // Lee la confirmación del usuario
            cin.ignore(); // Ignora el siguiente carácter (generalmente un salto de línea)
            if (confirmacion == 1) { // Comprueba si el usuario confirmó la eliminación
                continue; // Si el usuario confirmó, omite la escritura de este cliente en el archivo temporal
            }
        }
        temp.write((char*)&cliente, sizeof(clientes)); // Escribe el cliente en el archivo temporal
    }

    archivo.close(); // Cierra el archivo original
    temp.close(); // Cierra el archivo temporal

    remove("clientes.dat"); // Elimina el archivo original
    rename("temp.dat", "clientes.dat"); // Renombra el archivo temporal como el archivo original

    if (encontrado) { // Comprueba si se encontró el cliente
        cout << "Se eliminó exitosamente." << endl; // Si se encontró, muestra un mensaje de éxito
    } else {
        cout << "Cliente no encontrado." << endl; // Si no se encontró, muestra un mensaje de error
    }
    system("pause"); // Pausa la ejecución hasta que el usuario presione una tecla
}

void catalago_productos(){

}

void transaccion_venta(){

}

void ventas_por_dia(){

}

void ranking_prod(){
    cout << "En contrucción";
    system("pause");
}

void cambiar_pin(){
    cout << "En contruccion";
}
