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

// Responsavel por responder a requisicao do cliente
void resposta_servidor(int cliente_conect,char *arquivo){      
    int situacao;
    char *aux;
    //Resp é o cabeçalho da resposta do servidor, faltando apenas o tamanho do content-length
    char resp [255]= "HTTP/1.1 200 OK\r\n\
    Connection: close\r\n\
    Content-Type: text/html; charset=UTF-8\r\n\
    Content-Length: "; 
    char diretorio [1000] , tam_diretorio[10];    
    aux = strtok (arquivo, "/");     


    //Esse if executa quando é uma solicitação de diretório
    if(strcmp(arquivo,"HTTP") == 0){
        DIR *d;        
        d = opendir(".");
        if (d) {
            while ((dir = readdir(d)) != NULL) {
                strcat(diretorio,dir->d_name);
                strcat(diretorio,"\n"); 

            }
            sprintf(tam_diretorio,"%ld",strlen(diretorio));            
            strcat(resp,tam_diretorio);
            strcat(resp,"\r\n\r\n");
            printf("RESP : %s\n",resp);
            write(cliente_conect,resp,strlen(resp));
            printf("DIRE : %s\n",diretorio);
            write(cliente_conect,diretorio,strlen(diretorio));
            closedir(d);
        }
    }
    //Else é quando é uma solicitação de arquivo
    else {
        char tamanho2[50];
        FILE *arq = fopen(arquivo, "rb");   
        fseek(arq,0L, SEEK_END);
        long tamanho = ftell(arq);    
        printf("Arquivo e Tamanho: %s, %ld\n", arquivo, tamanho);
        sprintf(tamanho2,"%ld",tamanho);    
        fseek(arq, 0, SEEK_SET);        
        //Continuação da resposta em partes

        char resp_parte1[1024] = "HTTP/1.1 200 OK\r\naccept-ranges: bytes\r\n";
        
        char fname[250];
        sprintf(fname, "teste%d.txt", cliente_conect);
        FILE *teste = fopen(fname, "w");
        
        write(cliente_conect,resp_parte1,strlen(resp_parte1));
        
        //Adiciona o tamanho ao content length
        strcpy (resp_parte1,"content-length: ");
        strcat(resp_parte1, tamanho2);
        strcat(resp_parte1, "\r\n");      
        printf("PART1: %s\n",resp_parte1);
        write(cliente_conect,resp_parte1,strlen(resp_parte1));   
        
        //Parte 2 é complementando pós contet-length
        char resp_parte2[255] = "connection: close\r\n";
        strcpy (resp_parte2,"content-type: ");
        //resp_parte2 = "Content-Type: ";
        aux = strtok (arquivo, ".");    
        aux = strtok (NULL, "\0");
        printf("AUX: %s\n",aux);    
        
        //Seguindo a mime table para tipo de content-type ou tipo do arquivo a ser enviado
        if(strcmp(aux,"htm") == 0  || strcmp(aux,"html") == 0  || strcmp(aux,"txt") == 0 ){
            strcat(resp_parte2, "text/html\r\n\r\n");
            
        }
        else if (strcmp(aux,"jpeg") == 0  || strcmp(aux,"jpg") == 0 ){
            strcat(resp_parte2, "image/jpeg\r\n\r\n");

        }
        else if (strcmp(aux,"js") == 0 ){
            strcat(resp_parte2, "application/javascript\r\n\r\n");

        }
        else if (strcmp(aux,"pdf") == 0 ){
            printf("PDF");
            strcat(resp_parte2, "application/pdf\r\n\r\n");

        }
        else if (strcmp(aux,"png") == 0 ){
            strcat(resp_parte2, "image/png\r\n\r\n");
        }
        else if (strcmp(aux,"ico") == 0 ){
            strcat(resp_parte2, "image/x-icon\r\n\r\n");

        }
        
        printf("Parte2 : %s\n\n",resp_parte2);
        write(cliente_conect,resp_parte2,strlen(resp_parte2));


        //Agora identifica qual o arquivo solicitado pelo cliente para leitura e envio do mesmo
        printf("TAMANHO : %ld\n",tamanho);
        char *arquivo_solicitado = malloc(tamanho);

        fread(arquivo_solicitado,tamanho, 1,arq);
        
        
        //printf("AAAA: %s\n",arquivo_solicitado);
        

        printf("Arquivo: %s\n",arquivo_solicitado);
        // Resposta ao cliente caso a conexao tenha sucesso
        situacao = write(cliente_conect,arquivo_solicitado,tamanho); 
        printf("situacao %d\n", situacao);
    }
        
    //Erro em enviar a resposta 
    if(situacao == -1){
        perror("Erro ao enviar a resposta para o cleinte !");
    }
}
