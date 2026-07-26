#include "dtube.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char DMENU[40]  = "/usr/local/bin/dmenu";
char YTDLP[25]  = "/usr/bin/yt-dlp";
char MPV[25]    = "/usr/bin/mpv";
char XCLIP[32]  = "/usr/bin/xclip";
char HEIGHT[4]  = "720";
char LENGTH[3]  = "10";

int check_deps(void)
{
    char *deps[] = { DMENU, YTDLP, MPV, XCLIP };
    for (size_t i = 0; i < 3; ++i) {
        FILE *f = fopen(deps[i], "r");
        if (!f) {
            fprintf(stderr, "Could not find %s\n", deps[i]);
            return 0;
        } 

        fclose(f);
    }

    return 1;
}

int main(void)
{
    char *url = malloc(256);
    if (!check_deps()) return 1;

    char *menu = get_main_menu();
    FILE *m = popen(menu, "r");
    if (!m) return 1;

    char choice[32];
    fgets(choice, sizeof(choice), m);

    if (strcmp(choice, "4. QUIT\n") == 0) {
        pclose(m);
        return 0;
    } else {
        char *prompt = get_search_prompt();
        FILE *sp = popen(prompt, "r");
        if (!sp) return 1;

        char query[128];
        fgets(query, sizeof(query), sp);
        query[strcspn(query, "\n")] = '\0';

        char *search_cmd = get_search_cmd(query);
        FILE *sc = popen(search_cmd, "r");
        if (!sc) return 1;

        char line[256];
        while(fgets(line, sizeof(line), sc)) {
            char *token = strtok(line, ">> ");
            char *last = token;

            while (token != NULL) {
                last = token;
                token = strtok(NULL, ">> ");
            }

            url = last;
        }

        free(search_cmd);
        pclose(sc);
    }

    if (strcmp(choice, "1. PLAY VIDEO\n") == 0) {
        char *playback_cmd = get_video_cmd(url);
        system(playback_cmd);
        free(playback_cmd);
    }

    if (strcmp(choice, "2. DOWNLOAD VIDEO\n") == 0) {
        char *download_cmd = get_download_cmd(url);
        system(download_cmd);
        free(download_cmd);
    }

    if (strcmp(choice, "3. GET URL\n") == 0) {
        url[strcspn(url, "\n")] = '\0';
        char *copy_url = get_url_copy(url);
        system(copy_url);
        free(copy_url);
    }

    pclose(m);
    free(url);
    return 0;
}

char *get_main_menu(void)
{
    char *cmd = malloc(256);
    snprintf(cmd, 256,
            "echo '1. PLAY VIDEO\n2. DOWNLOAD VIDEO\n3. GET URL\n4. QUIT' | %s -l 4 -p 'DDTUBE:'", DMENU);
    return cmd;
}

char *get_search_prompt(void)
{
    char *cmd = malloc(128);
    snprintf(cmd, 128, "echo | %s -p 'DTUBE [YTSEARCH]:'", DMENU);
    return cmd;
}

char *get_search_cmd(char *query)
{
    char *cmd_str = malloc(256);
    snprintf(cmd_str, 128, "yt-dlp ytsearch%s:'%s' ", LENGTH, query);
    strcat(cmd_str, "--print \"%(title)-64.64s  >> %(webpage_url)s\" ");
    
    char dm_cmd[128];
    snprintf(dm_cmd, 128, "| dmenu -l %s -p \"DTUBE:\"", LENGTH);
    strcat(cmd_str, dm_cmd);

    return cmd_str;
}

char *get_video_cmd(char *url)
{
    char *cmd = malloc(128);
    snprintf(cmd, 128, "%s --ytdl --ytdl-format='bv*[height=%s]+ba' '%s'", MPV, HEIGHT, url);
    return cmd;
}

char *get_download_cmd(char *url)
{
    char *cmd = malloc(256);
    snprintf(cmd, 256, "%s -f 'bv*[height=%s]+ba' '%s'", YTDLP, HEIGHT, url);
    return cmd;
}

char *get_url_copy(char *url)
{
    char *cmd = malloc(256);
    snprintf(cmd, 256, "echo '%s' | %s -in -selection clipboard", url, XCLIP);
    return cmd;
}

