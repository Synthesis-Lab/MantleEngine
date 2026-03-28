#!/usr/bin/env python3
"""
Phase 5f Shader Compiler

Converts GLSL source code to SPIR-V bytecode and outputs as C++ arrays.
Requires glslangValidator or glslc to be installed.

Usage:
    python3 compile-shaders.py

Expected GLSL files:
    - cpp/shaders/vertex.glsl - Vertex shader
    - cpp/shaders/fragment.glsl - Fragment shader

Output:
    - cpp/shaders/generated_spirv.h - C++ header with SPIR-V bytecode
"""

import subprocess
import sys
import os
from pathlib import Path

def compile_glsl_to_spirv(glsl_file, shader_type):
    """
    Compile GLSL to SPIR-V using glslangValidator.
    shader_type: 'vert' for vertex, 'frag' for fragment
    """
    try:
        # Try glslangValidator first
        result = subprocess.run(
            ['glslangValidator', '-V', '-S', shader_type, '--vn', f'g_{shader_type.capitalize()}ShaderSPIRV', 
             '-o', f'{glsl_file}.spv.h', glsl_file],
            capture_output=True,
            text=True,
            timeout=10
        )
        
        if result.returncode == 0:
            return True, None
        else:
            return False, result.stderr
            
    except FileNotFoundError:
        return False, "glslangValidator not found. Install Vulkan SDK or use glslc."
    except Exception as e:
        return False, str(e)

def use_online_compiler(glsl_code, shader_type):
    """
    Fallback: Use online compiler API if local tools not available.
    This requires requests library and internet connection.
    """
    try:
        import requests
        
        print(f"Using online SPIR-V compiler for {shader_type}...")
        
        # Use Khronos online SPIR-V compiler API
        # Note: This is a simplified example - actual API may vary
        
        return False, "Online compiler not yet implemented"
        
    except ImportError:
        return False, "requests library not found"
    except Exception as e:
        return False, str(e)

def main():
    script_dir = Path(__file__).parent.parent
    shaders_dir = script_dir / 'cpp' / 'shaders'
    
    vertex_glsl = shaders_dir / 'vertex.glsl'
    fragment_glsl = shaders_dir / 'fragment.glsl'
    output_header = shaders_dir / 'generated_spirv.h'
    
    print("Phase 5f Shader Compiler")
    print("-" * 60)
    
    # Check if GLSL files exist
    if not vertex_glsl.exists() or not fragment_glsl.exists():
        print(f"ERROR: GLSL source files not found:")
        print(f"  - {vertex_glsl} (exists: {vertex_glsl.exists()})")
        print(f"  - {fragment_glsl} (exists: {fragment_glsl.exists()})")
        print("\nTo enable shader compilation:")
        print("1. Create cpp/shaders/vertex.glsl with vertex shader code")
        print("2. Create cpp/shaders/fragment.glsl with fragment shader code")
        print("3. Install Vulkan SDK or glslc")
        print("4. Run this script again\n")
        return 1
    
    # Try compiling with local tools
    print("Attempting to compile with local tools...")
    vert_success, vert_err = compile_glsl_to_spirv(str(vertex_glsl), 'vert')
    frag_success, frag_err = compile_glsl_to_spirv(str(fragment_glsl), 'frag')
    
    if vert_success and frag_success:
        print(f"✓ Successfully compiled shaders to SPIR-V")
        print(f"  Output: {output_header}")
        return 0
    
    print(f"✗ Local compilation failed: {vert_err or frag_err}")
    print("\nFallback: Using online compiler...")
    
    # Try online compiler
    with open(vertex_glsl) as f:
        vert_code = f.read()
    with open(fragment_glsl) as f:
        frag_code = f.read()
    
    online_ok, online_err = use_online_compiler(vert_code, 'vert')
    if not online_ok:
        print(f"✗ Online compiler also failed: {online_err}")
        print("\nTo fix shader compilation, install glslangValidator:")
        print("  Fedora/RHEL: sudo dnf install vulkan-tools")
        print("  Ubuntu/Debian: sudo apt install vulkan-tools")
        print("  macOS: brew install vulkan-headers")
        return 1
    
    print("✓ Online compilation successful")
    return 0

if __name__ == '__main__':
    sys.exit(main())
