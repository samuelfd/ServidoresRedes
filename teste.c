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

int cliente_socket = 0,aux = 0 ,cliente_thread = 0,id_thread = 0;
char mensagem[2048];

void resposta_servidor(int cliente_conect);

// Responsavel por responder a requisicao do cliente
void resposta_servidor(int cliente_conect){	
    int situacao;    

    char resposta [1024] = "\nHTTP/1.1 200 OK\n\
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
    char res_pro_stat_text = "\nHTTP/1.1 200 OK\n\
    ";
    char cab = "html; encoding=utf8\n\
    ";
    char content-length = ;
    char conexao = "close"; 

    //Resposta ao cliente caso a conexao tenha sucesso
    situacao = write(cliente_conect,resposta,sizeof(resposta)); 
   	
        
    //Erro em enviar a resposta 
	if(situacao == -1){
		perror("Erro ao enviar a resposta para o cleinte !");
    }
}




int main(int argc, char const *argv[]){
	printf("oioi\n");
    int porta = 8080;
    int new_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;

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
    int sck_cl;

    while(1){
        //Realiza a conexao do servidor com cliente
        
        sck_cl = accept(new_socket,(struct sockaddr *) &cliente, &tamanho_cliente);
        
        
        if(sck_cl >= 0){
            printf("Conecatdo com sucesso !\n");
        }
        else{
            perror("Erro ao se conectar !");
        }

        bzero(mensagem,2048);

        aux = read (sck_cl, mensagem, 2048);

        if(aux >= 0){
            printf("Requisicao do cliente:\n------------------------------------------------------\n%s\n",mensagem);
        }
        else{
            perror("Erro ao ler a mensagem do cliente!");
        }
        resposta_servidor(sck_cl);
    }

    close(new_socket);
        



    
    
    return 0;
}
