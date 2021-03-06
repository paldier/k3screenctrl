#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "handlers.h"
#include "mcu_proto.h"
#include "pages.h"
#include "requests.h"
#include "signals.h"
#include <shared.h>

static MCU_VERSION g_mcu_version;
void handle_mcu_version(const unsigned char *payload, int len) {
	char mcu_version[10]={0};
    if (len < 4) {
        syslog(LOG_WARNING,
               "Got malformed MCU version response. Length is %d\n", len);
        return;
    }

    g_mcu_version.patch_ver =
        payload[0] |
        payload[1] << 8; /* Do we need this endian compatabitity? */
    g_mcu_version.minor_ver = payload[2];
    g_mcu_version.major_ver = payload[3];

#if defined(BCMARM)
	snprintf(mcu_version, sizeof(mcu_version), "%hhd.%hhd.%hhd",payload[3], payload[2], payload[0]);
	nvram_set("mcu_version",mcu_version);
    syslog(LOG_WARNING, "MCU reported version as %hhd.%hhd.%hd\n",
#else
    syslog(LOG_INFO, "MCU reported version as %hhd.%hhd.%hd\n",
#endif
           g_mcu_version.major_ver, g_mcu_version.minor_ver,
           g_mcu_version.patch_ver);
}

int g_is_screen_on = 1;
void handle_key_press(const unsigned char *payload, int len) {
    if (len < 1) {
        syslog(LOG_WARNING, "Got malformed key press response. Length is %d\n",
               len);
        return;
    }
    refresh_screen_timeout();
    if (!g_is_screen_on) {
        /* Do not process key messages when waking up */
        request_notify_event(EVENT_WAKEUP);
        g_is_screen_on = 1;
        return;
    }
    switch (payload[0]) {
    case KEY_LEFT_SHORT:
        page_switch_prev();
        printf("KEY_LEFT_SHORT\n");
        break;
    case KEY_RIGHT_SHORT:
        page_switch_next();
        printf("KEY_RIGHT_SHORT\n");
        break;
    case KEY_MIDDLE_SHORT:
        page_switch_to(PAGE_WAN);
        printf("KEY_MIDDLE_SHORT\n");
        break;
    case KEY_MIDDLE_LONG:
        request_notify_event(EVENT_SLEEP);
        g_is_screen_on = 0;
		printf("KEY_MIDDLE_LONG\n");
        return;
    case KEY_LEFT_LONG:
		page_switch_to(PAGE_WIFI);
        printf("KEY_LEFT_LONG\n");
    case KEY_RIGHT_LONG:
		page_switch_to(PAGE_HOSTS);
        printf("KEY_RIGHT_LONG\n");
        return;
    default:
        syslog(LOG_WARNING, "unknown key code: %hhx\n", payload[0]);
        return;
    }
}

RESPONSE_HANDLER g_response_handlers[] = {
    {RESPONSE_MCU_VERSION, handle_mcu_version},
    {RESPONSE_KEY_PRESS, handle_key_press},
};
