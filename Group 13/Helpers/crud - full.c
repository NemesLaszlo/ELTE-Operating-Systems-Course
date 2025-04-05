#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct driver {
    unsigned int id;
    char name[50];
    char email[50];
    char phone[50];
    unsigned int exp;
    char working_days[60];
};

// Id tracking
unsigned int id = 1;

void read_in_console_message(const char *console_text, char *result) 
{
    printf("%s", console_text);
    scanf("\n%[^\t\n]s", result);
    // scanf(" %49[^\t\n]", result); // Prevents buffer overflow
    // scanf to limit the input length. If result is a char[50], allow only 49 characters (%49[^\t\n]s)
}

void file_creation()
{
    FILE *file = fopen("database.txt", "a");
    fclose(file);
}

struct driver *file_read(FILE *file)
{
    struct driver *actual = (struct driver *)malloc(sizeof(struct driver));
    if (!actual) return NULL;
    fscanf(file, "%u\t%[^\t]\t%s\t%s\t%u\t%[^\t\n]", &(actual->id), actual->name, actual->email, actual->phone, &(actual->exp), actual->working_days);
    return actual;
}

void id_setup()
{
    FILE *infile;
    struct driver *actual;
    id = 0;

    infile = fopen("database.txt", "r");
    if(infile != NULL) 
    {
        actual = file_read(infile);
        while(!feof(infile)) 
        {
            id = actual->id;
            free(actual);
			actual = NULL;
            actual = file_read(infile);
        }
        free(actual);
		actual = NULL;
    }
    fclose(infile);
    id++;
}

void driver_creation(const struct driver *createdDriver) 
{
    struct driver a;

    char read_value[50];
    unsigned int exp;
    if(createdDriver != NULL) 
    {
        a = *createdDriver;
    }
    a.id = id;
    id++;
    read_in_console_message("Name:\n", read_value);
    if(strcmp(read_value, "-\0") == 0) 
    {
        strcpy(a.name, createdDriver->name);
    }
    else 
    {
        strcpy(a.name, read_value);
    }
    read_in_console_message("Email:\n", read_value);
    if(strcmp(read_value, "-\0") == 0) 
    {
        strcpy(a.email, createdDriver->email);
    }
    else 
    {
        strcpy(a.email, read_value);
    }
    read_in_console_message("Phone:\n", read_value);
    if(strcmp(read_value, "-\0") == 0) 
    {
        strcpy(a.phone, createdDriver->phone);
    }
    else 
    {
        strcpy(a.phone, read_value);
    }
    printf("Experience:\n");
    scanf("%u", &exp);
    if(exp == 0)
    {
        a.exp = createdDriver->exp;
    }
    else
    {
        a.exp = exp;
    }
    read_in_console_message("Working Days:\n", read_value);
    if(strcmp(read_value, "-\0") == 0) 
    {
        strcpy(a.working_days, createdDriver->working_days);
    }
    else 
    {
        strcpy(a.working_days, read_value);
    }

    FILE *outfile;
    outfile = fopen("database.txt", "a");
    if(outfile == NULL)
    {
        fprintf(stderr, "\nCannot open file!\n");
        exit(1);
    }   
    int written = fprintf(outfile, "%u\t%s\t%s\t%s\t%u\t%s\n", a.id, a.name, a.email, a.phone, a.exp, a.working_days);
    if (written < 0)
    {
        printf("File writing error\n");
    }
    else
    {
        printf("Driver added succesfully\n");
    }

    fclose(outfile);
}

void list()
{
    FILE *infile;
    struct driver *a;
    size_t count = 0;

    infile = fopen("database.txt", "r");
    if(infile == NULL)
    {
        fprintf(stderr, "\nCannot open file!\n");
        exit(1);
    }

    a = file_read(infile);
    while(!feof(infile))
    {
        printf("Id = %u Name = %s Email = %s PhoneNumber = %s Exp = %u Working days = %s\n", a->id, a->name, a->email, a->phone, a->exp, a->working_days);
        free(a);
		a = NULL;
        a = file_read(infile);
        count++;
    }
    free(a);
	a = NULL;

    if(count == 0)
    {
        printf("File is empty\n");
    }
    fclose(infile);
}

void list_by_exp(unsigned int exp) 
{
    FILE *infile;
    struct driver *a;

    infile = fopen("database.txt", "r");
    if(infile == NULL)
    {
        fprintf(stderr, "\nCannot open file!\n");
        exit(1);
    }

    a = file_read(infile);
    while(!feof(infile))
    {
        if(a->exp == exp) 
        {
            printf("Id = %u Name = %s Email = %s PhoneNumber = %s Exp = %u Working days = %s\n", a->id, a->name, a->email, a->phone, a->exp, a->working_days);
        }
        free(a);
		a = NULL;
        a = file_read(infile);
    }
    free(a);
	a = NULL;
    fclose(infile);
}

void delete(unsigned int id, struct driver **deleted_driver, int message)
{
    FILE *infile;
    FILE *outfile;

    struct driver *a;
    int flag_remove;
    int flag_rename;

    // **deleted_driver -> *deleted_driver -> NULL
    if(deleted_driver != NULL)
    {
        *deleted_driver = NULL;
    }

    infile = fopen("database.txt", "r");
    if(infile == NULL)
    {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }

    outfile = fopen("delete.txt", "w");
    if(outfile == NULL)
    {
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }

    a = file_read(infile);
    while(!feof(infile))
    {
        if(a->id != id)
        {
            fprintf(outfile, "%u\t%s\t%s\t%s\t%u\t%s\n", a->id, a->name, a->email, a->phone, a->exp, a->working_days);
        }
        else
        {
            // **deleted_driver -> *deleted_driver -> a
            if(deleted_driver != NULL)
            {
                *deleted_driver = (struct driver *)malloc(sizeof(struct driver));
                memcpy(*deleted_driver, a, sizeof(struct driver));
            }
        }
        free(a);
		a = NULL;
        a = file_read(infile);
    }
    free(a);
	a = NULL;
    fclose(outfile);
    fclose(infile);
    flag_remove = remove("database.txt");
    flag_rename = rename("delete.txt", "database.txt");
    if(message == 1)
    {
        if(flag_remove == 0 && flag_rename == 0)
        {
            printf("Delete was successfuly\n");
        }
        else
        {
            printf("Delete was NOT successfuly\n");
        }
    }
    id_setup();
}

void modify(unsigned int id) 
{
    printf("Use - sing to keep the original value.\n");
    printf("In calse of EXP, use 0 value to keep the original experience.\n");
    struct driver *for_modification = NULL;
    delete(id, &for_modification, 0);
    if(for_modification == NULL)
    {
        printf("There is no driver to modify\n");
    }
    else 
    {
        driver_creation(for_modification);
    }
}

int main()
{
    int command;
    unsigned int id;
    unsigned int exp;

    file_creation();
    id_setup();
    do
    {
        printf("-----MENU-----\n");
        printf("Driver add - 1\n");
        printf("Driver full list - 2\n");
        printf("Driver list by exp - 3\n");
        printf("Driver modification - 4\n");
        printf("Driver delete - 5\n");
        printf("Exit - 6\n\n");
        switch (command)
        {
        case '1':
            driver_creation(NULL);
            break;
        case '2':
            list();
            break;
        case '3':
            printf("Experience:\n");
            scanf("%u", &exp);
            list_by_exp(exp);
            break;
        case '4':
            printf("Id:\n");
            scanf("%u", &id);
            modify(id);
            break;
        case '5':
            printf("Id:\n");
            scanf("%u", &id);
            delete (id, NULL, 1);
            break;
        case '6':
            printf("Exit\n");
            exit(0);
            break;
        default:
            break;
        }
    } while ((command = getchar()) != EOF);
    return 0;
}