# Makefile para apt-downgrade

PACKAGE_NAME = apt-downgrade
VERSION = 1.0.0
DEB_VERSION = 1
FULL_VERSION = $(VERSION)-$(DEB_VERSION)

# PPA / firma (defaults; override con _shared-keys/launchpad/ubuntu-tools.env)
SHARED_KEYS ?=
DEBSIGN_KEYID ?= 5077A813F9AE818752168EA173140C59FF3EBE5D
LAUNCHPAD_PPA ?= ppa:alanjmrt94/ubuntu-tools

# Directorios
SRC_DIR = src
BUILD_DIR = build
DEBIAN_DIR = debian
DESTDIR ?=
# dpkg-buildpackage deja artefactos en el directorio padre
PARENT_DIR = ..

# Compilador
CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS =

SOURCES = $(SRC_DIR)/main.c
TARGET = $(BUILD_DIR)/$(PACKAGE_NAME)

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin

# Ubuntu 20.04 → 26.x (LTS + intermedias + 26.04). Override: SERIES="jammy noble"
PPA_SERIES ?= focal jammy noble plucky questing resolute

.PHONY: all clean build package install uninstall test \
	deb-src sign upload ppa-release ppa-series ppa-series-upload load-ppa-env

all: build

build: $(TARGET)

$(TARGET): $(SOURCES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(LDFLAGS)

clean:
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
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.tar.*
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.dsc
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*_source.changes
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*_source.buildinfo
	rm -f $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)*.upload

test: build
	@chmod +x tests/run_tests.sh
	./tests/run_tests.sh

# Construir paquete .deb local (sin dh; útil offline)
package: clean build
	@echo "Construyendo paquete .deb..."
	@rm -rf $(BUILD_DIR)/package
	@install -d -m 755 $(BUILD_DIR)/package/usr/bin
	@install -d -m 755 $(BUILD_DIR)/package/usr/share/man/man1
	@install -d -m 755 $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)
	@install -d -m 755 $(BUILD_DIR)/package/DEBIAN
	@install -m 755 $(TARGET) $(BUILD_DIR)/package/usr/bin/$(PACKAGE_NAME)
	@strip --strip-unneeded $(BUILD_DIR)/package/usr/bin/$(PACKAGE_NAME)
	@install -m 644 man/$(PACKAGE_NAME).1 $(BUILD_DIR)/package/usr/share/man/man1/
	@gzip -9n -f $(BUILD_DIR)/package/usr/share/man/man1/$(PACKAGE_NAME).1
	@install -m 644 README.md $(BUILD_DIR)/package/usr/share/doc/$(PACKAGE_NAME)/
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
	@dpkg-deb --root-owner-group --build $(BUILD_DIR)/package $(PACKAGE_NAME)_$(FULL_VERSION)_amd64.deb
	@echo "Paquete creado: $(PACKAGE_NAME)_$(FULL_VERSION)_amd64.deb"

install: build
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/$(PACKAGE_NAME)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(PACKAGE_NAME)

load-ppa-env:
	@if [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		echo "Usando $(SHARED_KEYS)/launchpad/ubuntu-tools.env"; \
	else \
		echo "Aviso: no se encontró ubuntu-tools.env; usando defaults del Makefile"; \
	fi

# Paquete fuente para Launchpad (sin firmar)
deb-src: clean load-ppa-env
	@echo "Preparando paquete fuente para $(LAUNCHPAD_PPA) (serie: resolute)..."
	dpkg-buildpackage -S -us -uc
	@echo "Artefactos en $(PARENT_DIR)/:"
	@ls -1 $(PARENT_DIR)/$(PACKAGE_NAME)_$(VERSION)* $(PARENT_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)* 2>/dev/null || true

# Firmar .changes / .dsc (pide passphrase de GPG si aplica)
sign: load-ppa-env
	@key="$(DEBSIGN_KEYID)"; \
	if [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		set -a; . "$(SHARED_KEYS)/launchpad/ubuntu-tools.env"; set +a; \
		key="$$DEBSIGN_KEYID"; \
	fi; \
	changes="$(PARENT_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_source.changes"; \
	if [ ! -f "$$changes" ]; then \
		echo "No existe $$changes — ejecutá 'make deb-src' primero."; \
		exit 1; \
	fi; \
	echo "Firmando con $$key ..."; \
	debsign -k "$$key" "$$changes"

# Subir al PPA (requiere firma previa)
upload: load-ppa-env
	@ppa="$(LAUNCHPAD_PPA)"; \
	if [ -f "$(SHARED_KEYS)/launchpad/ubuntu-tools.env" ]; then \
		set -a; . "$(SHARED_KEYS)/launchpad/ubuntu-tools.env"; set +a; \
		ppa="$$LAUNCHPAD_PPA"; \
	fi; \
	changes="$(PARENT_DIR)/$(PACKAGE_NAME)_$(FULL_VERSION)_source.changes"; \
	if [ ! -f "$$changes" ]; then \
		echo "No existe $$changes — ejecutá 'make deb-src' y 'make sign' primero."; \
		exit 1; \
	fi; \
	echo "Subiendo a $$ppa ..."; \
	dput "$$ppa" "$$changes"

# Flujo completo (una sola serie del changelog actual): fuente → firmar → subir
ppa-release: deb-src sign upload

# Todas las series 20.04–26.x: genera y firma (sin subir)
ppa-series:
	@chmod +x scripts/ppa-release-series.sh
	SERIES="$(PPA_SERIES)" VERSION="$(VERSION)" DEB_VERSION="$(DEB_VERSION)" \
		./scripts/ppa-release-series.sh

# Todas las series: firma y sube al PPA
ppa-series-upload:
	@chmod +x scripts/ppa-release-series.sh
	SERIES="$(PPA_SERIES)" VERSION="$(VERSION)" DEB_VERSION="$(DEB_VERSION)" \
		UPLOAD=1 ./scripts/ppa-release-series.sh
