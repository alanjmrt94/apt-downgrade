#include "i18n.h"

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

static const Messages MSGS[LANG_COUNT] = {
    [LANG_EN] =
        {
            .blurb = "Tool to downgrade installed packages to an older version via apt.",
            .usage = "Usage: %s --current <current_version> --downgrade <target_version> "
                     "[options]\n",
            .options_header = "Most used options:",
            .opt_current = "  --current <ver>     Installed version to match (exact match)",
            .opt_downgrade = "  --downgrade <ver>   Target version for the downgrade",
            .opt_dry_run = "  --dry-run           Show the plan without installing",
            .opt_json = "  --json              Emit plan as JSON (implies dry-run)",
            .opt_yes = "  -y, --yes           Confirm without prompting",
            .opt_help = "  -h, --help          Show this help",
            .examples_header = "Examples:",
            .notes_header = "Notes:",
            .note_list = "  Listing and --dry-run do not require sudo.",
            .note_root = "  Installing requires root privileges (sudo).",
            .more_info = "See also: man apt-downgrade",
            .err_oom = "Error: out of memory.\n",
            .err_list_open = "Error: could not get the list of installed packages.\n",
            .err_list_close = "Error: failed while closing the package listing.\n",
            .err_read_answer = "Error: could not read the answer.\n",
            .err_root = "Error: installing requires root privileges. "
                        "Run with sudo or use --dry-run.\n",
            .err_exec = "Error: failed to run the command.\n",
            .err_status = "The command exited with status %d.\n",
            .no_packages = "No packages found with version %s.\n",
            .packages_header = "Packages matching current version %s (%zu):\n",
            .target_version = "\nTarget downgrade version: %s\n",
            .command_header = "\nCommand to run:\n  sudo %s\n",
            .dry_run_done = "\nDry-run mode: no changes were made.\n",
            .confirm_prompt = "\nProceed? (y/N): ",
            .cancelled = "Operation cancelled by the user.\n",
            .success = "Command completed successfully.\n",
        },
    [LANG_ES] =
        {
            .blurb = "Herramienta para bajar paquetes instalados a una versión anterior "
                     "mediante apt.",
            .usage = "Uso: %s --current <version_actual> --downgrade <version_objetivo> "
                     "[opciones]\n",
            .options_header = "Opciones más usadas:",
            .opt_current = "  --current <ver>     Versión instalada a buscar (coincidencia "
                           "exacta)",
            .opt_downgrade = "  --downgrade <ver>   Versión objetivo del downgrade",
            .opt_dry_run = "  --dry-run           Mostrar el plan sin instalar",
            .opt_json = "  --json              Emitir el plan en JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Confirmar sin preguntar",
            .opt_help = "  -h, --help          Mostrar esta ayuda",
            .examples_header = "Ejemplos:",
            .notes_header = "Notas:",
            .note_list = "  Listar y --dry-run no requieren sudo.",
            .note_root = "  La instalación sí requiere privilegios de root (sudo).",
            .more_info = "Más información: man apt-downgrade",
            .err_oom = "Error: memoria insuficiente.\n",
            .err_list_open =
                "Error: no se pudo obtener la lista de paquetes instalados.\n",
            .err_list_close = "Error: falló al cerrar el listado de paquetes.\n",
            .err_read_answer = "Error al leer la respuesta.\n",
            .err_root = "Error: la instalación requiere privilegios de root. "
                        "Ejecutá con sudo o usá --dry-run.\n",
            .err_exec = "Error al ejecutar el comando.\n",
            .err_status = "El comando terminó con estado %d.\n",
            .no_packages = "No se encontraron paquetes con la versión %s.\n",
            .packages_header =
                "Paquetes que coinciden con la versión actual %s (%zu):\n",
            .target_version = "\nVersión objetivo para downgrade: %s\n",
            .command_header = "\nComando a ejecutar:\n  sudo %s\n",
            .dry_run_done = "\nModo dry-run: no se realizaron cambios.\n",
            .confirm_prompt = "\n¿Desea proceder? (s/N): ",
            .cancelled = "Operación cancelada por el usuario.\n",
            .success = "Comando ejecutado correctamente.\n",
        },
    [LANG_PT] =
        {
            .blurb = "Ferramenta para fazer downgrade de pacotes instalados para uma "
                     "versão anterior via apt.",
            .usage = "Utilização: %s --current <versão_atual> --downgrade "
                     "<versão_alvo> [opções]\n",
            .options_header = "Opções mais usadas:",
            .opt_current =
                "  --current <ver>     Versão instalada a corresponder (exata)",
            .opt_downgrade = "  --downgrade <ver>   Versão alvo do downgrade",
            .opt_dry_run = "  --dry-run           Mostrar o plano sem instalar",
            .opt_json = "  --json              Emitir o plano em JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Confirmar sem perguntar",
            .opt_help = "  -h, --help          Mostrar esta ajuda",
            .examples_header = "Exemplos:",
            .notes_header = "Notas:",
            .note_list = "  Listar e --dry-run não requerem sudo.",
            .note_root = "  A instalação requer privilégios de root (sudo).",
            .more_info = "Mais informação: man apt-downgrade",
            .err_oom = "Erro: memória insuficiente.\n",
            .err_list_open =
                "Erro: não foi possível obter a lista de pacotes instalados.\n",
            .err_list_close = "Erro: falha ao fechar a listagem de pacotes.\n",
            .err_read_answer = "Erro ao ler a resposta.\n",
            .err_root = "Erro: a instalação requer privilégios de root. "
                        "Execute com sudo ou use --dry-run.\n",
            .err_exec = "Erro ao executar o comando.\n",
            .err_status = "O comando terminou com o estado %d.\n",
            .no_packages = "Nenhum pacote encontrado com a versão %s.\n",
            .packages_header = "Pacotes que correspondem à versão atual %s (%zu):\n",
            .target_version = "\nVersão alvo para downgrade: %s\n",
            .command_header = "\nComando a executar:\n  sudo %s\n",
            .dry_run_done = "\nModo dry-run: não foram feitas alterações.\n",
            .confirm_prompt = "\nDeseja continuar? (s/N): ",
            .cancelled = "Operação cancelada pelo utilizador.\n",
            .success = "Comando executado com sucesso.\n",
        },
    [LANG_PT_BR] =
        {
            .blurb = "Ferramenta para fazer downgrade de pacotes instalados para uma "
                     "versão anterior via apt.",
            .usage = "Uso: %s --current <versão_atual> --downgrade <versão_alvo> "
                     "[opções]\n",
            .options_header = "Opções mais usadas:",
            .opt_current =
                "  --current <ver>     Versão instalada a corresponder (exata)",
            .opt_downgrade = "  --downgrade <ver>   Versão alvo do downgrade",
            .opt_dry_run = "  --dry-run           Mostrar o plano sem instalar",
            .opt_json = "  --json              Emitir o plano em JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Confirmar sem perguntar",
            .opt_help = "  -h, --help          Mostrar esta ajuda",
            .examples_header = "Exemplos:",
            .notes_header = "Notas:",
            .note_list = "  Listar e --dry-run não exigem sudo.",
            .note_root = "  A instalação exige privilégios de root (sudo).",
            .more_info = "Mais informações: man apt-downgrade",
            .err_oom = "Erro: memória insuficiente.\n",
            .err_list_open =
                "Erro: não foi possível obter a lista de pacotes instalados.\n",
            .err_list_close = "Erro: falha ao fechar a listagem de pacotes.\n",
            .err_read_answer = "Erro ao ler a resposta.\n",
            .err_root = "Erro: a instalação exige privilégios de root. "
                        "Execute com sudo ou use --dry-run.\n",
            .err_exec = "Erro ao executar o comando.\n",
            .err_status = "O comando terminou com status %d.\n",
            .no_packages = "Nenhum pacote encontrado com a versão %s.\n",
            .packages_header = "Pacotes que correspondem à versão atual %s (%zu):\n",
            .target_version = "\nVersão alvo para downgrade: %s\n",
            .command_header = "\nComando a executar:\n  sudo %s\n",
            .dry_run_done = "\nModo dry-run: nenhuma alteração foi feita.\n",
            .confirm_prompt = "\nDeseja continuar? (s/N): ",
            .cancelled = "Operação cancelada pelo usuário.\n",
            .success = "Comando executado com sucesso.\n",
        },
    [LANG_FR] =
        {
            .blurb = "Outil pour rétrograder des paquets installés vers une version "
                     "antérieure via apt.",
            .usage = "Utilisation : %s --current <version_actuelle> --downgrade "
                     "<version_cible> [options]\n",
            .options_header = "Options les plus utilisées :",
            .opt_current = "  --current <ver>     Version installée à faire "
                           "correspondre (exacte)",
            .opt_downgrade = "  --downgrade <ver>   Version cible du downgrade",
            .opt_dry_run = "  --dry-run           Afficher le plan sans installer",
            .opt_json = "  --json              Émettre le plan en JSON (implique dry-run)",
            .opt_yes = "  -y, --yes           Confirmer sans demander",
            .opt_help = "  -h, --help          Afficher cette aide",
            .examples_header = "Exemples :",
            .notes_header = "Notes :",
            .note_list = "  Lister et --dry-run ne nécessitent pas sudo.",
            .note_root = "  L'installation nécessite les privilèges root (sudo).",
            .more_info = "Plus d'informations : man apt-downgrade",
            .err_oom = "Erreur : mémoire insuffisante.\n",
            .err_list_open =
                "Erreur : impossible d'obtenir la liste des paquets installés.\n",
            .err_list_close = "Erreur : échec lors de la fermeture de la liste.\n",
            .err_read_answer = "Erreur lors de la lecture de la réponse.\n",
            .err_root = "Erreur : l'installation nécessite les privilèges root. "
                        "Exécutez avec sudo ou utilisez --dry-run.\n",
            .err_exec = "Erreur lors de l'exécution de la commande.\n",
            .err_status = "La commande s'est terminée avec le statut %d.\n",
            .no_packages = "Aucun paquet trouvé avec la version %s.\n",
            .packages_header = "Paquets correspondant à la version actuelle %s (%zu) "
                               ":\n",
            .target_version = "\nVersion cible pour le downgrade : %s\n",
            .command_header = "\nCommande à exécuter :\n  sudo %s\n",
            .dry_run_done = "\nMode dry-run : aucun changement effectué.\n",
            .confirm_prompt = "\nContinuer ? (o/N) : ",
            .cancelled = "Opération annulée par l'utilisateur.\n",
            .success = "Commande exécutée avec succès.\n",
        },
    [LANG_DE] =
        {
            .blurb = "Werkzeug zum Zurücksetzen installierter Pakete auf eine ältere "
                     "Version mit apt.",
            .usage = "Verwendung: %s --current <aktuelle_version> --downgrade "
                     "<zielversion> [Optionen]\n",
            .options_header = "Häufig verwendete Optionen:",
            .opt_current = "  --current <ver>     Installierte Version (exakter "
                           "Treffer)",
            .opt_downgrade = "  --downgrade <ver>   Zielversion für das Downgrade",
            .opt_dry_run = "  --dry-run           Plan anzeigen ohne Installation",
            .opt_json = "  --json              Plan als JSON ausgeben (impliziert dry-run)",
            .opt_yes = "  -y, --yes           Ohne Rückfrage bestätigen",
            .opt_help = "  -h, --help          Diese Hilfe anzeigen",
            .examples_header = "Beispiele:",
            .notes_header = "Hinweise:",
            .note_list = "  Auflisten und --dry-run benötigen kein sudo.",
            .note_root = "  Die Installation erfordert Root-Rechte (sudo).",
            .more_info = "Weitere Informationen: man apt-downgrade",
            .err_oom = "Fehler: Nicht genug Speicher.\n",
            .err_list_open =
                "Fehler: Liste der installierten Pakete konnte nicht geladen "
                "werden.\n",
            .err_list_close = "Fehler: Schließen der Paketliste fehlgeschlagen.\n",
            .err_read_answer = "Fehler beim Lesen der Antwort.\n",
            .err_root = "Fehler: Installation erfordert Root-Rechte. "
                        "Mit sudo ausführen oder --dry-run verwenden.\n",
            .err_exec = "Fehler beim Ausführen des Befehls.\n",
            .err_status = "Befehl beendet mit Status %d.\n",
            .no_packages = "Keine Pakete mit Version %s gefunden.\n",
            .packages_header = "Pakete mit aktueller Version %s (%zu):\n",
            .target_version = "\nZielversion für das Downgrade: %s\n",
            .command_header = "\nAuszuführender Befehl:\n  sudo %s\n",
            .dry_run_done = "\nDry-run-Modus: Es wurden keine Änderungen vorgenommen.\n",
            .confirm_prompt = "\nFortfahren? (j/N): ",
            .cancelled = "Vorgang vom Benutzer abgebrochen.\n",
            .success = "Befehl erfolgreich ausgeführt.\n",
        },
    [LANG_IT] =
        {
            .blurb = "Strumento per eseguire il downgrade dei pacchetti installati a "
                     "una versione precedente tramite apt.",
            .usage = "Uso: %s --current <versione_attuale> --downgrade "
                     "<versione_destinazione> [opzioni]\n",
            .options_header = "Opzioni più usate:",
            .opt_current = "  --current <ver>     Versione installata da cercare "
                           "(corrispondenza esatta)",
            .opt_downgrade = "  --downgrade <ver>   Versione di destinazione del "
                             "downgrade",
            .opt_dry_run = "  --dry-run           Mostra il piano senza installare",
            .opt_json = "  --json              Emette il piano in JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Conferma senza chiedere",
            .opt_help = "  -h, --help          Mostra questo aiuto",
            .examples_header = "Esempi:",
            .notes_header = "Note:",
            .note_list = "  Elencare e --dry-run non richiedono sudo.",
            .note_root = "  L'installazione richiede i privilegi di root (sudo).",
            .more_info = "Ulteriori informazioni: man apt-downgrade",
            .err_oom = "Errore: memoria insufficiente.\n",
            .err_list_open =
                "Errore: impossibile ottenere l'elenco dei pacchetti installati.\n",
            .err_list_close = "Errore: chiusura dell'elenco pacchetti non riuscita.\n",
            .err_read_answer = "Errore durante la lettura della risposta.\n",
            .err_root = "Errore: l'installazione richiede i privilegi di root. "
                        "Esegui con sudo o usa --dry-run.\n",
            .err_exec = "Errore durante l'esecuzione del comando.\n",
            .err_status = "Il comando è terminato con stato %d.\n",
            .no_packages = "Nessun pacchetto trovato con la versione %s.\n",
            .packages_header = "Pacchetti corrispondenti alla versione attuale %s "
                               "(%zu):\n",
            .target_version = "\nVersione di destinazione per il downgrade: %s\n",
            .command_header = "\nComando da eseguire:\n  sudo %s\n",
            .dry_run_done = "\nModalità dry-run: nessuna modifica effettuata.\n",
            .confirm_prompt = "\nProcedere? (s/N): ",
            .cancelled = "Operazione annullata dall'utente.\n",
            .success = "Comando eseguito correttamente.\n",
        },
    [LANG_RU] =
        {
            .blurb = "Утилита для понижения версии установленных пакетов через apt.",
            .usage = "Использование: %s --current <текущая_версия> --downgrade "
                     "<целевая_версия> [параметры]\n",
            .options_header = "Часто используемые параметры:",
            .opt_current = "  --current <ver>     Установленная версия (точное "
                           "совпадение)",
            .opt_downgrade = "  --downgrade <ver>   Целевая версия для понижения",
            .opt_dry_run = "  --dry-run           Показать план без установки",
            .opt_json = "  --json              Вывести план в JSON (подразумевает dry-run)",
            .opt_yes = "  -y, --yes           Подтвердить без запроса",
            .opt_help = "  -h, --help          Показать эту справку",
            .examples_header = "Примеры:",
            .notes_header = "Примечания:",
            .note_list = "  Просмотр списка и --dry-run не требуют sudo.",
            .note_root = "  Установка требует прав root (sudo).",
            .more_info = "Подробнее: man apt-downgrade",
            .err_oom = "Ошибка: недостаточно памяти.\n",
            .err_list_open =
                "Ошибка: не удалось получить список установленных пакетов.\n",
            .err_list_close = "Ошибка: не удалось закрыть список пакетов.\n",
            .err_read_answer = "Ошибка чтения ответа.\n",
            .err_root = "Ошибка: для установки нужны права root. "
                        "Запустите с sudo или используйте --dry-run.\n",
            .err_exec = "Ошибка выполнения команды.\n",
            .err_status = "Команда завершилась со статусом %d.\n",
            .no_packages = "Пакеты с версией %s не найдены.\n",
            .packages_header = "Пакеты с текущей версией %s (%zu):\n",
            .target_version = "\nЦелевая версия для понижения: %s\n",
            .command_header = "\nКоманда для выполнения:\n  sudo %s\n",
            .dry_run_done = "\nРежим dry-run: изменения не внесены.\n",
            .confirm_prompt = "\nПродолжить? (y/N): ",
            .cancelled = "Операция отменена пользователем.\n",
            .success = "Команда выполнена успешно.\n",
        },
    [LANG_ZH_CN] =
        {
            .blurb = "通过 apt 将已安装软件包降级到旧版本的工具。",
            .usage = "用法: %s --current <当前版本> --downgrade <目标版本> [选项]\n",
            .options_header = "常用选项:",
            .opt_current = "  --current <ver>     要匹配的已安装版本（精确匹配）",
            .opt_downgrade = "  --downgrade <ver>   降级目标版本",
            .opt_dry_run = "  --dry-run           仅显示计划，不安装",
            .opt_json = "  --json              以 JSON 输出计划（隐含 dry-run）",
            .opt_yes = "  -y, --yes           无需确认直接继续",
            .opt_help = "  -h, --help          显示此帮助",
            .examples_header = "示例:",
            .notes_header = "说明:",
            .note_list = "  列出软件包和 --dry-run 不需要 sudo。",
            .note_root = "  安装需要 root 权限（sudo）。",
            .more_info = "更多信息: man apt-downgrade",
            .err_oom = "错误: 内存不足。\n",
            .err_list_open = "错误: 无法获取已安装软件包列表。\n",
            .err_list_close = "错误: 关闭软件包列表失败。\n",
            .err_read_answer = "错误: 无法读取回答。\n",
            .err_root = "错误: 安装需要 root 权限。请使用 sudo 运行或使用 "
                        "--dry-run。\n",
            .err_exec = "错误: 无法执行命令。\n",
            .err_status = "命令以状态 %d 结束。\n",
            .no_packages = "未找到版本为 %s 的软件包。\n",
            .packages_header = "匹配当前版本 %s 的软件包（%zu）:\n",
            .target_version = "\n降级目标版本: %s\n",
            .command_header = "\n要执行的命令:\n  sudo %s\n",
            .dry_run_done = "\n演练模式: 未做任何更改。\n",
            .confirm_prompt = "\n是否继续？(y/N): ",
            .cancelled = "操作已被用户取消。\n",
            .success = "命令已成功执行。\n",
        },
    [LANG_ZH_TW] =
        {
            .blurb = "透過 apt 將已安裝套件降級至舊版本的工具。",
            .usage = "用法: %s --current <目前版本> --downgrade <目標版本> [選項]\n",
            .options_header = "常用選項:",
            .opt_current = "  --current <ver>     要符合的已安裝版本（精確符合）",
            .opt_downgrade = "  --downgrade <ver>   降級目標版本",
            .opt_dry_run = "  --dry-run           僅顯示計畫，不安裝",
            .opt_json = "  --json              以 JSON 輸出計畫（隱含 dry-run）",
            .opt_yes = "  -y, --yes           無需確認直接繼續",
            .opt_help = "  -h, --help          顯示此說明",
            .examples_header = "範例:",
            .notes_header = "說明:",
            .note_list = "  列出套件與 --dry-run 不需要 sudo。",
            .note_root = "  安裝需要 root 權限（sudo）。",
            .more_info = "更多資訊: man apt-downgrade",
            .err_oom = "錯誤: 記憶體不足。\n",
            .err_list_open = "錯誤: 無法取得已安裝套件清單。\n",
            .err_list_close = "錯誤: 關閉套件清單失敗。\n",
            .err_read_answer = "錯誤: 無法讀取回答。\n",
            .err_root = "錯誤: 安裝需要 root 權限。請使用 sudo 執行或使用 "
                        "--dry-run。\n",
            .err_exec = "錯誤: 無法執行指令。\n",
            .err_status = "指令以狀態 %d 結束。\n",
            .no_packages = "找不到版本為 %s 的套件。\n",
            .packages_header = "符合目前版本 %s 的套件（%zu）:\n",
            .target_version = "\n降級目標版本: %s\n",
            .command_header = "\n要執行的指令:\n  sudo %s\n",
            .dry_run_done = "\n演練模式: 未進行任何變更。\n",
            .confirm_prompt = "\n是否繼續？(y/N): ",
            .cancelled = "操作已被使用者取消。\n",
            .success = "指令已成功執行。\n",
        },
    [LANG_JA] =
        {
            .blurb = "apt を使ってインストール済みパッケージを古いバージョンへダウングレード"
                     "するツール。",
            .usage = "使い方: %s --current <現在のバージョン> --downgrade "
                     "<対象バージョン> [オプション]\n",
            .options_header = "よく使うオプション:",
            .opt_current = "  --current <ver>     一致させるインストール済みバージョン"
                           "（完全一致）",
            .opt_downgrade = "  --downgrade <ver>   ダウングレード先のバージョン",
            .opt_dry_run = "  --dry-run           インストールせずに計画だけ表示",
            .opt_json = "  --json              計画を JSON で出力（dry-run を含む）",
            .opt_yes = "  -y, --yes           確認なしで続行",
            .opt_help = "  -h, --help          このヘルプを表示",
            .examples_header = "例:",
            .notes_header = "注意:",
            .note_list = "  一覧表示と --dry-run に sudo は不要です。",
            .note_root = "  インストールには root 権限（sudo）が必要です。",
            .more_info = "詳細: man apt-downgrade",
            .err_oom = "エラー: メモリ不足です。\n",
            .err_list_open =
                "エラー: インストール済みパッケージ一覧を取得できませんでした。\n",
            .err_list_close = "エラー: パッケージ一覧のクローズに失敗しました。\n",
            .err_read_answer = "エラー: 応答を読み取れませんでした。\n",
            .err_root = "エラー: インストールには root 権限が必要です。"
                        "sudo で実行するか --dry-run を使ってください。\n",
            .err_exec = "エラー: コマンドの実行に失敗しました。\n",
            .err_status = "コマンドはステータス %d で終了しました。\n",
            .no_packages = "バージョン %s のパッケージは見つかりませんでした。\n",
            .packages_header = "現在のバージョン %s に一致するパッケージ (%zu):\n",
            .target_version = "\nダウングレード先のバージョン: %s\n",
            .command_header = "\n実行するコマンド:\n  sudo %s\n",
            .dry_run_done = "\nドライランモード: 変更は行われませんでした。\n",
            .confirm_prompt = "\n続行しますか？ (y/N): ",
            .cancelled = "ユーザーにより操作がキャンセルされました。\n",
            .success = "コマンドは正常に完了しました。\n",
        },
    [LANG_KO] =
        {
            .blurb = "apt를 사용해 설치된 패키지를 이전 버전으로 다운그레이드하는 도구.",
            .usage = "사용법: %s --current <현재_버전> --downgrade <대상_버전> [옵션]\n",
            .options_header = "자주 쓰는 옵션:",
            .opt_current = "  --current <ver>     일치시킬 설치 버전(정확한 일치)",
            .opt_downgrade = "  --downgrade <ver>   다운그레이드 대상 버전",
            .opt_dry_run = "  --dry-run           설치하지 않고 계획만 표시",
            .opt_json = "  --json              계획을 JSON으로 출력(dry-run 포함)",
            .opt_yes = "  -y, --yes           확인 없이 진행",
            .opt_help = "  -h, --help          이 도움말 표시",
            .examples_header = "예:",
            .notes_header = "참고:",
            .note_list = "  목록 조회와 --dry-run은 sudo가 필요하지 않습니다.",
            .note_root = "  설치에는 root 권한(sudo)이 필요합니다.",
            .more_info = "자세한 정보: man apt-downgrade",
            .err_oom = "오류: 메모리가 부족합니다.\n",
            .err_list_open = "오류: 설치된 패키지 목록을 가져올 수 없습니다.\n",
            .err_list_close = "오류: 패키지 목록을 닫지 못했습니다.\n",
            .err_read_answer = "오류: 응답을 읽지 못했습니다.\n",
            .err_root = "오류: 설치에는 root 권한이 필요합니다. "
                        "sudo로 실행하거나 --dry-run을 사용하세요.\n",
            .err_exec = "오류: 명령을 실행하지 못했습니다.\n",
            .err_status = "명령이 상태 %d로 종료되었습니다.\n",
            .no_packages = "버전 %s인 패키지를 찾지 못했습니다.\n",
            .packages_header = "현재 버전 %s과(와) 일치하는 패키지(%zu):\n",
            .target_version = "\n다운그레이드 대상 버전: %s\n",
            .command_header = "\n실행할 명령:\n  sudo %s\n",
            .dry_run_done = "\n드라이런 모드: 변경 사항이 없습니다.\n",
            .confirm_prompt = "\n계속할까요? (y/N): ",
            .cancelled = "사용자가 작업을 취소했습니다.\n",
            .success = "명령이 성공적으로 완료되었습니다.\n",
        },
    [LANG_AR] =
        {
            .blurb = "أداة لخفض إصدارات الحزم المثبتة إلى إصدار أقدم عبر apt.",
            .usage = "الاستخدام: %s --current <الإصدار_الحالي> --downgrade "
                     "<الإصدار_الهدف> [خيارات]\n",
            .options_header = "الخيارات الأكثر استخدامًا:",
            .opt_current = "  --current <ver>     الإصدار المثبت للمطابقة (مطابقة "
                           "دقيقة)",
            .opt_downgrade = "  --downgrade <ver>   الإصدار الهدف للتخفيض",
            .opt_dry_run = "  --dry-run           عرض الخطة دون تثبيت",
            .opt_json = "  --json              إخراج الخطة بصيغة JSON (يتضمن dry-run)",
            .opt_yes = "  -y, --yes           التأكيد دون سؤال",
            .opt_help = "  -h, --help          عرض هذه المساعدة",
            .examples_header = "أمثلة:",
            .notes_header = "ملاحظات:",
            .note_list = "  العرض و--dry-run لا يتطلبان sudo.",
            .note_root = "  التثبيت يتطلب صلاحيات الجذر (sudo).",
            .more_info = "مزيد من المعلومات: man apt-downgrade",
            .err_oom = "خطأ: نفاد الذاكرة.\n",
            .err_list_open = "خطأ: تعذر الحصول على قائمة الحزم المثبتة.\n",
            .err_list_close = "خطأ: فشل إغلاق قائمة الحزم.\n",
            .err_read_answer = "خطأ: تعذر قراءة الإجابة.\n",
            .err_root = "خطأ: التثبيت يتطلب صلاحيات الجذر. "
                        "شغّل باستخدام sudo أو استخدم --dry-run.\n",
            .err_exec = "خطأ: فشل تنفيذ الأمر.\n",
            .err_status = "انتهى الأمر بالحالة %d.\n",
            .no_packages = "لم يتم العثور على حزم بالإصدار %s.\n",
            .packages_header = "الحزم المطابقة للإصدار الحالي %s (%zu):\n",
            .target_version = "\nإصدار التخفيض المستهدف: %s\n",
            .command_header = "\nالأمر المراد تنفيذه:\n  sudo %s\n",
            .dry_run_done = "\nوضع التجربة: لم يتم إجراء أي تغييرات.\n",
            .confirm_prompt = "\nهل تريد المتابعة؟ (y/N): ",
            .cancelled = "ألغى المستخدم العملية.\n",
            .success = "اكتمل الأمر بنجاح.\n",
        },
    [LANG_HI] =
        {
            .blurb = "apt के माध्यम से स्थापित पैकेजों को पुराने संस्करण पर डाउनग्रेड "
                     "करने का उपकरण।",
            .usage = "उपयोग: %s --current <वर्तमान_संस्करण> --downgrade "
                     "<लक्ष्य_संस्करण> [विकल्प]\n",
            .options_header = "सबसे उपयोगी विकल्प:",
            .opt_current = "  --current <ver>     मिलान हेतु स्थापित संस्करण "
                           "(सटीक मिलान)",
            .opt_downgrade = "  --downgrade <ver>   डाउनग्रेड का लक्ष्य संस्करण",
            .opt_dry_run = "  --dry-run           बिना इंस्टॉल किए योजना दिखाएँ",
            .opt_json = "  --json              योजना JSON में लिखें (dry-run निहित)",
            .opt_yes = "  -y, --yes           बिना पूछे पुष्टि करें",
            .opt_help = "  -h, --help          यह सहायता दिखाएँ",
            .examples_header = "उदाहरण:",
            .notes_header = "नोट्स:",
            .note_list = "  सूची और --dry-run के लिए sudo की आवश्यकता नहीं।",
            .note_root = "  इंस्टॉल के लिए root अधिकार (sudo) चाहिए।",
            .more_info = "अधिक जानकारी: man apt-downgrade",
            .err_oom = "त्रुटि: पर्याप्त मेमोरी नहीं।\n",
            .err_list_open =
                "त्रुटि: स्थापित पैकेजों की सूची प्राप्त नहीं हो सकी।\n",
            .err_list_close = "त्रुटि: पैकेज सूची बंद करने में विफल।\n",
            .err_read_answer = "त्रुटि: उत्तर पढ़ा नहीं जा सका।\n",
            .err_root = "त्रुटि: इंस्टॉल के लिए root अधिकार चाहिए। "
                        "sudo से चलाएँ या --dry-run उपयोग करें।\n",
            .err_exec = "त्रुटि: कमांड चलाने में विफल।\n",
            .err_status = "कमांड स्थिति %d के साथ समाप्त हुई।\n",
            .no_packages = "संस्करण %s वाले कोई पैकेज नहीं मिले।\n",
            .packages_header = "वर्तमान संस्करण %s से मेल खाने वाले पैकेज (%zu):\n",
            .target_version = "\nडाउनग्रेड का लक्ष्य संस्करण: %s\n",
            .command_header = "\nचलाने के लिए कमांड:\n  sudo %s\n",
            .dry_run_done = "\nड्राई-रन मोड: कोई बदलाव नहीं किए गए।\n",
            .confirm_prompt = "\nआगे बढ़ें? (y/N): ",
            .cancelled = "उपयोगकर्ता द्वारा कार्रवाई रद्द की गई।\n",
            .success = "कमांड सफलतापूर्वक पूर्ण हुई।\n",
        },
    [LANG_NL] =
        {
            .blurb = "Hulpmiddel om geïnstalleerde pakketten via apt terug te zetten "
                     "naar een oudere versie.",
            .usage = "Gebruik: %s --current <huidige_versie> --downgrade "
                     "<doelversie> [opties]\n",
            .options_header = "Meest gebruikte opties:",
            .opt_current = "  --current <ver>     Geïnstalleerde versie om te matchen "
                           "(exact)",
            .opt_downgrade = "  --downgrade <ver>   Doelversie voor de downgrade",
            .opt_dry_run = "  --dry-run           Plan tonen zonder te installeren",
            .opt_json = "  --json              Plan als JSON (impliceert dry-run)",
            .opt_yes = "  -y, --yes           Bevestigen zonder te vragen",
            .opt_help = "  -h, --help          Deze hulp tonen",
            .examples_header = "Voorbeelden:",
            .notes_header = "Opmerkingen:",
            .note_list = "  Opsommen en --dry-run vereisen geen sudo.",
            .note_root = "  Installeren vereist rootrechten (sudo).",
            .more_info = "Meer informatie: man apt-downgrade",
            .err_oom = "Fout: onvoldoende geheugen.\n",
            .err_list_open =
                "Fout: kon de lijst met geïnstalleerde pakketten niet ophalen.\n",
            .err_list_close = "Fout: sluiten van de pakketlijst mislukt.\n",
            .err_read_answer = "Fout: kon het antwoord niet lezen.\n",
            .err_root = "Fout: installeren vereist rootrechten. "
                        "Voer uit met sudo of gebruik --dry-run.\n",
            .err_exec = "Fout: opdracht uitvoeren mislukt.\n",
            .err_status = "De opdracht eindigde met status %d.\n",
            .no_packages = "Geen pakketten gevonden met versie %s.\n",
            .packages_header = "Pakketten die overeenkomen met huidige versie %s "
                               "(%zu):\n",
            .target_version = "\nDoelversie voor downgrade: %s\n",
            .command_header = "\nUit te voeren opdracht:\n  sudo %s\n",
            .dry_run_done = "\nDry-run-modus: er zijn geen wijzigingen aangebracht.\n",
            .confirm_prompt = "\nDoorgaan? (j/N): ",
            .cancelled = "Bewerking geannuleerd door de gebruiker.\n",
            .success = "Opdracht succesvol uitgevoerd.\n",
        },
    [LANG_PL] =
        {
            .blurb = "Narzędzie do obniżania wersji zainstalowanych pakietów za pomocą "
                     "apt.",
            .usage = "Użycie: %s --current <bieżąca_wersja> --downgrade "
                     "<wersja_docelowa> [opcje]\n",
            .options_header = "Najczęściej używane opcje:",
            .opt_current = "  --current <ver>     Zainstalowana wersja do dopasowania "
                           "(dokładne)",
            .opt_downgrade = "  --downgrade <ver>   Docelowa wersja obniżenia",
            .opt_dry_run = "  --dry-run           Pokaż plan bez instalacji",
            .opt_json = "  --json              Wypisz plan jako JSON (implikuje dry-run)",
            .opt_yes = "  -y, --yes           Potwierdź bez pytania",
            .opt_help = "  -h, --help          Pokaż tę pomoc",
            .examples_header = "Przykłady:",
            .notes_header = "Uwagi:",
            .note_list = "  Listowanie i --dry-run nie wymagają sudo.",
            .note_root = "  Instalacja wymaga uprawnień root (sudo).",
            .more_info = "Więcej informacji: man apt-downgrade",
            .err_oom = "Błąd: brak pamięci.\n",
            .err_list_open =
                "Błąd: nie udało się uzyskać listy zainstalowanych pakietów.\n",
            .err_list_close = "Błąd: nie udało się zamknąć listy pakietów.\n",
            .err_read_answer = "Błąd odczytu odpowiedzi.\n",
            .err_root = "Błąd: instalacja wymaga uprawnień root. "
                        "Uruchom z sudo lub użyj --dry-run.\n",
            .err_exec = "Błąd wykonania polecenia.\n",
            .err_status = "Polecenie zakończyło się ze statusem %d.\n",
            .no_packages = "Nie znaleziono pakietów w wersji %s.\n",
            .packages_header = "Pakiety pasujące do bieżącej wersji %s (%zu):\n",
            .target_version = "\nDocelowa wersja obniżenia: %s\n",
            .command_header = "\nPolecenie do wykonania:\n  sudo %s\n",
            .dry_run_done = "\nTryb dry-run: nie wprowadzono zmian.\n",
            .confirm_prompt = "\nKontynuować? (t/N): ",
            .cancelled = "Operacja anulowana przez użytkownika.\n",
            .success = "Polecenie wykonane pomyślnie.\n",
        },
    [LANG_TR] =
        {
            .blurb = "apt ile kurulu paketleri daha eski bir sürüme düşürmek için araç.",
            .usage = "Kullanım: %s --current <geçerli_sürüm> --downgrade "
                     "<hedef_sürüm> [seçenekler]\n",
            .options_header = "En çok kullanılan seçenekler:",
            .opt_current = "  --current <ver>     Eşleştirilecek kurulu sürüm "
                           "(tam eşleşme)",
            .opt_downgrade = "  --downgrade <ver>   Düşürme hedef sürümü",
            .opt_dry_run = "  --dry-run           Kurmadan planı göster",
            .opt_json = "  --json              Planı JSON olarak yaz (dry-run ima eder)",
            .opt_yes = "  -y, --yes           Sormadan onayla",
            .opt_help = "  -h, --help          Bu yardımı göster",
            .examples_header = "Örnekler:",
            .notes_header = "Notlar:",
            .note_list = "  Listeleme ve --dry-run için sudo gerekmez.",
            .note_root = "  Kurulum root yetkisi (sudo) ister.",
            .more_info = "Daha fazla bilgi: man apt-downgrade",
            .err_oom = "Hata: yetersiz bellek.\n",
            .err_list_open = "Hata: kurulu paket listesi alınamadı.\n",
            .err_list_close = "Hata: paket listesi kapatılamadı.\n",
            .err_read_answer = "Hata: yanıt okunamadı.\n",
            .err_root = "Hata: kurulum root yetkisi ister. "
                        "sudo ile çalıştırın veya --dry-run kullanın.\n",
            .err_exec = "Hata: komut çalıştırılamadı.\n",
            .err_status = "Komut %d durumuyla sona erdi.\n",
            .no_packages = "%s sürümüne sahip paket bulunamadı.\n",
            .packages_header = "Geçerli sürüm %s ile eşleşen paketler (%zu):\n",
            .target_version = "\nDüşürme hedef sürümü: %s\n",
            .command_header = "\nÇalıştırılacak komut:\n  sudo %s\n",
            .dry_run_done = "\nDry-run modu: değişiklik yapılmadı.\n",
            .confirm_prompt = "\nDevam edilsin mi? (e/N): ",
            .cancelled = "İşlem kullanıcı tarafından iptal edildi.\n",
            .success = "Komut başarıyla tamamlandı.\n",
        },
    [LANG_GL] =
        {
            .blurb = "Ferramenta para baixar paquetes instalados a unha versión "
                     "anterior mediante apt.",
            .usage = "Uso: %s --current <versión_actual> --downgrade "
                     "<versión_obxectivo> [opcións]\n",
            .options_header = "Opcións máis usadas:",
            .opt_current = "  --current <ver>     Versión instalada a buscar "
                           "(coincidencia exacta)",
            .opt_downgrade = "  --downgrade <ver>   Versión obxectivo do downgrade",
            .opt_dry_run = "  --dry-run           Amosar o plan sen instalar",
            .opt_json = "  --json              Emitir o plan en JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Confirmar sen preguntar",
            .opt_help = "  -h, --help          Amosar esta axuda",
            .examples_header = "Exemplos:",
            .notes_header = "Notas:",
            .note_list = "  Listar e --dry-run non requiren sudo.",
            .note_root = "  A instalación require privilexios de root (sudo).",
            .more_info = "Máis información: man apt-downgrade",
            .err_oom = "Erro: memoria insuficiente.\n",
            .err_list_open =
                "Erro: non se puido obter a lista de paquetes instalados.\n",
            .err_list_close = "Erro: fallou ao pechar a listaxe de paquetes.\n",
            .err_read_answer = "Erro ao ler a resposta.\n",
            .err_root = "Erro: a instalación require privilexios de root. "
                        "Executa con sudo ou usa --dry-run.\n",
            .err_exec = "Erro ao executar a orde.\n",
            .err_status = "A orde rematou co estado %d.\n",
            .no_packages = "Non se atoparon paquetes coa versión %s.\n",
            .packages_header =
                "Paquetes que coinciden coa versión actual %s (%zu):\n",
            .target_version = "\nVersión obxectivo para o downgrade: %s\n",
            .command_header = "\nOrde a executar:\n  sudo %s\n",
            .dry_run_done = "\nModo dry-run: non se fixeron cambios.\n",
            .confirm_prompt = "\nDesexa continuar? (s/N): ",
            .cancelled = "Operación cancelada polo usuario.\n",
            .success = "Orde executada correctamente.\n",
        },
    [LANG_CA] =
        {
            .blurb = "Eina per degradar paquets instal·lats a una versió anterior "
                     "mitjançant apt.",
            .usage = "Ús: %s --current <versió_actual> --downgrade "
                     "<versió_objectiu> [opcions]\n",
            .options_header = "Opcions més usades:",
            .opt_current = "  --current <ver>     Versió instal·lada a cercar "
                           "(coincidència exacta)",
            .opt_downgrade = "  --downgrade <ver>   Versió objectiu del downgrade",
            .opt_dry_run = "  --dry-run           Mostrar el pla sense instal·lar",
            .opt_json = "  --json              Emetre el pla en JSON (implica dry-run)",
            .opt_yes = "  -y, --yes           Confirmar sense preguntar",
            .opt_help = "  -h, --help          Mostrar aquesta ajuda",
            .examples_header = "Exemples:",
            .notes_header = "Notes:",
            .note_list = "  Llistar i --dry-run no requereixen sudo.",
            .note_root = "  La instal·lació requereix privilegis de root (sudo).",
            .more_info = "Més informació: man apt-downgrade",
            .err_oom = "Error: memòria insuficient.\n",
            .err_list_open =
                "Error: no s'ha pogut obtenir la llista de paquets instal·lats.\n",
            .err_list_close = "Error: ha fallat el tancament de la llista de paquets.\n",
            .err_read_answer = "Error en llegir la resposta.\n",
            .err_root = "Error: la instal·lació requereix privilegis de root. "
                        "Executeu amb sudo o useu --dry-run.\n",
            .err_exec = "Error en executar l'ordre.\n",
            .err_status = "L'ordre ha acabat amb l'estat %d.\n",
            .no_packages = "No s'han trobat paquets amb la versió %s.\n",
            .packages_header =
                "Paquets que coincideixen amb la versió actual %s (%zu):\n",
            .target_version = "\nVersió objectiu per al downgrade: %s\n",
            .command_header = "\nOrdre a executar:\n  sudo %s\n",
            .dry_run_done = "\nMode dry-run: no s'han fet canvis.\n",
            .confirm_prompt = "\nVoleu continuar? (s/N): ",
            .cancelled = "Operació cancel·lada per l'usuari.\n",
            .success = "Ordre executada correctament.\n",
        },
    [LANG_EU] =
        {
            .blurb = "Instalatutako paketeak bertsio zaharrago batera jaisteko "
                     "tresna apt bidez.",
            .usage = "Erabilera: %s --current <uneko_bertsioa> --downgrade "
                     "<helburu_bertsioa> [aukerak]\n",
            .options_header = "Aukera erabilienak:",
            .opt_current = "  --current <ver>     Bilatu beharreko instalatutako "
                           "bertsioa (zehatza)",
            .opt_downgrade = "  --downgrade <ver>   Jaitsieraren helburu-bertsioa",
            .opt_dry_run = "  --dry-run           Erakutsi plana instalatu gabe",
            .opt_json = "  --json              Plana JSON gisa idatzi (dry-run inplikatzen du)",
            .opt_yes = "  -y, --yes           Baieztatu galdetu gabe",
            .opt_help = "  -h, --help          Erakutsi laguntza hau",
            .examples_header = "Adibideak:",
            .notes_header = "Oharrak:",
            .note_list = "  Zerrendatzeak eta --dry-run-ek ez dute sudo behar.",
            .note_root = "  Instalazioak root baimenak behar ditu (sudo).",
            .more_info = "Informazio gehiago: man apt-downgrade",
            .err_oom = "Errorea: memoria nahikorik ez.\n",
            .err_list_open =
                "Errorea: ezin izan da instalatutako paketeen zerrenda lortu.\n",
            .err_list_close = "Errorea: pakete-zerrenda ixtean huts egin du.\n",
            .err_read_answer = "Errorea erantzuna irakurtzean.\n",
            .err_root = "Errorea: instalazioak root baimenak behar ditu. "
                        "Exekutatu sudo-rekin edo erabili --dry-run.\n",
            .err_exec = "Errorea komandoa exekutatzean.\n",
            .err_status = "Komandoa %d egoerarekin amaitu da.\n",
            .no_packages = "Ez da aurkitu %s bertsiodun paketerik.\n",
            .packages_header = "Uneko %s bertsioarekin bat datozen paketeak (%zu):\n",
            .target_version = "\nJaitsieraren helburu-bertsioa: %s\n",
            .command_header = "\nExekutatu beharreko komandoa:\n  sudo %s\n",
            .dry_run_done = "\nDry-run modua: ez da aldaketarik egin.\n",
            .confirm_prompt = "\nJarraitu? (b/N): ",
            .cancelled = "Eragiketa erabiltzaileak bertan behera utzi du.\n",
            .success = "Komandoa behar bezala exekutatu da.\n",
        },
};

static int tag_matches(const char *tag, const char *prefix) {
    size_t n = strlen(prefix);
    if (strncmp(tag, prefix, n) != 0) {
        return 0;
    }
    return tag[n] == '\0' || tag[n] == '_' || tag[n] == '.' || tag[n] == '@' ||
           tag[n] == '-';
}

static Lang lang_from_tag(const char *tag) {
    if (!tag || tag[0] == '\0' || strcmp(tag, "C") == 0 ||
        strcmp(tag, "C.UTF-8") == 0 || strcmp(tag, "POSIX") == 0) {
        return LANG_EN;
    }

    /* Orden importa: pt_BR antes que pt; zh_TW/zh_HK antes que zh. */
    if (tag_matches(tag, "pt_BR") || tag_matches(tag, "pt_br")) {
        return LANG_PT_BR;
    }
    if (tag_matches(tag, "zh_TW") || tag_matches(tag, "zh_tw") ||
        tag_matches(tag, "zh_HK") || tag_matches(tag, "zh_hk") ||
        tag_matches(tag, "zh_Hant") || tag_matches(tag, "zh_hant")) {
        return LANG_ZH_TW;
    }
    if (tag_matches(tag, "zh_CN") || tag_matches(tag, "zh_cn") ||
        tag_matches(tag, "zh_Hans") || tag_matches(tag, "zh_hans") ||
        tag_matches(tag, "zh")) {
        return LANG_ZH_CN;
    }
    if (tag_matches(tag, "es")) {
        return LANG_ES;
    }
    if (tag_matches(tag, "pt")) {
        return LANG_PT;
    }
    if (tag_matches(tag, "fr")) {
        return LANG_FR;
    }
    if (tag_matches(tag, "de")) {
        return LANG_DE;
    }
    if (tag_matches(tag, "it")) {
        return LANG_IT;
    }
    if (tag_matches(tag, "ru")) {
        return LANG_RU;
    }
    if (tag_matches(tag, "ja")) {
        return LANG_JA;
    }
    if (tag_matches(tag, "ko")) {
        return LANG_KO;
    }
    if (tag_matches(tag, "ar")) {
        return LANG_AR;
    }
    if (tag_matches(tag, "hi")) {
        return LANG_HI;
    }
    if (tag_matches(tag, "nl")) {
        return LANG_NL;
    }
    if (tag_matches(tag, "pl")) {
        return LANG_PL;
    }
    if (tag_matches(tag, "tr")) {
        return LANG_TR;
    }
    if (tag_matches(tag, "gl")) {
        return LANG_GL;
    }
    if (tag_matches(tag, "ca")) {
        return LANG_CA;
    }
    if (tag_matches(tag, "eu")) {
        return LANG_EU;
    }
    if (tag_matches(tag, "en")) {
        return LANG_EN;
    }

    return LANG_EN;
}

Lang i18n_detect_lang(void) {
    const char *override = getenv("APT_DOWNGRADE_LANG");
    if (override && override[0] != '\0') {
        return lang_from_tag(override);
    }

    const char *language = getenv("LANGUAGE");
    if (language && language[0] != '\0') {
        char first[64];
        size_t n = strcspn(language, ":");
        if (n >= sizeof(first)) {
            n = sizeof(first) - 1;
        }
        memcpy(first, language, n);
        first[n] = '\0';
        return lang_from_tag(first);
    }

    const char *keys[] = {"LC_ALL", "LC_MESSAGES", "LANG"};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        const char *value = getenv(keys[i]);
        if (!value || value[0] == '\0') {
            continue;
        }
        return lang_from_tag(value);
    }

    return LANG_EN;
}

const Messages *i18n_messages(Lang lang) {
    if (lang < 0 || lang >= LANG_COUNT) {
        return &MSGS[LANG_EN];
    }
    return &MSGS[lang];
}

int i18n_is_affirmative(Lang lang, int c) {
    c = tolower((unsigned char)c);
    /* Comunes internacionales */
    if (c == 'y' || c == 's' || c == 'o' || c == 'j') {
        return 1;
    }
    switch (lang) {
        case LANG_DE:
        case LANG_NL:
            return c == 'j';
        case LANG_FR:
            return c == 'o';
        case LANG_PL:
            return c == 't'; /* tak */
        case LANG_TR:
            return c == 'e'; /* evet */
        case LANG_EU:
            return c == 'b'; /* bai */
        case LANG_ES:
        case LANG_PT:
        case LANG_PT_BR:
        case LANG_IT:
        case LANG_GL:
        case LANG_CA:
            return c == 's';
        default:
            return c == 'y';
    }
}
