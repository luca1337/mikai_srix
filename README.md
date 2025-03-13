# LibMIKAI

## Introduction
MIKAI is a recent project that aims to avoid the sale of mykey program, making it **free** and **open source**.

**Donations are accepted and needed for the development of this project!** (only cryptocurrencies).

Bitcoin: bc1q6anwfehg99uymlgevh9enes5t674etjm85k8jd
Ethereum: 0x11d7D21B5715fD1AAB005a645614209295CA2fCE
Monero: 44meLyqcheWRUSdxsiXA5b4ZRKidCtangFTS6orM5wBu8SbYYtGhjZDMyEyBS5WT3PKs73V9ZMzCvavN9p8sdzVQJoXx56x

Current maintainer is [Lilz](https://t.me/Lilz073).
This library has been completely **rewritten** by him.

## LICENSE:
libmikai: Proprietary license (LICENSE file)
mikai compiled: Proprietary license (LICENSE file)

## Library structure
| Layer | Description                      |
|-------|----------------------------------|
| 4     | Public API (mikai.h)             |
| 3     | Application level (mykey folder) |
| 2     | SRIX4k commands (srix4k folder)  |
| 1     | ISO1443b-ST management (libnfc)  |
| 0     | Physic level (PN532)             |

## Getting started
Make sure that CMake is installed on your PC and run it selecting a target folder. 
Make sure you have installed libnfc-dev.