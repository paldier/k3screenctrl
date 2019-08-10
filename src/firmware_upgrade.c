#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include "common.h"
#include "frame.h"
#include "mcu_proto.h"
#include "config.h"
#include "hwdef.h"
#include "file_util.h"
#include "gpio.h"
#include "requests.h"
#if defined(BCMARM)
#include <shared.h>
#endif

#define UPGRADE_STAGE_IDLE 0
#define UPGRADE_STAGE_VERSION_CHECK 1
#define UPGRADE_STAGE_ERASE 2
#define UPGRADE_STAGE_FLASH 3
#define UPGRADE_STAGE_RESET 4
#define UPGRADE_STAGE_CONFIRM 5

/* usleep() this time before sending command */
#define UPGRADE_STAGE_VERSION_CHECK_USLEEP 500000
#define UPGRADE_STAGE_ERASE_USLEEP 100000
#define UPGRADE_STAGE_FLASH_USLEEP 10000 /* Sleep before every single frame */
#define UPGRADE_STAGE_RESET_USLEEP 1000000
#define UPGRADE_STAGE_CONFIRM_USLEEP UPGRADE_STAGE_VERSION_CHECK_USLEEP

/* Time out after this time after sending command */
#define UPGRADE_STAGE_VERSION_CHECK_TIMEOUT 3
#define UPGRADE_STAGE_ERASE_TIMEOUT 10
#define UPGRADE_STAGE_FLASH_TIMEOUT 3 /* Timeout for every single frame */
#define UPGRADE_STAGE_RESET_TIMEOUT 1
#define UPGRADE_STAGE_CONFIRM_TIMEOUT UPGRADE_STAGE_VERSION_CHECK_TIMEOUT

#define FLASH_IHEX_BATCH_SIZE 10 /* Send 10 lines of iHex file in one frame */

static int g_upgrade_stage = UPGRADE_STAGE_IDLE;
static int g_flash_finished = 0;

static void fwupgrade_step_success();

static unsigned char char2hex(const char* str) {
#define LOWER2HEX(digit) (((digit) >= 'a') ? (10 + ((digit) - 'a')) : ((digit) - '0'))
    const char digit0 = tolower(str[0]);
    const char digit1 = tolower(str[1]);

    if (digit0 == '\0') {
        return 0;
    }

    if (digit1 == '\0') {
        return LOWER2HEX(digit0);
    }

    return 16 * LOWER2HEX(digit0) + LOWER2HEX(digit1);
}

static int bl_request_mcu_version() {
    printf("INFO: Checking MCU version...\n");
    return frame_send(FRAME_BL_MCU_VERSION_REQ, NULL, 0);
}

static int bl_request_erase() {
    printf("INFO: Erasing flash...\n");
    return frame_send(FRAME_BL_ERASE_REQ, NULL, 0);
}

static int bl_request_flash(const char *fwpath) {
    static FILE *fp = NULL;
    static int offset = 0;

    int lines_read = 0;
    int bin_bytes_written = 0;
    char line_buf[512];
    char bin_buf[512];

    if (fp == NULL) {
        fp = fopen(CFG->firmware_path, "r");
        if (fp == NULL) {
            perror("Could not open %s but the flash has been erased - "
            "you may want to check the file and restart upgrade process with --skip-reset.\n");
            exit(EXIT_FAILURE);
        }
    }

    while (lines_read++ < FLASH_IHEX_BATCH_SIZE) {
        char *read_result = fgets(line_buf, sizeof(line_buf), fp);
        if (read_result != NULL) {
            if (line_buf[0] == ':') {
                /* Assume even length after ":" */
                int curr_pos = 1, line_len = strlen(read_result);

                if (line_buf[line_len - 1] == '\n') {
                    line_buf[line_len - 1] = '\0';
                    line_len--;
                }

                if (line_buf[line_len - 1] == '\r') {
                    line_buf[line_len - 1] = '\0';
                    line_len--;
                }

                while (curr_pos < line_len) {
                    bin_buf[bin_bytes_written++] = char2hex(&line_buf[curr_pos]);
                    curr_pos += 2;
                }
            }
        } else {
            if (feof(fp)) {
                // Finished sending
                g_flash_finished = 1;
            } else {
                // ??? Read error
                fprintf(stderr, "ERROR: could not read from firmware file: %d. "
                "You may want to check the file and restart upgrade process with --skip-reset.\n", ferror(fp));
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("INFO: Sending %d-byte chunk from offset %d\n", bin_bytes_written, offset);
    offset += bin_bytes_written;
    frame_send(FRAME_BL_FLASH_REQ, bin_buf, bin_bytes_written);
    return 1; // Suggest success
}

static int fwupgrade_reset_normal() {
    printf("INFO: Resetting MCU to normal mode...\n");
#if defined(BCMARM)
    if (set_gpio(SCREEN_BOOT_MODE_GPIO, BOOT_MODE_APP) == FAILURE ||
        set_gpio(SCREEN_RESET_GPIO, 0) == FAILURE ||
        set_gpio(SCREEN_RESET_GPIO, 1) == FAILURE) {
        fprintf(stderr, "Could not reset screen to normal mode\n");
        return FAILURE;
    }
#else
        if (gpio_set_value(SCREEN_BOOT_MODE_GPIO, boot_gpio) == FAILURE ||
            gpio_set_value(SCREEN_RESET_GPIO, 0) == FAILURE ||
            gpio_set_value(SCREEN_RESET_GPIO, 1) == FAILURE) {
            syslog(LOG_ERR, "Could not reset screen\n");
            return FAILURE;
        }
#endif
}

static int fwupgrade_set_stage(int stage, int timeout, int delay) {
    g_upgrade_stage = stage;
    usleep(delay);
    return alarm(timeout);
}

// Proceed to next step
static void fwupgrade_step_success() {
#define SET_STAGE(x) fwupgrade_set_stage(x, x ## _TIMEOUT, x ## _USLEEP)

    switch (g_upgrade_stage) {
        case UPGRADE_STAGE_IDLE:
            SET_STAGE(UPGRADE_STAGE_VERSION_CHECK);
            bl_request_mcu_version();
            break;
        case UPGRADE_STAGE_VERSION_CHECK:
            SET_STAGE(UPGRADE_STAGE_ERASE);
            bl_request_erase();
            break;
        case UPGRADE_STAGE_ERASE:
            SET_STAGE(UPGRADE_STAGE_FLASH);
            bl_request_flash(CFG->firmware_path);
            break;
        case UPGRADE_STAGE_FLASH:
            if (g_flash_finished != 1) {
                SET_STAGE(UPGRADE_STAGE_FLASH);
                bl_request_flash(CFG->firmware_path);
            } else {
                SET_STAGE(UPGRADE_STAGE_RESET);
                fwupgrade_reset_normal();
            }
            break;
        case UPGRADE_STAGE_RESET:
            /* We are in normal mode, request with normal app commands */
            SET_STAGE(UPGRADE_STAGE_CONFIRM);
            request_mcu_version();
            break;
        case UPGRADE_STAGE_CONFIRM:
            printf("MCU firmware has been successfully upgraded. You can restart k3screenctrl now.\n");
            exit(EXIT_SUCCESS);
            break;
    }
}

static void fwupgrade_handle_timeout() {
    switch (g_upgrade_stage) {
        case UPGRADE_STAGE_VERSION_CHECK:
            fprintf(stderr, "WARNING: MCU did not respond to version request. Wait until serial data in...\n");
            break;
        case UPGRADE_STAGE_ERASE:
            fprintf(stderr, "WARNING: flash erasing timed out. Wait until serial data in...\n");
            break;
        case UPGRADE_STAGE_FLASH:
            fprintf(stderr, "WARNING: flash writing timed out. Wait until serial data in...\n");
            break;
        case UPGRADE_STAGE_RESET:
            /* By design: wait for _TIMEOUT seconds and jump to next stage. */
            fwupgrade_step_success();
            break;
        case UPGRADE_STAGE_CONFIRM:
            fprintf(stderr, "WARNING: new firmware still not up. Retrying...\n");
            request_mcu_version();
            break;
    }
}

static void fwupgrade_frame_handler(const unsigned char *frame, int len) {
    unsigned char ver_major = 0, ver_minor = 0;

    switch (frame[0]) {
    case FRAME_BL_MCU_VERSION_REQ:
        ver_minor = frame[1];
        ver_major = frame[2];

        if (g_upgrade_stage != UPGRADE_STAGE_VERSION_CHECK &&
            g_upgrade_stage != UPGRADE_STAGE_CONFIRM) {
            fprintf(stderr, "MCU reported version but we did not request it?\n");
            return;
        }

        printf("INFO: MCU reported version: %d.%d\n", ver_major, ver_minor);
        if (ver_major != 1) {
            fprintf(stderr, "ERROR: Only MCU with major version = 1 is supported in firmware upgrade\n");
            exit(EXIT_FAILURE);
        }
        fwupgrade_step_success();
        break;

    case FRAME_BL_ERASE_REQ:
        if (g_upgrade_stage != UPGRADE_STAGE_ERASE) {
            fprintf(stderr, "WARNING: MCU reported erase successful but we did not request it?\n");
            return;
        }

        fwupgrade_step_success();
        break;

    case FRAME_BL_FLASH_REQ:
        if (g_upgrade_stage != UPGRADE_STAGE_FLASH) {
            fprintf(stderr, "WARNING: MCU reported flash successful but we did not request it?\n");
            return;
        }

        fwupgrade_step_success();
        break;
    case FRAME_APP:
        /* FRAME_APP can arrive anytime after resetting, this can be treated as success */
        if (g_upgrade_stage == UPGRADE_STAGE_CONFIRM || g_upgrade_stage == UPGRADE_STAGE_RESET) {
            g_upgrade_stage = UPGRADE_STAGE_CONFIRM; /* Skip to CONFIRM so we can exit */
            fwupgrade_step_success();
            return;
        }
        // fall thru
    default:
        fprintf(stderr, "WANRING: MCU reported %d in stage %d but we do not support that\n", frame[0], g_upgrade_stage);
        break;
    }
}


void fwupgrade_notify_signal(int sig) {
    switch (sig) {
    case SIGALRM:
        fwupgrade_handle_timeout();
        break;
    case SIGTERM:
    case SIGINT:
        if (g_upgrade_stage == UPGRADE_STAGE_ERASE ||
            g_upgrade_stage == UPGRADE_STAGE_FLASH) {
            fprintf(stderr, "WARNING: Currently erasing or flashing. SIGTERM/INT rejected.\n");
            return;
        }
        exit(EXIT_FAILURE);
        break;
    default:
        printf("INFO: signal %d received and ignored\n", sig);
        break;
    }
}

void fwupgrade_start() {
    FILE *tmp_fp = NULL;
    if ((tmp_fp = fopen(CFG->firmware_path, "r")) == NULL) {
        fprintf(stderr, "ERROR: file %s could not be read (%d).\n", CFG->firmware_path, ferror(tmp_fp));
        exit(EXIT_FAILURE);
    }
    fclose(tmp_fp);

    frame_set_received_callback(fwupgrade_frame_handler);
    fwupgrade_step_success(); // Begin from IDLE
}
