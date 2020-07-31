
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>                                                                                               
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "parser.h"
#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
using namespace std;

struct local_assignments{
    string name = "empty";
    string value = "empty";
};


bool check_if_background(char* command){
    if (strcmp(command,"set") == 0){
        return true;
    }
    else if(strcmp(command,"cd") == 0 ){
        return true;
    }
    else if(strcmp(command,"exit") == 0 ){
        return true;
    }
    else if(strcmp(command,"jobs") == 0 ){
        return true;
    }
    else if(strcmp(command,"kill") == 0 ){
        return true;
    }
    else if(strcmp(command,"fg") == 0 ){
        return true;
    }
    else if(strcmp(command,"bg") == 0 ){
        return true;
    }
    else if(strcmp(command,"pwd") == 0 ){
        return true;
    }
    else {
        return false;
    }
}

bool check_if_builtin2(char* command){

    string buffer = command;

    if (strcmp(command,"set") == 0){
        return true;
    }
    else if(strcmp(command,"cd") == 0 ){
        return true;
    }
    else if (strcmp(command,"pwd") == 0){
        return true;
    }
    else if(strcmp(command,"exit") == 0 ){
        return true;
    }
    else if (buffer.find("export") != string::npos){
        return true;
    }
    else{
        if(buffer.find("cd") != string::npos && buffer.length() > 2){
            return true;
        }
    }
    return false;
}

string find_built_in(char* command){

    string buffer = command;

    if (strcmp(command,"set") == 0){
        buffer = "set";
        return buffer;
    }
    else if (strcmp(command,"pwd") == 0){
        buffer = "pwd";
        return buffer;
    }
    else if(strcmp(command,"exit") == 0 ){
        buffer = "exit";
        return buffer;
    }
    else if (buffer.find("export") != string::npos){
        buffer = "export";
        return buffer;
    }
    else{
        if(buffer.find("cd") != string::npos){
            buffer = "cd";
            return buffer;
        }
    }
    buffer = "empty";
    return buffer;
}



void print_Debugging(CommandData* mydata){
    bool background_command = false;
    cout << "Number of Simple Commands: " << mydata -> numcommands  << endl;
        if(mydata -> numcommands >= 1){
            for(int y = 0; y < mydata -> numcommands; y++){
                cout << "Command " << y << ": " << mydata -> TheCommands[y].command << endl;
                if(mydata -> TheCommands[y].numargs > 0){
                    for(int i = 0; i < mydata -> TheCommands[y].numargs; i++){
                        cout << "Args[" << i << "]: " << mydata -> TheCommands[y].args[i] << endl;
                    }
                }
                if(strcmp(mydata -> TheCommands[y].command,"exit") == 0){
                   // stop_loop = true;
                }
               background_command = check_if_background(mydata -> TheCommands[y].command);
            }
        }
        if(mydata -> infile == NULL){
            cout << "Input File: " << "(NULL)" << endl;
        }
        else{
            cout << "Input File: " << mydata -> infile << endl;
        }
        if(mydata -> outfile == NULL){
            cout << "Output File: " << "(NULL)" << endl;
        }
        else{
            cout << "Output File: " << mydata -> outfile << endl;
        }
        if(mydata -> background == 0){
            cout << "Background: " << "Off" << endl;
        }
        else{
            cout << "Background: " << "On" << endl; 
        }
        if(background_command == true){
            cout << "Built-In Command: " << "Yes" << endl;
        }
        else{
            cout << "Built-In Command: " << "No" << endl; 
        }
}

bool is_Debug_Variable(string word){

    if(word.find("DEBUG") != string::npos){
        if(word.at(5) == '='){
            if(word.at(6) == 'n' && word.at(7) == 'o'){
                cout << "Debugging  is now off";
                return false;
            }
            if(word.at(6) == 'y' && word.at(7) == 'e' && word.at(8) == 's'){
                cout << "Debugging is now on";  
                return true;
            }
        }
    }
    else{
        return false;
    }
}

bool is_Home_Variable(string word){
    if(word.find("HOME") != string::npos && word.length() > 4){
        if(word.at(4) == '='){
            return true;
        }
        else {
            return false;
        }
    }
}

bool is_Path_Variable(string word){
    if(word.find("PATH") != string::npos && word.length() > 4){
        if(word.at(4) == '='){
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}





int main(int argc, char* argv[]){

    static local_assignments locals[100];
    local_assignments exports[100];

    int saved_stdout;
    int saved_stdin;

    bool stop_loop = false;
    int counter = 0;
    int counter2= 0;
    setenv("DEBUG","no",1);
    while(stop_loop == false){
        bool background_command = false;
        char path[255]; 
        *getcwd(path,sizeof(path));
        cout << endl;
        cout << path ;
        cout << " ";
        string line;
        getline(cin,line);
        char line_char[line.size() + 1];
        strcpy(line_char,line.c_str());

        CommandData* mydata;
        CommandData mydatastruct;
        mydata = &mydatastruct;
        int success = 0;
        success = ParseCommandLine(line_char,mydata);

        char* Debug = getenv("DEBUG");
        char* home_dir = getenv("HOME");
        if(Debug[0] == 'n' && Debug[1] == 'o'){

        }
        else{
            print_Debugging(mydata);
        }


        if(mydata -> numcommands <2 && mydata -> numcommands != 0){


            if(check_if_builtin2(mydata -> TheCommands[0].command) == true){
                string built_in = find_built_in(mydata -> TheCommands[0].command);
                if (built_in == "cd"){
                    cout << "We found cd command" << endl;
                    if(mydata -> TheCommands[0].numargs > 0){ // if cd has an argument
                        chdir(mydata -> TheCommands[0].args[0]);
                    }
                    else{ // otherwise take us to the home directory
                        chdir(home_dir);
                        cout << "Your direc should be changed";
                    }

                }
                if (built_in == "set"){
                    for(int xy = 0; xy < 99; xy++){
                        if(locals[xy].name != "empty"){
                            cout << locals[xy].name << " " << locals[xy].value << endl;
                        }
                    }

                }
                if (built_in == "pwd"){
                    cout << "Current Working Directory: ";
                    cout << path << endl;
                }
                if (built_in == "exit"){
                    cout << "We found exit command " << endl;
                    stop_loop = true;
                    int parent = getppid();
                    kill(parent, SIGTERM);
                    exit(1);
                }
                if (built_in == "export"){
                    string varname = mydata -> TheCommands[0].args[0];
                    for(int z = 0; z < 99; z++){
                        if(locals[z].name == varname){
                            char name[locals[z].name.size() + 1];
                            strcpy(name,locals[z].name.c_str());
                            char value[locals[z].value.size() + 1];
                            strcpy(value,locals[z].value.c_str());
                            setenv(name,value,1);
                            exports[counter2].name = locals[z].name;
                            counter2++;
                        }
                    }

                }

            }
            else{
              int status;
            int number = fork();

            if(number > 0){
                 saved_stdin = dup(STDIN_FILENO);
                 saved_stdout = dup(STDOUT_FILENO);
            }
            if(number == 0){
                if(mydata -> infile != NULL){
                    int fd0;
                    fd0 = open(mydata -> infile, O_RDONLY, 0);
                    dup2(fd0, 0); 
                    close(fd0);
                }

                if(mydata ->outfile != NULL){     
                    int fd = open(mydata ->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    dup2(fd,1);
                    close(fd);   
                }
                if(mydata ->outfile != NULL){     
                    int fd = open( mydata ->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
                    int _fd_dup = dup2(fd,1);
                    close(fd);   
                }

                string buffer;

                buffer = mydata -> TheCommands[0].command ;

                 if(buffer.find("=") != string::npos){
                         if(is_Debug_Variable(buffer) != true){
                            if(is_Home_Variable(buffer) != true){
                                 if(is_Path_Variable(buffer) != true){
                                    int i = buffer.find("=");
                                    string tmp = buffer.substr(0,i);
                                    string tmp2 = buffer.substr(i+1,buffer.length()-1);
                                    bool flag = false;
                                    int index = 0;
                                    for(int j = 0; j < counter; j++){
                                        if(locals[j].name == tmp){
                                            flag = true;
                                            index = j;
                                            break;
                                        }
                                    }
                                    if(flag == true){  
                                        locals[index].name = tmp;
                                        locals[index].value = tmp2;
                                        bool export_flag = false;
                                        for(int z = 0; z < 99; z++){
                                            if(exports[z].name == tmp){
                                                char char_export_name[tmp.size() + 1];
                                                strcpy(char_export_name,tmp.c_str());
                                                char char_export_value[tmp2.size() + 1];
                                                strcpy(char_export_value,tmp2.c_str());
                                                setenv(char_export_name,char_export_value,1);
                                            }
                                        }
                                        
                                    }
                                    else{
                                        locals[counter].name = buffer.substr(0,i);
                                        locals[counter].value = buffer.substr(i+1,buffer.length()-1);
                                        counter++;
                                    }
                                 }
                            }
                         }
                 }

                // This block covers the debug environment variable
                ////////////////////////////////////////////////
                    bool debug_test = is_Debug_Variable(buffer);
                    if(debug_test == true){
                        setenv("DEBUG","yes",1);
                    }
                    else{
                        setenv("DEBUG","no",1);
                    }
                    //////////////////////////////////////////////////
                    // This block covers the HOME environment variable
                    bool home_test = is_Home_Variable(buffer);
                    if (home_test == true){
                        string buffer2 = buffer.substr(5);
                        char* c2 = strcpy(new char[buffer2.length() + 1], buffer2.c_str());
                        int does_Directory_exist = 1;
                        does_Directory_exist = access(c2,0);
                        if(does_Directory_exist == 0){
                            setenv("HOME",c2,1);
                            delete[] c2;
                            cout << "Home has been set. Type HOME to see your new home directory" << endl;
                        }
                        else{
                            cout << "Not a valid directory" << endl;
                        }
                    }
                    if(buffer.length() == 4 && buffer.compare("HOME") == 0){
                        cout << "Home Directory: " << home_dir << endl;
                    }
                    //////////////////////////////////////////////////////
                    // This block covers the PATH environment variable
                    bool path_test = is_Path_Variable(buffer);
                
                    if(path_test == true){
                        string buffer3 = buffer.substr(5);
                        std::vector<char> chars(buffer3.begin(), buffer3.end());
                        chars.push_back('\0');
                        char *c3 = &chars[0];
                        int does_Directory_exist = 1;
                        does_Directory_exist = access(c3,0);
                        if(does_Directory_exist == 0){
                            setenv("PATH",c3,1);
                            cout << "Path has been set. Type PATH to see your total path" << endl;
                        }
                    }
                    if(buffer.length() == 4 && buffer.compare("PATH") == 0){
                        char* newpath = getenv("PATH");
                        cout << "PATH : " << newpath << endl;
                        free(newpath);
                    }
                ///////////////////////////////////////////////////////////
                // check for manually exported variables
                //////////////////////////////////////////////////////////

                for(int check = 0; check < 100; check++){
                   
                    if(exports[check].name == buffer){
                        char char_export[exports[check].name.size() + 1];
                        strcpy(char_export,exports[check].name.c_str());
                        char* exportvar = getenv(char_export);
                        cout << exportvar << endl;
                    }
                  
                }
            
                int accessResult = 1;
                accessResult = access(mydata -> TheCommands[0].command,0);

                if (accessResult == 0){
                    int numargs = mydata -> TheCommands[0].numargs;
                    char** args = NULL;
                    args = new char*[numargs + 2];                  
                    args[0] = mydata -> TheCommands[0].command;

                    for(int i = 1; i < numargs + 2; i++){

                        if(i + 1 >= numargs + 2){
                            args[i] = NULL;
                        }
                        else{
                            args[i] =mydata -> TheCommands[0].args[i-1];
                        }

                    }
                    if ( number == 0 )
                        execv( args[0], args ); // child: call execv with the path and the args
                    else
                        wait( &status );        // parent: wait for the child 
                }
                else{
                    int numargs = mydata -> TheCommands[0].numargs;
                    char** args = NULL;
                    args = new char*[numargs + 2];                  
                    args[0] = mydata -> TheCommands[0].command;

                    for(int i = 1; i < numargs + 2; i++){

                        if(i + 1 >= numargs + 2){
                            args[i] = NULL;
                        }
                        else{
                            args[i] =mydata -> TheCommands[0].args[i-1];
                        }

                    }

                    if ( number == 0 )
                        execvp( args[0], args ); // child: call execv with the path and the args
                    else
                        wait( &status );        // parent: wait for the child 
                }
            }
            if(number > 0){
                while (!(wait(&status) == number)); 
            }
            }
        }
    }





    exit(0);
}

