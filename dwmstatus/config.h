/*
 * Configuration file for dwmstatus
 */

#include <iwlib.h>
#include <mpd/client.h>

/* Battery level locations */
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

