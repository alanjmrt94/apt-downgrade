#include "i18n.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#ifndef APT_DOWNGRADE_VERSION
#define APT_DOWNGRADE_VERSION "1.1.0"
#endif

typedef struct {
    char name[256];
    char distribution[256];
} PackageInfo;

static Lang g_lang = LANG_EN;
static const Messages *g_msg = NULL;

static void init_lang(void) {
    g_lang = i18n_detect_lang();
    g_msg = i18n_messages(g_lang);
}

static void print_usage(FILE *out, const char *program) {
    fprintf(out, "apt-downgrade %s\n", APT_DOWNGRADE_VERSION);
    fprintf(out, "%s\n\n", g_msg->blurb);
    fprintf(out, g_msg->usage, program);
    fprintf(out, "\n%s\n", g_msg->options_header);
    fprintf(out, "%s\n", g_msg->opt_current);
    fprintf(out, "%s\n", g_msg->opt_downgrade);
    fprintf(out, "%s\n", g_msg->opt_dry_run);
    fprintf(out, "%s\n", g_msg->opt_yes);
    fprintf(out, "%s\n", g_msg->opt_help);
    fprintf(out, "\n%s\n", g_msg->examples_header);
    fprintf(out, "  %s --current 1.2.3-1 --downgrade 1.0.0-1 --dry-run\n", program);
    fprintf(out, "  sudo %s --current 1.2.3-1 --downgrade 1.0.0-1\n", program);
    fprintf(out, "\n%s\n", g_msg->notes_header);
    fprintf(out, "%s\n", g_msg->note_list);
    fprintf(out, "%s\n", g_msg->note_root);
    fprintf(out, "\n%s\n", g_msg->more_info);
}

static void add_package(PackageInfo **packages, size_t *count, size_t *capacity,
                        const char *name, const char *distribution) {
    if (*capacity == 0) {
        *capacity = 8;
        *packages = malloc(*capacity * sizeof(PackageInfo));
        if (!*packages) {
            fprintf(stderr, "%s", g_msg->err_oom);
            exit(EXIT_FAILURE);
        }
    } else if (*count == *capacity) {
        *capacity *= 2;
        PackageInfo *tmp = realloc(*packages, *capacity * sizeof(PackageInfo));
        if (!tmp) {
            free(*packages);
            fprintf(stderr, "%s", g_msg->err_oom);
            exit(EXIT_FAILURE);
        }
        *packages = tmp;
    }

    strncpy((*packages)[*count].name, name, sizeof((*packages)[*count].name) - 1);
    (*packages)[*count].name[sizeof((*packages)[*count].name) - 1] = '\0';
    strncpy((*packages)[*count].distribution, distribution,
            sizeof((*packages)[*count].distribution) - 1);
    (*packages)[*count].distribution[sizeof((*packages)[*count].distribution) - 1] =
        '\0';
    (*count)++;
}

static int parse_line(const char *line, PackageInfo **packages, size_t *count,
                      size_t *capacity, const char *target_version) {
    const char *slash = strchr(line, '/');
    if (!slash) {
        return 0;
    }

    const char *now_marker = strstr(slash, ",now ");
    if (!now_marker) {
        return 0;
    }

    const char *version_start = now_marker + 5;
    const char *space_after_version = strchr(version_start, ' ');
    if (!space_after_version) {
        return 0;
    }

    size_t version_len = (size_t)(space_after_version - version_start);
    char version[256];
    if (version_len == 0 || version_len >= sizeof(version)) {
        return 0;
    }
    memcpy(version, version_start, version_len);
    version[version_len] = '\0';

    if (strcmp(version, target_version) != 0) {
        return 0;
    }

    size_t name_len = (size_t)(slash - line);
    size_t distro_len = (size_t)(now_marker - (slash + 1));
    if (name_len == 0 || distro_len == 0) {
        return 0;
    }

    char name[256];
    char distribution[256];
    if (name_len >= sizeof(name) || distro_len >= sizeof(distribution)) {
        return 0;
    }

    memcpy(name, line, name_len);
    name[name_len] = '\0';
    memcpy(distribution, slash + 1, distro_len);
    distribution[distro_len] = '\0';

    add_package(packages, count, capacity, name, distribution);
    return 1;
}

static char *build_command(const PackageInfo *packages, size_t count,
                           const char *downgrade_version) {
    size_t buffer_size = 64;
    char *command = malloc(buffer_size);
    if (!command) {
        fprintf(stderr, "%s", g_msg->err_oom);
        exit(EXIT_FAILURE);
    }
    strcpy(command, "apt install");
    size_t current_len = strlen(command);

    for (size_t i = 0; i < count; ++i) {
        size_t addition =
            strlen(packages[i].name) + strlen(downgrade_version) + 4;
        if (current_len + addition + 1 >= buffer_size) {
            buffer_size = (current_len + addition + 1) * 2;
            char *tmp = realloc(command, buffer_size);
            if (!tmp) {
                free(command);
                fprintf(stderr, "%s", g_msg->err_oom);
                exit(EXIT_FAILURE);
            }
            command = tmp;
        }
        current_len += snprintf(command + current_len, buffer_size - current_len,
                                " %s=%s", packages[i].name, downgrade_version);
    }

    return command;
}

static int confirm_install(int assume_yes) {
    if (assume_yes) {
        return 1;
    }

    printf("%s", g_msg->confirm_prompt);
    fflush(stdout);

    char response[8];
    if (!fgets(response, sizeof(response), stdin)) {
        fprintf(stderr, "%s", g_msg->err_read_answer);
        return -1;
    }

    return i18n_is_affirmative(g_lang, (unsigned char)response[0]) ? 1 : 0;
}

int main(int argc, char *argv[]) {
    init_lang();

    const char *current_version = NULL;
    const char *downgrade_version = NULL;
    int dry_run = 0;
    int assume_yes = 0;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(stdout, argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "--dry-run") == 0) {
            dry_run = 1;
        } else if (strcmp(argv[i], "--yes") == 0 || strcmp(argv[i], "-y") == 0) {
            assume_yes = 1;
        } else if (strcmp(argv[i], "--current") == 0 && i + 1 < argc) {
            current_version = argv[++i];
        } else if (strcmp(argv[i], "--downgrade") == 0 && i + 1 < argc) {
            downgrade_version = argv[++i];
        } else {
            print_usage(stderr, argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (!current_version || !downgrade_version) {
        print_usage(stderr, argv[0]);
        return EXIT_FAILURE;
    }

    const char *list_cmd = getenv("APT_DOWNGRADE_LIST_CMD");
    if (!list_cmd || list_cmd[0] == '\0') {
        list_cmd = "apt list --installed 2>/dev/null";
    }

    FILE *fp = popen(list_cmd, "r");
    if (!fp) {
        fprintf(stderr, "%s", g_msg->err_list_open);
        return EXIT_FAILURE;
    }

    PackageInfo *packages = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char line[2048];

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Listing", 7) == 0 || line[0] == '\n' || line[0] == '\0') {
            continue;
        }
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        parse_line(line, &packages, &count, &capacity, current_version);
    }

    int list_status = pclose(fp);
    if (list_status == -1) {
        fprintf(stderr, "%s", g_msg->err_list_close);
        free(packages);
        return EXIT_FAILURE;
    }

    if (count == 0) {
        printf(g_msg->no_packages, current_version);
        free(packages);
        return EXIT_SUCCESS;
    }

    printf(g_msg->packages_header, current_version, count);
    for (size_t i = 0; i < count; ++i) {
        printf("  - %s (%s)\n", packages[i].name, packages[i].distribution);
    }
    printf(g_msg->target_version, downgrade_version);

    char *command = build_command(packages, count, downgrade_version);
    printf(g_msg->command_header, command);

    if (dry_run) {
        printf("%s", g_msg->dry_run_done);
        free(command);
        free(packages);
        return EXIT_SUCCESS;
    }

    int confirmed = confirm_install(assume_yes);
    if (confirmed < 0) {
        free(command);
        free(packages);
        return EXIT_FAILURE;
    }
    if (confirmed == 0) {
        printf("%s", g_msg->cancelled);
        free(command);
        free(packages);
        return EXIT_SUCCESS;
    }

    /* Tests en CI suelen correr como root; APT_DOWNGRADE_FORCE_NONROOT simula no-root. */
    {
        const char *force_nonroot = getenv("APT_DOWNGRADE_FORCE_NONROOT");
        int is_root = (geteuid() == 0);
        if (force_nonroot != NULL && force_nonroot[0] != '\0' &&
            strcmp(force_nonroot, "0") != 0) {
            is_root = 0;
        }
        if (!is_root) {
            fprintf(stderr, "%s", g_msg->err_root);
            free(command);
            free(packages);
            return EXIT_FAILURE;
        }
    }

    int status = system(command);
    if (status == -1) {
        fprintf(stderr, "%s", g_msg->err_exec);
        free(command);
        free(packages);
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("%s", g_msg->success);
        free(command);
        free(packages);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, g_msg->err_status, WEXITSTATUS(status));
    free(command);
    free(packages);
    return EXIT_FAILURE;
}
