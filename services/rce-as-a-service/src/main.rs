use std::error;
use std::net::{IpAddr, Ipv4Addr};

use rocket::data::{Data, ToByteUnit};
use rocket::{launch, post, routes};

use wasmtime::component::{Component, Linker, ResourceTable};
use wasmtime::{Engine, Store};
use wasmtime_wasi::bindings::Command;
use wasmtime_wasi::{WasiCtx, WasiCtxBuilder, WasiView};

pub struct ComponentRunStates {
    // These two are required basically as a standard way to enable the impl of IoView and
    // WasiView.
    // impl of WasiView is required by [`wasmtime_wasi::add_to_linker_sync`]
    pub wasi_ctx: WasiCtx,
    pub resource_table: ResourceTable,
    // You can add other custom host states if needed
}

impl WasiView for ComponentRunStates {
    fn ctx(&mut self) -> &mut WasiCtx {
        &mut self.wasi_ctx
    }

    fn table(&mut self) -> &mut ResourceTable {
        &mut self.resource_table
    }
}

async fn run_wasm(data: Data<'_>) -> Result<(), Box<dyn error::Error>> {
    let bytes = data.open(512.kibibytes()).into_bytes().await?;
    println!("Received {:?} bytes", bytes.len());

    let mut config = wasmtime::Config::new();
    config.async_support(true);
    let engine = Engine::new(&config)?;
    let mut linker = Linker::new(&engine);
    wasmtime_wasi::add_to_linker_async(&mut linker)?;

    // Create a WASI context an`d put it in a Store; all instances in the store
    // share this context. `WasiCtxBuilder` provides a number of ways to
    // configure what the target program will have access to.
    let wasi = WasiCtxBuilder::new()
        .inherit_stdio()
        .inherit_args()
        .inherit_network()
        .build();
    let state = ComponentRunStates {
        wasi_ctx: wasi,
        resource_table: ResourceTable::new(),
    };
    let mut store = Store::new(&engine, state);

    // Instantiate our component with the imports we've created, and run it.
    let component = Component::from_binary(&engine, &bytes)?;
    let command = Command::instantiate_async(&mut store, &component, &linker).await?;

    let res = command.wasi_cli_run().call_run(&mut store).await?;
    match res {
        Ok(()) => Ok(()),
        Err(()) => Err("execution failed".into()),
    }
}

#[post("/debug", format = "plain", data = "<data>")]
async fn debug(data: Data<'_>) {
    if let Err(e) = run_wasm(data).await {
        println!("error: {}", e);
    }
}

#[launch]
fn rocket() -> _ {
    let mut config = rocket::Config::default();
    config.address = IpAddr::V4(Ipv4Addr::new(0, 0, 0, 0));
    config.port = 9091;

    rocket::build()
        .configure(config)
        .mount("/api", routes![debug])
}
