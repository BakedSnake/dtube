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
char DFLAGS[64];

int check_deps(void)
{
    char *deps[] = { DMENU, YTDLP, MPV, XCLIP };
    for (size_t i = 0; i < 4; ++i) {
        FILE *f = fopen(deps[i], "r");
        if (!f) {
            fprintf(stderr, "Could not find %s\n", deps[i]);
            return 0;
        } 

        fclose(f);
    }

    return 1;
}

void gset_dmenu_flags(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i) {
        if (strstr(argv[i], "-") == NULL) {
            strcat(DFLAGS, "\"");
            strcat(DFLAGS, argv[i]);
            strcat(DFLAGS, "\" ");
        } else {
            strcat(DFLAGS, argv[i]);
            strcat(DFLAGS, " ");
        }
    }
}

int main(int argc, char **argv)
{
    char *url = malloc(256);
    if (!check_deps()) return 1;
    gset_dmenu_flags(argc, argv);

    char *menu = get_main_menu();
    FILE *m = popen(menu, "r");
    if (!m) return 1;

    char choice[32];
    fgets(choice, sizeof(choice), m);

    if (strcmp(choice, "6. QUIT\n") == 0) {
        pclose(m);
        return 0;
    }

    if (strcmp(choice, "1. PLAY VIDEO\n") == 0
    || strcmp(choice, "3. DOWNLOAD VIDEO\n") == 0) {
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

    if (strcmp(choice, "2. PLAY VIDEO FROM URL\n") == 0) {
        char *selection = get_from_clipboard_cmd();
        char *playback_cmd = get_video_cmd(selection);
        system(playback_cmd);
        free(playback_cmd);
        free(selection);
    }

    if (strcmp(choice, "3. DOWNLOAD VIDEO\n") == 0) {
        char *download_cmd = get_download_cmd(url);
        system(download_cmd);
        free(download_cmd);
    }

    if (strcmp(choice, "4. DOWNLOAD VIDEO FROM URL\n") == 0) {
        char *selection = get_from_clipboard_cmd();
        char *download_cmd = get_download_cmd(selection);
        system(download_cmd);
        free(download_cmd);
        free(selection);
    }

    if (strcmp(choice, "5. GET URL\n") == 0) {
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
            "echo '1. PLAY VIDEO\n"
            "2. PLAY VIDEO FROM URL\n"
            "3. DOWNLOAD VIDEO\n"
            "4. DOWNLOAD VIDEO FROM URL\n"
            "5. GET URL\n"
            "6. QUIT' | %s %s -l 6 -p 'DTUBE:'", DMENU, DFLAGS);
    return cmd;
}

char *get_search_prompt(void)
{
    char *cmd = malloc(256);
    snprintf(cmd, 256, "echo | %s %s -p 'DTUBE [YTSEARCH]:'", DMENU, DFLAGS);
    return cmd;
}

char *get_search_cmd(char *query)
{
    char *cmd_str = malloc(256);
    snprintf(cmd_str, 128, "yt-dlp ytsearch%s:'%s' ", LENGTH, query);
    strcat(cmd_str, "--print \"%(title)-64.64s  >> %(webpage_url)s\" ");
    
    char dm_cmd[128];
    snprintf(dm_cmd, 128, "| %s %s -l %s -p \"DTUBE:\"", DMENU, DFLAGS, LENGTH);
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

char *get_from_clipboard_cmd(void)
{
    char *cmd = malloc(256);
    snprintf(cmd, 256, "%s -selection clipboard -o", XCLIP);
    FILE *cl = popen(cmd, "r");
    if (!cl) fprintf(stderr, "Could not get clipboard selection\n");
    free(cmd);

    char *sel = malloc(256);
    fgets(sel, 256, cl);
    return sel;
}
