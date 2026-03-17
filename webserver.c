#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
	int port = 8000;
	char buffer[1024];
	if(argc > 1) port = atoi(argv[1]);

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(server_fd < 0){	
		perror("socket error");
		exit(1);
	}
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(port);
	bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
	listen(server_fd, 5);
	printf("luisteren op port %d\n", port);
	
	while(1) {
		int client = accept(server_fd, NULL, NULL);
		read(client,buffer, 1024);
		FILE *f = fopen("index.html", "r");
		if(f){
			char content[4096];
			int size = fread(content,1,4096,f);
			fclose(f);

			char header[256];
			sprintf(header, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n", size);
			write(client, header, strlen(header));
			write(client, content, size);
		} else{
			char *msg="HTTP/1.1 404 Found\r\n\r\nFile not found";
			write(client, msg, strlen(msg));
		}

		close(client);
	}
}
