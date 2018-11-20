#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>

struct sockaddr_in cliente, servidor;

socklen_t tamanho_cliente;

int cliente_socket,aux;


void servidor_FilaTarefas (int porta){

    int socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

    // Cria o socket
    if(socket==0){
        perror("Erro ao abrir o Socket");
        exit(EXIT_FAILURE);
    }

    // Auxilia em problemas de reuso da porta forcando o socket se ligar a porta 8080
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){ 
        perror("Erro no uso da funcao 'setsockopt'"); 
        exit(EXIT_FAILURE); 
    } 

    //Protcolo para trabalhar com internet / Endereco de conexao / Porta de conexao
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta);


    //Ligando a porta passa pelo usuario ao socket 
    if (bind(server_fd, (struct sockaddr *)&servidor,sizeof(servidor))<0){ 
        perror("Erro ao se conectar a porta do cliente"); 
        exit(EXIT_FAILURE); 
    } 


    //Dtermina o tamanho da fila de requisicoes que o servidor pode receber sem perdas de requisicoes.
    listen(socket,50);
    tamanho_cliente = sizeof(cliente);

    while(1){
        //Realiza a conexao do servidor com cliente
        cliente_conect = accept(socket,(struct sockaddr *) &cliente, &tamanho_cliente);

        if(cliente < 0){
            printf("Conecatdo com sucesso !");
        }
        else{
            perror("Erro ao se conectar !")
        }

        aux = read (cliente_conect, mensagem, 1024);

        if(aux < 0){
            printf("Requisicao do cliente:\n------------------------------------------------------\n%s\n",mensagem);
        }
        else(
            perror("Erro ao ler a mensagem do cliente!")
        )

    }

}

int main(int argc, char const *argv[]){

    int porta = atoi (argv[2]);

    servidor_FilaTarefas(porta);
    



    
    
    return 0;
}
