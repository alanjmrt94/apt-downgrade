#ifndef APT_DOWNGRADE_I18N_H
#define APT_DOWNGRADE_I18N_H

typedef enum {
    LANG_EN = 0,
    LANG_ES,
    LANG_PT,
    LANG_PT_BR,
    LANG_FR,
    LANG_DE,
    LANG_IT,
    LANG_RU,
    LANG_ZH_CN,
    LANG_ZH_TW,
    LANG_JA,
    LANG_KO,
    LANG_AR,
    LANG_HI,
    LANG_NL,
    LANG_PL,
    LANG_TR,
    LANG_GL,
    LANG_CA,
    LANG_EU,
    LANG_COUNT
} Lang;

typedef struct {
    const char *blurb;
    const char *usage;
    const char *options_header;
    const char *opt_current;
    const char *opt_downgrade;
    const char *opt_dry_run;
    const char *opt_yes;
    const char *opt_help;
    const char *examples_header;
    const char *notes_header;
    const char *note_list;
    const char *note_root;
    const char *more_info;
    const char *err_oom;
    const char *err_list_open;
    const char *err_list_close;
    const char *err_read_answer;
    const char *err_root;
    const char *err_exec;
    const char *err_status;
    const char *no_packages;
    const char *packages_header;
    const char *target_version;
    const char *command_header;
    const char *dry_run_done;
    const char *confirm_prompt;
    const char *cancelled;
    const char *success;
} Messages;

/* Detecta idioma: APT_DOWNGRADE_LANG → LANGUAGE → LC_* → LANG → en. */
Lang i18n_detect_lang(void);

const Messages *i18n_messages(Lang lang);

/* true si el carácter confirma (y/s/o/j y letras típicas del idioma). */
int i18n_is_affirmative(Lang lang, int c);

#endif
