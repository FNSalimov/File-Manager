#include <sys/types.h>
#include <dirent.h>
#include <panel.h>
#include <ncurses.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <clocale>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int n_choices1;
int n_choices2;
std::vector<std::string> choices1;
std::vector<std::string> dirs1;
std::vector<std::string> choices2;
std::vector<std::string> dirs2;
std::string str1_address("/home/firdavs/Firdavs");
std::string str2_address("/home/firdavs/Firdavs");


void open_dir_function(std::string address1, std::string address2) {
    str1_address += "/" + address1, str2_address += "/" + address2;
    choices1.clear(), choices2.clear(), dirs1.clear(), dirs2.clear();
    DIR *dir1, *dir2;
    struct dirent *entry1, *entry2;
    dir1 = opendir(str1_address.c_str());
    struct stat buf1, buf2;
    while ((entry1 = readdir(dir1)) != NULL) {
        std::string str(entry1->d_name);
        std::string result = "/" + str;
        if (lstat((str1_address + result).c_str(), &buf1) == 0)
            (S_ISDIR(buf1.st_mode)) ? dirs1.push_back(str), choices1.push_back(str) : choices1.push_back(str);
    }
    dir2 = opendir(str2_address.c_str());
    while ((entry2 = readdir(dir2)) != NULL) {
        std::string str(entry2->d_name);
        std::string result = "/" + str;
        if (lstat((str2_address + result).c_str(), &buf2) == 0) {
            (S_ISDIR(buf2.st_mode)) ? dirs2.push_back(str), choices2.push_back(str) : choices2.push_back(str);
        }
    }
    sort(choices1.begin(), choices1.end()), sort(choices2.begin(), choices2.end());
    n_choices1 = choices1.size(), n_choices2 = choices2.size();
    closedir(dir1), closedir(dir2);
}

void init_wins(WINDOW **wins, int highlight) {
	int ma_x, ma_y, y, yy, i;
	getmaxyx(stdscr, ma_y, ma_x);
	wins[0] = newwin(ma_y - 2, ma_x/2, 1, 1);
    box(wins[0], 0, 0);
    //wbkgdset(wins[0], COLOR_PAIR(1));
    y = 2;
    for(i = 0; i < n_choices1; ++i) {   
        if(highlight == i + 1) { /* High light the present choice */
            wattron(wins[0], A_REVERSE);
            if (find(dirs1.begin(), dirs1.end(), choices1[i]) != dirs1.end())
                mvwprintw(wins[0], y, 2, "/%s", choices1[i].c_str());
            else
                mvwprintw(wins[0], y, 2, "%s", choices1[i].c_str());
            wattroff(wins[0], A_REVERSE);
        }
        else {
            if (find(dirs1.begin(), dirs1.end(), choices1[i]) != dirs1.end())
                mvwprintw(wins[0], y, 2, "/%s", choices1[i].c_str());
            else
                mvwprintw(wins[0], y, 2, "%s", choices1[i].c_str());
        }
        ++y;
    }
    curs_set(0);
    wrefresh(wins[0]);
    wins[1] = newwin(ma_y - 2, ma_x/2, 1, ma_x/2 + 1);
    box(wins[1], 0, 0);
    //wbkgdset(wins[1], COLOR_PAIR(2));
    yy = 2;
    for(i = 0; i < n_choices2; ++i) {
        if(highlight == i + 1) { /* High light the present choice */
            wattron(wins[1], A_REVERSE);
            if (find(dirs2.begin(), dirs2.end(), choices2[i]) != dirs2.end())
                mvwprintw(wins[1], yy, 2, "/%s", choices2[i].c_str());
            else
                mvwprintw(wins[1], yy, 2, "%s", choices2[i].c_str());
            wattroff(wins[1], A_REVERSE);
        }
        else
            if (find(dirs2.begin(), dirs2.end(), choices2[i]) != dirs2.end())
                mvwprintw(wins[1], yy, 2, "/%s", choices2[i].c_str());
            else
                mvwprintw(wins[1], yy, 2, "%s", choices2[i].c_str());
        ++yy;
    }
    wrefresh(wins[1]);
}

void print_menu(WINDOW *menu_win, int highlight, int current_window) {
    int ma_x, ma_y;
    getmaxyx(menu_win, ma_y, ma_x);
    init_pair(1, COLOR_RED, COLOR_BLUE);
    int sizee, y = 2;
    box(menu_win, 0, 0);
    std::vector<std::string> current_choice = (!current_window) ? choices1 : choices2;
    sizee = (!current_window) ? n_choices1 : n_choices2;
    std::vector<std::string> current_dirs = (!current_window) ? dirs1 : dirs2;
    for(int i = 0; i < sizee; ++i) {   
        if(highlight == i + 1) {/* High light the present choice */  
            if (y > ma_y - 2) {
                wclear(menu_win);
                box(menu_win, 0, 0);
                y = 2;
            }
            wattron(menu_win, COLOR_PAIR(1));
            if (find(current_dirs.begin(), current_dirs.end(), current_choice[i]) != current_dirs.end())
                mvwprintw(menu_win, y, 2, "/%s", current_choice[i].c_str());
            else
                mvwprintw(menu_win, y, 2, "%s", current_choice[i].c_str());
            wattroff(menu_win, COLOR_PAIR(1));
        }
        else
            if (find(current_dirs.begin(), current_dirs.end(), current_choice[i]) != current_dirs.end())
                mvwprintw(menu_win, y, 2, "/%s", current_choice[i].c_str());
            else
                mvwprintw(menu_win, y, 2, "%s", current_choice[i].c_str());
        ++y;
    }
    wrefresh(menu_win);
}

void remove_files(int current_window, int highlight) {
    std::vector<std::string> current_choice = (!current_window) ? choices1 : choices2;
    std::string str_address = (!current_window) ? str1_address + "/" + current_choice[highlight - 1] : str2_address + "/" + current_choice[highlight - 1];
    if (current_choice[highlight - 1][0] != '/' && current_choice[highlight - 1][0] != '.') {
        remove(str_address.c_str());
        current_choice.erase(current_choice.begin() + highlight - 1);
    }
    else if(current_choice[highlight - 1][0] == '/') {
        if (fork() == 0)
            execlp("rm", "rm", "-R", str_address.c_str(), NULL);
        else
            wait(NULL);
    }
    (!current_window) ? choices1 = current_choice : choices2 = current_choice;
    open_dir_function("", "");
}
void copy_files_func(int current_window, int highlight, std::string fy_name) {
    std::vector<std::string> current_choice = (!current_window) ? choices1 : choices2;
    std::string str_address = (!current_window) ? str1_address + "/" + current_choice[highlight - 1] : str2_address + "/" + current_choice[highlight - 1];
    if (fork() == 0)
        execlp("cp", "cp", "-R", str_address.c_str(), fy_name.c_str(), NULL);
    else
        wait(NULL);
}

void move_files_func(int current_window, int highlight, std::string fy_name) {
    std::vector<std::string> current_choice = (!current_window) ? choices1 : choices2;
    std::string str_address = (!current_window) ? str1_address + "/" + current_choice[highlight - 1] : str2_address + "/" + current_choice[highlight - 1];
    if (fork() == 0)
        execlp("mv", "mv", str_address.c_str(), fy_name.c_str(), NULL);
    else
        wait(NULL);
}

int main() {
    setlocale(LC_CTYPE, "");
    //setlocale(LC_ALL, "");
    open_dir_function("", "");
	WINDOW *my_wins[2];
	PANEL *my_panels[2];
    int highlight[2] = {1, 1}, c, max_x, max_y;
	PANEL *top;
	initscr();
    getmaxyx(stdscr, max_y, max_x);
	cbreak();
	noecho();
	keypad(stdscr, true);
	start_color();
	init_pair(1, COLOR_RED, COLOR_WHITE);
    init_pair(2, COLOR_GREEN, COLOR_RED);
    //`wbkgd(stdscr, COLOR_PAIR(1));
    init_wins(my_wins, highlight[0]);                      //changed
    //wbkgd(my_wins[0], COLOR_PAIR(1));
    //wbkgd(my_wins[1], COLOR_PAIR(2));
    my_panels[0] = new_panel(my_wins[0]);
    my_panels[1] = new_panel(my_wins[1]);
    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[0]);
    mvprintw(0, 0, "TAB-change panel;F2-exit; F3-remove;F4-new directory;F5-copy;F6-move");
    update_panels();
    doupdate();
    top = my_panels[1];
    int current_window = 0;
    int n_choices;
    lable:
    top = (PANEL *)panel_userptr(top);
    top_panel(top);
    (top == my_panels[0]) ? current_window = 0, n_choices = n_choices1 : current_window = 1, n_choices = n_choices2;
    keypad(my_wins[current_window], true);
    WINDOW *win_forrr, *win_forr, *win_for;
    PANEL *panel_forrr, *panel_forr, *panel_for;
    int yy, xx, yyy, xxx, yyyy, xxxx, highlight_for;
    std::string di_name, fy_name;
    while(1) {
        c = wgetch(my_wins[current_window]);
        if (c == 9)
            goto lable;
        else if (c == 266) {
            for (int i = 0; i < 2; i++) {
                del_panel(my_panels[i]);
                delwin(my_wins[i]);
            }
            endwin();
            return 0;
        }
        n_choices = (!current_window) ? n_choices1 : n_choices2;
        switch(c) {
            case KEY_UP:
                if(highlight[current_window] == 1)
                    highlight[current_window] = n_choices;
                else
                    --highlight[current_window];
                break;
            case KEY_DOWN:
                if(highlight[current_window] == n_choices)
                    highlight[current_window] = 1;
                else 
                    ++highlight[current_window];
                break;
            case 10:
                (!current_window) ? open_dir_function(choices1[highlight[current_window]-1], "") : open_dir_function("", choices2[highlight[current_window]-1]);
                n_choices = (!current_window) ? n_choices1 : n_choices2;
                highlight[current_window] = 1;
                wclear(my_wins[current_window]);
                break;
            case 267:
                win_for = newwin(max_y/6, max_x/5, max_y/3, 2*max_x/5);
                panel_for = new_panel(win_for);
                show_panel(panel_for);
                box(win_for, 0, 0);
                getmaxyx(win_for, yy, xx);
                keypad(win_for, true);
                mvwprintw(win_for, yy/4, xx/2 - 3, "REALLY?");
                wattron(win_for, COLOR_PAIR(1)); 
                mvwprintw(win_for, yy/2, xx/3, "YES");
                wattroff(win_for, COLOR_PAIR(1));
                mvwprintw(win_for, yy/2, 2*xx/3, "NO");
                highlight_for = 1;
                while(1) {
                    int c_for = wgetch(win_for);
                    if (c_for == 9) {
                        if (highlight_for == 1) {
                            mvwprintw(win_for, yy/4, xx/2 - 3, "REALLY?");
                            mvwprintw(win_for, yy/2, xx/3, "YES");
                            wattron(win_for, COLOR_PAIR(1)); 
                            mvwprintw(win_for, yy/2, 2*xx/3, "NO");
                            wattroff(win_for, COLOR_PAIR(1));
                            highlight_for = 2;
                        }
                        else {
                            mvwprintw(win_for, yy/4, xx/2 - 3, "REALLY?");
                            wattron(win_for, COLOR_PAIR(1));
                            mvwprintw(win_for, yy/2, xx/3, "YES");
                            wattroff(win_for, COLOR_PAIR(1)); 
                            mvwprintw(win_for, yy/2, 2*xx/3, "NO");
                            highlight_for = 1;
                        }
                    }
                    else if(c_for == 10) {
                        if (highlight_for == 1) {
                            remove_files(current_window, highlight[current_window]);
                            n_choices = (!current_window) ? n_choices1 : n_choices2;
                            if (current_window == 0 && highlight[0] > (int)(choices1.size()))
                                highlight[0] = choices1.size();
                            else if(current_window == 1 && highlight[1] > (int)(choices2.size()))
                                highlight[1] = choices2.size();
                            wclear(my_wins[0]);
                            wclear(my_wins[1]);
                            print_menu(my_wins[0], highlight[0], 0);
                            print_menu(my_wins[1], highlight[1], 1);
                            break;
                        }
                        else
                            break;
                    }
                }
                del_panel(panel_for);
                delwin(win_for);
                break;
            case 268:
                win_forr = newwin(max_y/6, max_x/4, max_y/3, 2*max_x/5);
                panel_forr = new_panel(win_forr);
                show_panel(panel_forr);
                box(win_forr, 0, 0);
                getmaxyx(win_forr, yyy, xxx);
                keypad(win_forr, true);
                mvwprintw(win_forr, yyy/4, xxx/5, "Enter the name of directory");
                wmove(win_forr, 2, 2);
                curs_set(1);
                echo();
                di_name = "/";
                while(1) {
                    int dir_name = wgetch(win_forr);
                    if (dir_name == 10)
                        break;
                    di_name += static_cast<char>(dir_name);
                }
                noecho();
                curs_set(0);
                (!current_window) ? mkdir((str1_address + di_name).c_str(), 0777) : mkdir((str2_address + di_name).c_str(), 0777);
                open_dir_function("", "");
                wclear(my_wins[0]);
                wclear(my_wins[1]);
                print_menu(my_wins[0], highlight[0], 0);
                print_menu(my_wins[1], highlight[1], 1);
                del_panel(panel_forr);
                delwin(win_forr);
                break;
            case 269:
                win_forrr = newwin(max_y/6, max_x/4, max_y/3, 2*max_x/5);
                panel_forrr = new_panel(win_forrr);
                show_panel(panel_forrr);
                box(win_forrr, 0, 0);
                getmaxyx(win_forrr, yyyy, xxxx);
                keypad(win_forrr, true);
                mvwprintw(win_forrr, yyyy/4, xxxx/5, "Copy to(address): ");
                wmove(win_forrr, 2, 2);
                curs_set(1);
                echo();
                fy_name = "/";
                while(1) {
                    int fyl_name = wgetch(win_forrr);
                    if (fyl_name == 10)
                        break;
                    fy_name += static_cast<char>(fyl_name);
                }
                noecho();
                curs_set(0);
                copy_files_func(current_window, highlight[current_window], fy_name);
                fy_name = "";
                open_dir_function("", "");
                wclear(my_wins[0]);
                wclear(my_wins[1]);
                print_menu(my_wins[0], highlight[0], 0);
                print_menu(my_wins[1], highlight[1], 1);
                del_panel(panel_forrr);
                delwin(win_forrr);
                break;
            case 270:
                win_forrr = newwin(max_y/6, max_x/4, max_y/3, 2*max_x/5);
                panel_forrr = new_panel(win_forrr);
                show_panel(panel_forrr);
                box(win_forrr, 0, 0);
                getmaxyx(win_forrr, yyyy, xxxx);
                keypad(win_forrr, true);
                mvwprintw(win_forrr, yyyy/4, xxxx/5, "Move to(address): ");
                wmove(win_forrr, 2, 2);
                curs_set(1);
                echo();
                fy_name = "/";
                while(1) {
                    int fyl_name = wgetch(win_forrr);
                    if (fyl_name == 10)
                        break;
                    fy_name += static_cast<char>(fyl_name);
                }
                noecho();
                curs_set(0);
                move_files_func(current_window, highlight[current_window], fy_name);
                fy_name = "";
                open_dir_function("", "");
                wclear(my_wins[0]);
                wclear(my_wins[1]);
                print_menu(my_wins[0], highlight[0], 0);
                print_menu(my_wins[1], highlight[1], 1);
                del_panel(panel_forrr);
                delwin(win_forrr);
                break;
        }
        print_menu(my_wins[current_window], highlight[current_window], current_window);
        update_panels();
        doupdate();
    }
    return 0;
}
