use rhai::{Engine, EvalAltResult};

pub struct MantleCore {
    pub engine: Engine,
}

impl MantleCore {
    pub fn new() -> Self {
        let mut engine = Engine::new();
        
        // Buraya ileride MantleScript fonksiyonlarını kaydedeceğiz
        // Örneğin: engine.register_fn("print_mantle", || println!("Mantle says hi!"));

        Self { engine }
    }

    pub fn run_script(&self, code: &str) -> Result<(), Box<EvalAltResult>> {
        self.engine.run(code)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_script_execution() {
        let core = MantleCore::new();
        let result = core.run_script("print(\"MantleEngine is alive!\")");
        assert!(result.is_ok());
    }
}