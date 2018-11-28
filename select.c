#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>

#define PORT 2020
struct dirent *dir;
// Responsavel por responder a requisicao do cliente
void resposta_servidor(int cliente_conect,char *arquivo){	   
    int situacao;
    char *aux;
    char resp [255]= "HTTP/1.1 200 OK\r\n\
    Connection: close\r\n\
    Content-Type: text/html; charset=UTF-8\r\n\
    Content-Length: "; 
    char diretorio [1000] , tam_diretorio[10];    
    aux = strtok (arquivo, "/");     

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
    else {
        char tamanho2[50];
        FILE *arq = fopen(arquivo, "rb");   
        fseek(arq,0L, SEEK_END);
        long tamanho = ftell(arq);    
        printf("EEEEEEE: %s, %ld\n", arquivo, tamanho);
        sprintf(tamanho2,"%ld",tamanho);    
        fseek(arq, 0, SEEK_SET);        
        char resp_parte1[1024] = "HTTP/1.1 200 OK\r\naccept-ranges: bytes\r\n";
        
        char fname[250];
        sprintf(fname, "teste%d.txt", cliente_conect);
        FILE *teste = fopen(fname, "w");
        
        write(cliente_conect,resp_parte1,strlen(resp_parte1));
        
        strcpy (resp_parte1,"content-length: ");
        //resp_parte1 = "Content-Length: ";           
        strcat(resp_parte1, tamanho2);
        strcat(resp_parte1, "\r\n");      
        printf("PART1: %s\n",resp_parte1);
        write(cliente_conect,resp_parte1,strlen(resp_parte1));   
        char resp_parte2[255] = "connection: close\r\n";
        strcpy (resp_parte2,"content-type: ");
        //resp_parte2 = "Content-Type: ";
        aux = strtok (arquivo, ".");    
        aux = strtok (NULL, "\0");
        printf("AUX: %s\n",aux);    
        
        
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
        
        printf("AAAAAB : %s\n\n",resp_parte2);
        write(cliente_conect,resp_parte2,strlen(resp_parte2));

        printf("TAMANHOOO : %ld\n",tamanho);
        char *arquivo_solicitado = malloc(tamanho);

        fread(arquivo_solicitado,tamanho, 1,arq);
        
        
        //printf("AAAA: %s\n",arquivo_solicitado);
        

        printf("ARQQQQ: %s\n",arquivo_solicitado);
        // Resposta ao cliente caso a conexao tenha sucesso
        situacao = write(cliente_conect,arquivo_solicitado,tamanho); 
        printf("AAAAAAAAAAAAAAAAAAAAAA: situacao %d\n", situacao);
    }
        
    //Erro em enviar a resposta 
	if(situacao == -1){
		perror("Erro ao enviar a resposta para o cleinte !");
    }
}

 
int main(int argc, char *argv[]){
    /* master file descriptor list */
    fd_set master;
    /* temp file descriptor list for select() */
    fd_set read_fds;
    /* server address */
    struct sockaddr_in serveraddr;
    /* client address */
    struct sockaddr_in clientaddr;
    /* maximum file descriptor number */
    int fdmax;
    /* listening socket descriptor */
    int listener;
    /* newly accept()ed socket descriptor */
    int newfd;
    /* buffer for client data */
    char buf[1024];
    int nbytes;
    /* for setsockopt() SO_REUSEADDR, below */
    int yes = 1;
    int addrlen;
    int i, j;
    int aux = 0;
    char mensagem[2048], *arquivo,*split;
    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* get the listener */
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("Server-socket() error lol!");
        /*just exit lol!*/
        exit(1);
    }
    printf("Server-socket() is OK...\n");
    /*"address already in use" error message */
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        perror("Server-setsockopt() error lol!");
        exit(1);
    }
    printf("Server-setsockopt() is OK...\n");

    /* bind */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1){
        perror("Server-bind() error lol!");
        exit(1);
    }
    printf("Server-bind() is OK...\n");

    /* listen */
    if(listen(listener, 10) == -1){
        perror("Server-listen() error lol!");
        exit(1);
    }
    printf("Server-listen() is OK...\n");

    /* add the listener to the master set */
    FD_SET(listener, &master);
    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/

    /* loop */
    for(;;){
        /* copy it */
        read_fds = master;

        if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            perror("Server-select() error lol!");
            exit(1);
        }
        printf("Server-select() is OK...\n");

        /*run through the existing connections looking for data to be read*/
        for(i = 0; i <= fdmax; i++){
            if(FD_ISSET(i, &read_fds)){ /* we got one... */
                if(i == listener){
                    /* handle new connections */
                    addrlen = sizeof(clientaddr);
                    if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1){                        
                        perror("Server-accept() error lol!");
                    }
                    else{
                        aux = read (newfd, mensagem, 2048);
                        char resp [strlen(mensagem)];        
                        strcpy(resp,mensagem);        
                        split = strtok(resp, "/");
                        split = strtok(NULL, " ");
                        arquivo = split;
                        resposta_servidor(newfd,arquivo);
                        if(aux >= 0){            
                            printf("Requisicao do cliente:\n------------------------------------------------------\n%s\n",mensagem);
                        
                        }        
                        else{
                            perror("Erro ao ler a mensagem do cliente!");
                        }
                        printf("Server-accept() is OK...\n");

                        FD_SET(newfd, &master); /* add to master set */
                        if(newfd > fdmax){ /* keep track of the maximum */
                            fdmax = newfd;
                        }
                        printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr), newfd);
                    }
                }
                
            }
        }
    }
    return 0;
}
