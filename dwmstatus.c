#define _BSD_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

/* Wireless */
#include <iwlib.h>
#include <mpd/client.h>

#define BATT_NOW    "/sys/class/power_supply/BAT0/charge_now"
#define BATT_FULL   "/sys/class/power_supply/BAT0/charge_full"
#define BATT_STATUS "/sys/class/power_supply/BAT0/status"

/* Alsa stuff */
#define VOL "Master"
#define CH  "default"

/* Time settings */
#define DEFAULT_TZ "UTC"
#define TIME_FMT "%a %F / %T"

/* Wifi interface*/
#define INTERFACE "wlo1"

/* mpd */
#define MPD_SERVER "localhost"
#define MPD_PORT 6600

char *
smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	if ((ret=malloc(++len)) == NULL) {
		perror("malloc");
		exit(1);
	}

	va_start(fmtargs, fmt);
	vsnprintf(ret, len, fmt, fmtargs);
	va_end(fmtargs);

	return ret;
}

char *
get_time(char *tzname)
{
	char buf[30];
	time_t tim;
	struct tm *timtm;

	memset(buf, 0, sizeof(buf));
	tim = time(NULL);
	timtm = localtime(&tim);

	if ((timtm=localtime(&tim)) == NULL) {
		perror("localtime");
		exit(1);
	}

	if (!strftime(buf, sizeof(buf)-1, TIME_FMT, timtm)) {
		fprintf(stderr, "strftime == 0\n");
		exit(1);
	}

	return smprintf("[%s]", buf);
}

char*
get_bat()
{
    long  lnum1, lnum2 = 0;
    char* status = malloc(sizeof(char)*12);
    char  s = '?';
    FILE* fp = NULL;

    if ((fp = fopen(BATT_NOW, "r"))) {
        fscanf(fp, "%ld\n", &lnum1);
        fclose(fp);
        fp = fopen(BATT_FULL, "r");
        fscanf(fp, "%ld\n", &lnum2);
        fclose(fp);
        fp = fopen(BATT_STATUS, "r");
        fscanf(fp, "%s\n", status);
        fclose(fp);
        if (strcmp(status,"Charging") == 0)
            s = '+';
        if (strcmp(status,"Discharging") == 0)
            s = '-';
        if (strcmp(status,"Full") == 0)
            s = '=';
        return smprintf("[%c%ld]", s,(lnum1/(lnum2/100)));
    }
    else return smprintf("");
}

char*
runcmd(char* cmd)
{
	FILE* fp = popen(cmd, "r");
	char ln[30];
	if (fp == NULL) return NULL;
	fgets(ln, sizeof(ln)-1, fp);
	pclose(fp);
	ln[strlen(ln)-1]='\0';
	return smprintf("%s", ln);
}

char*
get_vol()
{
	int volume;
	char* result = runcmd("amixer get Master |awk '/Mono:/ {print $6}'");

	if(result == NULL) {
		return smprintf("");
	}

	if(strcmp(result,"[on]") != 0) {
		return smprintf("[Mute]");
	}

	result = runcmd("amixer get Master |awk '/Mono:/ {print $4}' | sed -e 's/\\[//g'|sed -e 's/%\\]//g'");

	if(result == NULL) {
		return smprintf("");
	}
	
    sscanf(result, "%i", &volume);
	return smprintf("[%d%%]", volume);
}

char*
get_wifi()
{
	int sock;
	struct iwreq wrq;
	char essid[40];

	sock = iw_sockets_open();

	/* Get ESSID */
	wrq.u.essid.pointer = (caddr_t) essid;
	wrq.u.essid.length = IW_ESSID_MAX_SIZE + 1;
	wrq.u.essid.flags = 0;

	if(iw_get_ext(sock, INTERFACE, SIOCGIWESSID, &wrq) < 0) {
		return smprintf("[iw_get_ext fail]");
	}

	iw_sockets_close(sock);

	return smprintf("[%s:%d]", essid, wrq.u.qual.qual);
}

char*
get_mpd()
{
	int elapsed = 0, total = 0;
	char* retstr = NULL;
	const char* title = NULL;
	const char* artist = NULL;
	struct mpd_song* song = NULL;
	struct mpd_connection* conn ;
	struct mpd_status* theStatus;

	if (!(conn = mpd_connection_new(MPD_SERVER, 0, MPD_PORT)) || mpd_connection_get_error(conn)){
		return smprintf("[MPD connection error]");
	}

	mpd_command_list_begin(conn, true);
	mpd_send_status(conn);
	mpd_send_current_song(conn);
	mpd_command_list_end(conn);

	theStatus = mpd_recv_status(conn);

	if ((theStatus) && (mpd_status_get_state(theStatus) == MPD_STATE_PLAY)) {
		mpd_response_next(conn);
		song = mpd_recv_song(conn);
		title = smprintf("%s",mpd_song_get_tag(song, MPD_TAG_TITLE, 0));
		artist = smprintf("%s",mpd_song_get_tag(song, MPD_TAG_ARTIST, 0));

		elapsed = mpd_status_get_elapsed_time(theStatus);
		total = mpd_status_get_total_time(theStatus);
		mpd_song_free(song);
		retstr = smprintf("[%.2d:%.2d/%.2d:%.2d %s - %s]",
						elapsed/60, elapsed%60,
						total/60, total%60,
						artist, title);
		free((char*)title);
		free((char*)artist);
	}
	else {
		retstr = smprintf("");
	}
	mpd_response_finish(conn);
	mpd_connection_free(conn);

	return retstr;
}

int
main(void)
{
	static Display *dpy;

	char *stat = NULL;
	char *tz   = NULL;
	char *tm   = NULL;
	char *bat  = NULL;
	char *vol  = NULL;
	char *wifi = NULL;
	char *mpd  = NULL;

	/* Get the timezone from the shell variable. If it's not found use utc */
	if((tz=getenv("TZ")) == NULL)
	{
		tz = DEFAULT_TZ;
	}

	if (!(dpy = XOpenDisplay(NULL)))
	{
		fprintf(stderr, "dwmstatus: cannot open display.\n");
		return 1;
	}

	for (;;sleep(1))
	{
		tm   = get_time(tz);
		bat  = get_bat();
		vol  = get_vol();
		wifi = get_wifi();
		mpd  = get_mpd();

		stat = smprintf("%s%s%s%s%s",
						mpd, wifi, vol, bat, tm);

		XStoreName(dpy, DefaultRootWindow(dpy), stat);
		XSync(dpy, False);

		free(mpd);
		free(vol);
		free(bat);
		free(tm);
		free(wifi);
		free(stat);
	}

	XCloseDisplay(dpy);

	return 0;
}

