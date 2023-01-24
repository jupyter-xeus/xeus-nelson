.. Copyright (c) 2023, Hind Montassif, Johan Mabille

   Distributed under the terms of the BSD 3-Clause License.

   The full license is in the file LICENSE, distributed with this software.

Build and configuration
=======================

General Build Options
---------------------

Building the xeus-nelson library
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``XEUS_NELSON_BUILD_STATIC``: Build xeus-nelson static library. **Enabled by default**.

Building and linking the kernel
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- ``XEUS_NELSON_BUILD_SHARED``: Split xnelson build into executable and library. **Enabled by default**.
- ``XEUS_NELSON_BUILD_EXECUTABLE``: Build the xnelson executable. **Enabled by default**.
- ``XEUS_NELSON_USE_SHARED_XEUS``: Link xnelson with the xeus shared library (instead of the static library). **Enabled by default**.
- ``XEUS_NELSON_USE_SHARED_XEUS_NELSON``: Link xnelson with the xeus-nelson shared library (instead of the static library). **Enabled by default**.

Other options
~~~~~~~~~~~~~

- ``XEUS_NELSON_EMSCRIPTEN_WASM_BUILD``: Build for wasm with emscripten. **Disabled by default**.


