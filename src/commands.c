#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include "commands.h"
#include "built_in.h"

#define SOCK_PATH "tpf_unix_sock.server"
#define SERVER_PATH "tpf_unix_sock.server"
#define CLIENT_PATH "tpf_unix_sock.client"

static struct built_in_command built_in_commands[] = {
  { "cd", do_cd, validate_cd_argv },
  { "pwd", do_pwd, validate_pwd_argv },
  { "fg", do_fg, validate_fg_argv }
};

static int is_built_in_command(const char* command_name)
{
  static const int n_built_in_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);

  for (int i = 0; i < n_built_in_commands; ++i) {
    if (strcmp(command_name, built_in_commands[i].command_name) == 0) {
      return i;
    }
  }

  return -1; // Not found
}

/*
 * Description: Currently this function only handles single built_in commands. You should modify this structure to launch process and offer pipeline functionality.
 */
void *threadfunction(void*threadid)
{
    long  tid;
    tid=(long)threadid;
    pthread_exit(NULL);
}



int evaluate_command(int n_commands, struct single_command (*commands)[512])
{
  if (n_commands > 0) {
    struct single_command* com = (*commands);

    assert(com->argc != 0);

    int built_in_pos = is_built_in_command(com->argv[0]);
    if (built_in_pos != -1) {
      if (built_in_commands[built_in_pos].command_validate(com->argc, com->argv)) {
        if (built_in_commands[built_in_pos].command_do(com->argc, com->argv) != 0) {
          fprintf(stderr, "%s: Error occurs\n", com->argv[0]);
        }
      } else {
        fprintf(stderr, "%s: Invalid arguments\n", com->argv[0]);
        return -1;
      }
    } else if (strcmp(com->argv[0], "") == 0) {
      return 0;
    } else if (strcmp(com->argv[0], "exit") == 0) {
      return 1;
    }
  
      else if(access(com->argv[0],X_OK)==0&&n_commands==1){                                       
      pid_t pid; 
      long t;
      int status;
      pid=fork();
      if(pid==0){
        execv(com->argv[0],com->argv);
        return 1;
        }
      else if(pid>0){
        waitpid(-1,&status,0);
        
        }
      else {
        fprintf(stderr, "fork error");
        return -1;
        }
      }
      else if(access(com->argv[0],X_OK)==0&&n_commands>1){
      pid_t pid;
      long t;
      int status;
      int len;
      int rc;
      int *fd;
      pthread_t threadID;
      if(access(com->argv[0],X_OK)==0){
      if(pid==0){
      pthread_create(&threadID,NULL,threadfunction,(void*)t);
      execv(com->argv[0],com->argv);
      return 1;
      }
     
      else if(pid>0){
      waitpid(-1,&status,0);
      int client_sock, rc, len;
      struct sockaddr_un server_sockaddr;
      struct sockaddr_un client_sockaddr;
      memset(&server_sockaddr,0,sizeof(struct sockaddr_un));
      memset(&client_sockaddr,0,sizeof(struct sockaddr_un));
      
      client_sock=socket(AF_UNIX,SOCK_STREAM,0);
      if(client_sock==-1){
         fprintf(stderr,"SOCKET ERROR");
         exit(1);
         }
      client_sockaddr.sun_family=AF_UNIX;
      strcpy(client_sockaddr.sun_path,CLIENT_PATH);
      len=sizeof(client_sockaddr);
      
      unlink(CLIENT_PATH);
      rc=bind(client_sock,(struct sockaddr*)&client_sockaddr,len);
      if(rc==-1){
         fprintf(stderr,"BIND ERROR");
         close(client_sock);
         exit(1);
         }  
      server_sockaddr.sun_family=AF_UNIX;
      strcpy(server_sockaddr.sun_path,SERVER_PATH);
      rc=connect(client_sock,(struct sockaddr*)&server_sockaddr,len);
      if(rc==-1){
	 fprintf(stderr,"CONNECT ERROR");
	 close(client_sock);
 	 exit(1);
  	 }
      //fd=dup(threadID);
      //send(client_sock,fd,sizeof(fd),0);
      
      pthread_join(threadID,(void**)&status);
      fprintf(stderr,"dj");
      com=(*commands+1);
      if(access(com->argv[0],X_OK)==0){
      pid=fork();
        if(pid==0){
	   execv(com->argv[0],com->argv);
	   return 1;
	 }
 	else if(pid>0){
	waitpid(-1,&status,0);
       
	 }	      
      fprintf(stderr,"%s\n",com->argv[0]); 
      } 
     }
    }
  }
   

      else{	
      fprintf(stderr, "%s: command not found\n", com->argv[0]);
      return -1;
      }
  }

  return 0;
}

void free_commands(int n_commands, struct single_command (*commands)[512])
{
  for (int i = 0; i < n_commands; ++i) {
    struct single_command *com = (*commands) + i;
    int argc = com->argc;
    char** argv = com->argv;

    for (int j = 0; j < argc; ++j) {
      free(argv[j]);
    }

    free(argv);
  }

  memset((*commands), 0, sizeof(struct single_command) * n_commands);
}
