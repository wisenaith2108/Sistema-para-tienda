/* 
Nombre: Sistema de Compra y Venta para una tienda
Autores:
    * TRIL** MA***, JAIR G*** (PRINCIPAL)
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
#include <ctime>
#include <cmath>
#include <iomanip>
using namespace std;

void bienvenida_unica();
bool login_page();
bool key_exist();

int login_state;

typedef unsigned long long max_digito; //por default 

const int cad = 65;
typedef char cad_char[cad];

typedef struct
{
    max_digito cod; //codigo del producto
    cad_char nombre; // nombre del producto
    int cant; //cantidad en stock
    float p_unit; //precio unitario
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

typedef struct {
    char fecha[11]; // Fecha en formato dd/mm/yy
    char hora[9]; // Hora en formato hh:mm:ss
    max_digito doc_cliente; // Documento del cliente
    max_digito cod_productos[30]; // Códigos de los productos
    int num_productos; // Número de productos en la venta
    float importe_final; // Importe final de la venta
} vendidos;

typedef struct{
    max_digito cod;
    cad_char nombre;
    int cant;
    float p_unit, p_total;
}boleta_temporal;

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

/*PUNTO DE CAJA: global y uso primario del vendedor*/
void catalogo_productos(); // (7)
void transaccion_venta(); // (8)

/*ESTADÍSTICAS*/
void ventas_por_dia(); //(9)
void ranking_prod(); // (10)

/*CONFIG*/
void cambiar_pin(bool perfil_local); // (0)
void corregir_datos_huerfanos();

int main(){
    SetConsoleOutputCP(65001);
    corregir_datos_huerfanos();
    while(true){
        if (key_exist()){
            //login_state = login_page();
            if (login_page() == 0){
                menu_admin();
            }else if (login_state == 1){
                menu_vendor();
            }else if (login_state == 2){
                system("cls");
                cout << "ERROR CRITICO. Cerrando programa..."<<endl;
                system("pause");
                exit(0);
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
    ofstream vendidos_file;
    bool val;
    
    { //BIENVENIDA
        cout << "[Solo se mostrará una vez esta ventana]\a\n\n";
        cout <<"Le damos la bienvenida!\n\n";
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

    vendidos_file.open("vendido.dat", ios::binary);
    if (vendidos_file.is_open())
        vendidos_file.close();
    
    system("cls");
    cout << "== OPCION A RECUPERACIÓN DE PIN ==" << endl;
    cout << "Debido a que es imposible recuperar tu PIN si lo olvidas, deberás comunicarte con el desarrollador del programa para recuperar las bases de datos." << endl;
    //cout << endl << "Contacto: 2023019535@unfv.edu.pe"<<endl<<endl;
    system("pause");
}

bool login_page(){
    system("cls");
    unsigned short int opc;
    cout << "== INICIO DE SESIÓN DEL PROGRAMA =="<<endl << endl;
    cout << "Elige tu perfil:"<<endl;
    cout << "1. Administrador\n2. Vendedor"<<endl<<endl;
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
    bool perfil_local;
    while (key.read((char *)&llave, sizeof(llave))){
        if (llave.perfil == 0){
            nombre = llave.nombre;
            perfil_local = llave.perfil;
        }
    }
    key.close();

    int opc;
    do{
        system("cls");
        cout << "Bienvenido de vuelta, "<< nombre <<"."<<endl<<endl;
        cout << "== Gestionar Almacen ==\n1. Ver Inventario\n2. Modificar inventario...\n3. Stock de productos"<<endl<<endl;
        cout << "== Gestionar Clientes ==\n4. Lista de Clientes\n5. Buscar cliente...\n6. Modificar Clientes"<<endl<<endl;
        cout << "== Gestionar Punto de Caja ==\n7. Ver Catálogo de productos\n8. Generar transacción de Venta..."<<endl<<endl;
        cout << "== Estadísticas ==\n9.  Ventas realizadas por día\n10. Productos más vendidos"<<endl;
        cout << "\n11. Cambiar PIN...\n\n0. Cerrar Sesión"<<endl<<endl;
        cout << "Seleccione una opción: ";
        cin >> opc;

        switch (opc)
        {   
            case 0:
                cout << endl<<endl;
                cout << "Cerrando sesión. ¡Hasta luego, "<< nombre << "!" << endl;
                system("pause");
                login_state = 2;
                return;
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
            case 7:
                catalogo_productos();
                break;
            case 8:
                transaccion_venta();
                break;
            case 9:
                ventas_por_dia();
                break;
            case 10:
                ranking_prod();
                break;
            case 11:
                cambiar_pin(perfil_local);
                return;
            default:
                cout << "Opción no válida. Por favor, seleccione una opción válida."<<endl<<endl;
        }
    }while (opc >=0 && opc <=11);
}

void menu_vendor() {
    llave llave;
    fstream key;
    key.open("key.dat", ios::in | ios::binary);

    string nombre;
    bool perfil_local;
    while (key.read((char *)&llave, sizeof(llave))) {
        if (llave.perfil == 1) {
            nombre = llave.nombre;
            perfil_local = llave.perfil;
        }
    }
    key.close();
    int opc;
    do {
        system("cls");
        cout << "Bienvenido de vuelta, " << nombre << "." << endl << endl;
        cout << "== Gestionar Punto de Caja ==\n1. Ver Catalogo de productos\n2. Generar transaccion de Venta..." << endl << endl;
        cout << "== Gestionar Clientes ==\n3. Lista de Clientes\n4. Buscar cliente...\n5. Modificar Clientes" << endl << endl;
        cout << "6. Cambiar PIN" << endl;
        cout << "0. Cerrar Sesion" << endl << endl;
        cout << "Seleccione una opcion: ";
        cin >> opc;

        switch (opc) {
            case 0:
                cout << endl<<endl;
                cout << "Cerrando sesión. ¡Hasta luego, "<< nombre << "!" << endl;
                system("pause");
                login_state = 2;
                return;
            case 1:
                catalogo_productos();
                break;
            case 2:
                transaccion_venta();
                break;
            case 3:
                lista_clientes();
                break;
            case 4:
                buscar_cliente();
                break;
            case 5:
                modificar_clientes();
                break;
            case 6:
                cambiar_pin(perfil_local);
                return;
            default:
                cout << "Opción no válida. Por favor, seleccione una opción válida." << endl;
        }
    } while (opc >= 0 && opc <= 6);
}

void ver_inventario() {
    fstream almacen;
    almacen.open("inventario.dat", ios::in | ios::binary);
    inventario ver;

    system("cls");
    cout << "== INVENTARIO ==" << endl << endl;

    if (almacen.is_open()) {
        while (almacen.read((char*)&ver, sizeof(ver))) {
            cout << "Nombre: " << ver.nombre << endl;
            cout << "Cod.: " << ver.cod << endl;
            
            if (ver.cant == 1) {
                cout << "Stock: " << ver.cant << " unidad" << endl;
            } else {
                cout << "Stock: " << ver.cant << " unidades" << endl;
            }

            cout << fixed << setprecision(2) << "Precio Unit.: S/ " << ver.p_unit << endl<<endl;
        }

        almacen.close();
    } else {
        cout << "No se pudo abrir el archivo de inventario." << endl;
    }

    system("pause");
}

void modificar_inventario(){
    system("cls");
    int opc1;
    do{
        system("cls");
        cout << "== INVENTARIO =="<<endl<<endl;
        cout << "1. Ver Inventario\n2. Actualizar stock...\n3. Agregar Productos nuevos...\n4. Modificar Producto...\n5. Borrar Producto...\n0. Salir menú"<<endl<<endl;
        cout << "Seleccione una opcion: ";
        cin >> opc1;

        switch (opc1)
        {
        case 0:
            return;
        case 1:
            ver_inventario();
            break;
        case 2:
            actualizar_stock();
            break;
        case 3:
            agregar_producto();
            break;
        case 4:
            editar_producto();
            break;
        case 5:
            borrar_producto();
            break;
        }
    }while (opc1 >0 && opc1 <=5);
}

void actualizar_stock() {
    max_digito cod;
    int opcion;
    do {
        system("cls");
        cout << "== ACTUALIZAR STOCK DEL PRODUCTO ==" << endl << endl;
        cout << "Ingrese el codigo del producto a actualizar: ";
        cin >> cod;

        fstream archivo_in("inventario.dat", ios::in | ios::out | ios::binary);
        if (archivo_in.is_open()) {
            inventario aux;
            bool encontrado = false;
            streampos pos;

            while (archivo_in.read((char *)&aux, sizeof(inventario)) && !encontrado) {
                if (cod == aux.cod) {
                    encontrado = true;
                    pos = archivo_in.tellg() - streampos(sizeof(inventario));
                    system("cls");
                    cout << "== ACTUALIZAR STOCK DEL PRODUCTO ==" << endl << endl;
                    cout << "Producto: " << aux.nombre << endl << "Stock actual: " << aux.cant << endl << endl;

                    cout << "1. Agregar existencias" << endl;
                    cout << "2. Retirar existencias" << endl;
                    cout << "0. Salir" << endl << endl;
                    cout << "Ingrese su opción: ";
                    cin >> opcion;

                    switch (opcion) {
                        case 1: {
                            int agregar;
                            do {
                                cout << "Ingrese la cantidad a agregar: ";
                                cin >> agregar;
                                if (agregar <= 0) {
                                    cout << "La cantidad debe ser un número positivo. Inténtalo de nuevo:" << endl;
                                }
                            } while (agregar <= 0);

                            if (agregar > 0) {
                                aux.cant += agregar;
                                cout << endl << "Se ha agregado el stock satisfactoriamente.\a";
                            }
                            break;
                        }
                        case 2: {
                            int retirar;
                            do {
                                cout << "Ingrese la cantidad a retirar: ";
                                cin >> retirar;
                                if (retirar > 0 && retirar <= aux.cant) {
                                    aux.cant -= retirar;
                                    cout << endl << "Se ha retirado el stock satisfactoriamente.\a";
                                    break; 
                                } else {
                                    cout << endl << "⚠️La cantidad debe ser postiva y no debe exceder el stock actual.\a" << endl;
                                    cout << "Vuelva a intentarlo." << endl << endl;
                                }
                            } while (true); 
                            break;
                        }
                        case 0:
                            return;
                        default:
                            cout << "Opción inválida." << endl;
                    }

                    archivo_in.seekp(pos);
                    archivo_in.write((char*)&aux, sizeof(inventario));
                    archivo_in.seekg(archivo_in.tellp());
                }
            }

            archivo_in.close();

            if (!encontrado) {
                cout << "Código de producto no encontrado. Vuelta a intentarlo.";
            }
        } else {
            cout << "No se pudo abrir el archivo binario." << endl;
        }
        
        cout << endl << endl << "== ¿Desea actualizar otro producto? ==" << endl;
        cout << "Sí: 1\nNo: 0" << endl << ">_ ";
        cin >> opcion;
    } while (opcion != 0);
}

void agregar_producto() {
    unsigned short int num;
    inventario agregar;
    fstream almacen;

    almacen.open("inventario.dat", ios::in | ios::out | ios::binary);

    if (!almacen.is_open()) {
        cerr << "Error al abrir el archivo del inventario." << endl;
        return;
    }

    system("cls");
    cout << "Ingrese la cantidad de items a registrar: ";
    cin >> num;

    cin.ignore();  // Consumir el carácter de nueva línea después de ingresar num

    for (int i = 0; i < num; i++) {
        system("cls");
        cout << i + 1 << ". Producto" << endl;

        cout << "Nombre del producto (máximo 64 caracteres): ";
        cin.getline(agregar.nombre, 64);

        cout << "Codigo num. del Producto: ";
        cin >> agregar.cod;

        // Verificar si el código ya existe en el inventario
        bool codigoExistente = false;
        almacen.clear();  // Limpiar el estado del archivo antes de buscar
        almacen.seekg(0, ios::beg);

        inventario aux;  // Declarar la variable auxiliar aquí
        while (almacen.read((char *)&aux, sizeof(aux))) {  // Utilizar aux en lugar de agregar
            if (agregar.cod == aux.cod) {
                codigoExistente = true;
                break;
            }
        }

        if (codigoExistente) {
            cout << endl<< "¡Error! El código de producto ya existe. Intente nuevamente.\a" << endl;
            i--;
            system("pause");
            continue;
        }

        cout << "Stock inicial: ";
        cin >> agregar.cant;

        cout << "Precio unitario (sin IGV): S/ ";
        cin >> agregar.p_unit;
        agregar.p_unit = agregar.p_unit * 100;

        
        agregar.p_unit /= 100;

        
        almacen.clear();
        almacen.seekp(0, ios::end);

        almacen.write((char *)&agregar, sizeof(agregar));
        cout << endl<<endl<<"Se añadió satisfactoriamente." << endl;
        system("pause");
    }

    almacen.close();
}


void editar_producto() {
    system("cls");
    
    fstream archivo;

    archivo.open("inventario.dat", ios::in | ios::out | ios::binary);
    if (!(archivo.is_open())){ 
        system("cls");
        cout << "Error al abrir el archivo" << endl;
        system("pause");
        return;
    }

    inventario producto;
    max_digito codigo;
    
    cout << "Ingrese el código del producto a editar: ";
    cin >> codigo;
    cin.ignore();
    
    bool encontrado = false;
    while (archivo.read((char *)&producto, sizeof(producto))){
        if (producto.cod == codigo){
            encontrado = true;
            cout << "== Producto Encontrado =="<<endl;
            cout << "Nombre: "<<producto.nombre<<"\n Precio Unitario: "<<producto.p_unit<<endl<<endl;
            cout << "Ingrese los Nuevo datos."<<endl;
            cout << "Nombre del producto: ";
            cin.getline(producto.nombre,cad);
            cout << "Ingrese el nuevo precio: S/ ";
            cin >> producto.p_unit;
            cin.ignore();
            long pos = archivo.tellg();
            archivo.seekp(pos - sizeof(producto));
            archivo.write((char*)&producto, sizeof(producto));
            cout << "Producto actualizado correctamente." <<endl;
            system("pause");
            break;
        }
    }
    
    archivo.close();
    
    if (!encontrado) {
        system("cls");
        cout <<"No se ha encontrado un producto con ese código";
        system("pause");
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
    max_digito codigo;
    cout << "Ingrese el código del producto que vas a eliminar:"<<endl;
    cout << ">_ ";
    cin >> codigo;
    cin.ignore();

    bool encontrado = false;
    while (archivo.read((char*)&producto, sizeof(producto))){
        if ((producto.cod == codigo)){
            encontrado = true;
            cout << "Estas a punto de eliminar este producto:"<<endl<<endl;
            cout << "Nombre: "<< producto.nombre<<endl;
            cout << "Cod.: "<<producto.cod<<endl;
            cout << "Precio Unit.: S/ "<<producto.p_unit<<endl<<endl;

            cout << "Para continuar, digite 1, de lo contrario 0:\n>_ ";
            int confirmacion;
            cin >> confirmacion;
            cin.ignore();
            if (confirmacion == 1) {
                continue;
            }
        }
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
    max_digito codigo;
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
            cout << fixed << setprecision(2) << "Precio Unit.: S/ " << ver.p_unit << endl;
            cout << endl;
            system("pause");
            break; 
        }
    }

    if (encontrado == false) {
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
    max_digito documento;
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

void modificar_clientes() {
    while (true) {
        system("cls");
        int opc;
        cout << "== CLIENTES ==" << endl << endl;
        cout << "1. Buscar cliente...\n2. Agregar clientes...\n3. Actualizar clientes...\n4. Borrar cliente...\n0. Salir" << endl << endl;
        cout << "Seleccione una opcion: ";
        cin >> opc;

        switch (opc) {
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
                break;
            case 0:
                cout << "Saliendo del menú de clientes..." << endl;
                return;
            default:
                cout << "Opción no válida. Por favor, seleccione una opción válida." << endl;
        }
        
        system("pause");
    }
}


void agregar_clientes() {
    unsigned short int num;
    clientes agregar;
    fstream lista_clientes;
    lista_clientes.open("clientes.dat", ios::app | ios::binary);

    if (lista_clientes.is_open()) {
        system("cls");
        cout << "Ingrese la cantidad de clientes a registrar: ";
        cin >> num;
        system("cls");

        for (int i = 0; i < num; i++) {
            system("cls");
            cin.sync();
            cout << "Nombre del Cliente (máximo 64 caracteres): ";
            cin.getline(agregar.nombre_cliente, 64);

            cout << "Tipo:\n0 = Persona, 1 = Empresa\n> ";
            cin >> agregar.tipo;

            if (agregar.tipo == 0) {
                cout << "Ingresa su DNI: ";
                cin >> agregar.doc;
            } else {
                cout << "Ingresa su RUC: ";
                cin >> agregar.doc;
            }

            // Verificar si el cliente ya existe
            bool cliente_existente = false;
            max_digito nuevo_doc;
            cout << "Ingresa su DNI o RUC nuevamente para confirmar: ";
            cin >> nuevo_doc;

            lista_clientes.clear();
            lista_clientes.seekg(0);

            while (lista_clientes.read((char*)&agregar, sizeof(agregar))) {
                if (strcmp(agregar.nombre_cliente, agregar.nombre_cliente) == 0 && agregar.doc == nuevo_doc) {
                    cliente_existente = true;
                    break;
                }
            }

            if (cliente_existente) {
                cout << "⚠️ Este cliente ya existe.\a" << endl;
            } else {
                lista_clientes.clear();
                lista_clientes.seekg(0, ios::end);

                lista_clientes.write((char*)&agregar, sizeof(agregar));
                cout << "✅ Se añadió satisfactoriamente." << endl;
            }
        }

        lista_clientes.close();
    }
}




void editar_cliente() {
    system("cls");
    fstream archivo;
    archivo.open("clientes.dat", ios::in | ios::out | ios::binary);

    if (!archivo.is_open()) {
        cout << "🚨 Error al abrir el archivo.\a" << endl;
        system("pause");
        return;
    }

    clientes cliente;
    max_digito documento;
    cout << "Ingrese el número de documento del cliente que desea editar: ";
    cin >> documento;
    cin.ignore();

    while (archivo.read((char*)&cliente, sizeof(clientes))) {
        if (cliente.doc == documento) {
            cout << "Cliente encontrado. Ingrese los nuevos datos." << endl;

            cout << "Nombre del cliente: ";
            cin.getline(cliente.nombre_cliente, cad);

            long pos = archivo.tellg();
            archivo.seekp(pos - sizeof(clientes));
            archivo.write((char*)&cliente, sizeof(clientes));

            cout << "✅ Datos actualizados correctamente." << endl;
            system("pause");
            break;
        }
    }
    archivo.close();
}


void borrar_cliente(){
    system("cls");
    fstream archivo, temp;

    archivo.open("clientes.dat", ios::in | ios::binary);
    temp.open("temp.dat", ios::out | ios::binary);

    if (!archivo.is_open()) {
        cout << "Error al abrir el archivo." << endl;
        system("pause");
        return;
    }

    clientes cliente;
    max_digito documento;
    cout << "Ingrese el numero de documento del cliente que desea eliminar: ";
    cin >> documento;
    cin.ignore();

    bool encontrado = false;
    while (archivo.read((char*)&cliente, sizeof(clientes))) {
        if (cliente.doc == documento) {
            encontrado = true;
            cout << "Deseas eliminar a:\n";
            cout << "Nombre: " << cliente.nombre_cliente << endl;
            if(cliente.tipo == 0){
                cout << "DNI: " << cliente.doc << endl;
            } else{
                cout << "RUC: " << cliente.doc << endl;
            }
            cout << "Para continuar digite 1, de lo contrario 0:\n> ";
            int confirmacion;
            cin >> confirmacion;
            cin.ignore();
            if (confirmacion == 1) {
                continue;
            }
        }
        temp.write((char*)&cliente, sizeof(clientes));
    }

    archivo.close();
    temp.close();

    remove("clientes.dat");
    rename("temp.dat", "clientes.dat");

    if (encontrado) {
        cout << "✅ Se eliminó exitosamente." << endl;
    } else {
        cout << "⚠️ Cliente no encontrado.\a" << endl;
    }
    system("pause");
}


void catalogo_productos() {
    system("cls");

    cout << "== CATÁLOGO DE PRODUCTOS ==" << endl;

    fstream archivo_inventario("inventario.dat", ios::in | ios::binary);

    if (archivo_inventario.is_open()) {
        inventario producto;

        while (archivo_inventario.read((char *)&producto, sizeof(inventario))) {
            cout << "* " << producto.nombre << " (" << producto.cant << " uni) - S/ " << fixed << setprecision(2) << producto.p_unit << endl;
            cout << "  Código: " << producto.cod << endl;
            cout << "---------------------------------------------" << endl;
        }

        archivo_inventario.close();
    } else {
        cout << "No se pudo abrir el archivo de inventario." << endl;
    }

    system("pause");
}

void transaccion_venta() {
    inventario producto;
    clientes cliente;
    int opcion;
    float importe_total = 0.00;
    int vendido = 0;
    fstream archivo_temp;
    const int max_cod_vendidos = 30; // Máximo 30 productos para vender
    char codigos_productos[max_cod_vendidos][10];
    int num_codigos = 0;
    system("copy inventario.dat buckup_inventario.dat");

    system("cls");
    cout << "== GENERAR VENTA ==" << endl
         << endl;
    cout << "Ingrese el documento del cliente: ";
    cin >> cliente.doc;
    cin.ignore();

    fstream archivo_cliente("clientes.dat", ios::in | ios::binary);
    if (archivo_cliente.is_open()) {
        clientes aux_cliente;
        bool encontrado_cliente = false;
        while (archivo_cliente.read((char *)&aux_cliente, sizeof(clientes)) && !encontrado_cliente) {
            if (cliente.doc == aux_cliente.doc) {
                encontrado_cliente = true;
                cliente = aux_cliente;  // Copiar la información del cliente
            }
        }
        archivo_cliente.close();
        if (!encontrado_cliente) {
            cout << "Documento de cliente no encontrado. Inténtalo de nuevo o ingresa 0 para salir." << endl;
            cin >> opcion;
            if (opcion == 0) {
                return;
            }
        }
    } else {
        cout << "No se pudo abrir el archivo de clientes." << endl;
        system("pause");
        return;
    }

    do {
        cout << "Ingrese el código del producto: ";
        cin >> producto.cod;

        cout << "Ingrese la cantidad: ";
        cin >> producto.cant;

        fstream archivo_in("inventario.dat", ios::in | ios::out | ios::binary);
        if (archivo_in.is_open()) {
            inventario aux; // Leer
            bool encontrado = false; // Ver si existe el producto
            streampos pos;

            while (archivo_in.read((char *)&aux, sizeof(inventario)) && !encontrado) {
                if (producto.cod == aux.cod) {
                    system("cls");
                    cout << "== GENERAR VENTA ==" << endl
                         << endl;
                    encontrado = true;
                    vendido = vendido + producto.cant;

                    cout << "Nombre del producto: " << aux.nombre << endl;
                    cout << "Precio unitario: S/ " << aux.p_unit << endl;

                    if (vendido <= aux.cant) {
                        aux.cant -= vendido;
                        pos = archivo_in.tellg() - streampos(sizeof(inventario));
                        archivo_in.seekp(pos);
                        archivo_in.write((char *)&aux, sizeof(inventario));
                        archivo_in.seekg(archivo_in.tellp());
                        float total_producto = producto.cant * aux.p_unit;

                        cout << "Total del producto: S/ " << total_producto << endl;
                        importe_total += total_producto;
                    } else {
                        cout << endl << "⚠️ No hay suficiente stock para la cantidad solicitada.\a" << endl;
                    }
                }
            }

            archivo_in.close();

            if (!encontrado) {
                cout << endl << "No se encontró el producto con el código ingresado." << endl;
            }
        } else {
            cout << "No se pudo abrir el archivo binario." << endl;
            system("pause");
            return;
        }
        sprintf(codigos_productos[num_codigos], "%lld", producto.cod);
        num_codigos++;

        cout << endl << "Desea agregar más productos 1: Sí, 0: No: \a";
        cin >> opcion;
    } while (opcion == 1);

    system("cls");
    float igv = importe_total * 0.18;
    igv = roundf(igv * 100) / 100;
    cout << "== GENERAR VENTA ==" << endl
         << endl;
    cout << "Cliente: " << cliente.nombre_cliente << endl;
    cout << "Importe total: S/ " << fixed << setprecision(2) << importe_total << endl;
    cout << "IGV (18%): S/ " << igv << endl;
    float importe_final = (importe_total + igv);
    importe_final = roundf(importe_final * 100) / 100;

    cout << "Importe final: S/ " << importe_final << endl
         << endl;

    cout << "1. Procesar venta" << endl;
    cout << "0. Cancelar venta" << endl;
    cout << "Ingrese su opción: ";
    cin >> opcion;

    if (opcion == 1) {

        time_t t = time(0);
        tm *now = localtime(&t);

        fstream archivo_vendido("vendidos.dat", ios::app | ios::binary);
        if (archivo_vendido.is_open()) {
            vendidos venta;
            strftime(venta.fecha, sizeof(venta.fecha), "%d/%m/%Y", now);
            strftime(venta.hora, sizeof(venta.hora), "%H:%M:%S", now);

            venta.doc_cliente = cliente.doc;
            venta.num_productos = num_codigos;

            for (int i = 0; i < num_codigos; i++) {
                venta.cod_productos[i] = strtoull(codigos_productos[i], NULL, 10);
            }

            venta.importe_final = importe_final;

            archivo_vendido.write((char *)&venta, sizeof(vendidos));
            archivo_vendido.close();

        } else {
            cout << "No se pudo abrir el archivo de ventas." << endl;
            system("pause");
            return;
        }

        remove("buckup_inventario.dat");
        remove("boleta_temporal.dat");
        cout << endl << "Venta procesada con éxito." << endl;
        system("pause");

    } else if (opcion == 0) {
        remove("inventario.dat");
        rename("buckup_inventario.dat", "inventario.dat");
        remove("boleta_temporal.dat");
        system("cls");
        cout << "== VENTA CANCELADA ==\a" << endl;
        system("pause");
    } else {
        cout << "\nOpción inválida.\a" << endl;
        system("pause");
    }
}


void ventas_por_dia(){
    system("cls");
    char fecha_consulta[11];
    vendidos venta;

    cout << "Ingrese la fecha de consulta (dd/mm/yyyy): ";
    cin >> fecha_consulta;

    fstream archivo_venta("vendidos.dat", ios::in | ios::binary);
    if (archivo_venta.is_open()) {
        while (archivo_venta.read((char*)&venta, sizeof(vendidos))) {
            if (strcmp(venta.fecha, fecha_consulta) == 0) {
                cout << "Fecha: " << venta.fecha << endl;
                cout << "Hora: " << venta.hora << endl;
                cout << "Documento del cliente: " << venta.doc_cliente << endl;
                cout << "Productos vendidos: ";
                for (int i = 0; i < venta.num_productos; i++) {
                    cout << venta.cod_productos[i] << " ";
                }
                cout << endl;
                cout << "Importe final: " << fixed <<setprecision(2)<< venta.importe_final << endl;
                cout << endl;
            }
        }
        archivo_venta.close();
    } else {
        cout << "No se pudo abrir el archivo de ventas." << endl;
        system("pause");
    }
    system("pause");
}

void ranking_prod() {
    system("cls");

    cout << "== PRODUCTOS MÁS VENDIDOS ==" << endl;

    fstream archivo_vendido("vendidos.dat", ios::in | ios::binary);

    if (archivo_vendido.is_open()) {
        const int max_productos = 30;
        int codigos[max_productos] = {0};

        vendidos venta;

        string nombres[max_productos];
       for (int i = 0; i < max_productos; i++) {
		    nombres[i] = "";
		}

        fstream archivo_inventario("inventario.dat", ios::in | ios::binary);
        if (archivo_inventario.is_open()) {
            inventario producto;
            while (archivo_inventario.read((char *)&producto, sizeof(inventario))) {
                int codigo = producto.cod;
                nombres[codigo - 1] = producto.nombre;
            }
            archivo_inventario.close();
        } else {
            cout << "No se pudo abrir el archivo de inventario." << endl;
            return;
        }

        while (archivo_vendido.read((char *)&venta, sizeof(vendidos))) {
            for (int i = 0; i < venta.num_productos; i++) {
                max_digito codigo_producto = venta.cod_productos[i];
                codigos[codigo_producto - 1]++;
            }
        }

        archivo_vendido.close();

        int posicion = 1;
        for (int i = 0; i < max_productos; i++) {
            if (!nombres[i].empty() && codigos[i] > 0) {
                cout << posicion << "° Producto" << endl;
                cout << "Nombre: " << nombres[i] << " (Código: " << i + 1 << ")" << endl;

                if (codigos[i] == 1) {
                    cout << "Unidad vendida: 1 unidad" << endl;
                } else {
                    cout << "Unidades vendidas: " << codigos[i] << " unidades" << endl;
                }

                cout << "---------------------------------------------" << endl;
                posicion++;
            }
        }
    } else {
        cout << "No se pudo abrir el archivo de ventas." << endl;
    }

    system("pause");
}

void cambiar_pin(bool perfil_local) {
    unsigned long nuevo_pin;
    
    system("cls");
    cout << "== CAMBIAR PIN ==" << endl << endl;

    if (perfil_local == 0) {
        cout << "Vamos a cambiar el PIN del administrador..." << endl;
    } else if (perfil_local == 1) {
        cout << "Vamos a cambiar el PIN del vendedor..." << endl;
    }

    cout << "Ingrese el nuevo PIN (6 dígitos): ";
    cin >> nuevo_pin;

    if (nuevo_pin >= 100000 && nuevo_pin <= 999999) {
        fstream key("key.dat", ios::in | ios::out | ios::binary);
        if (key.is_open()) {
            llave acceso;
            bool encontrado = false;
            streampos pos;

            while (key.read((char*)&acceso, sizeof(llave)) && !encontrado) {
                if (perfil_local == acceso.perfil) {
                    encontrado = true;
                    pos = key.tellp() - streampos(sizeof(llave));
                    acceso.pin = nuevo_pin;
                    cout << endl << "Se ha cambiado el PIN satisfactoriamente.\a" << endl;

                    key.seekp(pos, ios::beg);
                    key.write((char*)&acceso, sizeof(llave));
                    key.seekg(key.tellp());
                }
            }

            key.close();

            if (!encontrado) {
                cout << "No se encontró el perfil en el archivo de claves. Inténtalo de nuevo." << endl;
            }
        } else {
            cout << "No se pudo abrir el archivo de claves." << endl;
        }
    } else {
        cout << "⚠️ El PIN debe ser un número de 6 dígitos. No se ha realizado ningún cambio." << endl;
    }

    system("pause");
}

void corregir_datos_huerfanos(){
    if (ifstream("buckup_inventario.dat")) {
        remove("inventario.dat");
        rename("buckup_inventario.dat", "inventario.dat");
        remove("buckup_inventario.dat");
    }else{
        return;
    }

}
