#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

struct sockaddr_in cliente, servidor;

socklen_t tamanho_cliente;

int cliente_socket = 0,aux = 0 ,cliente_thread = 0,id_thread = 0,cliente_conect = 0;
char mensagem[2048];

void resposta_servidor(int cliente);
void *cria_thread(void *cliente_thread);


void servidor_FilaTarefas (int porta){

    int new_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    pthread_t nova_thread [8]; // intel core i5 possui dois 4 nucleos reais e simula mais 4 virtuais

    // Cria o socket
    if(new_socket < 0){
        perror("Erro ao abrir o Socket");
        exit(EXIT_FAILURE);
    }

    // Auxilia em problemas de reuso da porta forcando o socket se ligar a porta 8080
    if (setsockopt(new_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))){ 
        perror("Erro no uso da funcao 'setsockopt'"); 
        exit(EXIT_FAILURE); 
    } 

    //Protcolo para trabalhar com internet / Endereco de conexao / Porta de conexao
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(porta);


    //Ligando a porta passa pelo usuario ao socket 
    if (bind(new_socket, (struct sockaddr *)&servidor,sizeof(servidor)) < 0){ 
        perror("Erro ao se conectar a porta do cliente"); 
        exit(EXIT_FAILURE); 
    } 


    //Dtermina o tamanho da fila de requisicoes que o servidor pode receber sem perdas de requisicoes.
    listen(new_socket,50);
    tamanho_cliente = sizeof(cliente);

    while(1){
        //Realiza a conexao do servidor com cliente
        cliente_conect = accept(new_socket,(struct sockaddr *) &cliente, &tamanho_cliente);

        if(cliente_conect >= 0){
            printf("Conecatdo com sucesso !\n");
        }
        else{
            perror("Erro ao se conectar !");
        }

        bzero(mensagem,2048);

        if(id_thread == 4) id_thread = 0;

        aux = read (cliente_conect, mensagem, 2048);

        if(aux >= 0){
            printf("Requisicao do cliente:\n------------------------------------------------------\n%s\n",mensagem);
        }
        else{
            perror("Erro ao ler a mensagem do cliente!");
        }
        // Atribui a requisicao do cliente a uma thread
        cliente_thread = pthread_create(&nova_thread[id_thread], NULL, cria_thread, &cliente_conect);

        if(cliente_thread != 0) 
            perror("ERRO na criação de uma nova thread.");

        pthread_join(nova_thread[id_thread], NULL);
        id_thread++;  
        printf("ID : %d\n",id_thread);     

    }

    close(new_socket);

}

// Funcao ponteiro void responsavel pela thread
void *cria_thread(void *cliente_conect){
    int *cliente_porta = (void *) cliente_conect;
   
    resposta_servidor(*cliente_porta);
    close(*cliente_porta);
    pthread_exit(NULL);
}

// Responsavel por responder a requisicao do cliente
void resposta_servidor(int cliente_conect){	
    int situacao;    

    char resposta [1024] = "HTTP/1.1 200 OK\n\
    Date: Mon, 26 Oct 2018 23:59:59 GMT\n\
    Server: Sam007 (Unix)  (Ubuntu/Linux)\n\
    Accept-Ranges: bytes\n\
    Content-Length: 170\n\
    Connection: close\n\
    Content-Type: text/html; charset=UTF-8\n\n\
    <!DOCTYPE html><html>\n\
    <head>\n\
    <title>FilaDeTarefas</title>\n\
    </head>\n\
    <body>\n\
    <h1>Conectado com Sucesso ! <h1>\n\
    </body>\n\
    </html>\r\n";

    //Resposta ao cliente caso a conexao tenha sucesso
    situacao = write(cliente_conect,resposta,sizeof(resposta)); 
   	
        
    //Erro em enviar a resposta 
	if(situacao == -1){
		perror("Erro ao enviar a resposta para o cleinte !");
    }
}




int main(int argc, char const *argv[]){

    int porta = atoi (argv[2]);

    servidor_FilaTarefas(porta);
    



    
    
    return 0;
}
