#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include <crow.h>

// data de db 
const std::string CONNECTION_STRING = "dbname=postgres user=postgres password=123 host=localhost port=5432";

// funcion muestra conexion a la base de datos
void consultarDB() {
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
}

// funcion para enviar logs a postgres
void enviarLog(const std::string& mensaje, int dia, int mes, int anio, int tienda) {
    try {
        // conectar db
        pqxx::connection C(CONNECTION_STRING);

        // sentecia SQL para insertar el log
        C.prepare("insertarLog", "INSERT INTO logmsgs (mensaje, dia, mes, anio, tienda) VALUES ($1, $2, $3, $4, $5);");

        // creacion de transaccion
        pqxx::work W(C);

        // consulta parametrizada
        W.exec(pqxx::prepped{ "insertarLog" }, pqxx::params{mensaje, dia, mes, anio, tienda});
        
        // confirma transaccion
        W.commit();

		// mostrar datos ingresados
        std::cout << "Log guardado en PostgreSQL: Mensaje='" << mensaje << "', Tienda=" << tienda << ", Fecha=" << dia << "/" << mes << "/" << anio << std::endl;
    } catch (const std::exception& e){
        // capturar errores
        std::cerr << "Error al guardar log en PostgreSQL: " << e.what() << std::endl;
        
    }
}

// funcion para solicitar data desde consola
void insertarDataConsola() {
    std::string mensaje;
    int dia, mes, anio, tienda;

    std::cout << "mensaje: ";
    std::getline(std::cin, mensaje);

    std::cout << "dia: ";
    std::cin >> dia;

    std::cout << "mes: ";
    std::cin >> mes;

    std::cout << "anio: ";
    std::cin >> anio;

    std::cout << "tienda: ";
    std::cin >> tienda;

    // Limpiar el buffer en caso de que se use getline después de cin
    std::cin.ignore();

    enviarLog(mensaje, dia, mes, anio, tienda);
}

// funcion con crow app
void webApp() {
    // aplicación crow
    crow::SimpleApp app;

    // CORS para options
    CROW_ROUTE(app, "/log").methods("OPTIONS"_method)
        ([&](const crow::request& req) {
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "*");
        res.add_header("Access-Control-Allow-Headers", "*");
        res.add_header("Access-Control-Max-Age", "86400"); // Cache preflight por 24 horas
        res.code = 200;
        res.body = "";
        return res;
     });

    // definir endpoint en /log (json)
    CROW_ROUTE(app, "/log").methods("POST"_method)
        ([&](const crow::request& req) { // mtodo POST
        // habilitar cors
        crow::response res;
        res.add_header("Access-Control-Allow-Origin", "*"); // permitir cualquier origen
        res.add_header("Access-Control-Allow-Headers", "*");

        // manejo de peticiones
        if (req.method == crow::HTTPMethod::OPTIONS) {
            res.code = 200; // respuesta OK
            return res;
        }

        // cargar peticion json
        auto json_data = crow::json::load(req.body);

        // verificar si json es valido
        if (!json_data || !json_data.count("mensaje") || !json_data.count("dia") ||
            !json_data.count("mes") || !json_data.count("anio") || !json_data.count("tienda")) {

            res.code = 400; // bad request
            res.set_header("Content-Type", "application/json");
            res.write("{\"message\": \"Datos incompletos o inválidos\"}");
            return res;
        }

        try {
            // extraer data de json
            std::string mensaje = json_data["mensaje"].s();
            int dia = static_cast<int>(json_data["dia"].i());
            int mes = static_cast<int>(json_data["mes"].i());
            int anio = static_cast<int>(json_data["anio"].i());
            int tienda = static_cast<int>(json_data["tienda"].i());

            // llamar funcion para guardar log
            enviarLog(mensaje, dia, mes, anio, tienda);

            res.code = 200; // ok
            res.set_header("Content-Type", "application/json");
            res.write("{\"message\": \"Log guardado correctamente\"}");
            return res;
        }
        catch (const std::exception& e) {
            std::cerr << "Error al procesar la solicitud: " << e.what() << std::endl;
            res.code = 500; // error interno
            res.set_header("Content-Type", "application/json");
            res.write("{\"message\": \"Datos incompletos o inválidos\"}");
            return res;
        }

    });

    // establecer puerto
    app.port(18080).multithreaded().run();

    std::cout << "Servidor C++ iniciado en http://localhost:18080" << std::endl;
    std::cout << "Esperando peticiones..." << std::endl;

}

// funicion para mostra registros
void mostrarLogs() {
    try {
        // Intentar conectar a la base de datos
        pqxx::connection C(CONNECTION_STRING);

        // conuslta
        pqxx::work W(C);
        pqxx::result R = W.exec("SELECT * FROM logmsgs;");

        // Encabezado
        std::cout << std::left << std::setw(30) << "mensaje"
            << std::setw(5) << "dia"
            << std::setw(5) << "mes"
            << std::setw(7) << "anio"
            << std::setw(10) << "tienda" << "\n";

        std::cout << std::string(80, '-') << "\n";

        for (const auto& row : R) {
            std::string mensaje = row[1].as<std::string>();
            std::string dia = row[2].as<std::string>();
            std::string mes = row[3].as<std::string>();
            std::string anio = row[4].as<std::string>();
            std::string tienda = row[5].as<std::string>();

            std::cout << std::left << std::setw(30) << mensaje
                << std::setw(5) << dia
                << std::setw(5) << mes
                << std::setw(7) << anio
                << std::setw(10) << tienda << "\n";
        }

    }
    catch (const std::exception& e) {
        // Si hay un error, se captura y se imprime un mensaje
        std::cerr << "Error al conectar a la base de datos: " << e.what() << std::endl;
    }

}


int main() {
    // Ejecucion de funciones
	std::cout << "Ejecutando main() " << std::endl;

    // funcion de version
    //consultarDB();

    // enviar log mediante parámetros en función
	//enviarLog("Mensaje de prueba", 15, 10, 2025, 290);

    // enviar log desde consola
    //insertarDataConsola();

    // enviar log desde aplicacion web
    //webApp();

    // mostrar logs
    mostrarLogs();

    return 0; // Devuelve un código de éxito
}
