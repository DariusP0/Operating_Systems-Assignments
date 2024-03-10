#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int listDir(const char *path,int recursive,int size, int sizeGreater, int exec)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[2048];
    struct stat statbuf;

    dir = opendir(path);
 
    if(dir == NULL) {
        perror("Could not open directory");
        return -1;
    }
 
    while((entry = readdir(dir)) != NULL) {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
        
            snprintf(fullPath, 2048, "%s/%s", path, entry->d_name);
        
            if(lstat(fullPath, &statbuf) == 0) {

                    
                if(size==1)
                {
                    if(S_ISREG(statbuf.st_mode))
                    {
                        if(sizeGreater<statbuf.st_size)
                        {
                            printf("%s\n", fullPath);
                        }
                    }
                }
                
                if(size==0)
                {   
                    if(exec == 1)
                    {
                            if(statbuf.st_mode & S_IXUSR) //de pe StackOverflow: https://stackoverflow.com/questions/32985809/how-to-know-if-a-file-is-readable-by-other-user-in-c
                            printf("%s\n",fullPath);
                    }
                    else
                    printf("%s\n", fullPath);
                }
                
                if(recursive == 1)
                {
                    if(S_ISDIR(statbuf.st_mode))
                    {  
                        listDir(fullPath,recursive,size,sizeGreater,exec);
                    }
                }
            
            }
        }
    }
    
    closedir(dir);
    return 0;
 }
 
 int parse(char *path)
 {
    short int header_size;
    int version;
    unsigned char nr_sections ;
    char magic[5];
    char header_name[21];
    short int sect_type;
    int sect_offset;
    int sect_size;
    int fd = open(path, O_RDONLY);
    
    lseek(fd,-6,SEEK_END);

    read(fd,&header_size,2);

    read(fd,magic,4);
    
    magic[4]='\0';
    if(strcmp(magic,"TaRQ")!=0)
    {
        printf("ERROR\nwrong magic");
        return -1;
    }
    lseek(fd,-header_size,SEEK_END);
    
    read(fd,&version,4);
    read(fd,&nr_sections,1);
    if(version<54 || version>159)
    {
        printf("ERROR\nwrong version");
        return -1;
    }
    if(nr_sections<5 || nr_sections>14)
    {
        printf("ERROR\nwrong sect_nr");
            return -1;
    }


     
            
            for(int i = 0 ; i<nr_sections; i++)
    {
        
        read(fd,header_name,20);
        header_name[20]='\0';
        read(fd,&sect_type,2);
        read(fd,&sect_offset,4);
        read(fd,&sect_size,4);
        if(sect_type!=12 && sect_type!=48 && sect_type!=68 && sect_type!=50 && sect_type!=93)
        {
            printf("ERROR\nwrong sect_types\n");
            return -1;
        }

    }
    lseek(fd,-30*nr_sections,SEEK_CUR);
        
         printf("SUCCESS\n");
      printf("version=%d\n",version);
      printf("nr_sections=%d\n",nr_sections);
     for(int i = 0 ; i<nr_sections; i++)
    {
        
        read(fd,header_name,20);
        header_name[21]='\0';
         read(fd,&sect_type,2);
         read(fd,&sect_offset,4);
        read(fd,&sect_size,4);
        
          
        printf("section%d: ",i+1);
        printf("%s ",header_name);
       
       
        printf("%d ",sect_type);
        
        printf("%d\n", sect_size);
    }
    
    close(fd);
  
    
    return 0;
 }

 int extract(char *path, int section, int nr_linie)
 {
    short int header_size;
    unsigned char nr_sections ;
    int sect_offset;
    int sect_size;
    int fd = open(path, O_RDONLY);
    
    lseek(fd,-6,SEEK_END);

    read(fd,&header_size,2);

    lseek(fd,-header_size+4,SEEK_END);
    
    read(fd,&nr_sections,1);
    
     
     for(int i = 0 ; i<nr_sections; i++)
    {

        if(i+1==section)
        {
            lseek(fd,22,SEEK_CUR);
            read(fd,&sect_offset,4);
            read(fd,&sect_size,4);
        }
        else
        {
            lseek(fd,30,SEEK_CUR);
        }
    }
    lseek(fd,sect_offset,SEEK_SET);
    int j = 0;
    if(nr_linie==1){
        printf("\n");
    }
    char car;
    while(j<nr_linie)
    {
            if(read(fd,&car,1)==0 || car==0x00)
                break;
            if(car==0x0A)
            {
                j++;
            }
            if(j+1==nr_linie)
             printf("%c",car);

    }
          
    // printf("%d\n",nr_sections);
    // printf("%d\n",sect_offset);
    // printf("%d\n",sect_size);
    
    close(fd);
  
    
    return 0;
 }

int findall(char *path)
{
    DIR *dir = NULL;
    struct dirent *entry = NULL;
    char fullPath[2048];
    struct stat statbuf;

    dir = opendir(path);

    if(dir == NULL) 
    {
        printf("ERROR\ninvalid directory path");
        return -1;
    }

    while((entry = readdir(dir)) != NULL) 
    {
        if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(fullPath, 2048, "%s/%s", path, entry->d_name);

            if(lstat(fullPath, &statbuf) == 0) 
            {
                if(S_ISREG(statbuf.st_mode))
                {   
                    int fd = open(fullPath,O_RDONLY);
                    short int header_size;
                    int sect_offset = 0;
                    unsigned char nr_sections ;
                    int sect_size = 0;
                    int version;
                    char magic[5];
                    short int sect_type;
                    int valid = 1;
                    lseek(fd, -6, SEEK_END);
                    read(fd, &header_size, 2);

                    read(fd, magic, 4);
                    magic[4] = '\0';

                    if (strcmp(magic, "TaRQ") != 0) 
                    {
                        //printf("\nmagic%s\n\n\n\n\\\\",magic);
                        valid = 0;
                    }

                    lseek(fd, -header_size, SEEK_END);
                    read(fd, &version, 4);
                    read(fd, &nr_sections, 1);

                    if (version < 54 || version > 159 || nr_sections < 5 || nr_sections > 14) 
                    {
                        //printf("\nversion%s\n\n\n\n\\\\",magic);
                        valid = 0;

                    }
                           for(int i = 0 ; i<nr_sections; i++)
                    {
                        
                        lseek(fd,20,SEEK_CUR);
                        read(fd,&sect_type,2);
                       lseek(fd,8,SEEK_CUR);
                        if(sect_type!=12 && sect_type!=48 && sect_type!=68 && sect_type!=50 && sect_type!=93)
                        {
                            valid=0;
                        }

                    }
                    lseek(fd,-30*nr_sections,SEEK_CUR);
                    for(int i = 0 ; i<nr_sections; i++)
                    {
                        lseek(fd, 20, SEEK_CUR);
                        read(fd, &sect_type, 2);

                        read(fd,&sect_offset,4);
                        read(fd,&sect_size,4);
                        char buffer[sect_size+1];


                        lseek(fd,sect_offset,SEEK_SET);
                        read(fd,buffer,sect_size+1);
                        buffer[sect_size]='\0';
                        int j = 1;
                        for(int k=0; k<sect_size; k++)
                        {
                            if(buffer[k] == 0x0A)
                            {
                                j++;
                            }
                        }

                        if(j == 15 && valid ==1)
                        {
                            printf("%s\n",fullPath);
                            break;

                        }

                        lseek(fd,-header_size+5+(i+1)*30,SEEK_END);                        
                    }

                    close(fd);
                }
                
                if(S_ISDIR(statbuf.st_mode))
                {  
                    findall(fullPath);
                }
            }
        }
    }

    closedir(dir);
    return 0;
}

int main(int argc, char **argv){
    if(argc >= 2){
        
        char path[10000];
        int recursive = 0;
        int value = 0;
        int size = 0;
        int exec = 0;
        int section = 0;
        int nr_linie = 0;
        if(strcmp(argv[1], "variant") == 0)
        {
            printf("54973\n");
        }
        
        if(strcmp(argv[1],"list") == 0)
    	{
            printf("SUCCESS\n");
            for(int i = 2; i<argc; i++)
            {
            
                
                    if(strstr(argv[i],"path="))
                    {
                        sscanf(argv[i]+5,"%s",path);
                    }
                    if(strcmp(argv[i],"recursive") == 0)
                    {
                        recursive = 1;
                    }
                    if(strstr(argv[i],"size_greater="))
                    {
                        size = 1;
                        sscanf(argv[i]+13,"%d",&value);
                    
                    }
                    if(strstr(argv[i],"has_perm_execute"))
                    {
                        exec = 1;
                    }
            }

          listDir(path,recursive,size,value,exec);
        }
        
        if(strcmp(argv[1],"parse") == 0)
        {   

            if(strstr(argv[2],"path="))
            {
              sscanf(argv[2]+5,"%s",path);
            }
            parse(path);
        }
    
        if(strcmp(argv[1],"extract") == 0)
        {

            printf("SUCCESS");
            if(strstr(argv[2],"path="))
            {
              sscanf(argv[2]+5,"%s",path);
            }
            if(strstr(argv[3],"section="))
            {
                sscanf(argv[3]+8,"%d", &section);
            }
            if(strstr(argv[4],"line="))
            {
                sscanf(argv[4]+5,"%d", &nr_linie);
            }
            extract(path,section,nr_linie);
        }
         if(strcmp(argv[1],"findall") == 0)
        {
            printf("SUCCESS\n");
            if(strstr(argv[2],"path="))
            {
              sscanf(argv[2]+5,"%s",path);
            }
            findall(path);
        }
    }    
 
    return 0;
}