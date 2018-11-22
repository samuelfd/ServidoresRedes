#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <strings.h>

void resposta_servidor(int cliente_conect)
{
    printf("ABB %d\n", cliente_conect);
    int situacao;

    char resposta[1024] = "HTTP/1.1 200 OK\n\
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
    situacao = write(cliente_conect, resposta, sizeof(resposta));

    //Erro em enviar a resposta
    if (situacao == -1)
    {
        perror("Erro ao enviar a resposta para o cliente !");
    }
}
int main()
{

    int servidor_socketforked, cliente_socketforked;
    int servidor_len, cliente_len;
    struct sockaddr_in servidor;
    struct sockaddr_in cliente;

    // Inicia o Socket
    servidor_socketforked = socket(AF_INET, SOCK_STREAM, 0);

    // Inicia o servidor com zero
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY;
    servidor.sin_port = htons(8001);

    // Tamanho do servidor e sua associação com o socket
    servidor_len = sizeof(servidor);
    bind(servidor_socketforked, (struct sockaddr *)&servidor, servidor_len);

    // Cria a  fila de conexões e delimita o tamanho da fila de clientes

    listen(servidor_socketforked, 15);
    cliente_len = sizeof(cliente);

    for (;;)
    {

        char resposta[1024];

        printf("Servidor Aguardando\n");
        // Aceita a conexão com o cliente
        cliente_socketforked = accept(servidor_socketforked, (struct sockaddr *)&cliente, &cliente_len);
        //printf("AAA:  %d\n", cliente_socketforked);
        // Cria um processo para um cliente onde apenas o processo filho executa
        if (fork() == 0)
        {
            bzero(resposta,1014);
            // Lê a requisição do cliente
            read(cliente_socketforked, &resposta, 1024);
            //Tempo para lançar outra thread
            // sleep(5);
            
            printf("Mensagem Recebida:\n %s\n", resposta);
            resposta_servidor(cliente_socketforked);
            close(cliente_socketforked);
            exit(0);

            // Caso não atenda acima o processo é pai e então deve ser finalizado
        }
        else
        {
            close(cliente_socketforked);
        }
    }
}