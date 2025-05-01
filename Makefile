CC=clang
CFLAGS=-Wall -Wextra -Werror -O2
ARCH_FLAGS=-arch arm64 -arch x86_64
TARGET=build/libiokitrewrite.dylib
PACKAGE=build/img2track-mac-wrapper.zip

.PHONY: default
default: $(TARGET)

$(TARGET): src/iokitrewrite.c
	@mkdir -p build
	$(CC) $(CFLAGS) $(ARCH_FLAGS) -dynamiclib -o $@ $< -framework System -framework IOKit -framework CoreFoundation -framework CoreServices

.PHONY: run
run: $(TARGET)
	DYLD_INSERT_LIBRARIES=./$(TARGET) /Applications/img2track.app/Contents/MacOS/img2track

.PHONY: package
package: $(PACKAGE)

$(PACKAGE): $(TARGET) scripts/img2track
	@mkdir -p build
	@rm -f $@
	zip --junk-paths --must-match $@ $(TARGET) scripts/img2track

.PHONY: clean
clean:
	rm -rf build
