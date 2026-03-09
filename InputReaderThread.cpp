#include "InputReaderThread.hpp"

void readInputThread(std::atomic<bool>& exit)
{
    std::stringstream ss;
    ss << std::this_thread::get_id();
    LogToFile("[ReadInput] Thread " + ss.str() + ".");

    while (!exit) {
        if (GetAsyncKeyState(VK_RCONTROL) & 0x8000) {

            std::cout << "\n[INPUT] Modalita' inserimento IP manuale" << std::endl;
            std::cout << "Digita l'IP (es. 192.168.1.10) e premi Invio: ";

            std::string newIp;
            if (std::cin >> newIp) {

                sockaddr_in sa;
                ZeroMemory(&sa, sizeof(sa)); // Puliamo la struct
                sa.sin_family = AF_INET;
                sa.sin_port = htons(9999); // La porta che usano i tuoi client

                // Convertiamo la stringa in indirizzo IP reale
                int result = inet_pton(AF_INET, newIp.c_str(), &(sa.sin_addr));

                if (result == 1) {
                    // CHIAMATA AL TUO DATAMODEL
                    // Passiamo direttamente la struct sockaddr_in come vuole la tua funzione
                    DataModel::getInstance().addClient(sa);
                }
                else {
                    std::cout << "[ERRORE] IP non valido: " << newIp << std::endl;
                }
            }

            // Pulizia del buffer per evitare che il 'cin' faccia casino al prossimo giro
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
