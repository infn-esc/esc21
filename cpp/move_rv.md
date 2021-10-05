---
title: Move semantics
layout: main
category: cpp
---

The goal of this exercise is to appreciate the performance effect of move
semantics and we'll do in the context of returning a value from a function.

Open the program [`move.cpp`]({{site.exercises_repo}}/hands-on/cpp/move.cpp). It
contains a slight variation of the `make_vector` function introduced in one of
the previous exercises.

Measure the time it takes to execute it applying the following variations:

* the result is returned from the function

* the result is passed to the function as an output parameter (by reference or
  by pointer)

* the result is returned from the function, but the Return Value Optimization is
  disabled (how?)

Is there any difference?
