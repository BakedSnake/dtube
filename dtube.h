#define _POSIX_C_SOURCE 200112L

#ifndef DTUBE_H
#define DTUBE_H

extern char DMENU[40];
extern char YTDLP[25];
extern char MPV[25];
extern char XCLIP[32];
extern char HEIGHT[4];
extern char LENGTH[3];

/*
 * Creates main menu options
 */

char *get_main_menu(void);

/*
 * Creates search prompt
 */

char *get_search_prompt(void);

/*
 * Creates yt-dlp search command
 */

char *get_search_cmd(char *query);

/*
 * Creates mpv ytdl playback command
 */

char *get_video_cmd(char *url);

/*
 * Creates yt-dlp download command
 */

char *get_download_cmd(char *url);

/*
 * Copy url into clipboard
 */

char *get_url_copy(char *url);

/*
 * Create Get clipboard content command
 */

char *get_from_clipboard_cmd(void);

#endif
