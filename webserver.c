#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

char *get_mime_type(char *path){
	if(strstr(path, ".html")) return "text/html";
	if(strstr(path, ".css")) return "text/css";
	if(strstr(path, ".js")) return "application/javascript";
	return "text/plain";
}

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
		memset(buffer, 0, sizeof(buffer));
		read(client,buffer, 1024);
		char method[16], path[256];
		sscanf(buffer, "%s %s", method, path);
		
		if(strcmp(path, "/") == 0){
			strcpy(path, "/index.html");
		}		
		
		char filepath [256];
		sprintf(filepath, ".%s", path);

		FILE *f = fopen(filepath, "rb");
		if(f){
			char content[4096];
			int size;

			fseek(f, 0, SEEK_END);
			int file_size = ftell(f);
			rewind(f);

			char *type = get_mime_type(path);			

			char header[256];
			sprintf(header,
				"HTTP/1.1 200 OK\r\n"
				"Content-Length: %d\r\n"
				"Content-Type: %s\r\n"
				"\r\n",
				file_size, type);

			write(client, header, strlen(header));
			
			while((size = fread(content, 1, sizeof(content), f)) > 0){
				write(client, content, size);
			}
		} else{
			char *msg="HTTP/1.1 404 Not Found\r\n\r\nFile not found";
			write(client, msg, strlen(msg));
		}

		close(client);
	}
}
