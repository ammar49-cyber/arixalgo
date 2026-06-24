pub fn version() -> &'static str {
    "0.1.0"
}

pub async fn hello() -> String {
    format!("Hello from ARIX-Distributed v{}", version())
}
