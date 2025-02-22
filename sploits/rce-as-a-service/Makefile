.PHONY: all clean docker wasm
.NOTPARALLEL:

WASM_DIR = wasm
DOCKER_TAG = wasm-builder-rust-cargo
TARGETS = $(WASM_DIR)/read_files.wasm $(WASM_DIR)/steal_password.wasm

all: docker $(TARGETS)

docker:
	docker build -t $(DOCKER_TAG) .

wasm: $(TARGETS)

$(WASM_DIR)/read_files.wasm: docker
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/read_files:/src \
		$(DOCKER_TAG)
	sudo chown -R $(shell id -u):$(shell id -g) read_files/target
	mv read_files/target/wasm32-wasip2/release/read_files.wasm $@

$(WASM_DIR)/steal_password.wasm: docker
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/steal_password:/src \
		$(DOCKER_TAG)
	sudo chown $(shell id -u):$(shell id -g) steal_password/target
	mv steal_password/target/wasm32-wasip2/release/steal_password.wasm $@

clean:
	rm -rf $(WASM_DIR)/*.wasm