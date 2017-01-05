#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "http.h"

#define BUF_SIZE 1024
#define USERAGENT "HTMLGET 1.0"
#define CONNECTION "Close"



Buffer* send_socket(int soc, char *message, Buffer* http,char **server_reply) {
	
	ssize_t bytes = send(soc, message, BUF_SIZE,0);
	if( bytes <= 0)
    {
        perror("send failured");
        return NULL;
    }
    
    size_t recived_len = 0;
    int reserved = BUF_SIZE;
	http -> length = 0;
	
	while((recived_len = recv(soc, server_reply, BUF_SIZE, 0)) > 0){
		while (http->length + recived_len > reserved) {
			
						reserved= reserved * 2;
						http->data = realloc(http->data, reserved);
			
				}
        memcpy(http->data + http->length, server_reply, recived_len);
		http->length += recived_len;
	}
		
	if (recived_len < 0)
	{
		return NULL;
	}
	return http;
	}


Buffer* http_query(char *host, char *page, int port) {
	//char *getpage = page;
	int status;
	char port_num[20];
	sprintf(port_num,"%d", port);
	Buffer *http =(Buffer*)malloc(sizeof(Buffer));
	char *server_reply[BUF_SIZE];
	http->data =(char*)malloc(BUF_SIZE);
	char *message;
	if(page[0] == '/'){
		message = "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n"; 
	}else{
		message = "GET /%s HTTP/1.0\r\nHost: %s\r\n\r\n"; 
		}
	char query[BUF_SIZE]; 
	struct addrinfo their_addrinfo;//server address info
	struct addrinfo *their_addr = NULL;// connector's address information	
	memset(&their_addrinfo, 0, sizeof(struct addrinfo));// make sure the struct is empty
	memset(query, 0, BUF_SIZE);// make sure the struct is empty
	memset(http->data, 0, BUF_SIZE);// make sure the struct is empty
	sprintf(query, message, page, host); 
	their_addrinfo.ai_family = AF_INET;        // use an internet address and accept both IPv4 and IPv6
	their_addrinfo.ai_socktype = SOCK_STREAM;
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0){
		return NULL;
		}
  
	if ((status = getaddrinfo( host, port_num, &their_addrinfo, &their_addr)) !=0){// host name(ip), portnumber, struct addrinfo, linkied list result
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return NULL;
		};// get ip address
	   
	int rc = connect(sockfd, their_addr -> ai_addr, their_addr-> ai_addrlen);// file descripter, struct containing the destination port and IP address, length in bytes of the server
 
	if(rc == -1) {// return -1 on error and set the variable errno.
		return NULL;
	}
	
	freeaddrinfo(their_addr);
	http = send_socket(sockfd,query,http,server_reply);
	
	close(sockfd);
	
	return http;
}

// split http content from the response string
char* http_get_content(Buffer *response) {
	char* header_end = strstr(response->data, "\r\n\r\n");  
    if(header_end) {
		return header_end + 4;
	}else{
		return response->data; 
		}
	}

Buffer *http_url(const char *url) {
	char host[BUF_SIZE];
	strncpy(host, url, BUF_SIZE);
	char *page = strstr(host, "/");
	if(page) {
		page[0] = '\0';
		++page;
		return http_query(host, page, 80);
	}else{
    fprintf(stderr, "could not split url into host/page %s\n", url);
    return NULL;
  }
}

