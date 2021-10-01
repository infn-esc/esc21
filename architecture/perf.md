---
title: Profiling using perf
layout: main
category: Architecture
---

# PERF

`perf` is a tool to profile a process using *Hardware Performance Counters*. Each counter counts *events* in the CPU
such as cycles, executed instructions, loads from a given level of the memory caches, branches...

`perf list` lists the event that are predefined inside the tool.

The easiest way to use `perf` is to profile the whole application (say `./a.out`) using a default set of events

```shell
perf stat -d ./a.out
```

or even

```shell
perf stat -d -d -d ./a.out
```

One can choose a set of events and list them on the command line, for example:

```shell
perf stat -e task-clock -e cycles -e instructions \
-e branch-instructions -e branch-misses \
-e idle-cycles-frontend -e idle-cycles-backend \
-e cache-references -e cache-misses -e L1-dcache-loads -e L1-dcache-load-misses ./a.out
```

The [`doPerf`]({{site.exercises_repo}}/hands-on/architecture/doPerf) script is available, that runs `perf` on an
executable, counting those events.

For large applications more details can be obtained running ``perf record``  that will produce a file containing all sampled events and their location in the application.
``perf record  --call-graph=dwarf`` will produce a full call-graph. On more recent Intel hardware (since Haswell)
one can use ``perf record  --call-graph=lbr`` which is faster and produces a more compact report.
``perf report`` can be used to display the detailed profile.
To identify performance "hotspot" at code level compile with "-g" and "perf report" will interleave source code with assembler.

perf record/report is well documented in
https://perf.wiki.kernel.org/index.php/Tutorial#Sample_analysis_with_perf_report

an interesting reading is also
https://stackoverflow.com/questions/27742462/understanding-linux-perf-report-output


a wrapper defining more user-friedly name for INTEL counters can be downloaded
{% highlight bash %}
cd;git clone https://github.com/andikleen/pmu-tools.git
{% endhighlight %}
in your home directory
and executed in place of `perf` as
 {% highlight bash %}
~/pmu-tools/ocperf.py
{% endhighlight %}
try
{% highlight bash %}
~/pmu-tools/ocperf.py list
{% endhighlight %}
to have a list of ALL available counters (and their meaning)
The actual name of the counters keep changing, so for a detail analysis one has to tailor the events to the actual hardware...

for an example (tailored to the Ivy-Bridge machines used for the exercise) see
[`doOCPerfIB`]({{site.exercises_repo}}/hands-on/architecture/doOCPerfIB)

or an example (tailored to the Skylake-avx512 machine also available used for the exercise) see
[`doOCPerfSX`]({{site.exercises_repo}}/hands-on/architecture/doOCPerfSX)

The "TopDown" metodology and the use of toplel tool is documented in
https://github.com/andikleen/pmu-tools/wiki/toplev-manual
and in the excellent slides by A.Y. himself
http://www.cs.technion.ac.il/~erangi/TMA_using_Linux_perf__Ahmad_Yasin.pdf


## Excercise 1

### Architecture: Front-end

Consider [branchPredictor.cpp]({{site.exercises_repo}}/hands-on/architecture/branchPredictor.cpp). Compile it and
measure its performance with and without the sorting (enough to run with and without an argument). Explain the
behaviour. Try the "original" version: possibly with a different compiler (gcc 4.7 for instance). Understand what it is
happening (godbold can be of help). Modify the code to make it "branchless".

### Architecture: back-end

Use [backend.cpp]({{site.exercises_repo}}/hands-on/architecture/backend.cpp)

compile (c++ -Wall -g -march=native) with different compiler options (-O2,-O3, -Ofast, -funroll-loops) measure performance,
indetify "hotspot", modify code to speed it up.


Excercise 2
===========


Discover where and why time is wasted
-------------------------------------

Use perf to discover why the performance of the four programs
``randg.py``, ``randgNP.py``, ``randg.cpp`` and ``erfinv_t.cpp``
(the last two to compile with ``c++ -Ofast -march=native``)
are so vastly different even if there are supposed to perfom exaclty the same work:
throw "N" random gaussian numbers and compute average and rms....

Hints:
try for instance
``~/pmu-tools/toplev.py -l3  -v --single-thread taskset -c 10  --show-sample python randgNP.py``
(instead of 10 use your "student number", this is to limit runinng on one cpu, one for each student)
then run what suggested for sampling (but instead of -g use ``--call-graph=dwarf`` ``or --call-graph=lbr``
display the results with either ``perf report -g graph,0.05,caller`` or ``perf report -g graph,0.05,callee`` or the simple
``perf report -g none --no-children``
You can add ``--stdio`` to have the graph printed on the screen on in a file


Excercise 3
===========

Exchange the order of the loops in the matrix multiplication
------------------------------------------------------------

Use [matmul.cpp]({{site.exercises_repo}}/hands-on/architecture/matmul.cpp)

Compile
{% highlight bash %}
c++ -O2 -fopt-info-vec -march=native
{% endhighlight %}
Measure. What's happening?
{% highlight bash %}
perf stat -d ./a.out
{% endhighlight %}

Recompile with<br>
-O3  (aggressive optimization and vectorization)<br>
-Ofast (allow reordering of math operation)<br>
Add -funroll-loops (force loop unrolling)

Change the product in a division
(use `doOCPerfIB`)

More tests
==========


Caveat: as in Exercise 2 compiler optimization choices may affect performances well bejond
code changes and HW architecture.



Compare Horner Method with Estrin
----------------------------------


Use [PolyTest.cpp]({{site.exercises_repo}}/hands-on/architecture/PolyTest.cpp)

compile, measure performance and eventually change compiler options as in Exercise 2

try also [pipeline.cpp]({{site.exercises_repo}}/hands-on/architecture/pipeline.cpp)


Branch predictor in OO code
----------------------------------

Use [Virtual.cpp]({{site.exercises_repo}}/hands-on/architecture/Virtual.cpp)

compile, measure performance and eventually change compiler options as in Exercise 2

Measure in various conditions
   * Remove “random_shuffle”
   * Increase number of Derived Classes
   * Try to change the order in the vector of pointers
   * Try to see if using an _ad-hoc_ type identification makes a difference
   * Compare with a SOA
   * Try “AnyOf”





Different form of “Branching” in conditional code
----------------------------------

Use [branchPredictor.cpp]({{site.exercises_repo}}/hands-on/architecture/branchPredictor.cpp)

compile, measure performance with and without the sorting (enough to run with and without an argument)
explain behaviour. Try the "original" version: eventually with a different compiler (gcc 4.7 for instance).
Undestand what it is happening (godbold can be of help). Modify the code to make it "branchless".


Use [Branch.cpp]({{site.exercises_repo}}/hands-on/architecture/Branch.cpp)

compile, measure performance and eventually change compiler options as in Exercise 2

Measure in various conditions
   * Remove “random_shuffle”
   * change the way the conditions are expressed

