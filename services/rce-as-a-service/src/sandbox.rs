use crate::misc::Result;
use crate::storage::Storage;
use serde::Serialize;
use std::path::PathBuf;
use std::sync::Arc;
use wasmtime::component::{Component, Linker, ResourceTable};
use wasmtime::{Engine, Store};
use wasmtime_wasi::bindings::Command;
use wasmtime_wasi::{DirPerms, FilePerms, WasiCtx, WasiCtxBuilder, WasiView};

pub struct ComponentRunStates {
    pub wasi_ctx: WasiCtx,
    pub resource_table: ResourceTable,
}

impl WasiView for ComponentRunStates {
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
    linker: Linker<ComponentRunStates>,
    storage: Arc<Storage>,
}

impl Sandbox {
    pub fn new(storage: Arc<Storage>) -> Result<Self> {
        let mut config = wasmtime::Config::new();
        config.async_support(true);

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

        let run_wasm = self.run_wasm(wasm, args, dir.path()).await;
        let close_dir = dir.close().await;
        match (run_wasm, close_dir) {
            (Err(e1), _) => Err(e1),
            (Ok(_), Err(e2)) => Err(e2),
            (Ok(wasm_result), Ok(_)) => Ok(wasm_result),
        }
    }

    async fn run_wasm(
        &self,
        wasm: &[u8],
        args: &[impl AsRef<str>],
        host_mount: PathBuf,
    ) -> Result<ExecutionResult> {
        let wasi = WasiCtxBuilder::new()
            .args(args)
            .inherit_network()
            .inherit_stdio() // TODO: stream stdout and stderr into execution result somehow
            .preopened_dir(&host_mount, "/", DirPerms::all(), FilePerms::all())?
            .build();

        let state = ComponentRunStates {
            wasi_ctx: wasi,
            resource_table: ResourceTable::new(),
        };
        let mut store = Store::new(&self.engine, state);

        // store.set_epoch_deadline(ticks_beyond_current);
        // let limiter = Limiter::new(1000, 1000);
        // store.limiter_async(limiter);
        // store.set_fuel(fuel)

        let component = Component::from_binary(&self.engine, &wasm)?;
        let command = Command::instantiate_async(&mut store, &component, &self.linker).await?;

        let res = command.wasi_cli_run().call_run(&mut store).await?;
        match res {
            Ok(()) => Ok(ExecutionResult {
                stdout: String::new(),
                stderr: String::new(),
            }),
            Err(()) => Err("execution failed".into()),
        }
    }
}
