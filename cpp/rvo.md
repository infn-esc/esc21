---
title: Return Value Optimization
layout: main
category: cpp
---

The goal of this exercise is to appreciate the performance effect of the _Return Value Optimization_.

Open the program [`rvo.cpp`]({{site.exercises_repo}}/hands-on/cpp/rvo.cpp). It contains a slight variation of the
`make_vector` function introduced in one of the previous exercises.

Measure the time it takes to execute it, applying the following variations:

* the result is returned from the function

* the result is passed to the function as an output parameter (by reference or
  by pointer)

Is there any difference?

When can it make a difference?
