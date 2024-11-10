#include <stdio.h>      
#include <stdlib.h>     
#include <unistd.h>     
#include <string.h>     
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h>      
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.c"


void read_input(char * prompt, char * buffer, int buffer_size) {
  printf("%s", prompt);
  fgets(buffer, buffer_size, stdin);
  buffer[strcspn(buffer, "\n")] = 0;
}
void do_register(int sockfd) {
  char username[BUFLEN], password[BUFLEN];
  printf("username=");
  fgets(username, BUFLEN, stdin);
  if (username[strlen(username) - 1] == '\n') {
    username[strlen(username) - 1] = 0;
  }
  printf("password=");
  fgets(password, BUFLEN, stdin);
  if (password[strlen(password) - 1] == '\n') {
    password[strlen(password) - 1] = 0;
  }
  if (strchr(username, ' ') != NULL) {
    printf("ERROR: Invalid Username\n");
    return;
  }
  char ** form_data = calloc(1, sizeof(char));
  form_data[0] = calloc(LINELEN, sizeof(char));

  JSON_Value * root_value = json_value_init_object();
  JSON_Object * root_object = json_value_get_object(root_value);
  json_object_set_string(root_object, "username", username);
  json_object_set_string(root_object, "password", password);
  char * json_string = json_serialize_to_string(root_value);

  memcpy(form_data[0], json_string, strlen(json_string));
  char * message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/register", "application/json", form_data, 1, NULL, 0, NULL);

  send_to_server(sockfd, message);

  char * response = receive_from_server(sockfd);
  if (strstr(response, "error")) {
    printf("ERROR: username already in use\n");
    return;
  }
  printf("OK Succesfully Registered\n");
}
char * do_login(int sockfd) {
  char username[BUFLEN], password[BUFLEN];
  printf("username=");
  fgets(username, BUFLEN, stdin);
  if (username[strlen(username) - 1] == '\n') {
    username[strlen(username) - 1] = 0;
  }
  printf("password=");
  fgets(password, BUFLEN, stdin);
  if (password[strlen(password) - 1] == '\n') {
    password[strlen(password) - 1] = 0;
  }
  if (strchr(username, ' ')) {
    printf("ERROR: Invalid username\n");
    return NULL;
  }
  char ** form_data = calloc(1, sizeof(char));
  form_data[0] = calloc(LINELEN, sizeof(char));

  JSON_Value * root_value = json_value_init_object();
  JSON_Object * root_object = json_value_get_object(root_value);
  json_object_set_string(root_object, "username", username);
  json_object_set_string(root_object, "password", password);
  char * json_string = json_serialize_to_string(root_value);

  memcpy(form_data[0], json_string, strlen(json_string));
  char * message = compute_post_request("34.246.184.49", "/api/v1/tema/auth/login", "application/json", form_data, 1, NULL, 0, NULL);

  send_to_server(sockfd, message);
  char * response = receive_from_server(sockfd);

  if (strstr(response, "error")) {
    printf("ERROR Invalid combination of username and password\n");
    return NULL;
  } else {
    printf("Succes logged in succesfully\n");
    char * cookie = strstr(response, "Cookie: ");
    for (int i = 0; i < strlen(cookie); i++) {
      if (cookie[i] == '\r' && cookie[i + 1] == '\n') {
        cookie[i] = 0;

      }
    }
    cookie += strlen("Cookie: ");
    char * final_cookie = calloc(LINELEN, sizeof(char));
    memcpy(final_cookie, cookie, strlen(cookie));
    return final_cookie;
  }
}
char * do_enter_library(int sockfd, char * cookie) {
  if (cookie == NULL) {
    printf("ERROR: not logged in.\n");
    return NULL;
  }
  char ** cookies = calloc(1, sizeof(char * ));
  cookies[0] = calloc(LINELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  char * message = compute_get_request("34.246.184.49", "/api/v1/tema/library/access", NULL, cookies, 1, NULL);
  send_to_server(sockfd, message);
  char * response = receive_from_server(sockfd);
  response = strstr(response, "{");

  JSON_Value * response_value = json_parse_string(response);
  JSON_Object * response_object = json_value_get_object(response_value);
  const char * token = json_object_get_string(response_object, "token");
  char * new_token = malloc(1024);
  memcpy(new_token, token, strlen(token));
  if (token != NULL) {
    printf("Succes Extracted token: %s\n", token);
  } else {
    printf("ERROR: Token not found in the response.\n");
  }

  return new_token;
}
void add_book(int sockfd, char * token) {
  if (token == NULL) {
    printf("ERROR: No access to the library.\n");
    return;
  }

  char title[LINELEN], author[LINELEN], publisher[LINELEN],
    genre[LINELEN], page_count[LINELEN];
  char ** form_data = calloc(1, sizeof(char * ));
  form_data[0] = calloc(LINELEN, sizeof(char));

  read_input("title=", title, LINELEN);
  read_input("author=", author, LINELEN);
  read_input("publisher=", publisher, LINELEN);
  read_input("genre=", genre, LINELEN);
  read_input("page_count=", page_count, LINELEN);

  if (strcmp(title, "") == 0 || strcmp(author, "") == 0 || strcmp(publisher, "") == 0 || strcmp(genre, "") == 0) {
    printf("ERROR: Invalid Book fields\n");
    return;
  }

  JSON_Value * root_value = json_value_init_object();
  JSON_Object * root_object = json_value_get_object(root_value);

  json_object_set_string(root_object, "title", title);
  json_object_set_string(root_object, "author", author);
  json_object_set_string(root_object, "genre", genre);
  json_object_set_string(root_object, "publisher", publisher);
  json_object_set_string(root_object, "page_count", page_count);
  if (atoi(page_count) == 0) {
    printf("ERROR: Invalid page_count\n");
    return;
  }

  char * serialized_string = json_serialize_to_string_pretty(root_value);

  memcpy(form_data[0], serialized_string, strlen(serialized_string));
  char * message = compute_post_request("34.246.184.49", "/api/v1/tema/library/books", "application/json", form_data, 1, NULL, 0, token);

  send_to_server(sockfd, message);

  char * response = receive_from_server(sockfd);
  if (strstr(response, "error") != NULL) {
    printf("ERROR: bla bla\n");
    return;
  }

  // Cleanup
  printf("Book added succesfully.\n");
  free(response);
  free(message);
  free(form_data[0]);
  free(form_data);

}

void do_get_books(int sockfd, char * token) {

  if (token == NULL) {
    printf("ERROR: No access to the library.\n");
    return;
  }

  char * message = compute_get_request("34.246.184.49", "/api/v1/tema/library/books", NULL, NULL, 0, token);
  send_to_server(sockfd, message);

  char * response = receive_from_server(sockfd);

  char * library = strstr(response, "[{");
  if (library == NULL) {
    library = strstr(response, "[");
  }
  free(response);
  free(message);
  printf("%s\n", library);
}

void do_get_book(int sockfd, char * token) {

  if (token == NULL) {
    printf("ERROR: No access to the library.\n");
    return;
  }

  char id[LINELEN];
  read_input("id=", id, LINELEN);

  char bookid[LINELEN];
  memcpy(bookid, "/api/v1/tema/library/books/\0", strlen("/api/v1/tema/library/books/") + 1);
  strcat(bookid, id);

  char * message = compute_get_request("34.246.184.49", bookid, NULL, NULL, 0, token);
  send_to_server(sockfd, message);

  char * response = receive_from_server(sockfd);

  char * book = strstr(response, "{");
  printf("%s\n", book);
}

void delete_book(int sockfd, char * token) {

  if (token == NULL) {
    printf("ERROR: No access to the library.\n");
    return;
  }

  char id[LINELEN];
  read_input("id=", id, LINELEN);

  char bookid[LINELEN];
  memcpy(bookid, "/api/v1/tema/library/books/\0", strlen("/api/v1/tema/library/books/") + 1);
  strcat(bookid, id);

  char * message = compute_delete_request("34.246.184.49", bookid, NULL, NULL, 0, token);
  send_to_server(sockfd, message);

  char * response = receive_from_server(sockfd);
  if (strstr(response, "error")) {
    printf("ERROR Something bad happened\n");
    return;
  }

  printf("Book deleted succesfully.\n");
}

void do_logout(int sockfd, char * cookie) {

  if (cookie == NULL) {
    printf("ERROR: Not logged in.\n");
    return;
  }

  char ** cookies = calloc(1, sizeof(char * ));
  cookies[0] = calloc(LINELEN, sizeof(char));
  memcpy(cookies[0], cookie, strlen(cookie));
  char * message = compute_get_request("34.246.184.49", "/api/v1/tema/auth/logout", NULL, cookies, 1, NULL);
  send_to_server(sockfd, message);
  char * response = receive_from_server(sockfd);
  if (strstr(response, "error")) {
    printf("ERROR Something bad happened\n");
    return;
  }

  printf("Succes logged out succesfully.\n");
}

int main(int argc, char * argv[]) {

  int sockfd;
  char * token = NULL;
  char * cookie = NULL;

  while (1) {
    sockfd = open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
      perror("Failed to open connection");
      return 1;
    }

    char command[BUFLEN];

    fgets(command, LINELEN, stdin);
    command[strcspn(command, "\n")] = 0;

    if (strcmp(command, "register") == 0) {
      do_register(sockfd);
      close(sockfd);
    } else if (strcmp(command, "login") == 0) {
      cookie = do_login(sockfd);
      close(sockfd);
    } else if (strcmp(command, "enter_library") == 0) {
      token = do_enter_library(sockfd, cookie);
      close(sockfd);
    } else if (strcmp(command, "get_books") == 0) {
      do_get_books(sockfd, token);
      close(sockfd);
    } else if (strcmp(command, "get_book") == 0) {
      do_get_book(sockfd, token);
      close(sockfd);
    } else if (strcmp(command, "add_book") == 0) {
      add_book(sockfd, token);
      close(sockfd);
    } else if (strcmp(command, "delete_book") == 0) {
      delete_book(sockfd, token);
      close(sockfd);
    } else if (strcmp(command, "logout") == 0) {
      do_logout(sockfd, cookie);
      close(sockfd);

      if (cookie != NULL) {
        free(cookie);
      }
      cookie = NULL;
      if (token != NULL) {
        free(token);
      }
      token = NULL;
    } else if (strcmp(command, "exit") == 0) {
      close(sockfd);
      break;
    } else {
      printf("Unknown command: %s\n", command);
      close(sockfd);
    }
  }

  return 0;
}