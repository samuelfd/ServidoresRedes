#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>          
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include "func.h"

/*
<> Trabalho desenvolvido Disciplina de Redes
<> Alunos: João Caetano
           Matheus Reis
           Samuel Ribeiro
*/

int main(int argc, char const *argv[]){
    struct sockaddr_in cliente, servidor;

	socklen_t tamanho_cliente;

	int cliente_socket = 0,aux = 0;
	char mensagem[2048];

    int porta = atoi (argv[2]);
    int new_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    printf("Conecatdo");
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
    listen(new_socket,1);
    tamanho_cliente = sizeof(cliente);
    int sck_cl;

    while(1){
        //Servidor à espera de conexão
        
        sck_cl = accept(new_socket,(struct sockaddr *) &cliente, &tamanho_cliente);
        
        //Cliente conectou, um socket foi aberto

        //Variáveis "globais"
        int aux = 0;
        char mensagem[2048], *arquivo,*split;   //Mensagem é a mensagem recebida pelo cliente
        //Arquivo é o arquivo para ser enviado

        //Cliente_porta é o socket criado com a conexão do cliente
        uint64_t cliente_porta = (uint64_t) sck_cl;
        
        //Aux é a mensagem recebida pelo cliente
        aux = read (cliente_porta, mensagem, 2048);
        char resp [strlen(mensagem)];        
        strcpy(resp,mensagem);        
        split = strtok(resp, "/");
        split = strtok(NULL, " ");
        arquivo = split;
        printf("ARQUIVO : %s\n",arquivo);

        if(sck_cl >= 0){
            printf("Conecatdo com sucesso !\n");
        }
        else{
            perror("Erro ao se conectar !");
        }

        if(aux >= 0){
            printf("Requisicao do cliente:\n------------------------------------------------------\n%s\n",mensagem);
        }
        else{
            perror("Erro ao ler a mensagem do cliente!");
        }
        resposta_servidor(cliente_porta, arquivo);
        close(cliente_porta);
    }

    close(new_socket);
    
    
    return 0;
}
