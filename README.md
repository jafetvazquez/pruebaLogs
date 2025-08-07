# pruebaLogs

conectar db con vs community (c++)

1.- tener instalado pgadmin4 (version 16)
2.- tener instalado visual studio community
3.- crear tabla en db para logs (campos = id, mensaje, dia, mes, anio, tienda)
4.- abrir powershell como admin, ir a cd C:\
5.- usar comandos git clone https://github.com/microsoft/vcpkg.git
            cd vcpkg
            .\bootstrap-vcpkg.bat

6.- comando .\vcpkg integrate install
7.- comando .\vcpkg install libpqxx:x64-windows
8.- comando .\vcpkg install crow:x64-windows

9.- código:
#include <iostream>
#include <pqxx/pqxx>

int main() {
    try {
        // conectar db
        pqxx::connection C(CONNECTION_STRING);

        // creacion de transaccion
        pqxx::work W(C);

        // sentecia SQL para mostrar version
        pqxx::result R = W.exec("SELECT version();");

        for (const auto& row : R) {
            std::cout << "Versión del servidor: " << row[0].as<std::string>() << std::endl;
        }
    }
    catch (const std::exception& e) {
        // capturar errores
        std::cerr << "Error al conectar: " << e.what() << std::endl;

    }

    return 0; // Devuelve un código de éxito
}


10.- solucion > propiedades > general > cambiar version de c++ a 20
11.- iniciar sin depurar
12.- usar funciones dependiendo caso
13.- usar web app con Python, usar 

	python -m http.server 1000 

	desde carpeta donde esta index.html



paginas de documentación 
https://crowcpp.org/1.2.1/guides/auth/
https://github.com/jtv/libpqxx/blob/master/include/pqxx/doc/prepared-statement.md
https://libpqxx.readthedocs.io/stable/index.html
https://crowcpp.org/1.2.1/reference/structcrow_1_1_c_o_r_s_rules.html#details
