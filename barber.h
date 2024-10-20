// Default input sizes
#define MAXLINE 1024
#define MAXARGS 128

// Redirecting IDs
#define NR 0
#define RI 1
#define RO 2
#define ARO 3
#define RSOSE 4
#define ASOSE 5

// Includes
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// Used to print environment variables
extern char **environ;

// Redirection struct
typedef struct Redirect
{
    char *last_arg;
    int redirect_type;
} Redirect;

// LocalVariable structure
typedef struct LocalVariable
{
    char *var;
    char *val;
    struct LocalVariable *next;
} LocalVariable;

// LocalVariableList structure
typedef struct LocalVariableList
{
    LocalVariable *head;
    LocalVariable *end;
    int size;
} LocalVariableList;

// HistoryItem structure
typedef struct HistoryItem
{
    char **args;
    int arg_count;
    struct HistoryItem *next;
} HistoryItem;

// History LinkedList structure
typedef struct History
{
    struct HistoryItem *head;
    int size;
    int max;
} History;

// Header needed for history callback
int handle_command(char **args, int arg_count, Redirect *redirect, LocalVariableList *local, History *history, int prev_rc, FILE *file);
