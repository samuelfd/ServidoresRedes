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
#include <dirent.h> 

struct sockaddr_in cliente, servidor;

socklen_t tamanho_cliente;

struct dirent *dir;


int cliente_socket = 0,aux = 0 ,cliente_thread = 0,id_thread = 0,cliente_conect = 0;
char mensagem[2048], *arquivo,*split;

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
              

        aux = read (cliente_conect, mensagem, 2048);
        char resp [strlen(mensagem)];        
        strcpy(resp,mensagem);        
        split = strtok(resp, "/");
        split = strtok(NULL, " ");
        arquivo = split;
        printf("ARQUIVO : %s\n",arquivo);

        

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
    char *aux,tamanho2[50];
    FILE *arq = fopen(arquivo, "rb");   
    fseek(arq,0, SEEK_END);
    long tamanho = ftell(arq);    
    sprintf(tamanho2,"%ld",tamanho);    
    fseek(arq, 0, SEEK_SET);   
    char resp [80];
    char resp_parte1[255] = "HTTP/1.1 200 OK\r\n";
    write(cliente_conect,resp_parte1,sizeof(resp_parte1));
    strcpy (resp_parte1,"Content-Length: ");
    //resp_parte1 = "Content-Length: ";           
    strcat(resp_parte1, tamanho2);
    strcat(resp_parte1, "\r\n");      
    printf("PART1: %s\n",resp_parte1);
    write(cliente_conect,resp_parte1,sizeof(resp_parte1));   
    char resp_parte2[255] = "Connection: close\r\n";
    strcpy (resp_parte2,"Content-Type: ");
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
    write(cliente_conect,resp_parte2,sizeof(resp_parte2));

    char *arquivo_solicitado = malloc(tamanho + 1);
    fread(arquivo_solicitado,tamanho, 1,arq);
    //printf("AAAA: %s\n",arquivo_solicitado);
    

    printf("ARQ: %s\n",arquivo_solicitado);
    // Resposta ao cliente caso a conexao tenha sucesso
    situacao = write(cliente_conect,arquivo_solicitado,sizeof(arquivo_solicitado)); 
   	
        
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
