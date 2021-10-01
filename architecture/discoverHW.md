---
title: A look to your Hardware
layout: main
category: Architecture
---

To know which is the architecture of the machine you are logged in use
`lscpu`
googling for the Model Name will provide more detail.

More details about the NUMA setup can be obtained with `lstopo` or `numactl -H`.

To know which architecture the compiler believes to be the ``native`` one use
```bash
 gcc -march=native -Q --help=target | grep march | awk '{ print $2 }'
```
