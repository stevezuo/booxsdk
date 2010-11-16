# The Onyx Boox SDK

## Introduction

The Onyx Boox SDK is used for developing applications on the Boox
ebook readers produced by Onyx International.

## Development environment

The toolchain used for cross-compiling can be obtained at
`http://dev.onyxcommunity.com/sdk/freescale-toolchain.tar.gz`. Run the
following shell commands to install it:
    cd /tmp
    wget http://dev.onyxcommunity.com/sdk/freescale-toolchain.tar.gz
    sudo mkdir -p /opt
    cd /opt && sudo tar -xzf /tmp/freescale-toolchain.tar.gz

## Building the SDK

Grab the source:
    git clone git@github.com:onyx-intl/booxsdk.git
    git submodule update --init

To build the SDK, install CMake and Rake, then run
    rake build:arm:default

CCache and DistCC will be used if they are available. If you do not
want to use DistCC, append `DISABLE_DISTCC=1` to the build command.

The SDK is frequently updated and sometimes may break compatibility
with earlier versions. If you want your application to run no matter
which version of the libraries are installed on the device, you can
link your application against the static libraries. You can build the
static libraries by running
    rake build:arm:static
