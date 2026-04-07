#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    const std::string thermal_path = "/sys/class/thermal/thermal_zone0/temp";

    while (true) {
        std::ifstream file(thermal_path);
        
        if (!file.is_open()) {
            std::cerr << "Erreur : Impossible d'acceder au capteur." << std::endl;
            return 1;
        }

        std::string raw_temp;
        std::getline(file, raw_temp);
        file.close();

        try {
            double temp = std::stod(raw_temp) / 1000.0;
            std::cout << "CPU Temp: " << temp << " °C" << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Erreur de conversion : " << e.what() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    return 0;
}
