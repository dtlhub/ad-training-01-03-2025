.PHONY: all clean docker-images
.NOTPARALLEL:

WASM_DIR = wasm
DOCKER_C = wasm-builder-c
DOCKER_GO = wasm-builder-go
DOCKER_RUST = wasm-builder-rust
DOCKER_ZIG = wasm-builder-zig

C_TARGETS = $(WASM_DIR)/simple-c.wasm $(WASM_DIR)/reverser-c.wasm $(WASM_DIR)/writer-c.wasm $(WASM_DIR)/reader-c.wasm
GO_TARGETS = $(WASM_DIR)/simple-go.wasm $(WASM_DIR)/reverser-go.wasm $(WASM_DIR)/writer-go.wasm $(WASM_DIR)/reader-go.wasm
RUST_TARGETS = $(WASM_DIR)/simple-rust.wasm $(WASM_DIR)/reverser-rust.wasm $(WASM_DIR)/writer-rust.wasm $(WASM_DIR)/reader-rust.wasm
ZIG_TARGETS = $(WASM_DIR)/simple-zig.wasm $(WASM_DIR)/reverser-zig.wasm $(WASM_DIR)/writer-zig.wasm $(WASM_DIR)/reader-zig.wasm

all: docker-images $(C_TARGETS) $(GO_TARGETS) $(RUST_TARGETS) $(ZIG_TARGETS)

docker-images:
	docker build -t $(DOCKER_C) c/
	docker build -t $(DOCKER_GO) go/
	docker build -t $(DOCKER_RUST) rust/
	docker build -t $(DOCKER_ZIG) zig/

# C targets
$(WASM_DIR)/simple-c.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/c/simple:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_C) /src/main.c
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reverser-c.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/c/reverser:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_C) /src/main.c
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/writer-c.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/c/writer:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_C) /src/main.c
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reader-c.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/c/reader:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_C) /src/main.c
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

# Go targets
$(WASM_DIR)/simple-go.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/go/simple:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_GO) /src/main.go
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reverser-go.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/go/reverser:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_GO) /src/main.go
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/writer-go.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/go/writer:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_GO) /src/main.go
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reader-go.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/go/reader:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_GO) /src/main.go
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

# Rust targets
$(WASM_DIR)/simple-rust.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/rust/simple/src:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_RUST) /src/main.rs
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reverser-rust.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/rust/reverser/src:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_RUST) /src/main.rs
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/writer-rust.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/rust/writer/src:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_RUST) /src/main.rs
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reader-rust.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/rust/reader/src:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_RUST) /src/main.rs
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

# Zig targets
$(WASM_DIR)/simple-zig.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/zig/simple:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_ZIG) /src/main.zig
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reverser-zig.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/zig/reverser:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_ZIG) /src/main.zig
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/writer-zig.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/zig/writer:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_ZIG) /src/main.zig
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

$(WASM_DIR)/reader-zig.wasm: docker-images
	mkdir -p $(WASM_DIR)
	docker run --rm \
		-v $(PWD)/zig/reader:/src \
		-v $(PWD)/$(WASM_DIR):/out \
		$(DOCKER_ZIG) /src/main.zig
	mv $(WASM_DIR)/output.wasm $@
	sudo chown $(shell id -u):$(shell id -g) $@

c-targets: $(C_TARGETS)

go-targets: $(GO_TARGETS)

rust-targets: $(RUST_TARGETS)

zig-targets: $(ZIG_TARGETS)

c-clean:
	rm -rf $(C_TARGETS)

go-clean:
	rm -rf $(GO_TARGETS)

rust-clean:
	rm -rf $(RUST_TARGETS)

zig-clean:
	rm -rf $(ZIG_TARGETS)

clean:
	rm -rf $(WASM_DIR)/*.wasm