#ifndef HEADER
#define HEADER
#include "header.cpp"
#endif

// #include "process_normal_mode_input.cpp"

string get_permissions(struct stat stat_file) {
    string permissions;

    permissions += (S_ISDIR(stat_file.st_mode)? "d": "-");

    permissions += ((stat_file.st_mode & S_IRUSR)? "r": "-");
    permissions += ((stat_file.st_mode & S_IWUSR)? "w": "-");
    permissions += ((stat_file.st_mode & S_IXUSR)? "x": "-");

    permissions += ((stat_file.st_mode & S_IRGRP)? "r": "-");
    permissions += ((stat_file.st_mode & S_IWGRP)? "w": "-");
    permissions += ((stat_file.st_mode & S_IXGRP)? "x": "-");

    permissions += ((stat_file.st_mode & S_IROTH)? "r": "-");
    permissions += ((stat_file.st_mode & S_IWOTH)? "w": "-");
    permissions += ((stat_file.st_mode & S_IXOTH)? "x": "-");

    return permissions;
        
}

bool comp(vector<string> v1, vector<string> v2) {
    return v1[v1.size() - 1] < (v2[v2.size() - 1]);
}

int get_cols(vector<int> max_width) {
    int cols = 0;
    for(int i = 0; i < NUM_LS_FIELDS - 1; i++) {
        cols += max_width[i];
        cols++;
    }
    return cols + 1;
}

string get_size(int f_size_i) {
    double f_size_d = BYTE_TO_KB(f_size_i);
    long long f_size_l = f_size_d * 10;
    f_size_d = f_size_l / 10.0;
    string f_size = to_string(f_size_d);
    f_size = f_size.substr(0, f_size.length() - 5);
    f_size += "K";
    return f_size;
}

void list_directory_content() {
    char *dir_path = path_for_ls;
    DIR *dir;
    struct dirent *entry;
    struct stat stat_file;
    struct tm *timeinfo;
    char time_ch_arr[100];
    // vector<vector<string>> directory_content_all;
    directory_content_all.clear();
    vector<int> max_width(NUM_LS_FIELDS);
    
    dir = opendir(dir_path);

    if(dir == NULL) {
        raise_error("Cannot open directory");
    }
    while((entry = readdir(dir)) != NULL) {
        vector<string> directory_content;
        string f_name = entry->d_name;
        string file_path = dir_path;
        file_path += "/";
        file_path += f_name;
        char *file_name = &file_path[0];
        int field_no = 0;
        if(stat(file_name, &stat_file) == -1) {
            raise_error("Cannot find the status of file");
        }

        string permissions;
        permissions = get_permissions(stat_file);
        directory_content.push_back(permissions);
        max_width[field_no++] = 10;

        uid_t uid = stat_file.st_uid;
        gid_t gid = stat_file.st_gid;


        /* to get the owner and group name from
         owner and group id */
        struct passwd *password = getpwuid(uid);
        struct group *group = getgrgid(gid);
        
        string user_name = password->pw_name;
        max_width[field_no] = max(max_width[field_no], (int)user_name.size());
        field_no++;
        string group_name = group->gr_name;
        max_width[field_no] = max(max_width[field_no], (int)group_name.size());
        field_no++;

        directory_content.push_back(user_name);
        directory_content.push_back(group_name);


        string f_size = get_size(stat_file.st_size);
        directory_content.push_back(f_size);
        max_width[field_no] = max(max_width[field_no], (int)f_size.size());
        field_no++;

        time_t last_mtime = stat_file.st_mtime;
        timeinfo = localtime(&last_mtime);
        
        strftime(time_ch_arr, sizeof(time_ch_arr), "%F %R", timeinfo);
        string time_str = time_ch_arr;
        directory_content.push_back(time_str);
        max_width[field_no] = max(max_width[field_no], (int) time_str.size());
        field_no++;

        directory_content.push_back(f_name);
        max_width[field_no] = max(max_width[field_no], (int) f_name.size());

        directory_content_all.push_back(directory_content);
                
    }
    sort(directory_content_all.begin(), directory_content_all.end(), comp);
    num_entries = directory_content_all.size();
    int width;
    for(int i = 0; i < num_entries; i++) {
        vector<string> v = directory_content_all[i];
        int num_cols = v.size();
        for(int j = 0; j < num_cols; j++) {
            string s = v[j];
            width = max_width[j];
            if(j == num_cols - 1 || j == 1 || j == 2) {
                cout << setw(width) << left << s << " ";
            }
            else {
                cout << setw(width) << right << s << " ";
            }
        }
        cout << "\n";
    }
    move_cursor(num_entries, get_cols(max_width));
}
