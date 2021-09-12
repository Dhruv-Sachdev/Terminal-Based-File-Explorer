#ifndef HEADER
#define HEADER
#include "header.h"
#endif

#ifndef PROCESS_NORMAL_MODE
#define PROCESS_NORMAL_MODE
#include "process_normal_mode_input.h"
#endif

mode_t get_file_permissions(struct stat &stat_file) {
    mode_t file_mode = 0;
    if(stat_file.st_mode & S_IRUSR) {
        file_mode |= S_IRUSR;
    }
    if(stat_file.st_mode & S_IWUSR) {
        file_mode |= S_IWUSR;
    }
    if(stat_file.st_mode & S_IXUSR) {
        file_mode |= S_IXUSR;
    }

    if(stat_file.st_mode & S_IRGRP) {
        file_mode |= S_IRGRP;
    }
    if(stat_file.st_mode & S_IWGRP) {
        file_mode |= S_IWGRP;
    }
    if(stat_file.st_mode & S_IXGRP) {
        file_mode |= S_IXGRP;
    }
    
    if(stat_file.st_mode & S_IROTH) {
        file_mode |= S_IROTH;
    }
    if(stat_file.st_mode & S_IWOTH) {
        file_mode |= S_IWOTH;
    }
    if(stat_file.st_mode & S_IXOTH) {
        file_mode |= S_IXOTH;
    }
    // cout << "File permissions " << file_mode << "\n";
    return file_mode;
}

string get_absolute_path(string path) {
    string abs_path;
    if(path[0] == '~') {
        abs_path = home_path;
        abs_path += path.substr(1);
    }
    else {
        abs_path = path_for_ls;
        abs_path += "/";
        abs_path += path;
    }
    return abs_path;
}

void rename_file(string src, string dest) {
    cout << src << "\n";
    cout << dest << "\n";
    rename(src.c_str(), dest.c_str());
    cout << "Done\n";
}

string join_path(string one, string two) {
    one += "/";
    one += two;
    return one;
}

void create_file(string path, int permissions) {
    int fd;
    if((fd = creat(path.c_str(), permissions)) == -1) {
        raise_error("Could not create file\n");
    }
    close(fd);
}

void create_dir(string path, int permissions) {
    mkdir(path.c_str(), permissions);
    chmod(path.c_str(), permissions);
}


void remove_file(string path) {
    string delete_file_path = get_absolute_path(path);
    struct stat stat_file;
    if(stat(delete_file_path.c_str(), &stat_file) < 0) {
        raise_error("Could not get status");
    }
    if(!S_ISDIR(stat_file.st_mode)) {
        // cout << delete_file_path << "\n";
        remove(delete_file_path.c_str());
    }
    
    // string delete_file_path = home_path;
    // delete_file_path += "/";
    // delete_file_path += relative_file_path;
    // cout << "Delete file path is" << delete_file_path << "\n";
    // remove(&delete_file_path[0]);
}

void remove_dir(string path) {
    // cout << path << "\n";
    DIR *dir;
    struct dirent *entry;
    dir = opendir(path.c_str());
    if(dir == NULL) {
        if(errno == ENOTDIR) {
            remove(path.c_str());
            // cout << "Not dir\n";
        }
        return;
    }
    while((entry = readdir(dir)) != NULL) {
        string f_name = entry->d_name;
        if(f_name == "." || f_name == "..") {
            continue;
        }
        string file_path = join_path(path, f_name);
        remove_dir(file_path);
    }
    rmdir(path.c_str());
    // cout << "removing dir now " << path << "\n";

} 

void cpy_file_helper(string src_path, string dest_path) {
    
    char buf[BUFSIZ];
    size_t size;

    struct stat stat_file;
    if(stat(src_path.c_str(), &stat_file) < 0) {
        raise_error("Could not find stat");
    }
    mode_t file_permissions = get_file_permissions(stat_file);
    // create_file(dest_path, file_permissions);

    FILE* src = fopen(src_path.c_str(), "rb");
    FILE* dest = fopen(dest_path.c_str(), "wb");
    
    while (size = fread(buf, 1, BUFSIZ, src)) {
        fwrite(buf, 1, size, dest);
    }

    fclose(src);
    fclose(dest);

    chmod(dest_path.c_str(), file_permissions);

}

void cpy_directory_helper(string src_dir_path, string dest_dir_path) {
    struct stat stat_file;
    if(stat(src_dir_path.c_str(), &stat_file) < 0)
        return;
    int dest_dir_perm = get_file_permissions(stat_file);
    create_dir(dest_dir_path, dest_dir_perm);
    DIR *dir;
    dir = opendir(src_dir_path.c_str());
    if(dir == NULL)
        return;
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL) {
        string f_name = entry->d_name;
        if(f_name == "." || f_name == "..")
            continue;
        string src_file_path = join_path(src_dir_path, f_name);
        string dest_file_path = join_path(dest_dir_path, f_name);
        struct stat stat_file;
        if(stat(src_file_path.c_str(), &stat_file) < 0)
            continue;
        if(S_ISDIR(stat_file.st_mode)) {
            cpy_directory_helper(src_file_path, dest_file_path);
            //remove if move called
        }
        else {
            cpy_file_helper(src_file_path, dest_file_path);
            //remove if move called
        }
    }
    closedir(dir);
    return;

}

void cpy_file(string cmd, bool to_delete = false) {
    stringstream ss(cmd);
    string c1;
    ss >> c1;
    string src_path, path, dest_path, dest_dir, file_name;
    vector<string> paths;
    while(ss >> path) {
        paths.push_back(path);
    }
    int n = paths.size();
    dest_dir = paths[n - 1];
    dest_dir = get_absolute_path(dest_dir);
    struct stat stat_file;
    for(int i = 0; i < n - 1; i++) {
        src_path = get_absolute_path(paths[i]);
        if(stat(src_path.c_str(), &stat_file) < 0) {
            raise_error("Could not find status");
        }
        size_t pos = paths[i].rfind('/');
        if(pos != string::npos)
            file_name = paths[i].substr(pos + 1);
        else
            file_name = paths[i];
        
        dest_path = join_path(dest_dir, file_name);
        cout << src_path << "\n";
        cout << dest_path << "\n";
        if(!S_ISDIR(stat_file.st_mode)) {
            cpy_file_helper(src_path, dest_path);
            if(to_delete) {
                remove(src_path.c_str());
            }
        }
        else {
            cpy_directory_helper(src_path, dest_path);
            if(to_delete) {
                remove_dir(src_path);
            }
        }

    }
}

bool search(string search_name, string path = path_for_ls) {
    string file_name;
    struct dirent *entry;
    DIR *dir;
    dir = opendir(path.c_str());
    if(dir == NULL) {
        // cout << "Error no" << errno << "\n";
        return false;
    }
    while((entry = readdir(dir)) != NULL) {
        struct stat stat_file;
        string f_name = entry->d_name;
        if(f_name == "." || f_name == "..")
            continue;
        // cout << f_name << "\n";
        if(f_name == search_name)
            return true;
        string f_path = join_path(path, f_name);
        if(stat(f_path.c_str(), &stat_file) < 0) {
            continue;
        }
        if(S_ISDIR(stat_file.st_mode)) {
            if(search(search_name, f_path))
                return true;
        }
        
    }
    return false;

}


int process_command(string cmd) {
    stringstream ss(cmd);
    string c1;
    string c2;
    ss >> c1;

    if(c1 == "copy") {
        cpy_file(cmd);
    }
    else if(c1== "move") {
        cpy_file(cmd, true);
    }
    else if(c1== "rename") {
        string src_path, dest_path;
        ss >> src_path;
        ss >> dest_path;
        string src_abs_path = get_absolute_path(src_path);
        string dest_abs_path = get_absolute_path(dest_path);
        rename_file(src_path, dest_path);
    }
    else if(c1== "create_file") {
        string file_name, dir_path, file_path;
        ss >> file_name;
        ss >> dir_path;
        file_path = get_absolute_path(dir_path);
        file_path = join_path(file_path, file_name);
        cout << file_path << "\n";
        file_path += "/";
        file_path += file_name;
        create_file(file_path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    }
    else if(c1== "create_dir") {
        string dir_name, dir_path, file_path;
        ss >> dir_name;
        ss >> dir_path;
        file_path = get_absolute_path(dir_path);
        struct stat stat_file;
        stat(file_path.c_str(), &stat_file);
        int permissions = get_file_permissions(stat_file);
        file_path += "/";
        file_path += dir_name;
        cout << file_path;
        create_dir(file_path, permissions);
    }
    else if(c1== "delete_file") {
        string relative_file_path;
        ss >> relative_file_path;
        remove_file(relative_file_path);
        
    }
    else if(c1== "delete_dir") {
        string path;
        ss >> path;
        path = get_absolute_path(path);
        // cout << path << "\n";
        remove_dir(path);
    }
    else if(c1== "goto") {
        string dir_path = home_path;
        string s;
        ss >> s;
        dir_path += s.substr(1);
        goto_dir(dir_path);
        move_cursor(window_size.ws_row, 1);
    }
    else if(c1 == "search") {
        string name;
        ss >> name;
        printf("\033[2K");
        move_cursor(window_size.ws_row, 1);
        cout << ((search(name)) ? "true": "false");
        
        return 1;
    }
    else {
        cout << "Command is invalid";
    }
    return 0;

}

void enter_command_mode() {
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &window_size);
    move_cursor(window_size.ws_row, 1);
    printf(":");
    string cmd;
    while(1) {
        char c = cin.get();
        if(c == ESCAPE) {
            printf("Going back to normal mode\n");
            return;
        }
        else if(c == BACKSPACE) {
            if(!cmd.empty())
                cmd.pop_back();
                printf("\033[2K");
                move_cursor(window_size.ws_row, 1);
                printf(":");
                cout << cmd;
        }
        else if(c == ENTER) {
            // cout << "Processing cmd now\n";
            int ret = process_command(cmd);
            cmd = "";
            if(ret != 1) {
                printf("\033[2K");
                move_cursor(window_size.ws_row, 1);
                printf(":");
                
            }
        }
        else {
            cmd += c;
            cout << c;
        }
        
        
    }
}