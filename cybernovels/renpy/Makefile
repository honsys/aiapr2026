
.PHONY: build clean

RENPY_SDK_DIR = renpy_sdk/renpy

build:
	@echo "Cloning Ren'Py SDK..."
	mkdir -p renpy_sdk
	git clone https://github.com/renpy/renpy.git $(RENPY_SDK_DIR)
	@echo "Ren'Py SDK cloned to $(RENPY_SDK_DIR)"

clean:
	@echo "Removing Ren'Py SDK folder..."
	rm -rf $(RENPY_SDK_DIR)
	rm -rf renpy_sdk
	@echo "Ren'Py SDK folder removed."
