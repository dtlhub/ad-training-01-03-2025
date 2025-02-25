use crate::misc::{Error, Result};
use crate::storage::Storage;
use base64::prelude::BASE64_STANDARD;
use base64::Engine as _;
use rocket::data::ToByteUnit;
use rocket::http::Status;
use serde::Serialize;
use std::path::PathBuf;
use std::sync::Arc;
use wasmtime::component::{Component, Linker, ResourceTable};
use wasmtime::{Engine, Store, StoreLimits, StoreLimitsBuilder};
use wasmtime_wasi::bindings::Command;
use wasmtime_wasi::{pipe, DirPerms, FilePerms, WasiCtx, WasiCtxBuilder, WasiView};

pub struct ComponentState {
    pub wasi_ctx: WasiCtx,
    pub resource_table: ResourceTable,
    pub limits: StoreLimits,
}

impl ComponentState {
    pub fn new(wasi: WasiCtx) -> Self {
        Self {
            wasi_ctx: wasi,
            resource_table: ResourceTable::new(),
            limits: StoreLimitsBuilder::new()
                .memory_size(3.mebibytes().as_u64() as usize)
                .memories(5)
                .instances(5)
                .tables(100)
                .build(),
        }
    }
}

impl WasiView for ComponentState {
    fn ctx(&mut self) -> &mut WasiCtx {
        &mut self.wasi_ctx
    }

    fn table(&mut self) -> &mut ResourceTable {
        &mut self.resource_table
    }
}

#[derive(Serialize)]
#[serde(crate = "rocket::serde")]
pub struct ExecutionResult {
    pub stdout: String,
    pub stderr: String,
}

pub struct Sandbox {
    engine: Engine,
    linker: Linker<ComponentState>,
    storage: Arc<Storage>,
}

impl Sandbox {
    pub fn new(storage: Arc<Storage>) -> Result<Self> {
        let mut config = wasmtime::Config::new();
        config.async_support(true);
        config.consume_fuel(true);

        let engine = Engine::new(&config)?;
        let mut linker = Linker::new(&engine);
        wasmtime_wasi::add_to_linker_async(&mut linker)?;

        Ok(Self {
            engine,
            linker,
            storage,
        })
    }

    pub async fn execute_sandboxed(
        &self,
        user: &String,
        wasm: &[u8],
        args: &[impl AsRef<str>],
    ) -> Result<ExecutionResult> {
        let mut dir = self.storage.setup_user_directory(user).await?;

        let start_time = std::time::Instant::now();
        let run_wasm = self.run_wasm(wasm, args, dir.path()).await;
        let execution_time = start_time.elapsed();

        let close_dir = dir.close().await;

        let err = match (run_wasm, close_dir) {
            (Err(e1), _) => Err(e1.with_context(format!("execute code from user {}", user))),
            (Ok(_), Err(e2)) => {
                Err(e2.with_context(format!("close user directory for user {}", user)))
            }
            (Ok(wasm_result), Ok(_)) => Ok(wasm_result),
        };

        match err {
            Ok(_) => log::info!(
                "Successfully executed code from user {} (TimeElapsed: {:?})",
                user,
                execution_time
            ),
            Err(ref e) => log::info!(
                "Error executing code from user {} (TimeElapsed: {:?}): {}",
                user,
                execution_time,
                e
            ),
        };

        err
    }

    async fn run_wasm(
        &self,
        wasm: &[u8],
        args: &[impl AsRef<str>],
        host_mount: &PathBuf,
    ) -> Result<ExecutionResult> {
        let stdout = pipe::MemoryOutputPipe::new(64.kibibytes().as_u64() as usize);
        let stderr = pipe::MemoryOutputPipe::new(64.kibibytes().as_u64() as usize);

        let mut real_args = args.iter().map(|a| a.as_ref()).collect::<Vec<_>>();
        real_args.insert(0, "wasm");

        let wasi = WasiCtxBuilder::new()
            .stdin(pipe::ClosedInputStream {})
            .stdout(stdout.clone())
            .stderr(stderr.clone())
            .args(real_args.leak())
            .inherit_env()
            .inherit_network()
            .preopened_dir(host_mount, "/", DirPerms::all(), FilePerms::all())?
            .build();

        let state = ComponentState::new(wasi);
        let mut store = Store::new(&self.engine, state);
        store.limiter(|state| &mut state.limits);
        store.set_fuel(1_200_000)?;

        let as_bad_request = |e: wasmtime::Error| Error::new(Status::BadRequest, e.to_string());

        let component = Component::from_binary(&self.engine, &wasm).map_err(as_bad_request)?;
        let command = Command::instantiate_async(&mut store, &component, &self.linker)
            .await
            .map_err(as_bad_request)?;

        let result = command
            .wasi_cli_run()
            .call_run(&mut store)
            .await
            .map_err(as_bad_request)?;

        match result {
            Ok(()) => Ok(ExecutionResult {
                stdout: BASE64_STANDARD.encode(stdout.contents()),
                stderr: BASE64_STANDARD.encode(stderr.contents()),
            }),
            Err(()) => Err(Error::new(Status::BadRequest, "execution failed")),
        }
    }
}
