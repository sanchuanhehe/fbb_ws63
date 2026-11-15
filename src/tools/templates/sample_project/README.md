# Sample project

A minimal, buildable out-of-tree fbb project. This tree is the default
template used by `fbb create-project <name>`, which copies it to the
target path under the new project's name.

## Folder contents

```
├── fbb-project.toml        Project manifest: name / chip / target
├── CMakeLists.txt          Project entry; loads SDK's project.cmake
├── main
│   ├── CMakeLists.txt      Build config for the project's main component
│   └── app.c               Application entry (app_run + osal_printk)
└── README.md               This file
```

## Usage

1. Put your application code in `main/app.c`. The template registers
   a `hello_entry()` with `app_run()`, which the SDK calls during boot.
2. Add or remove source files in `main/CMakeLists.txt` (`SOURCES` list).
3. Edit `fbb-project.toml` if you need a different chip / target.
4. Build from the project directory:

   ```
   fbb build
   ```

   The CLI auto-detects `fbb-project.toml`, locates the SDK
   (via `$FBB_SDK_DIR` or auto-discovery), and forwards the build.

## How it wires up

`CMakeLists.txt` calls `include("$ENV{FBB_SDK_DIR}/build/cmake/project.cmake")`,
which is the SDK's public entry. It defines a `project()` macro that runs
the SDK's prologue + epilogue around the real `project()` call — so your
`project(<name>)` line transparently builds the firmware.

The SDK's component tree (`drivers/` / `middleware/` / `kernel/` / etc.)
is loaded from the SDK root; your `main/` (and optional `components/`) is
added on top and linked into the final image.

Customer code lives entirely in this directory — no need to clone or
modify the SDK to develop your application. Upgrading the SDK means
pointing `FBB_SDK_DIR` at a newer release.
