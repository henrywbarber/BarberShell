#include "barber.h"

// Helper Method: create local variable
LocalVariable *create_local_variable(char *var, char *val)
{
    LocalVariable *newVar = (LocalVariable *)malloc(sizeof(LocalVariable));
    newVar->var = strdup(var);
    newVar->val = strdup(val);
    newVar->next = NULL;
    return newVar;
}

// Helper Method: add LocalVariable to list (return 1: success, return 0: fail)
int add_local_variable(char *var, char *val, LocalVariableList *local)
{
    LocalVariable *newVar = create_local_variable(var, val);
    if (newVar->var == NULL || newVar->val == NULL)
    {
        return 0;
    }
    // Save head if first variable
    if (local->size == 0)
    {
        local->head = newVar;
        local->end = newVar;
    }
    else
    {
        local->end->next = newVar;
        local->end = newVar;
    }
    local->size += 1;
    return 1;
}

// Helper Method: find local var in list
LocalVariable *find_local_var(LocalVariableList *local, char *var)
{
    LocalVariable *curr = local->head;
    while (curr != NULL)
    {
        if (strcmp(curr->var, var) == 0)
        {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

// Helper Method: free all local var data
void free_local_variables(LocalVariableList *local)
{
    LocalVariable *curr = local->head;
    while (curr != NULL)
    {
        LocalVariable *next = curr->next;
        free(curr->var);
        free(curr->val);
        free(curr);
        curr = next;
    }
    local->head = NULL;
    local->end = NULL;
    local->size = 0;
    free(local);
}

// Helper method: Create HistoryItem
HistoryItem *create_history_item(char **args, int arg_count)
{
    HistoryItem *newHistory = (HistoryItem *)malloc(sizeof(HistoryItem));
    if (newHistory == NULL)
    {
        fprintf(stderr, "Error: Could not malloc HistoryItem\n");
        return NULL;
    }

    // Copy args into HistoryItem args array
    newHistory->args = (char **)malloc(sizeof(char *) * arg_count);
    if (newHistory->args == NULL)
    {
        fprintf(stderr, "Error: Could not malloc HistoryItem->args\n");
        free(newHistory);
        return NULL;
    }

    for (int i = 0; i < arg_count; i++)
    {
        newHistory->args[i] = strdup(args[i]); // Duplicate the string
        if (newHistory->args[i] == NULL)
        {
            fprintf(stderr, "Error: Could not malloc HistoryItem->args copy\n");
            free(newHistory->args);
            free(newHistory);
            return NULL;
        }
    }

    // Set other vars
    newHistory->arg_count = arg_count;
    newHistory->next = NULL;

    return newHistory;
}

// Helper Method: Find index of historyItem in History (if exists)
int history_contains(History *history, HistoryItem *history_item)
{
    // Loop through history to find item
    HistoryItem *curr = history->head;
    int index = 0;
    while (curr != NULL)
    {
        if (curr->arg_count == history_item->arg_count)
        {
            int match = 1;
            for (int j = 0; j < curr->arg_count; j++)
            {
                if (strcmp(curr->args[j], history_item->args[j]) != 0)
                {
                    match = 0;
                }
            }
            if (match)
            {
                return index;
            }
        }
        curr = curr->next;
        index++;
    }

    // Not found
    return -1;
}

// Helper Method: Add HistoryItem to History LinkedList
void add_history_item(History *history, HistoryItem *history_item)
{
    int in_history = history_contains(history, history_item);

    // Item in history and is infront, no change
    if (in_history >= 0)
    {
        return;
    }
    // Item not in history, add to the front, handle size
    else
    {
        // Size stays in bounds
        if (history->size < history->max)
        {
            history_item->next = history->head;
            history->head = history_item;
            history->size++;
        }
        // History is full, add new item to front and remove last
        else
        {
            history_item->next = history->head;
            history->head = history_item;

            // Find second last item
            HistoryItem *currItem = history->head;
            for (int i = 0; i < history->size - 1; i++)
            {
                currItem = currItem->next;
            }

            // Free last item
            free(currItem->next->args);
            free(currItem->next);
            currItem->next = NULL;
        }
    }
}

// Helper Method: Adjust history size
void set_history_size(History *history, int new_size)
{
    // Decreasing size
    if (history->max > new_size)
    {
        // Remove items with index greater than new_size
        if (history->size > new_size)
        {
            HistoryItem *curr = history->head;
            for (int i = 0; i < new_size - 1; i++)
            {
                curr = curr->next;
            }

            // Free remaining
            HistoryItem *to_free = curr->next;
            curr->next = NULL;
            while (to_free != NULL)
            {
                HistoryItem *next = to_free->next;
                free(to_free->args);
                free(to_free);
                to_free = next;
            }

            history->size = new_size;
        }
        history->max = new_size;
    }
    // Increasing size
    else
    {
        history->max = new_size;
    }
}

// Helper Method: Free history data
void free_history(History *history)
{
    HistoryItem *current = history->head;
    HistoryItem *next_item;

    // Loop each HistoryItem
    while (current != NULL)
    {
        next_item = current->next;

        for (int i = 0; i < current->arg_count; i++)
        {
            free(current->args[i]);
        }

        free(current->args);
        free(current);
        current = next_item;
    }
    history->head = NULL;
    history->size = 0;
    free(history);
}

// Helper Method: identify pointer to value corresponding to variable if any
char *replace_var(char *token, LocalVariableList *local)
{
    // Check environment vars first
    char *env_val = getenv(token);
    if (env_val != NULL)
    {
        return env_val;
    }

    // Not found, check local
    LocalVariable *curr = local->head;
    while (curr != NULL)
    {
        if (strcmp(token, curr->var) == 0)
        {
            return curr->val;
        }
        curr = curr->next;
    }

    // No variable found, return empty string
    char *empty = "";
    return empty;
}

void built_in_exit(LocalVariableList *local, History *history, int prev_rc, FILE *file)
{
    // Free memory first
    free_local_variables(local);
    free_history(history);
    if (file != NULL)
    {
        fclose(file);
    }
    exit(prev_rc);
}

// https://www.geeksforgeeks.org/chdir-in-c-language-with-examples/
int built_in_cd(char **args, int arg_count)
{
    // Ensure argument validity
    if (arg_count == 2)
    {
        // Change with argument
        if (chdir(args[1]) != 0)
        {
            fprintf(stderr, "Error: cd could not access %s\n", args[1]);
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: Invalid cd arguments\n");
        return 1;
    }
    return 0;
}

// https://man7.org/linux/man-pages/man3/getenv.3.html
int built_in_export(char **args, int arg_count)
{
    // Ensure argument validity
    if (arg_count == 2)
    {
        // Seperate/tokenize argument
        char *var = strtok(args[1], "=");
        char *value = strtok(NULL, "=");

        // Ensure proper argument
        if (!var || !value)
        {
            fprintf(stderr, "Error: Invalid export var format\n");
            return 1;
        }

        // Check for empty string val --> clear variable
        if (value[0] == '"' && value[1] == '"' && value[2] == '\0')
        {
            char *empty = "";
            value = empty;
        }
            
        // Ensure set works
        if (setenv(var, value, 1) != 0)
        {
            fprintf(stderr, "Error: Could not add/changing env var\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: Invalid export arguments\n");
        return 1;
    }
    return 0;
}

int built_in_local(char **args, int arg_count, LocalVariableList *local)
{
    // Ensure argument validity
    if (arg_count == 2)
    {
        if (args[1] == NULL || strlen(args[1]) < 2)
        {
            fprintf(stderr, "Error: Invalid local format\n");
            return 1;
        }

        // Seperate/tokenize argument
        char *var = strtok(args[1], "=");
        char *val = strtok(NULL, "=");

        // Ensure proper argument
        if (!var)
        {
            fprintf(stderr, "Error: Invalid local var format\n");
            return 1;
        }

        // Check for empty string val --> clear variable
        if (!val || val[0] == '\0')
        {
            // Replace existing variable
            LocalVariable *curr = local->head;
            while (curr != NULL)
            {
                if (strcmp(curr->var, var) == 0)
                {
                    char *empty = "";
                    curr->val = empty;
                    return 0;
                }
                curr = curr->next;
            }

            fprintf(stderr, "Error: cannot clear local var that does not exist\n");
            return 1;
        }
        // Not clearing, changing or creating variable
        else
        {
            // Check for val variable replacement
            if (val[0] == '$')
            {
                val = replace_var(val + 1, local);
            }

            // Check if var already exists
            // Replace existing variable
            LocalVariable *curr = local->head;
            while (curr != NULL)
            {
                if (strcmp(curr->var, var) == 0)
                {
                    curr->val = strdup(val);
                    if (curr->val == NULL)
                    {
                        fprintf(stderr, "Error: duplicating value string\n");
                        return 1;
                    }
                    return 0;
                }
                curr = curr->next;
            }

            // Var does not exist, add new
            if (!add_local_variable(var, val, local))
            {
                fprintf(stderr, "Error: duplicating for new local\n");
                return 1;
            }
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: Invalid local arguments\n");
        return 1;
    }
    return 0;
}

int built_in_vars(int arg_count, LocalVariableList *local)
{
    if (arg_count == 1)
    {
        LocalVariable *curr = local->head;
        while (curr != NULL)
        {
            printf("%s=%s\n", curr->var, curr->val);
            curr = curr->next;
        }
    }
    else
    {
        fprintf(stderr, "Error: Invalid vars arguments\n");
        return 1;
    }
    return 0;
}

int built_in_history(char **args, int arg_count, Redirect *redirect, LocalVariableList *local, History *history, int prev_rc, FILE *file)
{
    // Show History List
    if (arg_count == 1)
    {
        // Print History
        HistoryItem *curr_item = history->head;
        int index = 1;
        while (curr_item != NULL)
        {
            printf("%i)", index);
            for (int j = 0; j < curr_item->arg_count; j++)
            {
                printf(" %s", curr_item->args[j]);
            }
            printf("\n");

            curr_item = curr_item->next;
            index++;
        }
    }
    // Use command from history
    else if (arg_count == 2)
    {
        int index = atoi(args[1]);

        // Check if index is within bounds
        if (index > 0 && index <= history->size)
        {
            HistoryItem *curr_item = history->head;

            // Get item at index
            for (int i = 1; i < index; i++)
            {
                curr_item = curr_item->next;
            }

            // Execute the command stored in the history item
            handle_command(curr_item->args, curr_item->arg_count, redirect, local, history, prev_rc, file);
        }
        else
        {
            fprintf(stderr, "Error: History index out of bounds\n");
            return 1;
        }
    }
    // Change history size
    else if (arg_count == 3)
    {
        int new_size = atoi(args[2]);
        char *set = "set";
        if (strcmp(set, args[1]) == 0)
        {
            if (new_size >= 1 && new_size <= 50)
            {
                // Adjust history size
                set_history_size(history, new_size);
            }
            else
            {
                fprintf(stderr, "Error: Command history size out of bounds: [1, 50]\n");
                return 1;
            }
        }
        else
        {
            fprintf(stderr, "Error: Improper history usage\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Error: Improper history usage\n");
        return 1;
    }
    return 0;
}

// https://man7.org/linux/man-pages/man3/scandir.3.html
int built_in_ls(int arg_count)
{
    // Ensure usage
    if (arg_count == 1)
    {
        // Get elements
        char *shell_path = ".";
        // char *shell_path = getenv("PATH");
        struct dirent **fileList;
        int num_files = scandir(shell_path, &fileList, NULL, alphasort);
        // Print elements (not . or .. which should be sorted to front)
        for (int i = 2; i < num_files; i++)
        {
            // Ignoring hidden files like gitignore
            if (fileList[i]->d_name[0] != '.')
            {
                printf("%s\n", fileList[i]->d_name);
            }
        }

        // Free memory
        for (int i = 0; i < num_files; i++)
        {
            free(fileList[i]);
        }
        free(fileList);
    }
    else
    {
        fprintf(stderr, "Error: Invalid ls command arguments!\n");
        return 1;
    }
    return 0;
}

void crop_redirect(char **args, int *arg_count)
{
    args[*arg_count - 1] = NULL;
    (*arg_count)--;
}

void restore_fd_free_redirect(Redirect *redirect, int in_d, int out_d, int err_d)
{
    if (redirect != NULL && !redirect->redirect_type)
    {
        dup2(in_d, STDIN_FILENO);
        dup2(out_d, STDOUT_FILENO);
        dup2(err_d, STDERR_FILENO);

        close(in_d);
        close(out_d);
        close(err_d);
    }
}

int handle_command(char **args, int arg_count, Redirect *redirect, LocalVariableList *local, History *history, int prev_rc, FILE *file)
{
    int fd;
    int in_d = dup(STDIN_FILENO);
    int out_d = dup(STDOUT_FILENO);
    int err_d = dup(STDERR_FILENO);
    switch(redirect->redirect_type)
    {
        case NR: {
            // No Redirecting
            break;
        }
        case RI: {
            // Get sign pointer
            char *arg_copy = strdup(redirect->last_arg);
            if (arg_copy == NULL)
            {
                return 1;
            }
            char *sign = strchr(arg_copy, '<');

            // Get file pointer
            char *file_name = sign + 1;

            // Attempt to find an n
            int new_fd = STDIN_FILENO;

            if (sign != NULL)
            {
                *sign = '\0';
                int parse = atoi(arg_copy);
                if (parse != 0)
                {
                    new_fd = parse;
                }
            }

            // Access
            fd = open(file_name, O_RDONLY);
            if (dup2(fd, new_fd) == -1)
            {
                fprintf(stderr, "Error: could not change fd\n");
                close(fd);
                return 1;
            }
            close(fd);
            break;
        }
        case RO: {
            // Get sign pointer
            char *arg_copy = strdup(redirect->last_arg);
            if (arg_copy == NULL)
            {
                return 1;
            }
            char *sign = strchr(arg_copy, '>');

            // Get file pointer
            char *file_name = sign + 1;

            // Attempt to find an n
            int new_fd = STDOUT_FILENO;
            if (sign != NULL)
            {
                *sign = '\0';
                int parse = atoi(arg_copy);
                if (parse != 0)
                {
                    new_fd = parse;
                }
            }

            // Access
            fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            if (fd == -1)
            {
                fprintf(stderr, "Error: could not open file\n");
                return 1;
            }
            if (dup2(fd, new_fd) == -1)
            {
                fprintf(stderr, "Error: could not change fd\n");
                close(fd);
                return 1;
            }
            close(fd);
            break;
        }
        case ARO: {
            // Get sign pointer
            char *arg_copy = strdup(redirect->last_arg);
            if (arg_copy == NULL)
            {
                return 1;
            }
            char *sign = strchr(arg_copy, '>');

            // Get file pointer
            char *file_name = sign + 2;

            // Attempt to find an n
            int new_fd = STDOUT_FILENO;
            if (sign != NULL)
            {
                *sign = '\0';
                int parse = atoi(arg_copy);
                if (parse != 0)
                {
                    new_fd = parse;
                }
            }

            // Access
            fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "Error: could not open file\n");
                return 1;
            }
            if (dup2(fd, new_fd) == -1)
            {
                fprintf(stderr, "Error: could not change fd\n");
                close(fd);
                return 1;
            }
            close(fd);
            break;
        }
        case RSOSE: {
            // Get sign pointer
            char *arg_copy = strdup(redirect->last_arg);
            if (arg_copy == NULL)
            {
                return 1;
            }
            char *sign = strchr(arg_copy, '&');

            // Get file pointer
            char *file_name = sign + 2;

            // Access
            fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "Error: could not open file\n");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1)
            {
                fprintf(stderr, "Error: could not change fd\n");
                close(fd);
                return 1;
            }
            close(fd);
            break;
        }
        case ASOSE: {
            // Get sign pointer
            char *arg_copy = strdup(redirect->last_arg);
            if (arg_copy == NULL)
            {
                return 1;
            }
            char *sign = strchr(arg_copy, '&');

            // Get file pointer
            char *file_name = sign + 3;

            // Access
            fd = open(file_name, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd == -1)
            {
                fprintf(stderr, "Error: could not open file\n");
                return 1;
            }
            if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1)
            {
                fprintf(stderr, "Error: could not change fd\n");
                close(fd);
                return 1;
            }
            close(fd);
            break;
        }
        default:
            fprintf(stderr, "Error: handling redirects\n");
            return 1;
    }

    // Remove redirect from args if it was present
    if (redirect->redirect_type)
    {
        crop_redirect(args, &arg_count);
    }

    // Redirect complete now handle command
    if (args[0] == NULL)
    {
        fprintf(stderr, "Error: No indentifiable command found!\n");
        return 1;
    }
    if (strcmp(args[0], "exit") == 0)
    {
        if (arg_count == 1)
        {
            built_in_exit(local, history, prev_rc, file);
        }
        else
        {
            fprintf(stderr, "Error: Inproper exit arguments\n");
            return 1;
        }
    }
    if (strcmp(args[0], "cd") == 0)
    {
        return built_in_cd(args, arg_count);
    }
    if (strcmp(args[0], "export") == 0)
    {
        return built_in_export(args, arg_count);
    }
    if (strcmp(args[0], "local") == 0)
    {
        return built_in_local(args, arg_count, local);
    }
    if (strcmp(args[0], "vars") == 0)
    {
        return built_in_vars(arg_count, local);
    }
    if (strcmp(args[0], "history") == 0)
    {
        return built_in_history(args, arg_count, redirect, local, history, prev_rc, file);
    }
    if (strcmp(args[0], "ls") == 0)
    {
        return built_in_ls(arg_count);
    }
    // Not built in function! Do following:

    // https://git.doit.wisc.edu/cdis/cs/courses/cs537/fall24/public/discussion_material/-/blob/main/week3/fork_exec.c?ref_type=heads

    HistoryItem *newItem = create_history_item(args, arg_count);
    if (newItem == NULL)
    {
        // Error recorded and frees made already
        restore_fd_free_redirect(redirect, in_d, out_d, err_d);
        return 1;
    }

    add_history_item(history, newItem);

    // Create fork to exec command
    // https://linux.die.net/man/2/access
    char *new_path = NULL;
    char *path = NULL;
    int status;
    pid_t rc, w;
    rc = fork();
    if (rc < 0)
    {
        // Fork failed
        fprintf(stderr, "Error: Fork failed\n");
        restore_fd_free_redirect(redirect, in_d, out_d, err_d);
        return 1;
    }
    else if (rc == 0)
    {
        // Child Process
        // Check check execute access of args
        if (access(args[0], X_OK) == 0)
        {
            exit(execv(args[0], args));
        }
        else
        {
            // Get env var Path 
            char *path_pointer = getenv("PATH");
            if (path_pointer == NULL)
            {
                exit(-1);
            }

            path = malloc(strlen(path_pointer) + 1);
            if (path == NULL)
            {
                exit(-1);
            }
            strcpy(path, path_pointer);

            // Split by :
            char *token = strtok(path, ":");

            while (token != NULL)
            {
                // Get path of token and arged func concated
                new_path = malloc(strlen(token) + strlen(args[0]) + 2);
                if (new_path == NULL)
                {
                    exit(-1);
                }

                // dest, source: dest <-- dest + source
                strcpy(new_path, token);
                strcat(new_path, "/");
                strcat(new_path, args[0]);

                // Try access on new path
                if (access(new_path, X_OK) == 0)
                {
                    exit(execv(new_path, args));
                }
                token = strtok(NULL, ":");
            }
            // No path found
            exit(-1);
        }
    }
    else
    {
        // Parent Process
        // https://stackoverflow.com/questions/47441871/why-should-we-check-wifexited-after-wait-in-order-to-kill-child-processes-in-lin
        w = waitpid(rc, &status, 0);
        if (w == -1)
        {
            // Waitpid failure
            fprintf(stderr, "Error: waitpid failure\n");
            restore_fd_free_redirect(redirect, in_d, out_d, err_d);
            free(path);
            free(new_path);
            return 1;
        }
        if (WIFEXITED(status))
        {
            // Succesful child exit
            int child_rc = WEXITSTATUS(status);
            restore_fd_free_redirect(redirect, in_d, out_d, err_d);
            free(path);
            free(new_path);
            return child_rc;
        }
        else
        {
            // Failed child exit
            fprintf(stderr, "Error: Child process failed exit\n");
            restore_fd_free_redirect(redirect, in_d, out_d, err_d);
            free(path);
            free(new_path);
            return 1;
        }
    }
}

int handle_argument(char *input, LocalVariableList *local, History *history, int prev_rc, FILE *file)
{
    char *token;
    char *args[MAXARGS];
    // Get args via tokens
    token = strtok(input, " ");
    // Handle comment:
    if (token[0] != '#')
    {
        // Loop other tokens
        int arg_count = 0;
        while (token != NULL && arg_count < MAXARGS - 1)
        {
            // Replace $<var> with local var
            if (token[0] == '$')
            {
                // Crop out '$'
                args[arg_count] = replace_var(token + 1, local);
                if (args[arg_count] == NULL)
                {
                    fprintf(stderr, "Error: count not allocate empty\n");
                    prev_rc = 1;
                    break;
                }
            }
            else
            {
                // Save token into args
                args[arg_count] = token;
            }

            // Get next token
            arg_count++;
            token = strtok(NULL, " ");
        }

        // Terminate args (just in case)
        args[arg_count] = NULL;

        // Scrape redirect
        struct Redirect *redirect = malloc(sizeof(Redirect));
        if (redirect == NULL)
        {
            fprintf(stderr, "Error: could not malloc redirect\n");
            return 1;
        }
        redirect->last_arg = args[arg_count - 1];
        redirect->redirect_type = NR;

        if (strstr(redirect->last_arg, "&>>") != NULL)
        {
            redirect->redirect_type = ASOSE;
        }
        else if (strstr(redirect->last_arg, "&>") != NULL)
        {
            redirect->redirect_type = RSOSE;
        }
        else if (strstr(redirect->last_arg, ">>") != NULL)
        {
            redirect->redirect_type = ARO;
        }
        else if (strstr(redirect->last_arg, ">") != NULL)
        {
            redirect->redirect_type = RO;
        }
        else if (strstr(redirect->last_arg, "<") != NULL)
        {
            redirect->redirect_type = RI;
        }
        int rc = handle_command(args, arg_count, redirect, local, history, prev_rc, file);
        free(redirect);
        return rc;
    }
    return prev_rc;
}

void interactive_loop(LocalVariableList *local, History *history)
{
    int prev_rc = 0;
    char input[MAXLINE];
    FILE *file = NULL;
    while (1)
    {
        // Prompt user
        printf("barber> ");
        // Piazza recommended
        fflush(stdout);
        // Get input line
        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            built_in_exit(local, history, prev_rc, file);
        }
        // Handle ctrl-d input
        if (feof(stdin))
        {
            built_in_exit(local, history, prev_rc, file);
        }
        // Remove newline (incase)
        input[strcspn(input, "\n")] = '\0';
        // Handle and breakdown argument
        prev_rc = handle_argument(input, local, history, prev_rc, file);
    }
}

void batch_loop(char *file_name, LocalVariableList *local, History *history)
{
    int prev_rc = 0;
    char input[MAXLINE];
    FILE *file = fopen(file_name, "r");
    if (file == NULL)
    {
        fprintf(stderr, "Error: Could not access file: %s\n", file_name);
        exit(1);
    }
    // Loop through lines of file
    while (fgets(input, sizeof(input), file) != NULL)
    {
        // Piazza recommended
        fflush(stdout);
        // Remove newline (incase)
        input[strcspn(input, "\n")] = '\0';
        // Handle and breakdown argument
        prev_rc = handle_argument(input, local, history, prev_rc, file);
    }
    built_in_exit(local, history, prev_rc, file);
}

// Handle startup types: Interactive (user) or Batch (file)
int main(int argc, char **argv)
{
    // Set correct path
    if (setenv("PATH", "/bin", 1) == -1)
    {
        fprintf(stderr, "Error: could not set env path\n");
        exit(1);
    }

    // Init local storage
    LocalVariableList *local = malloc(sizeof(LocalVariableList));
    if (local == NULL)
    {
        fprintf(stderr, "Error: malloc local\n");
        exit(1);
    }
    local->head = NULL;
    local->end = NULL;
    local->size = 0;

    // Init history storage
    History *history = malloc(sizeof(History));
    if (history == NULL)
    {
        fprintf(stderr, "Error: malloc history\n");
        free(local);
        exit(1);
    }
    history->head = NULL;
    history->size = 0;
    history->max = 5;

    if (argc == 1)
    {
        interactive_loop(local, history);
    }
    else if (argc == 2)
    {
        batch_loop(argv[1], local, history);
    }
    else
    {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        free(local);
        free(history);
        exit(1);
    }
}
