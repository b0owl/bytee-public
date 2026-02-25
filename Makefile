UNAME_S := $(shell uname -s)
UNAME_M := $(shell uname -m)
STATIC_LINK :=

ifeq ($(UNAME_S),Darwin)
  # Apple Silicon (arm64) uses /opt/homebrew, Intel/Rosetta uses /usr/local
  ifeq ($(UNAME_M),arm64)
    BREW_PREFIX := /opt/homebrew
  else
    BREW_PREFIX := /usr/local
  endif
  PKG_CONFIG := PKG_CONFIG_PATH=$(BREW_PREFIX)/lib/pkgconfig $(BREW_PREFIX)/bin/pkg-config
  GL_LIBS := -framework OpenGL
  GLFW_LIBS := -L$(BREW_PREFIX)/opt/glfw/lib -lglfw
else ifneq (,$(findstring MINGW,$(UNAME_S)))
  # Windows (MinGW/MSYS2)
  PKG_CONFIG := pkg-config
  GL_LIBS := -lopengl32
  GLFW_LIBS := -lglfw3
  STATIC_LINK := -static-libgcc -static-libstdc++
else ifneq (,$(findstring MSYS,$(UNAME_S)))
  # Windows (MSYS2)
  PKG_CONFIG := pkg-config
  GL_LIBS := -lopengl32
  GLFW_LIBS := -lglfw3
  STATIC_LINK := -static-libgcc -static-libstdc++
else
  # Linux
  PKG_CONFIG := pkg-config
  GL_LIBS := -lGL -lGLU
  GLFW_LIBS := -lglfw
  STATIC_LINK := -static-libgcc -static-libstdc++
endif

FT_CFLAGS := $(shell $(PKG_CONFIG) --cflags freetype2)
FT_LIBS := $(shell $(PKG_CONFIG) --libs freetype2)

.PHONY: engine engine-verbose engine-test unit-tests engine-portable individual clean

# ── Engine library ────────────────────────────────────────────────────────────
engine:
	@echo "BUILD 󰑃  Engine Library"
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  Deleting previous build..."
	@rm -f libengine.a && rm -rf obj/
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  Compiling sources..."
	@mkdir -p obj
	@BUILD_OUTPUT=$$(find engine/src -type f \( -name "*.cpp" -o -name "*.c" \) 2>/dev/null | while read file; do \
		OBJNAME=$$(echo $$file | sed 's/.*\///g' | sed -E 's/\.(cpp|c)$$/.o/g'); \
		g++ -c -fPIC -Iengine $(FT_CFLAGS) $$file -o obj/$$OBJNAME 2>&1 || exit 1; \
	done); \
	BUILD_EXIT=$$?; \
	echo "$$BUILD_OUTPUT" | grep -q "error:" && echo -e "\033[1;33m│ └ 󰑃\033[0m  \033[1;41;30mFATAL ERROR!\033[0m"; \
	echo "$$BUILD_OUTPUT" | grep -q "warning:" && echo -e "\033[1;33m│ └ 󰑃\033[0m  \033[1;43;30mWARNING!\033[0m"; \
	if [ $$BUILD_EXIT -ne 0 ]; then echo -e "\033[1;33m│ └ 󰑃\033[0m  \033[1;41;30mBuild failed\033[0m"; exit $$BUILD_EXIT; fi
	@if ls obj/*.o 1> /dev/null 2>&1; then ar rcs libengine.a obj/*.o; else echo -e "\033[1;33m│ └ 󰑃\033[0m  \033[1;43;30mNo object files compiled\033[0m"; exit 1; fi
engine-verbose:
	@echo "BUILD 󰑃  Engine Library (Verbose)"
	@rm -f libengine.a && rm -rf obj/ && mkdir -p obj
	@find engine/src -type f \( -name "*.cpp" -o -name "*.c" \) 2>/dev/null | while read file; do \
		OBJNAME=$$(echo $$file | sed 's/.*\///g' | sed -E 's/\.(cpp|c)$$/.o/g'); \
		echo -e "\033[1;33m│ ├ 󰑃\033[0m  $$file"; \
		g++ -c -fPIC -Iengine $(FT_CFLAGS) $$file -o obj/$$OBJNAME; \
	done
	@ar rcs libengine.a obj/*.o
	@echo -e "\033[1;33m│ └ 󰑃\033[0m  Done"

# ── Engine test (demo) ────────────────────────────────────────────────────────
engine-test: engine
	@echo "TEST 󰑃  Engine"
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  Building..."
	@g++ -o engine-test demo/main.cpp demo/objects.cpp demo/scenes/game_scene.cpp demo/scenes/title_screen.cpp demo/scenes/settings_scene.cpp -I. -Iengine -Idemo -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS) $(STATIC_LINK)
	@echo -e "\033[1;33m│ └ 󰑃\033[0m  Running..."
	@./engine-test

# ── Unit tests ────────────────────────────────────────────────────────────────
unit-tests: engine
	@rm -rf bin/tests && mkdir -p bin/tests
	@echo "TESTS 󰑃  Unit Tests"
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  FrameAllocator"
	@g++ -o bin/tests/FrameAllocator tests/FrameAllocator.cpp -I. -Iengine -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS)
	@./bin/tests/FrameAllocator | sed 's/^/\x1b[1;33m│ │ ├ 󰑃\x1b[0m  /'
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  StaticAllocator"
	@g++ -o bin/tests/StaticAllocator tests/StaticAllocator.cpp -I. -Iengine -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS)
	@./bin/tests/StaticAllocator | sed 's/^/\x1b[1;33m│ │ ├ 󰑃\x1b[0m  /'
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  WindowCreation"
	@g++ -o bin/tests/WindowCreation tests/WindowCreation.cpp -I. -Iengine -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS)
	@./bin/tests/WindowCreation | sed 's/^/\x1b[1;33m│ │ ├ 󰑃\x1b[0m  /'
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  Collisions"
	@g++ -o bin/tests/Collisions tests/Collisions.cpp -I. -Iengine -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS)
	@./bin/tests/Collisions | sed 's/^/\x1b[1;33m│ │ ├ 󰑃\x1b[0m  /'
	@echo -e "\033[1;33m│ └ 󰑃\033[0m  Pieces"
	@g++ -o bin/tests/Pieces tests/Pieces.cpp demo/objects.cpp -I. -Iengine -Idemo -L. -lengine $(GL_LIBS) $(GLFW_LIBS) $(FT_LIBS)
	@./bin/tests/Pieces | sed 's/^/\x1b[1;33m│   └ 󰑃\x1b[0m  /'

# ── Portable single-header ────────────────────────────────────────────────────
engine-portable:
	@echo "BUILD 󰑃  Portable Single Header"
	@echo -e "\033[1;33m│ ├ 󰑃\033[0m  Generating dist/bytee_single.h..."
	@python3 tools/amalgamate.py . | sed 's/^/\x1b[1;33m│ └ 󰑃\x1b[0m  /'

# ── Individual file ───────────────────────────────────────────────────────────
individual:
	@[ -z "$(FILE)" ] && echo "ERROR 󰑃  FILE not specified" && exit 1
	@echo "BUILD 󰑃  Individual File"
	@BASENAME=$$(basename $(FILE) .c); [ "$$BASENAME" = "$(FILE)" ] && BASENAME=$$(basename $(FILE) .cpp); \
	gcc $(FILE) -o $$BASENAME && echo -e "\033[1;33m│ └ 󰑃\033[0m  Running $$BASENAME..." && ./$$BASENAME

# ── Clean ─────────────────────────────────────────────────────────────────────
clean:
	@echo "CLEAN 󰑃  Build Artifacts"
	@rm -f libengine.a engine-test && rm -rf obj/ bin/ dist/
	@echo -e "\033[1;33m│ └ 󰑃\033[0m  Done"
