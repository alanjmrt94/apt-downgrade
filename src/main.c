#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
    char name[256];
    char distribution[256];
} PackageInfo;

static void print_usage(FILE *out, const char *program) {
    fprintf(out,
            "Uso: %s --current <version_actual> --downgrade <version_objetivo> [opciones]\n"
            "\n"
            "Opciones:\n"
            "  -h, --help       Mostrar esta ayuda\n"
            "  --dry-run        Listar y mostrar el comando sin instalar\n"
            "  -y, --yes        Confirmar automáticamente la instalación\n"
            "\n"
            "Notas:\n"
            "  Listar y --dry-run no requieren sudo.\n"
            "  La instalación sí requiere privilegios de root (sudo).\n",
            program);
}

static void add_package(PackageInfo **packages, size_t *count, size_t *capacity,
                        const char *name, const char *distribution) {
    if (*capacity == 0) {
        *capacity = 8;
        *packages = malloc(*capacity * sizeof(PackageInfo));
        if (!*packages) {
            fprintf(stderr, "Error: memoria insuficiente.\n");
            exit(EXIT_FAILURE);
        }
    } else if (*count == *capacity) {
        *capacity *= 2;
        PackageInfo *tmp = realloc(*packages, *capacity * sizeof(PackageInfo));
        if (!tmp) {
            free(*packages);
            fprintf(stderr, "Error: memoria insuficiente.\n");
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

/* Parsea una línea de `apt list --installed`.
 * Formatos soportados:
 *   pkg/suite,now VERSION arch [flags]
 *   pkg/suite1,suite2,now VERSION arch [flags]
 */
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

    const char *version_start = now_marker + 5; /* después de ",now " */
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
        fprintf(stderr, "Error: memoria insuficiente.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(command, "apt install");
    size_t current_len = strlen(command);

    for (size_t i = 0; i < count; ++i) {
        size_t addition =
            strlen(packages[i].name) + strlen(downgrade_version) + 4; /* " pkg=ver" */
        if (current_len + addition + 1 >= buffer_size) {
            buffer_size = (current_len + addition + 1) * 2;
            char *tmp = realloc(command, buffer_size);
            if (!tmp) {
                free(command);
                fprintf(stderr, "Error: memoria insuficiente.\n");
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

    printf("\n¿Desea proceder? (s/N): ");
    fflush(stdout);

    char response[8];
    if (!fgets(response, sizeof(response), stdin)) {
        fprintf(stderr, "Error al leer la respuesta.\n");
        return -1;
    }

    return tolower((unsigned char)response[0]) == 's' ? 1 : 0;
}

int main(int argc, char *argv[]) {
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

    /* Permite inyectar un comando de listado en tests (p. ej. cat fixture). */
    const char *list_cmd = getenv("APT_DOWNGRADE_LIST_CMD");
    if (!list_cmd || list_cmd[0] == '\0') {
        list_cmd = "apt list --installed 2>/dev/null";
    }

    FILE *fp = popen(list_cmd, "r");
    if (!fp) {
        fprintf(stderr, "Error: no se pudo obtener la lista de paquetes instalados.\n");
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
        /* Quitar salto de línea final para parsing estable */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        parse_line(line, &packages, &count, &capacity, current_version);
    }

    int list_status = pclose(fp);
    if (list_status == -1) {
        fprintf(stderr, "Error: falló al cerrar el listado de paquetes.\n");
        free(packages);
        return EXIT_FAILURE;
    }

    if (count == 0) {
        printf("No se encontraron paquetes con la versión %s.\n", current_version);
        free(packages);
        return EXIT_SUCCESS;
    }

    printf("Paquetes que coinciden con la versión actual %s (%zu):\n", current_version,
           count);
    for (size_t i = 0; i < count; ++i) {
        printf("  - %s (%s)\n", packages[i].name, packages[i].distribution);
    }
    printf("\nVersión objetivo para downgrade: %s\n", downgrade_version);

    char *command = build_command(packages, count, downgrade_version);
    printf("\nComando a ejecutar:\n  sudo %s\n", command);

    if (dry_run) {
        printf("\nModo dry-run: no se realizaron cambios.\n");
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
        printf("Operación cancelada por el usuario.\n");
        free(command);
        free(packages);
        return EXIT_SUCCESS;
    }

    if (geteuid() != 0) {
        fprintf(stderr,
                "Error: la instalación requiere privilegios de root. "
                "Ejecutá con sudo o usá --dry-run.\n");
        free(command);
        free(packages);
        return EXIT_FAILURE;
    }

    int status = system(command);
    if (status == -1) {
        fprintf(stderr, "Error al ejecutar el comando.\n");
        free(command);
        free(packages);
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        printf("Comando ejecutado correctamente.\n");
        free(command);
        free(packages);
        return EXIT_SUCCESS;
    }

    fprintf(stderr, "El comando terminó con estado %d.\n", WEXITSTATUS(status));
    free(command);
    free(packages);
    return EXIT_FAILURE;
}
