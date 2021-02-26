#include <cstdio>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <sys/file.h>


void plus() {
    FILE *file = fopen("test.txt", "r");
    if (!file) {
        std::cout << "Cant open file" << std::endl;
    }
    int r = flock(fileno(file), LOCK_EX);
    std::cout << r << std::endl;
    int index;
    fscanf(file,"%d", &index);
    fclose(file);
    FILE *file_ = fopen("test.txt", "w+");
    fprintf(file_, "%d", index + 1);
    flock(fileno(file_), LOCK_UN);
    fclose(file_);
}

void  minus(bool is_for_clear = false) {
    FILE *file = fopen("test.txt", "r");
    if (!file) {
        std::cout << "Can't open file" << std::endl;
    }
    int r = flock(fileno(file), LOCK_EX);
    std::cout << r << std::endl;
    int index;
    fscanf(file,"%d", &index);
    fclose(file);
    FILE *file_ = fopen("test.txt", "w+");
    flock(fileno(file_), LOCK_UN);
    if (is_for_clear)
        fprintf(file_, "%s", "0");
    else
        fprintf(file_, "%d", index - 1);
    fclose(file_);
}

int get_current_value() {
    FILE *file = fopen("test.txt", "r");
    if (!file) {
        std::cout << "Cant open file" << std::endl;
    }
    int r = flock(fileno(file), LOCK_EX);
    std::cout << r << std::endl;
    int index;
    fscanf(file,"%d", &index);
    fclose(file);
    return index;
}


// при использовании using namespace std появляется ошбика
class server {
private:
    struct sockaddr_in client;
    socklen_t client_len;
    int port;
    int socket_connect;
    struct sockaddr_in server_addr;
    int client_sock;
    bool created;
public:
    // functions
    server(int port);
    void create_socket();
    void printERR(std::string error, bool critical);
    int listen_on_port();
    bool accept_new_connection();
    void send_hello();
    int request_();
    int close_client_sock();
    void close_socket_connect();
};


server::server(int port) {
    this->port = port;
    this->socket_connect = socket(AF_INET, SOCK_STREAM, 0); // create socket
    if (socket_connect == -1)
        printERR("Failed to create socket", true);
    }

void server::printERR(std::string error, bool critical) {
    std::cout << error << std::endl;
    if (critical)
        this-> created = false;
}

int server::listen_on_port(){
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    int enable = 1;
    if (setsockopt(socket_connect, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        printERR("Failed to set reuse option", true);
    }
    int err = bind(socket_connect, (struct sockaddr*)&server_addr,  sizeof(server_addr));
    if (err == -1) {
        printERR("Failed to bind on port " + std::to_string(port), true);
    }
    int backlog = 100;
    err = listen(socket_connect, backlog);
    if (err == -1) {
        printERR("Failed  to listen on port" +  std::to_string(port), true);
    }
    std::cout << "Listen on port: " << port << std::endl;
    return 0;
}


void server::send_hello() {
    for (int i=0; i<50; i++)
    {
        const char* hello =  "-";
        write(client_sock, hello, strlen(hello));
    }
    const char* hello =  "\nwelcome to my tcp server, Bru!\n print B to get: \n bitcoin course \n $ to get dollar course \n print e to exit \n";
    write(client_sock, hello, strlen(hello));
    for (int i=0; i<50; i++)
    {
        const char* hello_ =  "-";
        write(client_sock, hello_, strlen(hello_));
    }
    const char* last =  "\n";
    write(client_sock, last, strlen(last));
}


bool server::accept_new_connection(){
    // Обработка и взятие из очереди 1 подключения !
    client_len = sizeof(client);
    client_sock = accept(socket_connect,(struct sockaddr*)&client, &client_len);
    if (client_sock < 0){
        printERR("Accept failed" , true);
        return false;
    }
    std::cout << "Accepted new connection" << std::endl;
    return true;
}
int server::request_() {
    send_hello();
    char c_request[50] = {0};
    while (c_request[0]!='e'){
        read(client_sock, &c_request, 50);
        std::string request(c_request);
        std::cout << "Request: " << request << std::endl;
        const char* c_response;
        if (request[0] == 'B')
            c_response  = "13.11 23:15 1btc = 6466$";
        else
            if (request[0] == '$')
                c_response  = "13.11 23:15 1$ = 68.08₽";
            else
                if (request[0] == 'e')
                {
                    c_response  = "bye";
                    minus();
                }
                    else
                    c_response = "Repeat, please";
                    //c_response = (std::to_string(get_current_value())).c_str();
        //const char* c_response  = request.c_str();
        write(client_sock, c_response, strlen(c_response));
        write(client_sock, "\n", 2);
    }
    return 0;
}


int server::close_client_sock()
{
    if (close(client_sock) == -1)
    {
        printERR("Close error" , true);
        return -1;
    }
    return close(client_sock);
}


void server::close_socket_connect(){
    if (close(socket_connect) == -1)
        printERR("Close error", true);

}
int convert_args(char *arr) {
    int out=0;
    int i = 0;
    while(int(arr[i])!=0)
    {
        out*=10;
        out+=int(arr[i])-48;
        i++;
    }
    return out;
}


int main(int argc, char* argv[]) {
    minus(true);
    int port = convert_args(argv[1]);
    server s(port);
    s.listen_on_port();
    int index = 0;
    while (index < 10) {
        if (get_current_value() < 3)
            if (s.accept_new_connection())
                    {
                        index++;
                        plus();
                        int pid = fork();
                        if (pid == 0) {
                            s.request_();
                            break;
                        }
                s.close_client_sock();
                    }
                }
    s.close_socket_connect();
    return 0;
}
