# Makefile para apt-downgrade

PACKAGE_NAME = apt-downgrade
VERSION = 1.2.0
DEB_VERSION = 1
FULL_VERSION = $(VERSION)-$(DEB_VERSION)

# PPA / firma. Opcional: export SHARED_KEYS=/ruta/a/_shared-keys
# (archivo launchpad/ubuntu-tools.env). Sin path absoluto en el repo.
SHARED_KEYS ?=
DEBSIGN_KEYID ?= 5077A813F9AE818752168EA173140C59FF3EBE5D
LAUNCHPAD_PPA ?= ppa:alanjmrt94/ubuntu-tools

# Directorios
SRC_DIR = src
BUILD_DIR = build
DIST_DIR = dist
DEBIAN_DIR = debian
DESTDIR ?=
# dpkg-buildpackage siempre deja artefactos en el directorio padre del source tree
PARENT_DIR = ..

# Compilador
CC = gcc
CFLAGS = -Wall -Wextra -O2 -DAPT_DOWNGRADE_VERSION=\"$(VERSION)\"
LDFLAGS =

SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/i18n.c
TARGET = $(BUILD_DIR)/$(PACKAGE_NAME)

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin

# Ubuntu 20.04 → 26.x (LTS + intermedias + 26.04). Override: SERIES="jammy noble"
PPA_SERIES ?= focal jammy noble plucky questing resolute

.PHONY: all clean clean-build build package install uninstall test \
	deb deb-src sign upload ppa-release ppa-series ppa-series-upload \
	load-ppa-env gather-dist

all: build

build: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

# Mueve artefactos de dpkg-buildpackage (padre) a dist/
gather-dist:
	@mkdir -p $(DIST_DIR)
	@moved=0; \
	for f in $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)* \
		$(PARENT_DIR)/$(PACKAGE_NAME)-dbgsym_$(VERSION)*; do \
		[ -e "$$f" ] || continue; \
		mv -f "$$f" $(DIST_DIR)/; \
		moved=1; \
	done; \
	if [ "$$moved" -eq 0 ]; then \
		echo "No se encontraron artefactos en $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*"; \
		exit 1; \
	fi; \
	echo "Artefactos en $(DIST_DIR)/:"; \
	ls -1 $(DIST_DIR)/$(PACKAGE_NAME)_$(VERSION)* \
		$(DIST_DIR)/$(PACKAGE_NAME)-dbgsym_$(VERSION)* 2>/dev/null || true

# Limpia build y restos en raíz/padre; no toca dist/ (para encadenar package→deb→deb-src)
clean-build:
	rm -rf $(BUILD_DIR)
	rm -f *.deb
	rm -f $(PACKAGE_NAME)_*.deb
	rm -f $(PACKAGE_NAME)_*.tar.gz
	rm -f $(PACKAGE_NAME)_*.tar.xz
	rm -f $(PACKAGE_NAME)_*.dsc
	rm -f $(PACKAGE_NAME)_*.changes
	rm -f $(PACKAGE_NAME)_*.build
	rm -f $(PACKAGE_NAME)_*.buildinfo
	rm -f $(PACKAGE_NAME)_*.upload
	rm -f $(PACKAGE_NAME)_*.ddeb
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.tar.*
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.dsc
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.changes
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.buildinfo
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.upload
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.deb
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.ddeb
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)-dbgsym_$(VERSION)*.ddeb

clean: clean-build
	rm -rf $(DIST_DIR)

test: build
	@chmod +x tests/run_tests.sh
	./tests/run_tests.sh

# Construir paquete .deb local (sin dh; útil offline)
package: clean-build build
	@echo "Construyendo paquete .deb..."
	@rm -rf $(BUILD_DIR)/package
	@mkdir -p $(DIST_DIR)
	@install -d -m 755 $(BUILD_DIR)/package/usr/bin
	@install -d -m 755 $(BUILD_DIR)/package/usr/share/man/man1
	@install -d -m 755 $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)
	@install -d -m 755 $(BUILD_DIR)/package/DEBIAN
	@install -m 755 $(TARGET) $(BUILD_DIR)/package/usr/bin/$(PACKAGE_NAME)
	@strip --strip-unneeded $(BUILD_DIR)/package/usr/bin/$(PACKAGE_NAME)
	@install -m 644 man/$(PACKAGE_NAME).1 $(BUILD_DIR)/package/usr/share/man/man1/
	@gzip -9n -f $(BUILD_DIR)/package/usr/share/man/man1/$(PACKAGE_NAME).1
	@for loc in es pt pt_BR fr de it ru zh_CN zh_TW ja ko ar hi nl pl tr gl ca eu; do \
		install -d -m 755 $(BUILD_DIR)/package/usr/share/man/$$loc/man1; \
		install -m 644 man/$$loc/man1/$(PACKAGE_NAME).1 \
			$(BUILD_DIR)/package/usr/share/man/$$loc/man1/; \
		gzip -9n -f $(BUILD_DIR)/package/usr/share/man/$$loc/man1/$(PACKAGE_NAME).1; \
	done
	@install -m 644 README.md $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/
	@install -m 644 README-es.md $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/
	@install -m 644 $(DEBIAN_DIR)/copyright $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/
	@install -m 644 $(DEBIAN_DIR)/changelog $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/changelog.Debian
	@gzip -9n -f $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/changelog.Debian
	@find $(BUILD_DIR)/package -type d -exec chmod 755 {} \;
	@{ \
		echo "Package: $(PACKAGE_NAME)"; \
		echo "Version: $(FULL_VERSION)"; \
		echo "Architecture: amd64"; \
		echo "Maintainer: alanjmrt94 <alanjmartinez94@gmail.com>"; \
		echo "Depends: libc6 (>= 2.31)"; \
		echo "Section: admin"; \
		echo "Priority: optional"; \
		echo "Homepage: https://launchpad.net/~alanjmrt94/+archive/ubuntu/ubuntu-tools"; \
		echo "Description: Downgrade installed packages on Ubuntu/Debian"; \
		echo " Command-line tool to find installed packages matching an exact"; \
		echo " version and downgrade them to a target version via apt."; \
	} > $(BUILD_DIR)/package/DEBIAN/control
	@cd $(BUILD_DIR)/package && find usr -type f -exec md5sum {} \; > DEBIAN/md5sums
	@dpkg-deb --root-owner-group --build $(BUILD_DIR)/package $(DIST_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_amd64.deb
	@echo "Paquete creado: $(DIST_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_amd64.deb"

# Paquete binario con debhelper (artefactos en dist/; sin firmar)
deb: clean-build load-ppa-env
	@echo "Construyendo paquete binario $(FULL_VERSION)..."
	dpkg-buildpackage -b -us -uc
	@$(MAKE) gather-dist

install: build
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(PACKAGE_NAME)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(PACKAGE_NAME)

load-ppa-env:
	@if [ -n "$(SHARED_KEYS)" ] && [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		echo "Usando $(SHARED_KEYS)/launchpad/ubuntu-tools.env"; \
	else \
		echo "Aviso: SHARED_KEYS no apunta a ubuntu-tools.env; usando defaults del Makefile"; \
	fi

# Paquete fuente para Launchpad (sin firmar) → dist/
deb-src: clean-build load-ppa-env
	@echo "Preparando paquete fuente para $(LAUNCHPAD_PPA) (serie: resolute)..."
	dpkg-buildpackage -S -us -uc
	@$(MAKE) gather-dist

# Firmar .changes / .dsc en dist/ (pide passphrase de GPG si aplica)
sign: load-ppa-env
	@key="$(DEBSIGN_KEYID)"; \
	if [ -n "$(SHARED_KEYS)" ] && [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		set -a; . "$(SHARED_KEYS)/launchpad/ubuntu-tools.env"; set +a; \
		key="$$DEBSIGN_KEYID"; \
	fi; \
	changes="$(DIST_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_source.changes"; \
	if [ ! -f "$$changes" ]; then \
		changes="$(DIST_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_amd64.changes"; \
	fi; \
	if [ ! -f "$$changes" ]; then \
		echo "No existe .changes en $(DIST_DIR)/ — ejecutá 'make deb-src' o 'make deb' primero."; \
		exit 1; \
	fi; \
	echo "Firmando $$changes con $$key ..."; \
	debsign -k "$$key" "$$changes"

# Subir al PPA (requiere firma previa)
upload: load-ppa-env
	@ppa="$(LAUNCHPAD_PPA)"; \
	if [ -n "$(SHARED_KEYS)" ] && [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		set -a; . "$(SHARED_KEYS)/launchpad/ubuntu-tools.env"; set +a; \
		ppa="$$LAUNCHPAD_PPA"; \
	fi; \
	changes="$(DIST_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_source.changes"; \
	if [ ! -f "$$changes" ]; then \
		echo "No existe $$changes — ejecutá 'make deb-src' y 'make sign' primero."; \
		exit 1; \
	fi; \
	echo "Subiendo a $$ppa ..."; \
	dput "$$ppa" "$$changes"

# Flujo completo (una sola serie del changelog actual): fuente → firmar → subir
ppa-release: deb-src sign upload

# Todas las series 20.04–26.x: genera y firma (sin subir)
# Requiere scripts/ppa-release-series.sh (local-only, no está en el remoto).
ppa-series:
	@test -f scripts/ppa-release-series.sh || { \
		echo "Falta scripts/ppa-release-series.sh (helper local, no versionado)."; \
		exit 1; \
	}
	@chmod +x scripts/ppa-release-series.sh
	SERIES="$(PPA_SERIES)" VERSION="$(VERSION)" DEB_VERSION="$(DEB_VERSION)" \
		SHARED_KEYS="$(SHARED_KEYS)" ./scripts/ppa-release-series.sh

# Todas las series: firma y sube al PPA
ppa-series-upload:
	@test -f scripts/ppa-release-series.sh || { \
		echo "Falta scripts/ppa-release-series.sh (helper local, no versionado)."; \
		exit 1; \
	}
	@chmod +x scripts/ppa-release-series.sh
	SERIES="$(PPA_SERIES)" VERSION="$(VERSION)" DEB_VERSION="$(DEB_VERSION)" \
		SHARED_KEYS="$(SHARED_KEYS)" UPLOAD=1 ./scripts/ppa-release-series.sh
